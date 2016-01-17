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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.19 $
// $Date: 2004/10/30 00:05:32 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/dispBeamColumn/DispBeamColumn3d.cpp,v $

// Written: MHS
// Created: Feb 2001
//
// Description: This file contains the class definition for XC::DispBeamColumn3d.

#include <domain/mesh/element/truss_beam_column/dispBeamColumn/DispBeamColumn3d.h>
#include <domain/mesh/node/Node.h>
#include <material/section/SeccionBarraPrismatica.h>
#include <domain/mesh/element/coordTransformation/CrdTransf3d.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/element/Information.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/recorder/response/ElementResponse.h>
#include "domain/load/beam_loads/BeamMecLoad.h"
#include "material/section/ResponseId.h"
#include "utility/actor/actor/MovableVector.h"

XC::Matrix XC::DispBeamColumn3d::K(12,12);
XC::Vector XC::DispBeamColumn3d::P(12);
double XC::DispBeamColumn3d::workArea[200];
XC::GaussQuadRule1d01 XC::DispBeamColumn3d::quadRule;

XC::DispBeamColumn3d::DispBeamColumn3d(int tag, int nd1, int nd2,
				       int numSec,const std::vector<SeccionBarraPrismatica *> &s,
                                   CrdTransf3d &coordTransf, double r)
  :BeamColumnWithSectionFDTrf3d(tag, ELE_TAG_DispBeamColumn3d,numSec),
   q(6), q0(), p0(), rho(r)
  {
    load.reset(12);
    setSections(s);
    set_transf(&coordTransf);

    // Set connected external node IDs
    theNodes.set_id_nodos(nd1,nd2);
  }

XC::DispBeamColumn3d::DispBeamColumn3d(int tag,int numSec,const Material *m,const CrdTransf *trf)
  :BeamColumnWithSectionFDTrf3d(tag, ELE_TAG_DispBeamColumn3d,numSec,m,trf),
   q(6), q0(), p0(), rho(0.0) {load.reset(12);}

XC::DispBeamColumn3d::DispBeamColumn3d(int tag)
  :BeamColumnWithSectionFDTrf3d(tag, ELE_TAG_DispBeamColumn3d,0),
   q(6), q0(), p0(), rho(0.0) {load.reset(12);}

//! @brief Constructor virtual.
XC::Element* XC::DispBeamColumn3d::getCopy(void) const
  { return new XC::DispBeamColumn3d(*this); }

int XC::DispBeamColumn3d::getNumDOF(void) const
  { return 12; }

