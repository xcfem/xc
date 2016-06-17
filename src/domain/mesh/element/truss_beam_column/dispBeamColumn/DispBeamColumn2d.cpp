//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------

/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.25 $
// $Date: 2004/10/30 00:05:32 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/dispBeamColumn/DispBeamColumn2d.cpp,v $

// Written: MHS
// Created: Feb 2001
//
// Description: This file contains the class definition for XC::DispBeamColumn2d.

#include <domain/mesh/element/truss_beam_column/dispBeamColumn/DispBeamColumn2d.h>
#include "domain/component/Parameter.h"
#include <domain/mesh/node/Node.h>
#include <material/section/SeccionBarraPrismatica.h>
#include <domain/mesh/element/coordTransformation/CrdTransf2d.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <domain/domain/Domain.h>
#include <cstring>
#include <domain/mesh/element/Information.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/recorder/response/ElementResponse.h>
#include "domain/load/beam_loads/BeamMecLoad.h"
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/quadrule/GaussQuadRule1d01.h>
#include "material/section/ResponseId.h"
#include "utility/actor/actor/MovableVector.h"

 XC::Matrix XC::DispBeamColumn2d::K(6,6);
 XC::Vector XC::DispBeamColumn2d::P(6);
double XC::DispBeamColumn2d::workArea[100];
 XC::GaussQuadRule1d01 XC::DispBeamColumn2d::quadRule;

XC::DispBeamColumn2d::DispBeamColumn2d(int tag, int nd1, int nd2,
				       int numSec,const std::vector<SeccionBarraPrismatica *> &s,
                                   CrdTransf2d &coordTransf, double r)
  :BeamColumnWithSectionFDTrf2d(tag, ELE_TAG_DispBeamColumn2d,numSec), q(3), rho(r)
  {
    load.reset(6);
    setSections(s);
    set_transf(&coordTransf);

    // Set connected external node IDs
    theNodes.set_id_nodos(nd1,nd2);

    q0[0] = 0.0;
    q0[1] = 0.0;
    q0[2] = 0.0;

    p0[0] = 0.0;
    p0[1] = 0.0;
    p0[2] = 0.0;

// AddingSensitivity:BEGIN /////////////////////////////////////
    parameterID = 0;
// AddingSensitivity:END //////////////////////////////////////
  }

XC::DispBeamColumn2d::DispBeamColumn2d(int tag,int numSec,const Material *m,const CrdTransf *trf)
  :BeamColumnWithSectionFDTrf2d(tag, ELE_TAG_DispBeamColumn2d,numSec,m,trf), q(3), rho(0.0)
  {
    load.reset(6);
    q0[0] = 0.0;
    q0[1] = 0.0;
    q0[2] = 0.0;

    p0[0] = 0.0;
    p0[1] = 0.0;
    p0[2] = 0.0;

// AddingSensitivity:BEGIN /////////////////////////////////////
    parameterID = 0;
// AddingSensitivity:END //////////////////////////////////////
  }

XC::DispBeamColumn2d::DispBeamColumn2d(int tag)
  :BeamColumnWithSectionFDTrf2d(tag, ELE_TAG_DispBeamColumn2d,1), q(3), rho(0.0)
  {
    load.reset(6);
    q0[0] = 0.0;
    q0[1] = 0.0;
    q0[2] = 0.0;

    p0[0] = 0.0;
    p0[1] = 0.0;
    p0[2] = 0.0;

// AddingSensitivity:BEGIN /////////////////////////////////////
    parameterID = 0;
// AddingSensitivity:END //////////////////////////////////////
  }

//! @brief Constructor virtual.
XC::Element* XC::DispBeamColumn2d::getCopy(void) const
  { return new XC::DispBeamColumn2d(*this); }

int XC::DispBeamColumn2d::getNumDOF(void) const
  { return 6; }

void XC::DispBeamColumn2d::setDomain(Domain *theDomain)
  {
    // Check XC::Domain is not null - invoked when object removed from a domain
    BeamColumnWithSectionFDTrf2d::setDomain(theDomain);
    if(theDomain)
      {
        int dofNd1 = theNodes[0]->getNumberDOF();
        int dofNd2 = theNodes[1]->getNumberDOF();

        if(dofNd1 != 3 || dofNd2 != 3)
          {
            //std::cerr << "FATAL ERROR XC::DispBeamColumn2d (tag: %d), has differing number of DOFs at its nodes",
            //        this->getTag());
            return;
          }
        initialize_trf();

        const double L = theCoordTransf->getInitialLength();

        if(L == 0.0)
          {
            // Add some error check
          }
        this->update();
      }
  }

