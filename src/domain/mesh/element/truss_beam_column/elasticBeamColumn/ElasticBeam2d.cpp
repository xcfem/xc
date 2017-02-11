//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
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

// $Revision: 1.17 $
// $Date: 2004/10/30 00:05:32 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/elasticBeamColumn/ElasticBeam2d.cpp,v $


// File: ~/model/ElasticBeam2d.C
//
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the class definition for XC::ElasticBeam2d.
// ElasticBeam2d is a 2d beam element. As such it can only
// connect to a node with 6-dof.

#include <domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticBeam2d.h>
#include "domain/component/Parameter.h"
#include <domain/load/ElementalLoad.h>
#include "domain/load/beam_loads/BeamMecLoad.h"
#include "domain/load/beam_loads/BeamStrainLoad.h"

#include <domain/domain/Domain.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

#include <domain/mesh/element/utils/coordTransformation/CrdTransf2d.h>
#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/ElementResponse.h>
#include <domain/mesh/node/Node.h>

#include "material/section/repres/CrossSectionProperties2d.h"
#include "material/section/ResponseId.h"
#include "utility/actor/actor/MovableVector.h"

XC::Matrix XC::ElasticBeam2d::K(6,6);
XC::Vector XC::ElasticBeam2d::P(6);
XC::Matrix XC::ElasticBeam2d::kb(3,3);

void XC::ElasticBeam2d::set_transf(const CrdTransf *trf)
  {
    if(theCoordTransf)
      {
        delete theCoordTransf;
        theCoordTransf= nullptr;
      }
    if(trf)
      {
        const CrdTransf2d *tmp= dynamic_cast<const CrdTransf2d *>(trf);
        if(tmp)
          theCoordTransf = tmp->getCopy();
        else
          {
            std::cerr << "ElasticBeam2d::ElasticBeam2d -- failed to get copy of coordinate transformation\n";
            exit(-1);
          }
      }
    else
      std::cerr << "ElasticBeam2d::set_transf; se pasó a null pointer." << std::endl;
  }

XC::ElasticBeam2d::ElasticBeam2d(int tag)
  :ProtoBeam2d(tag,ELE_TAG_ElasticBeam2d), eInic(2), alpha(0.0), d(0.0), rho(0.0),
  q(3), theCoordTransf(nullptr)
  {
    load.reset(6);
    q0[0] = 0.0;
    q0[1] = 0.0;
    q0[2] = 0.0;

    p0[0] = 0.0;
    p0[1] = 0.0;
    p0[2] = 0.0;

  }

//! @brief Constructor.
XC::ElasticBeam2d::ElasticBeam2d(int tag,const Material *m,const CrdTransf *trf)
  :ProtoBeam2d(tag,ELE_TAG_ElasticBeam2d,m), eInic(2), alpha(0.0), d(0.0), rho(0.0),
  q(3), theCoordTransf(nullptr)
  {
    load.reset(6);
    q0[0] = 0.0;
    q0[1] = 0.0;
    q0[2] = 0.0;

    p0[0] = 0.0;
    p0[1] = 0.0;
    p0[2] = 0.0;

    set_transf(trf);
  }

//! @brief Constructor.
XC::ElasticBeam2d::ElasticBeam2d(int tag, double a, double e, double i, int Nd1, int Nd2,
                                 CrdTransf2d &coordTransf, double Alpha, double depth,
                                 double r)
  :ProtoBeam2d(tag,ELE_TAG_ElasticBeam2d,a,e,i,Nd1,Nd2), eInic(2), alpha(Alpha), d(depth),
   rho(r),  q(3), theCoordTransf(nullptr)
  {
    load.reset(6);
    set_transf(&coordTransf);

    q0[0] = 0.0;
    q0[1] = 0.0;
    q0[2] = 0.0;

    p0[0] = 0.0;
    p0[1] = 0.0;
    p0[2] = 0.0;

  }

//! @brief Constructor de copia.
XC::ElasticBeam2d::ElasticBeam2d(const ElasticBeam2d &otro)
  :ProtoBeam2d(otro), eInic(otro.eInic), alpha(otro.alpha), d(otro.d), rho(otro.rho),
  q(otro.q), theCoordTransf(nullptr)
  {
    set_transf(otro.theCoordTransf);

    q0[0] = otro.q0[0];
    q0[1] = otro.q0[1];
    q0[2] = otro.q0[2];

    p0[0] = otro.p0[0];
    p0[1] = otro.p0[1];
    p0[2] = otro.p0[2];
  }