void XC::DispBeamColumn3d::setDomain(Domain *theDomain)
  {
    BeamColumnWithSectionFDTrf3d::setDomain(theDomain);
        // Check XC::Domain is not null - invoked when object removed from a domain
    if(theDomain)
      {
        int dofNd1 = theNodes[0]->getNumberDOF();
        int dofNd2 = theNodes[1]->getNumberDOF();

        if(dofNd1 != 6 || dofNd2 != 6)
          {
            //std::cerr << "FATAL ERROR XC::DispBeamColumn3d (tag: %d), has differing number of DOFs at its nodes",
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

int XC::DispBeamColumn3d::commitState(void)
  {
    int retVal = 0;

    // call element commitState to do any base class stuff
    if((retVal = this->XC::BeamColumnWithSectionFDTrf3d::commitState()) != 0)
      { std::cerr << "XC::DispBeamColumn3d::commitState () - failed in base class"; }

    retVal += theCoordTransf->commitState();
    return retVal;
  }

int XC::DispBeamColumn3d::revertToLastCommit(void)
  {
    int retVal = 0;

    // Loop over the integration points and revert to last committed state
  const size_t numSections= getNumSections();
    for(size_t i= 0;i<numSections;i++)
      retVal += theSections[i]->revertToLastCommit();
    retVal += theCoordTransf->revertToLastCommit();
    return retVal;
  }

int XC::DispBeamColumn3d::revertToStart(void)
  {
    int retVal = 0;

    // Loop over the integration points and revert states to start
    const size_t numSections= getNumSections();
    for(size_t i= 0;i<numSections; i++)
      retVal += theSections[i]->revertToStart();
    retVal += theCoordTransf->revertToStart();
    return retVal;
  }

int XC::DispBeamColumn3d::update(void)
  {
    // Update the transformation
    theCoordTransf->update();

    // Get basic deformations
    const XC::Vector &v = theCoordTransf->getBasicTrialDisp();

    const double L = theCoordTransf->getInitialLength();
    const double oneOverL = 1.0/L;
    const size_t numSections= getNumSections();
    const XC::Matrix &pts = quadRule.getIntegrPointCoords(numSections);

    // Loop over the integration points
    for(size_t i = 0; i < numSections; i++)
      {
        int order= theSections[i]->getOrder();
        const XC::ID &code = theSections[i]->getType();

        Vector e(workArea, order);

        double xi6 = 6.0*pts(i,0);

        int j;
        for(j = 0; j < order; j++)
          {
            switch(code(j))
              {
              case SECTION_RESPONSE_P:
                e(j) = oneOverL*v(0);
                break;
              case SECTION_RESPONSE_MZ:
                e(j) = oneOverL*((xi6-4.0)*v(1) + (xi6-2.0)*v(2));
                break;
              case SECTION_RESPONSE_MY:
                e(j) = oneOverL*((xi6-4.0)*v(3) + (xi6-2.0)*v(4));
                break;
              case SECTION_RESPONSE_T:
                e(j) = oneOverL*v(5);
                break;
              default:
                e(j) = 0.0;
                break;
              }
          }
        // Set the section deformations
        theSections[i]->setTrialSectionDeformation(e);
      }
    return 0;
  }

const XC::Matrix &XC::DispBeamColumn3d::getTangentStiff(void) const
  {
    static Matrix kb(6,6);

    // Zero for integral
    kb.Zero();
    q.Zero();

    const size_t numSections= getNumSections();
    const XC::Matrix &pts= quadRule.getIntegrPointCoords(numSections);
    const XC::Vector &wts= quadRule.getIntegrPointWeights(numSections);

    const double L = theCoordTransf->getInitialLength();
    const double oneOverL = 1.0/L;

    // Loop over the integration points
    for(size_t i = 0; i < numSections; i++)
      {
        int order = theSections[i]->getOrder();
        const XC::ID &code = theSections[i]->getType();

        Matrix ka(workArea, order, 6);
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
        for(j = 0; j < order; j++)
          {
            switch(code(j))
              {
              case SECTION_RESPONSE_P:
                for(k = 0; k < order; k++)
                  ka(k,0) += ks(k,j)*wti;
                  break;
              case SECTION_RESPONSE_MZ:
                for(k = 0; k < order; k++)
                  {
                    tmp = ks(k,j)*wti;
                    ka(k,1) += (xi6-4.0)*tmp;
                    ka(k,2) += (xi6-2.0)*tmp;
                  }
                break;
              case SECTION_RESPONSE_MY:
                for(k = 0; k < order; k++)
                  {
                    tmp = ks(k,j)*wti;
                    ka(k,3) += (xi6-4.0)*tmp;
                    ka(k,4) += (xi6-2.0)*tmp;
                  }
                break;
              case SECTION_RESPONSE_T:
                for(k = 0; k < order; k++)
                  ka(k,5) += ks(k,j)*wti;
                break;
              default:
                break;
              }
          }
    for(j = 0; j < order; j++) {
      switch (code(j)) {
      case SECTION_RESPONSE_P:
        for(k = 0; k < 6; k++)
          kb(0,k) += ka(j,k);
        break;
      case SECTION_RESPONSE_MZ:
        for(k = 0; k < 6; k++) {
          tmp = ka(j,k);
          kb(1,k) += (xi6-4.0)*tmp;
          kb(2,k) += (xi6-2.0)*tmp;
        }
        break;
      case SECTION_RESPONSE_MY:
        for(k = 0; k < 6; k++) {
          tmp = ka(j,k);
          kb(3,k) += (xi6-4.0)*tmp;
          kb(4,k) += (xi6-2.0)*tmp;
        }
        break;
      case SECTION_RESPONSE_T:
        for(k = 0; k < 6; k++)
          kb(5,k) += ka(j,k);
        break;
      default:
        break;
      }
    }

    //q.addMatrixTransposeVector(1.0, *B, s, wts(i));
    double si;
    for(j = 0; j < order; j++) {
      si = s(j)*wts(i);
      switch(code(j)) {
      case SECTION_RESPONSE_P:
        q(0) += si;
        break;
      case SECTION_RESPONSE_MZ:
        q(1) += (xi6-4.0)*si; q(2) += (xi6-2.0)*si;
        break;
      case SECTION_RESPONSE_MY:
        q(3) += (xi6-4.0)*si; q(4) += (xi6-2.0)*si;
        break;
      case SECTION_RESPONSE_T:
        q(5) += si;
        break;
      default:
        break;
      }
    }

  }

  q(0)+= q0[0];
  q(1)+= q0[1];
  q(2)+= q0[2];
  q(3)+= q0[3];
  q(4)+= q0[4];

  // Transform to global stiffness
  K = theCoordTransf->getGlobalStiffMatrix(kb, q);
    if(isDead())
      K*=dead_srf;
    return K;
  }

const XC::Matrix &XC::DispBeamColumn3d::getInitialBasicStiff(void) const
{
  static XC::Matrix kb(6,6);

  // Zero for integral
  kb.Zero();

  const size_t numSections= getNumSections();
  const XC::Matrix &pts = quadRule.getIntegrPointCoords(numSections);
  const XC::Vector &wts = quadRule.getIntegrPointWeights(numSections);

  double L = theCoordTransf->getInitialLength();
  double oneOverL = 1.0/L;

  // Loop over the integration points
  for(size_t i = 0; i < numSections; i++) {

    int order = theSections[i]->getOrder();
    const XC::ID &code = theSections[i]->getType();

    Matrix ka(workArea, order, 6);
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
      case SECTION_RESPONSE_MY:
        for(k = 0; k < order; k++) {
          tmp = ks(k,j)*wti;
          ka(k,3) += (xi6-4.0)*tmp;
          ka(k,4) += (xi6-2.0)*tmp;
        }
        break;
      case SECTION_RESPONSE_T:
        for(k = 0; k < order; k++)
          ka(k,5) += ks(k,j)*wti;
        break;
      default:
        break;
      }
    }
    for(j = 0; j < order; j++) {
      switch (code(j)) {
      case SECTION_RESPONSE_P:
        for(k = 0; k < 6; k++)
          kb(0,k) += ka(j,k);
        break;
      case SECTION_RESPONSE_MZ:
        for(k = 0; k < 6; k++) {
          tmp = ka(j,k);
          kb(1,k) += (xi6-4.0)*tmp;
          kb(2,k) += (xi6-2.0)*tmp;
        }
        break;
      case SECTION_RESPONSE_MY:
        for(k = 0; k < 6; k++) {
          tmp = ka(j,k);
          kb(3,k) += (xi6-4.0)*tmp;
          kb(4,k) += (xi6-2.0)*tmp;
        }
        break;
      case SECTION_RESPONSE_T:
        for(k = 0; k < 6; k++)
          kb(5,k) += ka(j,k);
        break;
      default:
        break;
      }
    }

  }

  return kb;
}

const XC::Matrix &XC::DispBeamColumn3d::getInitialStiff(void) const
  {
    const XC::Matrix &kb = this->getInitialBasicStiff();
    // Transform to global stiffness
    K = theCoordTransf->getInitialGlobalStiffMatrix(kb);
    if(isDead())
      K*=dead_srf;
    return K;
  }

const XC::Matrix &XC::DispBeamColumn3d::getMass(void) const
{
  K.Zero();

  if(rho == 0.0)
    return K;

  double L = theCoordTransf->getInitialLength();
  double m = 0.5*rho*L;

  K(0,0) = K(1,1) = K(2,2) = K(6,6) = K(7,7) = K(8,8) = m;

    if(isDead())
      K*=dead_srf;
    return K;
  }

void XC::DispBeamColumn3d::zeroLoad(void)
  {
    BeamColumnWithSectionFDTrf3d::zeroLoad();
    q0.zero();
    p0.zero();
    return;
  }

int XC::DispBeamColumn3d::addLoad(ElementalLoad *theLoad, double loadFactor)
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
            std::cerr << "XC::DispBeamColumn2d::addLoad() -- load type unknown for element with tag: " <<
              this->getTag() << std::endl;
            return -1;
          }
      }
    return 0;
  }