int XC::DispBeamColumn2d::commitState()
  {
    int retVal = 0;

    // call element commitState to do any base class stuff
    if((retVal = this->XC::BeamColumnWithSectionFDTrf2d::commitState()) != 0) {
      std::cerr << "XC::DispBeamColumn2d::commitState () - failed in base class";
    }

    retVal += theCoordTransf->commitState();

    return retVal;
  }

int XC::DispBeamColumn2d::revertToLastCommit()
  {
    int retVal = 0;

  const size_t numSections= getNumSections();
    // Loop over the integration points and revert to last committed state
    for(size_t i = 0; i < numSections; i++)
         retVal += theSections[i]->revertToLastCommit();

    retVal += theCoordTransf->revertToLastCommit();

    return retVal;
  }

int XC::DispBeamColumn2d::revertToStart()
{
    int retVal = 0;

  const size_t numSections= getNumSections();
    // Loop over the integration points and revert states to start
    for(size_t i= 0;i<numSections; i++)
      retVal += theSections[i]->revertToStart();

    retVal += theCoordTransf->revertToStart();

    return retVal;
}

int
XC::DispBeamColumn2d::update(void)
{
  // Update the transformation
  theCoordTransf->update();

  // Get basic deformations
  const XC::Vector &v = theCoordTransf->getBasicTrialDisp();

  double L = theCoordTransf->getInitialLength();
  double oneOverL = 1.0/L;
  const size_t numSections= getNumSections();
  const XC::Matrix &pts = quadRule.getIntegrPointCoords(numSections);

  // Loop over the integration points
  for(size_t i = 0; i < numSections; i++) {

    int order = theSections[i]->getOrder();
    const XC::ID &code = theSections[i]->getType();

    Vector e(workArea, order);

    double xi6 = 6.0*pts(i,0);

    int j;
    for(j = 0; j < order; j++) {
      switch(code(j)) {
      case SECTION_RESPONSE_P:
        e(j) = oneOverL*v(0); break;
      case SECTION_RESPONSE_MZ:
        e(j) = oneOverL*((xi6-4.0)*v(1) + (xi6-2.0)*v(2)); break;
      default:
        e(j) = 0.0; break;
      }
    }

    // Set the section deformations
    theSections[i]->setTrialSectionDeformation(e);
  }

  return 0;
}

const XC::Matrix &XC::DispBeamColumn2d::getTangentStiff(void) const
{
  static Matrix kb(3,3);

  // Zero for integral
  kb.Zero();
  q.Zero();

  const size_t numSections= getNumSections();
  const XC::Matrix &pts = quadRule.getIntegrPointCoords(numSections);
  const XC::Vector &wts = quadRule.getIntegrPointWeights(numSections);

  double L = theCoordTransf->getInitialLength();
  double oneOverL = 1.0/L;

  // Loop over the integration points
  for(size_t i = 0; i < numSections; i++)
    {

    int order = theSections[i]->getOrder();
    const XC::ID &code = theSections[i]->getType();

    Matrix ka(workArea, order, 3);
    ka.Zero();

    double xi6 = 6.0*pts(i,0);

    // Get the section tangent stiffness and stress resultant
    const XC::Matrix &ks = theSections[i]->getSectionTangent();
    const XC::Vector &s = theSections[i]->getStressResultant();

    // Perform numerical integration
    //kb.addMatrixTripleProduct(1.0, *B, ks, wts(i)/L);
    double wti = wts(i)*oneOverL;
    double tmp;
    int j, k;
    for(j = 0; j < order; j++) {
      switch(code(j)) {
      case SECTION_RESPONSE_P:
        for(k = 0; k < order; k++)
          ka(k,0) += ks(k,j)*wti;
        break;
      case SECTION_RESPONSE_MZ:
        for(k = 0; k < order; k++) {
          tmp = ks(k,j)*wti;
          ka(k,1) += (xi6-4.0)*tmp;
          ka(k,2) += (xi6-2.0)*tmp;
        }
        break;
      default:
        break;
      }
    }
    for(j = 0; j < order; j++) {
      switch (code(j)) {
      case SECTION_RESPONSE_P:
        for(k = 0; k < 3; k++)
          kb(0,k) += ka(j,k);
        break;
      case SECTION_RESPONSE_MZ:
        for(k = 0; k < 3; k++) {
          tmp = ka(j,k);
          kb(1,k) += (xi6-4.0)*tmp;
          kb(2,k) += (xi6-2.0)*tmp;
        }
        break;
      default:
        break;
      }
    }

    //q.addMatrixTransposeVector(1.0, *B, s, wts(i));
    double si;
    for(j = 0; j < order; j++)
      {
        si = s(j)*wts(i);
        switch(code(j)) {
        case SECTION_RESPONSE_P:
          q(0) += si; break;
        case SECTION_RESPONSE_MZ:
          q(1) += (xi6-4.0)*si; q(2) += (xi6-2.0)*si; break;
        default:
          break;
      }
    }

  }

  // Add effects of element loads, q = q(v) + q0
  q(0) += q0[0];
  q(1) += q0[1];
  q(2) += q0[2];

    // Transform to global stiffness
    K = theCoordTransf->getGlobalStiffMatrix(kb, q);
    if(isDead())
      K*=dead_srf;
    return K;
  }