//! @brief Operador asignación.
XC::ElasticBeam2d &XC::ElasticBeam2d::operator=(const XC::ElasticBeam2d &otro)
  {
    ProtoBeam2d::operator=(otro);
    eInic= otro.eInic;
    alpha= otro.alpha;
    d= otro.d;
    rho= otro.rho;
    q= otro.q;
    set_transf(otro.theCoordTransf);

    q0[0] = otro.q0[0];
    q0[1] = otro.q0[1];
    q0[2] = otro.q0[2];

    p0[0] = otro.p0[0];
    p0[1] = otro.p0[1];
    p0[2] = otro.p0[2];
    return *this;
  }

//! @brief Constructor virtual.
XC::Element* XC::ElasticBeam2d::getCopy(void) const
  { return new ElasticBeam2d(*this); }


XC::ElasticBeam2d::~ElasticBeam2d(void)
  { if(theCoordTransf) delete theCoordTransf; }

//! @brief Devuelve (si puede) un apuntador a la coordinate transformation.
XC::CrdTransf *XC::ElasticBeam2d::getCoordTransf(void)
  { return theCoordTransf; }

//! @brief Devuelve (si puede) un apuntador a la coordinate transformation.
const XC::CrdTransf *XC::ElasticBeam2d::getCoordTransf(void) const
  { return theCoordTransf; }


void XC::ElasticBeam2d::setDomain(Domain *theDomain)
  {
    ProtoBeam2d::setDomain(theDomain);


    int dofNd1 = theNodes[0]->getNumberDOF();
    int dofNd2 = theNodes[1]->getNumberDOF();
    if(dofNd1 != 3)
      {
        std::cerr << "XC::ElasticBeam2d::setDomain -- Node 1: " << theNodes.getTagNode(0)
               << " has incorrect number of DOF\n";
        exit(-1);
      }

    if(dofNd2 != 3)
      {
        std::cerr << "XC::ElasticBeam2d::setDomain -- Node 2: " << theNodes.getTagNode(1)
               << " has incorrect number of DOF\n";
        exit(-1);
      }
    if(theCoordTransf)
      {
        if(theCoordTransf->initialize(theNodes[0], theNodes[1]) != 0)
          {
            std::cerr << "XC::ElasticBeam2d::setDomain -- Error initializing coordinate transformation\n";
            exit(-1);
          }
        double L= theCoordTransf->getInitialLength();
        if(L == 0.0)
          {
            std::cerr << "XC::ElasticBeam2d::setDomain -- Element has zero length\n";
            exit(-1);
          }
      }
    else
      std::cerr << "XC::ElasticBeam2d::setDomain -- El elemento no tiene coordinate transformation." << std::endl;
   }

int XC::ElasticBeam2d::setInitialSectionDeformation(const Vector &def)
  {
    eInic= def;
    return 0;
  }

const XC::Vector &XC::ElasticBeam2d::getSectionDeformation(void) const
  {
    static Vector retval(3);
    theCoordTransf->update();
    const double L = theCoordTransf->getInitialLength();
    // retval(0)= (dx2-dx1)/L: Elongación sufrida por el elemento/L.
    // retval(1)= (dy1-dy2)/L: Giro en torno a z/L.
    // retval(2)= (dy1-dy2)/L: Giro en torno a z/L.
    retval= theCoordTransf->getBasicTrialDisp()/L;
    retval(0)-= eInic(0);
    retval(1)-= eInic(1);
    retval(2)-= eInic(1);
    return retval;
  }

int XC::ElasticBeam2d::commitState(void)
  {
    int retVal= XC::Element::commitState();
    // call element commitState to do any base class stuff
    if(retVal!=0)
      std::cerr << "XC::ElasticBeam2d::commitState () - failed in base class";
    retVal += theCoordTransf->commitState();
    return retVal;
  }

int XC::ElasticBeam2d::revertToLastCommit()
  { return theCoordTransf->revertToLastCommit(); }