int XC::DispBeamColumn3d::addInertiaLoadToUnbalance(const XC::Vector &accel)
  {
    // Check for a XC::quick return
    if(rho == 0.0)
      return 0;

  // Get R * accel from the nodes
  const XC::Vector &Raccel1 = theNodes[0]->getRV(accel);
  const XC::Vector &Raccel2 = theNodes[1]->getRV(accel);

  if(6 != Raccel1.Size() || 6 != Raccel2.Size()) {
    std::cerr << "XC::DispBeamColumn3d::addInertiaLoadToUnbalance matrix and vector sizes are incompatable\n";
    return -1;
  }

  const double L = theCoordTransf->getInitialLength();
  const double m = 0.5*rho*L;

  // Want to add ( - fact * M R * accel ) to unbalance
  // Take advantage of lumped mass matrix
  load(0) -= m*Raccel1(0);
  load(1) -= m*Raccel1(1);
  load(2) -= m*Raccel1(2);
  load(6) -= m*Raccel2(0);
  load(7) -= m*Raccel2(1);
  load(8) -= m*Raccel2(2);

  return 0;
}

const XC::Vector &XC::DispBeamColumn3d::getResistingForce(void) const
{
  const size_t numSections= getNumSections();
  const XC::Matrix &pts = quadRule.getIntegrPointCoords(numSections);
  const XC::Vector &wts = quadRule.getIntegrPointWeights(numSections);

  // Zero for integration
  q.Zero();

  // Loop over the integration points
  for(size_t i = 0; i < numSections; i++) {

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
        q(0) += si;
        break;
      case SECTION_RESPONSE_MZ:
        q(1) += (xi6-4.0)*si; q(2) += (xi6-2.0)*si;
        break;
      case SECTION_RESPONSE_MY:
        q(3) += (xi6-4.0)*si; q(4) += (xi6-2.0)*si;
        break;
      case SECTION_RESPONSE_T:
        q(5) += si;
        break;
      default:
        break;
      }
    }

  }

  q(0) += q0[0];
  q(1) += q0[1];
  q(2) += q0[2];
  q(3) += q0[3];
  q(4) += q0[4];

  // Transform forces
  Vector p0Vec= p0.getVector();
  P = theCoordTransf->getGlobalResistingForce(q, p0Vec);

  // Subtract other external nodal loads ... P_res = P_int - P_ext
  P.addVector(1.0, load, -1.0);
    if(isDead())
      P*=dead_srf;
    return P;
  }