const XC::Matrix &XC::DispBeamColumn2d::getInitialBasicStiff(void) const
{
  static XC::Matrix kb(3,3);

  // Zero for integral
  kb.Zero();

  const size_t numSections= getNumSections();
  const XC::Matrix &pts = quadRule.getIntegrPointCoords(numSections);
  const XC::Vector &wts = quadRule.getIntegrPointWeights(numSections);

  double L = theCoordTransf->getInitialLength();
  double oneOverL = 1.0/L;

  // Loop over the integration points
  for(size_t i= 0;i<numSections;i++) {

    int order = theSections[i]->getOrder();
    const XC::ID &code = theSections[i]->getType();

    Matrix ka(workArea, order, 3);
    ka.Zero();

    double xi6 = 6.0*pts(i,0);

    // Get the section tangent stiffness and stress resultant
    const XC::Matrix &ks = theSections[i]->getInitialTangent();

    // Perform numerical integration
    //kb.addMatrixTripleProduct(1.0, *B, ks, wts(i)/L);
    double wti = wts(i)*oneOverL;
    double tmp;
    int j, k;
    for(j = 0; j < order; j++) {
      switch(code(j)) {
      case SECTION_RESPONSE_P:
        for(k = 0; k < order; k++)
          ka(k,0) += ks(k,j)*wti;
        break;
      case SECTION_RESPONSE_MZ:
        for(k = 0; k < order; k++) {
          tmp = ks(k,j)*wti;
          ka(k,1) += (xi6-4.0)*tmp;
          ka(k,2) += (xi6-2.0)*tmp;
        }
        break;
      default:
        break;
      }
    }
    for(j = 0; j < order; j++) {
      switch (code(j)) {
      case SECTION_RESPONSE_P:
        for(k = 0; k < 3; k++)
          kb(0,k) += ka(j,k);
        break;
      case SECTION_RESPONSE_MZ:
        for(k = 0; k < 3; k++) {
          tmp = ka(j,k);
          kb(1,k) += (xi6-4.0)*tmp;
          kb(2,k) += (xi6-2.0)*tmp;
        }
        break;
      default:
        break;
      }
    }

  }
    return kb;
}

const XC::Matrix &XC::DispBeamColumn2d::getInitialStiff(void) const
  {
    const XC::Matrix &kb = this->getInitialBasicStiff();
    // Transform to global stiffness
    K = theCoordTransf->getInitialGlobalStiffMatrix(kb);
    if(isDead())
      K*=dead_srf;
    return K;
  }

const XC::Matrix&XC::DispBeamColumn2d::getMass(void) const
  {
    K.Zero();
    if(rho == 0.0)
      return K;
    const double L = theCoordTransf->getInitialLength();
    const double m = 0.5*rho*L;
    K(0,0) = K(1,1) = K(3,3) = K(4,4) = m;
    if(isDead())
      K*=dead_srf;
    return K;
  }

void XC::DispBeamColumn2d::zeroLoad(void)
  {
    BeamColumnWithSectionFDTrf2d::zeroLoad();

    q0[0] = 0.0;
    q0[1] = 0.0;
    q0[2] = 0.0;

    p0[0] = 0.0;
    p0[1] = 0.0;
    p0[2] = 0.0;
    return;
  }