int XC::ElasticBeam2d::revertToStart()
  { return theCoordTransf->revertToStart(); }

int XC::ElasticBeam2d::update(void)
  { return theCoordTransf->update(); }

//! @brief Returns the direction vector of element strong axis
//! expressed in the global coordinate system.
const XC::Vector &XC::ElasticBeam2d::getVDirEjeFuerteGlobales(void) const
  {
    if(theCoordTransf)
      {
        const Vector eF= getVDirEjeFuerteLocales();
        return theCoordTransf->getVectorGlobalCoordFromLocal(eF);
      }
    else
      {
        std::cerr << "ElasticBeam2d::getVDirEjeFuerteGlobales; no se ha asignado una coordinate transformation."
                  << std::endl;
        return P;
      }
  }

//! @brief Returns the direction vector of element weak axis
//! expressed in the global coordinate system.
const XC::Vector &XC::ElasticBeam2d::getVDirEjeDebilGlobales(void) const
  {
    if(theCoordTransf)
      {
        const Vector eD= getVDirEjeDebilLocales();
        return theCoordTransf->getVectorGlobalCoordFromLocal(eD);
      }
    else
      {
        std::cerr << "ElasticBeam2d::getVDirEjeDebilGlobales; no se ha asignado una coordinate transformation."
                  << std::endl;
        return P;
      }
  }


const XC::Matrix &XC::ElasticBeam2d::getTangentStiff(void) const
  {
    const Vector &v= getSectionDeformation();

    const double EA= ctes_scc.EA(); // EA
    const double EI2= 2.0*ctes_scc.EI(); // 2EI
    const double EI4= 2.0*EI2; // 4EI

    // determine q = kv + q0
    q(0) = EA*v(0);
    q(1) = EI4*v(1) + EI2*v(2);
    q(2) = EI2*v(1) + EI4*v(2);

    q(0)+= q0[0];
    q(1)+= q0[1];
    q(2)+= q0[2];

    const double L = theCoordTransf->getInitialLength();
    kb(0,0)= EA/L;
    kb(1,1)= kb(2,2)= EI4/L;
    kb(2,1)= kb(1,2)= EI2/L;

    
    static Matrix retval;
    retval= theCoordTransf->getGlobalStiffMatrix(kb,q);
    if(isDead())
      retval*=dead_srf;
    return retval;
  }

const XC::Matrix &XC::ElasticBeam2d::getInitialStiff(void) const
  {
    const double L = theCoordTransf->getInitialLength();

    const double EoverL   = ctes_scc.E()/L;
    const double EAoverL  = ctes_scc.A()*EoverL; // EA/L
    const double EIoverL2 = 2.0*ctes_scc.I()*EoverL; // 2EI/L
    const double EIoverL4 = 2.0*EIoverL2; // 4EI/L

    kb(0,0) = EAoverL;
    kb(1,1) = kb(2,2) = EIoverL4;
    kb(2,1) = kb(1,2) = EIoverL2;

    static Matrix retval;
    retval= theCoordTransf->getInitialGlobalStiffMatrix(kb);
    if(isDead())
      retval*=dead_srf;
    return retval;
  }

const XC::Matrix &XC::ElasticBeam2d::getMass(void) const
  {
    K.Zero();

    if(rho>0.0)
      {
        const double L = theCoordTransf->getInitialLength();
        const double m = 0.5*rho*L;
        const double mpeq= m*1.0e-10;

        K(0,0)= m;
        K(1,1)= m;
        K(2,2)= mpeq; //Modificado por LCPT para evitar matriz M singular.

        K(3,3)= m;
        K(4,4)= m;
        K(5,5)= mpeq; //Modificado por LCPT para evitar matriz M singular.
      }
    if(isDead())
      K*=dead_srf;
    return K;
  }

void XC::ElasticBeam2d::zeroLoad(void)
  {
    ProtoBeam2d::zeroLoad();

    q0[0] = 0.0;
    q0[1] = 0.0;
    q0[2] = 0.0;

    p0[0] = 0.0;
    p0[1] = 0.0;
    p0[2] = 0.0;

    return;
  }