const XC::Vector &XC::DispBeamColumn3d::getResistingForceIncInertia(void) const
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
    P(2) += m*accel1(2);
    P(6) += m*accel2(0);
    P(7) += m*accel2(1);
    P(8) += m*accel2(2);

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

//! @brief Envía los miembros por el canal que se pasa como parámetro.
int XC::DispBeamColumn3d::sendData(CommParameters &cp)
  {
    int res= BeamColumnWithSectionFDTrf3d::sendData(cp);
    res+= cp.sendVector(q,getDbTagData(),CommMetaData(13));
    res+= p0.sendData(cp,getDbTagData(),CommMetaData(14));
    res+= q0.sendData(cp,getDbTagData(),CommMetaData(15));
    res+= cp.sendDouble(rho,getDbTagData(),CommMetaData(16));
    return res;
  }

//! @brief Recibe los miembros por el canal que se pasa como parámetro.
int XC::DispBeamColumn3d::recvData(const CommParameters &cp)
  {
    int res= BeamColumnWithSectionFDTrf3d::recvData(cp);
    res+= cp.receiveVector(q,getDbTagData(),CommMetaData(13));
    res+= p0.receiveData(cp,getDbTagData(),CommMetaData(14));
    res+= q0.receiveData(cp,getDbTagData(),CommMetaData(15));
    res+= cp.receiveDouble(rho,getDbTagData(),CommMetaData(16));
    return res;
  }

int XC::DispBeamColumn3d::sendSelf(CommParameters &cp)
  {
    inicComm(17);
    int res= sendData(cp);

    const int dataTag= getDbTag(cp);
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "NLBeamColumn3d::sendSelf -- failed to send ID data\n";
    return res;
  }