int XC::DispBeamColumn2d::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(isDead())
      std::cerr << nombre_clase() 
                << "; se intentó cargar el elemento "
                << getTag() << " que está desactivado." 
                << std::endl;
    else
      {
        if(BeamMecLoad *beamMecLoad= dynamic_cast<BeamMecLoad *>(theLoad))
          {
            const double L = theCoordTransf->getInitialLength();
            beamMecLoad->addReactionsInBasicSystem(L,loadFactor,p0); // Accumulate reactions in basic system
            beamMecLoad->addFixedEndForcesInBasicSystem(L,loadFactor,q0); // Fixed end forces in basic system
          }
        else
          {
            std::cerr << "XC::DispBeamColumn2d::DispBeamColumn2d -- load type unknown for element with tag: "
                      << this->getTag() << "XC::DispBeamColumn2d::addLoad()\n";
            return -1;
          }
      }
    return 0;
  }

int XC::DispBeamColumn2d::addInertiaLoadToUnbalance(const XC::Vector &accel)
  {
        // Check for a XC::quick return
        if(rho == 0.0)
                return 0;

        // Get R * accel from the nodes
        const XC::Vector &Raccel1 = theNodes[0]->getRV(accel);
        const XC::Vector &Raccel2 = theNodes[1]->getRV(accel);

    if(3 != Raccel1.Size() || 3 != Raccel2.Size()) {
      std::cerr << "XC::DispBeamColumn2d::addInertiaLoadToUnbalance matrix and vector sizes are incompatable\n";
      return -1;
    }

        double L = theCoordTransf->getInitialLength();
        double m = 0.5*rho*L;

    // Want to add ( - fact * M R * accel ) to unbalance
        // Take advantage of lumped mass matrix
        load(0) -= m*Raccel1(0);
        load(1) -= m*Raccel1(1);
        load(3) -= m*Raccel2(0);
        load(4) -= m*Raccel2(1);

    return 0;
}

const XC::Vector &XC::DispBeamColumn2d::getResistingForce(void) const
{
  const size_t numSections= getNumSections();
  const XC::Matrix &pts = quadRule.getIntegrPointCoords(numSections);
  const XC::Vector &wts = quadRule.getIntegrPointWeights(numSections);

  // Zero for integration
  q.Zero();

  // Loop over the integration points
  for(size_t i= 0; i < numSections; i++) {

    int order = theSections[i]->getOrder();
    const XC::ID &code = theSections[i]->getType();

    double xi6 = 6.0*pts(i,0);

    // Get section stress resultant
    const XC::Vector &s = theSections[i]->getStressResultant();

    // Perform numerical integration on internal force
    //q.addMatrixTransposeVector(1.0, *B, s, wts(i));

    double si;
    for(int j = 0; j < order; j++) {
      si = s(j)*wts(i);
      switch(code(j)) {
      case SECTION_RESPONSE_P:
        q(0) += si; break;
      case SECTION_RESPONSE_MZ:
        q(1) += (xi6-4.0)*si; q(2) += (xi6-2.0)*si; break;
      default:
        break;
      }
    }

  }

  // Add effects of element loads, q = q(v) + q0
  q(0) += q0[0];
  q(1) += q0[1];
  q(2) += q0[2];

  // Vector for reactions in basic system
  Vector p0Vec= p0.getVector();

  P = theCoordTransf->getGlobalResistingForce(q, p0Vec);

  // Subtract other external nodal loads ... P_res = P_int - P_ext
  //P.addVector(1.0, load, -1.0);
  P(0) -= load(0);
  P(1) -= load(1);
  P(2) -= load(2);
  P(3) -= load(3);
  P(4) -= load(4);
  P(5) -= load(5);

    if(isDead())
      P*=dead_srf;
    return P;
  }