int XC::ElasticBeam2d::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(isDead())
      std::cerr << nombre_clase() 
                << "; se intentó cargar el elemento "
                << getTag() << " que está desactivado." 
                << std::endl;
    else
      {
        const double L = theCoordTransf->getInitialLength();
        if(BeamMecLoad *beamMecLoad= dynamic_cast<BeamMecLoad *>(theLoad))
          {
            beamMecLoad->addReactionsInBasicSystem(L,loadFactor,p0); // Accumulate reactions in basic system
            beamMecLoad->addFixedEndForcesInBasicSystem(L,loadFactor,q0); // Fixed end forces in basic system
          }
        else if(BeamStrainLoad *strainLoad= dynamic_cast<BeamStrainLoad *>(theLoad)) //Deformaciones impuestas.
          {
            const int order= 2;
            const RespPMz code;
            const Vector &e1= strainLoad->getSection1Deformation(order,code)*loadFactor;
            const Vector &e2= strainLoad->getSection2Deformation(order,code)*loadFactor;
            Vector ezero= getInitialSectionDeformation();
            ezero+= (e2+e1)/2;
            setInitialSectionDeformation(ezero);
          }
        else
          {
            std::cerr << "ElasticBeam2d::addLoad()  -- load type unknown for element with tag: " << this->getTag() << std::endl;
            return -1;
          }
      }
    return 0;
  }

int XC::ElasticBeam2d::addInertiaLoadToUnbalance(const XC::Vector &accel)
  {
    if(rho!=0.0)
      {
        // Get R * accel from the nodes
        const XC::Vector &Raccel1 = theNodes[0]->getRV(accel);
        const XC::Vector &Raccel2 = theNodes[1]->getRV(accel);

        if(3 != Raccel1.Size() || 3 != Raccel2.Size())
          {
            std::cerr << "XC::ElasticBeam2d::addInertiaLoadToUnbalance matrix and vector sizes are incompatable\n";
            return -1;
          }

        // Want to add ( - fact * M R * accel ) to unbalance
        // Take advantage of lumped mass matrix
        const double L = theCoordTransf->getInitialLength();
        const double m = 0.5*rho*L;

        load(0)-= m * Raccel1(0);
        load(1)-= m * Raccel1(1);

        load(3)-= m * Raccel2(0);
        load(4)-= m * Raccel2(1);
      }
    return 0;
  }

const XC::Vector &XC::ElasticBeam2d::getResistingForceIncInertia(void) const
  {
    P= getResistingForce();

    // add the damping forces if rayleigh damping
    if(!rayFactors.Nulos())
      P+= this->getRayleighDampingForces();

    if(rho!=0.0)
      {
        const XC::Vector &accel1 = theNodes[0]->getTrialAccel();
        const XC::Vector &accel2 = theNodes[1]->getTrialAccel();

        const double L = theCoordTransf->getInitialLength();
        const double m = 0.5*rho*L;

        P(0) += m * accel1(0);
        P(1) += m * accel1(1);

        P(3) += m * accel2(0);
        P(4) += m * accel2(1);
      }
    if(isDead())
      P*=dead_srf;
    return P;
  }