int XC::DispBeamColumn3d::recvSelf(const CommParameters &cp)
  {
    inicComm(17);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "NLBeamColumn3d::recvSelf() - failed to recv ID data";
    else
      res+= recvData(cp);
    return res;
  }

void XC::DispBeamColumn3d::Print(std::ostream &s, int flag)
{
  s << "\nDispBeamColumn3d, element id:  " << this->getTag() << std::endl;
  s << "\tConnected external nodes:  " << theNodes;
  s << "\tmass density:  " << rho << std::endl;

  double N, Mz1, Mz2, Vy, My1, My2, Vz, T;
  double L = theCoordTransf->getInitialLength();
  double oneOverL = 1.0/L;

  N   = q(0);
  Mz1 = q(1);
  Mz2 = q(2);
  Vy  = (Mz1+Mz2)*oneOverL;
  My1 = q(3);
  My2 = q(4);
  Vz  = -(My1+My2)*oneOverL;
  T   = q(5);

  s << "\tEnd 1 Forces (P Mz Vy My Vz T): "
    << -N+p0[0] << ' ' << Mz1 << ' ' <<  Vy+p0[1] << ' ' << My1 << ' ' <<  Vz+p0[3] << ' ' << -T << std::endl;
  s << "\tEnd 2 Forces (P Mz Vy My Vz T): "
    <<  N << ' ' << Mz2 << ' ' << -Vy+p0[2] << ' ' << My2 << ' ' << -Vz+p0[4] << ' ' <<  T << std::endl;

  //for(int i = 0; i < numSections; i++)
  //theSections[i]->Print(s,flag);
}


XC::Response *XC::DispBeamColumn3d::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
{
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
      return new XC::ElementResponse(this, 3, Vector(6));

    // plastic rotation -
    else if(argv[0] == "plasticRotation" || argv[0] == "plasticDeformation")
      return new XC::ElementResponse(this, 4, Vector(6));

    // section response -
    else if(argv[0] == "section" || argv[0] == "-section")
      {
        const size_t argc= argv.size();
        if(argc <= 2)
          return 0;
        const size_t sectionNum = atoi(argv[1]);
        if(sectionNum > 0 && sectionNum <= numSections)
          {
            std::vector<std::string> argv2(argv);
            argv2.erase(argv2.begin(),argv2.begin()+2);
            return theSections[sectionNum-1]->setResponse(argv2,eleInfo);
          }
        else
          return 0;
      }
    else
      return 0;
  }

int XC::DispBeamColumn3d::getResponse(int responseID, Information &eleInfo)
{
  double N, V, M1, M2, T;
  double L = theCoordTransf->getInitialLength();
  double oneOverL = 1.0/L;

  if(responseID == 1)
    return eleInfo.setVector(this->getResistingForce());

  else if(responseID == 2) {
    // Axial
    N = q(0);
    P(6) =  N;
    P(0) = -N+p0[0];

    // Torsion
    T = q(5);
    P(9) =  T;
    P(3) = -T;

    // Moments about z and shears along y
    M1 = q(1);
    M2 = q(2);
    P(5)  = M1;
    P(11) = M2;
    V = (M1+M2)*oneOverL;
    P(1) =  V+p0[1];
    P(7) = -V+p0[2];

    // Moments about y and shears along z
    M1 = q(3);
    M2 = q(4);
    P(4)  = M1;
    P(10) = M2;
    V = -(M1+M2)*oneOverL;
    P(2) = -V+p0[3];
    P(8) =  V+p0[4];

    return eleInfo.setVector(P);
  }

  // Chord rotation
  else if(responseID == 3)
    return eleInfo.setVector(theCoordTransf->getBasicTrialDisp());

  // Plastic rotation
  else if(responseID == 4) {
    static XC::Vector vp(6);
    static XC::Vector ve(6);
    const XC::Matrix &kb = this->getInitialBasicStiff();
    kb.Solve(q, ve);
    vp = theCoordTransf->getBasicTrialDisp();
    vp -= ve;
    return eleInfo.setVector(vp);
  }

  else
    return -1;
}