const XC::Vector &XC::DispBeamColumn2d::getResistingForceIncInertia(void) const
{

  this->getResistingForce();

  if(rho != 0.0) {
    const XC::Vector &accel1 = theNodes[0]->getTrialAccel();
    const XC::Vector &accel2 = theNodes[1]->getTrialAccel();

    // Compute the current resisting force
    this->getResistingForce();

    double L = theCoordTransf->getInitialLength();
    double m = 0.5*rho*L;

    P(0) += m*accel1(0);
    P(1) += m*accel1(1);
    P(3) += m*accel2(0);
    P(4) += m*accel2(1);

    // add the damping forces if rayleigh damping
    if(!rayFactors.Nulos())
      P += this->getRayleighDampingForces();

  } else {

    // add the damping forces if rayleigh damping
    if(!rayFactors.KNulos())
      P += this->getRayleighDampingForces();
  }
    if(isDead())
      P*=dead_srf; //XXX Se aplica 2 veces sobre getResistingForce: arreglar.
    return P;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::DispBeamColumn2d::sendData(CommParameters &cp)
  {
    int res= BeamColumnWithSectionFDTrf2d::sendData(cp);
    res+= cp.sendVector(q,getDbTagData(),CommMetaData(13));
    res+= p0.sendData(cp,getDbTagData(),CommMetaData(14));
    res+= q0.sendData(cp,getDbTagData(),CommMetaData(15));
    res+= cp.sendDouble(rho,getDbTagData(),CommMetaData(16));
    res+= cp.sendInt(parameterID,getDbTagData(),CommMetaData(17));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::DispBeamColumn2d::recvData(const CommParameters &cp)
  {
    int res= BeamColumnWithSectionFDTrf2d::recvData(cp);
    res+= cp.receiveVector(q,getDbTagData(),CommMetaData(13));
    res+= p0.receiveData(cp,getDbTagData(),CommMetaData(14));
    res+= q0.receiveData(cp,getDbTagData(),CommMetaData(15));
    res+= cp.receiveDouble(rho,getDbTagData(),CommMetaData(16));
    res+= cp.receiveInt(parameterID,getDbTagData(),CommMetaData(17));
    return res;
  }

int XC::DispBeamColumn2d::sendSelf(CommParameters &cp)
  {
    inicComm(18);
    int res= sendData(cp);

    const int dataTag= getDbTag(cp);
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "NLBeamColumn2d::sendSelf -- failed to send ID data\n";
    return res;
  }

int XC::DispBeamColumn2d::recvSelf(const CommParameters &cp)
  {
    inicComm(18);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "NLBeamColumn2d::recvSelf() - failed to recv ID data";
    else
      res+= recvData(cp);
    return res;
  }

void XC::DispBeamColumn2d::Print(std::ostream &s, int flag)
{
  s << "\nDispBeamColumn2d, element id:  " << this->getTag() << std::endl;
  s << "\tConnected external nodes:  " << theNodes;
  s << "\tCoordTransf: " << theCoordTransf->getTag() << std::endl;
  s << "\tmass density:  " << rho << std::endl;

  double L = theCoordTransf->getInitialLength();
  double P  = q(0);
  double M1 = q(1);
  double M2 = q(2);
  double V = (M1+M2)/L;
  s << "\tEnd 1 Forces (P V M): " << -P+p0[0]
    << " " << V+p0[1] << " " << M1 << std::endl;
  s << "\tEnd 2 Forces (P V M): " << P
    << " " << -V+p0[2] << " " << M2 << std::endl;

  //  for(size_t i = 0; i < numSections; i++)
  // theSections[i]->Print(s,flag);
}


XC::Response *XC::DispBeamColumn2d::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
  {
    const int argc= argv.size();
    const size_t numSections= getNumSections();
    // global force -
    if(argv[0] == "forces" || argv[0] == "force"
                || argv[0] == "globalForce" || argv[0] == "globalForces")
                return new XC::ElementResponse(this, 1, P);

    // local force -
    else if(argv[0] == "localForce" || argv[0] == "localForces")
                return new XC::ElementResponse(this, 2, P);

    // chord rotation -
    else if(argv[0] == "chordRotation" || argv[0] == "chordDeformation"
             || argv[0] == "basicDeformation")
      return new XC::ElementResponse(this, 3, Vector(3));

    // plastic rotation -
    else if(argv[0] == "plasticRotation" || argv[0] == "plasticDeformation")
      return new XC::ElementResponse(this, 4, Vector(3));

    // section response -
    else if(argv[0] == "section" || argv[0] == "-section") {
      if(argc <= 2)
        return 0;

      size_t sectionNum = atoi(argv[1]);
      if(sectionNum > 0 && sectionNum <= numSections)
        {
          std::vector<std::string> argv2(argv);
          argv2.erase(argv2.begin(),argv2.begin()+2);
          return theSections[sectionNum-1]->setResponse(argv2, eleInfo);
        }
      else
        return 0;
    }

    else
      return 0;
}

int XC::DispBeamColumn2d::getResponse(int responseID, Information &eleInfo)
{
  double V;
  double L = theCoordTransf->getInitialLength();

  if(responseID == 1)
    return eleInfo.setVector(this->getResistingForce());

  else if(responseID == 2) {
      P(3) =  q(0);
      P(0) = -q(0)+p0[0];
      P(2) = q(1);
      P(5) = q(2);
      V = (q(1)+q(2))/L;
      P(1) =  V+p0[1];
      P(4) = -V+p0[2];
      return eleInfo.setVector(P);
  }

  // Chord rotation
  else if(responseID == 3)
    return eleInfo.setVector(theCoordTransf->getBasicTrialDisp());

  // Plastic rotation
  else if(responseID == 4) {
    static XC::Vector vp(3);
    static XC::Vector ve(3);
    const XC::Matrix &kb = this->getInitialBasicStiff();
    kb.Solve(q, ve);
    vp = theCoordTransf->getBasicTrialDisp();
    vp -= ve;
    return eleInfo.setVector(vp);
  }

  else
    return -1;
}


// AddingSensitivity:BEGIN ///////////////////////////////////
int XC::DispBeamColumn2d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    //
    // From the parameterID value it should be possible to extract
    // information about:
    //  1) Which parameter is in question. The parameter could
    //     be at element, section, or material level.
    //  2) Which section and material number (tag) it belongs to.
    //
    // To accomplish this the parameterID is given the following value:
    //     parameterID = type + 1000*matrTag + 100000*sectionTag
    // ...where 'type' is an integer in the range (1-99) and added 100
    // for each level (from material to section to element).
    //
    // Example:
    //    If 'E0' (case 2) is random in material #3 of section #5
    //    the value of the parameterID at this (element) level would be:
    //    parameterID = 2 + 1000*3 + 100000*5 = 503002
    //    As seen, all given information can be extracted from this number.
    //

    // Initial declarations
    int ok = -1;
    const size_t numSections= getNumSections();
    const size_t argc= argv.size();

        // If the parameter belongs to the element itself
        if(argv[0] == "rho")
          return param.addObject(1, this);
        // If the parameter is belonging to a section or lower
        else if(argv[0] == "section" || argv[0] == "-section")
          {

                // For now, no parameters of the section itself:
                if(argc<5) {
                        std::cerr << "For now: cannot handle parameters of the section itself." << std::endl;
                        return -1;
                }

                // Get section and material tag numbers from user input
                int paramSectionTag = atoi(argv[1]);

                // Find the right section and call its setParameter method
                for(size_t i=0; i<numSections; i++)
                  {
                    if(paramSectionTag == theSections[i]->getTag())
                      {
                        std::vector<std::string> argv2(argv);
                        argv2.erase(argv2.begin(),argv2.begin()+2);
                        ok = theSections[i]->setParameter(argv2, param);
                      }
                  }

                // Check if the ok is valid
                if(ok < 0) {
                        std::cerr << "XC::DispBeamColumn2d::setParameter() - could not set parameter. " << std::endl;
                        return -1;
                }
                else {
                        // Return the ok value (according to the above comments)
                        return ok;
                }
        }

        // Otherwise parameter is unknown for this class
        else {
                return -1;
        }
}

int
XC::DispBeamColumn2d::updateParameter (int parameterID, Information &info)
{
        // If the parameterID value is not equal to 1 it belongs
        // to section or material further down in the hierarchy.

  const size_t numSections= getNumSections();
        if(parameterID == 1) {

                this->rho = info.theDouble;
                return 0;

        }
        else if(parameterID > 0 ) {

                // Extract the section number
                int sectionNumber = (int)( floor((double)parameterID) / (100000) );

                int ok = -1;
                for(size_t i=0; i<numSections; i++) {
                        if(sectionNumber == theSections[i]->getTag()) {
                                ok = theSections[i]->updateParameter(parameterID, info);
                        }
                }

                if(ok < 0) {
                        std::cerr << "XC::DispBeamColumn2d::updateParameter() - could not update parameter. " << std::endl;
                        return ok;
                }
                else {
                        return ok;
                }
        }
        else {
                std::cerr << "XC::DispBeamColumn2d::updateParameter() - could not update parameter. " << std::endl;
                return -1;
        }
}




int
XC::DispBeamColumn2d::activateParameter(int passedParameterID)
{
        // Note that the parameteID that is stored here at the
        // element level contains all information about section
        // and material tag number:
        parameterID = passedParameterID;

  const size_t numSections= getNumSections();
        if(passedParameterID == 0 ) {

          // "Zero out" all flags downwards through sections/materials
          for(size_t i=0; i<numSections; i++) {
            theSections[i]->activateParameter(passedParameterID);
          }
        }

        else if(passedParameterID == 1) {
          // Don't treat the 'rho' for now
        }

        else {

          // Extract section and material tags from the passedParameterID
          int activeSectionTag = (int)( floor((double)passedParameterID) / (100000) );

          // Go down to the sections and set appropriate flags
          for(size_t i=0; i<numSections; i++) {
            if(activeSectionTag == theSections[i]->getTag()) {
              theSections[i]->activateParameter(passedParameterID);
            }
          }
        }

        return 0;
}



const XC::Matrix &
XC::DispBeamColumn2d::getKiSensitivity(int gradNumber)
{
        K.Zero();
        return K;
}

const XC::Matrix &
XC::DispBeamColumn2d::getMassSensitivity(int gradNumber)
{
        K.Zero();
        return K;
}



const XC::Vector &
XC::DispBeamColumn2d::getResistingForceSensitivity(int gradNumber)
{
  const size_t numSections= getNumSections();
        const XC::Matrix &pts = quadRule.getIntegrPointCoords(numSections);
        const XC::Vector &wts = quadRule.getIntegrPointWeights(numSections);

        double L = theCoordTransf->getInitialLength();
        double oneOverL = 1.0/L;

        // Zero for integration
        q.Zero();
        static XC::Vector qsens(3);
        qsens.Zero();

        // Some extra declarations
        static XC::Matrix kbmine(3,3);
        kbmine.Zero();

        int j, k;
        double d1oLdh = 0.0;

        // Check if a nodal coordinate is random
        bool randomNodeCoordinate = false;
        static XC::ID nodeParameterID(2);
        nodeParameterID(0) = theNodes[0]->getCrdsSensitivity();
        nodeParameterID(1) = theNodes[1]->getCrdsSensitivity();
        if(nodeParameterID(0) != 0 || nodeParameterID(1) != 0) {

                randomNodeCoordinate = true;

                const XC::Vector &ndICoords = theNodes[0]->getCrds();
                const XC::Vector &ndJCoords = theNodes[1]->getCrds();

                double dx = ndJCoords(0) - ndICoords(0);
                double dy = ndJCoords(1) - ndICoords(1);

                if(nodeParameterID(0) == 1) // here x1 is random
                  d1oLdh = dx/(L*L*L);
                if(nodeParameterID(0) == 2) // here y1 is random
                  d1oLdh = dy/(L*L*L);

                if(nodeParameterID(1) == 1) // here x2 is random
                  d1oLdh = -dx/(L*L*L);
                if(nodeParameterID(1) == 2) // here y2 is random
                  d1oLdh = -dy/(L*L*L);
        }

        // Loop over the integration points
        for(size_t i= 0; i < numSections; i++) {

          int order = theSections[i]->getOrder();
          const XC::ID &code = theSections[i]->getType();

                double xi6 = 6.0*pts(i,0);
                double wti = wts(i);

                // Get section stress resultant gradient
                const XC::Vector &s = theSections[i]->getStressResultant();
                const XC::Vector &sens = theSections[i]->getStressResultantSensitivity(gradNumber,true);

                // Perform numerical integration on internal force gradient
                //q.addMatrixTransposeVector(1.0, *B, s, wts(i));

                double si;
                double sensi;
                for(j = 0; j < order; j++) {
                        si = s(j)*wti;
                        sensi = sens(j)*wti;
                        switch(code(j)) {
                        case SECTION_RESPONSE_P:
                                q(0) += si;
                                qsens(0) += sensi;
                                break;
                        case SECTION_RESPONSE_MZ:
                                q(1) += (xi6-4.0)*si;
                                q(2) += (xi6-2.0)*si;
                                qsens(1) += (xi6-4.0)*sensi;
                                qsens(2) += (xi6-2.0)*sensi;
                                break;
                        default:
                                break;
                        }
                }

                if(randomNodeCoordinate) {


                        // Perform numerical integration to obtain basic stiffness matrix
                        //kb.addMatrixTripleProduct(1.0, *B, ks, wts(i)/L);
                        double tmp;

                        const XC::Matrix &ks = theSections[i]->getSectionTangent();
                        Matrix ka(workArea, order, 3);
                        ka.Zero();

                        for(j = 0; j < order; j++) {
                                switch(code(j)) {
                                case SECTION_RESPONSE_P:
                                        for(k = 0; k < order; k++) {
                                                ka(k,0) += ks(k,j)*wti;
                                        }
                                        break;
                                case SECTION_RESPONSE_MZ:
                                        for(k = 0; k < order; k++) {
                                                tmp = ks(k,j)*wti;
                                                ka(k,1) += (xi6-4.0)*tmp;
                                                ka(k,2) += (xi6-2.0)*tmp;
                                        }
                                        break;
                                default:
                                        break;
                                }
                        }
                        for(j = 0; j < order; j++) {
                                switch (code(j)) {
                                case SECTION_RESPONSE_P:
                                        for(k = 0; k < 3; k++) {
                                                kbmine(0,k) += ka(j,k);
                                        }
                                        break;
                                case SECTION_RESPONSE_MZ:
                                        for(k = 0; k < 3; k++) {
                                                tmp = ka(j,k);
                                                kbmine(1,k) += (xi6-4.0)*tmp;
                                                kbmine(2,k) += (xi6-2.0)*tmp;
                                        }
                                        break;
                                default:
                                        break;
                                }
                        }
                }

        }

        static XC::Vector dqdh(3);
        const XC::Vector &dAdh_u = theCoordTransf->getBasicTrialDispShapeSensitivity();
        //dqdh = (1.0/L) * (kbmine * dAdh_u);
        dqdh.addMatrixVector(0.0, kbmine, dAdh_u, oneOverL);

        static XC::Vector dkbdh_v(3);
        const XC::Vector &A_u = theCoordTransf->getBasicTrialDisp();
        //dkbdh_v = (d1oLdh) * (kbmine * A_u);
        dkbdh_v.addMatrixVector(0.0, kbmine, A_u, d1oLdh);

        // Transform forces
        static XC::Vector dummy(3);                // No distributed loads

        // Term 5
        P = theCoordTransf->getGlobalResistingForce(qsens,dummy);

        if(randomNodeCoordinate) {
                // Term 1
                P += theCoordTransf->getGlobalResistingForceShapeSensitivity(q,dummy);

                // Term 2
                P += theCoordTransf->getGlobalResistingForce(dqdh,dummy);

                // Term 4
                P += theCoordTransf->getGlobalResistingForce(dkbdh_v,dummy);
        }

        return P;
}



// NEW METHOD
int XC::DispBeamColumn2d::commitSensitivity(int gradNumber, int numGrads)
{
    // Get basic deformation and sensitivities
        const XC::Vector &v = theCoordTransf->getBasicTrialDisp();

        static XC::Vector vsens(3);
        vsens = theCoordTransf->getBasicDisplSensitivity(gradNumber);

        double L = theCoordTransf->getInitialLength();
        double oneOverL = 1.0/L;
  const size_t numSections= getNumSections();
        const XC::Matrix &pts = quadRule.getIntegrPointCoords(numSections);

        // Some extra declarations
        double d1oLdh = 0.0;

        // Check if a nodal coordinate is random
        bool randomNodeCoordinate = false;
        static XC::ID nodeParameterID(2);
        nodeParameterID(0) = theNodes[0]->getCrdsSensitivity();
        nodeParameterID(1) = theNodes[1]->getCrdsSensitivity();
        if(nodeParameterID(0) != 0 || nodeParameterID(1) != 0) {

                vsens += theCoordTransf->getBasicTrialDispShapeSensitivity();

                randomNodeCoordinate = true;

                const XC::Vector &ndICoords = theNodes[0]->getCrds();
                const XC::Vector &ndJCoords = theNodes[1]->getCrds();

                double dx = ndJCoords(0) - ndICoords(0);
                double dy = ndJCoords(1) - ndICoords(1);

                if(nodeParameterID(0) == 1) // here x1 is random
                  d1oLdh = dx/(L*L*L);
                if(nodeParameterID(0) == 2) // here y1 is random
                  d1oLdh = dy/(L*L*L);

                if(nodeParameterID(1) == 1) // here x2 is random
                  d1oLdh = -dx/(L*L*L);
                if(nodeParameterID(1) == 2) // here y2 is random
                  d1oLdh = -dy/(L*L*L);
        }

        // Loop over the integration points
        for(size_t i = 0; i < numSections; i++) {

                int order = theSections[i]->getOrder();
                const XC::ID &code = theSections[i]->getType();

                Vector e(workArea, order);

                double xi6 = 6.0*pts(i,0);

                for(int j = 0; j < order; j++) {
                        switch(code(j)) {
                        case SECTION_RESPONSE_P:
                                e(j) = oneOverL*vsens(0)
                                + d1oLdh*v(0);
                                break;
                        case SECTION_RESPONSE_MZ:
                                e(j) = oneOverL*((xi6-4.0)*vsens(1) + (xi6-2.0)*vsens(2))
                                + d1oLdh*((xi6-4.0)*v(1) + (xi6-2.0)*v(2));
                                break;
                        default:
                                e(j) = 0.0;
                                break;
                        }
                }

                // Set the section deformations
                theSections[i]->commitSensitivity(e,gradNumber,numGrads);
        }

        return 0;
}


// AddingSensitivity:END /////////////////////////////////////////////