const XC::Vector &XC::ElasticBeam2d::getResistingForce(void) const
  {
    const Vector &v= getSectionDeformation();

    const double EA= ctes_scc.EA(); // EA
    const double EI2= 2.0*ctes_scc.EI(); // 2EI
    const double EI4= 2.0*EI2; // 4EI

    // determine q = kv + q0
    q(0)= EA*v(0);
    q(1)= EI4*v(1) + EI2*v(2);
    q(2)= EI2*v(1) + EI4*v(2);

    q(0)+= q0[0];
    q(1)+= q0[1];
    q(2)+= q0[2];

    // Vector for reactions in basic system
    Vector p0Vec= p0.getVector();

    P = theCoordTransf->getGlobalResistingForce(q, p0Vec);

    // P = P - load;
    P.addVector(1.0, load, -1.0);

    if(isDead())
      P*=dead_srf;
    return P;
  }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::ElasticBeam2d::getDbTagData(void) const
  {
    static DbTagData retval(12);
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::ElasticBeam2d::sendData(CommParameters &cp)
  {
    int res= ProtoBeam2d::sendData(cp);
    res+= sendCoordTransf(8,9,10,cp);
    res+= cp.sendVector(eInic,getDbTagData(),CommMetaData(11));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::ElasticBeam2d::recvData(const CommParameters &cp)
  {
    int res= ProtoBeam2d::recvData(cp);
    theCoordTransf= recvCoordTransf2d(8,9,10,cp);
    res+= cp.receiveVector(eInic,getDbTagData(),CommMetaData(11));
    return res;
  }

int XC::ElasticBeam2d::sendSelf(CommParameters &cp)
  {
    inicComm(12);
    int res= sendData(cp);

    const int dataTag= getDbTag();
    res= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ElasticBeam2d::sendSelf -- could not send data.\n";
    return res;
  }

int XC::ElasticBeam2d::recvSelf(const CommParameters &cp)
  {
    const int dataTag= getDbTag();
    inicComm(12);
    int res = cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ElasticBeam2d::recvSelf() - failed to receive ID data.\n";
    else
      res+= recvData(cp);
    return res;
  }

void XC::ElasticBeam2d::Print(std::ostream &s, int flag)
  {
    if(flag == -1)
      {
        int eleTag = this->getTag();
        s << "EL_BEAM\t" << eleTag << "\t";
        s << 0 << "\t" << 0 << "\t" << theNodes.getTagNode(0) << "\t" << theNodes.getTagNode(1) ;
        s << "0\t0.0000000\n";
      }
    else
      {
        this->getResistingForce();
        s << "\nElasticBeam2d: " << this->getTag() << std::endl;
        s << "\tConnected Nodes: " << theNodes;
        s << "\tCoordTransf: " << theCoordTransf->getTag() << std::endl;
        s << "\tmass density:  " << rho << std::endl;
        const double P  = q(0);
        const double M1 = q(1);
        const double M2 = q(2);
        const double L = theCoordTransf->getInitialLength();
        const double V = (M1+M2)/L;
        s << "\tEnd 1 Forces (P V M): " << -P+p0[0]
          << " " << V+p0[1] << " " << M1 << std::endl;
        s << "\tEnd 2 Forces (P V M): " << P
          << " " << -V+p0[2] << " " << M2 << std::endl;
      }
   }

XC::Response *XC::ElasticBeam2d::setResponse(const std::vector<std::string> &argv, Parameter &param)
  {
    // stiffness
    if(argv[0] == "stiffness")
        return new ElementResponse(this, 1, K);

    // global forces
    else
      if(argv[0] == "force" || argv[0] == "forces" ||
         argv[0] == "globalForce" || argv[0] == "globalForces")
        return new ElementResponse(this, 2, P);

    // local forces
      else
        if(argv[0] == "localForce" || argv[0] == "localForces")
          return new ElementResponse(this, 3, P);
        else
          return 0;
  }

int XC::ElasticBeam2d::getResponse(int responseID, Information &eleInfo)
  {
    double N, M1, M2, V;
    double L = theCoordTransf->getInitialLength();

    switch(responseID)
      {
      case 1: // stiffness
        return eleInfo.setMatrix(this->getTangentStiff());
      case 2: // global forces
        return eleInfo.setVector(this->getResistingForce());
      case 3: // local forces
        // Axial
        N = q(0);
        P(3) =  N;
        P(0) = -N+p0[0];
        // Moment
        M1 = q(1);
        M2 = q(2);
        P(2) = M1;
        P(5) = M2;
        // Shear
        V = (M1+M2)/L;
        P(1) =  V+p0[1];
        P(4) = -V+p0[2];
        return eleInfo.setVector(P);
      default:
        return -1;
      }
  }

int XC::ElasticBeam2d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    const size_t argc= argv.size();
    if(argc < 1)
    return -1;

    // E of the beam interior
    if(argv[0] == "E")
      return param.addObject(1, this);

    // A of the beam interior
    if(argv[0] == "A")
      return param.addObject(2, this);
  
    // I of the beam interior
    if(argv[0] == "I")
      return param.addObject(3, this);
    return -1;
  }

int XC::ElasticBeam2d::updateParameter(int parameterID, Information &info)
  {
    switch(parameterID)
      {
      case -1:
        return -1;
      default:
        return ctes_scc.updateParameter(parameterID,info);
      }
  }

