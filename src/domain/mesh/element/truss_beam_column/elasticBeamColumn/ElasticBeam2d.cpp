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
          theCoordTransf= tmp->getCopy();
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; failed to get a copy of coordinate transformation."
	              << std::endl;
            exit(-1);
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; pointer to coordinate transformation is null."
		<< std::endl;
  }

void XC::ElasticBeam2d::init(void)
  {
    load.reset(6);
    q0.zero();
    p0.zero();
  }

XC::ElasticBeam2d::ElasticBeam2d(int tag)
  :ProtoBeam2d(tag,ELE_TAG_ElasticBeam2d), alpha(0.0), d(0.0),
   q(3), theCoordTransf(nullptr), release(0)
  { init(); }

//! @brief Constructor.
XC::ElasticBeam2d::ElasticBeam2d(int tag,const Material *m,const CrdTransf *trf)
  :ProtoBeam2d(tag,ELE_TAG_ElasticBeam2d,m), alpha(0.0), d(0.0),
   q(3), theCoordTransf(nullptr), release(0)
  {
    init();
    set_transf(trf);
  }

//! @brief Constructor.
XC::ElasticBeam2d::ElasticBeam2d(int tag, double a, double e, double i, int Nd1, int Nd2, CrdTransf2d &coordTransf, double Alpha, double depth, double r, int rel)
  : ProtoBeam2d(tag,ELE_TAG_ElasticBeam2d,a,e,i,Nd1,Nd2), alpha(Alpha),
    d(depth), q(3), theCoordTransf(nullptr), release(rel)
  {
    setRho(r);
    init();
    set_transf(&coordTransf);
  }

//! @brief Copy constructor.
XC::ElasticBeam2d::ElasticBeam2d(const ElasticBeam2d &other)
  :ProtoBeam2d(other), alpha(other.alpha), d(other.d),
   q(other.q), theCoordTransf(nullptr), release(other.release)
  {
    set_transf(other.theCoordTransf);

    q0= other.q0;
    p0= other.p0;
  }

//! @brief Assignment operator.
XC::ElasticBeam2d &XC::ElasticBeam2d::operator=(const ElasticBeam2d &other)
  {
    ProtoBeam2d::operator=(other);
    alpha= other.alpha;
    d= other.d;
    q= other.q;
    set_transf(other.theCoordTransf);
    release= other.release;

    q0= other.q0;
    p0= other.p0;
    return *this;
  }

//! @brief Virtual constructor.
XC::Element* XC::ElasticBeam2d::getCopy(void) const
  { return new ElasticBeam2d(*this); }


XC::ElasticBeam2d::~ElasticBeam2d(void)
  { if(theCoordTransf) delete theCoordTransf; }

//! @brief Returns (if possible) a pointer to the coordinate transformation.
XC::CrdTransf *XC::ElasticBeam2d::getCoordTransf(void)
  { return theCoordTransf; }

//! @brief Returns (if possible) a pointer to the coordinate transformation.
const XC::CrdTransf *XC::ElasticBeam2d::getCoordTransf(void) const
  { return theCoordTransf; }


void XC::ElasticBeam2d::setDomain(Domain *theDomain)
  {
    ProtoBeam2d::setDomain(theDomain);


    const int dofNd1= theNodes[0]->getNumberDOF();
    if(dofNd1 != 3)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; Node 1: " << theNodes.getTagNode(0)
                  << " has incorrect number of DOF: "
	          << dofNd1 << " instead of 3.\n";
        exit(-1);
      }

    const int dofNd2= theNodes[1]->getNumberDOF();
    if(dofNd2 != 3)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; Node 2: " << theNodes.getTagNode(1)
                  << " has incorrect number of DOF: "
	          << dofNd2 << " instead of 3.\n";
        exit(-1);
      }
    if(theCoordTransf)
      {
        if(theCoordTransf->initialize(theNodes[0], theNodes[1]) != 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; error initializing coordinate transformation\n";
            exit(-1);
          }
        double L= theCoordTransf->getInitialLength();
        if(L==0.0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; element has zero length\n";
            exit(-1);
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; the element has not coordinate transformation."
		<< std::endl;
   }

//! @brief Compute the current strain.
const XC::Vector &XC::ElasticBeam2d::computeCurrentStrain(void) const
  {
    static Vector retval(3);
    theCoordTransf->update();
    const double L= theCoordTransf->getInitialLength();
    retval= theCoordTransf->getBasicTrialDisp()/L;
    return retval;
  }

//! @brief Update element state.
int XC::ElasticBeam2d::update(void)
  {
    int retval= ProtoBeam2d::update();
    retval+= theCoordTransf->update();
    return retval;
  }

//! @brief Commit the element state.
int XC::ElasticBeam2d::commitState(void)
  {
    int retVal= ProtoBeam2d::commitState();
    // call element commitState to do any base class stuff
    if(retVal!=0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed in base class.";
    retVal+= theCoordTransf->commitState();
    return retVal;
  }

//! @brief Revert the element to its last commited state.
int XC::ElasticBeam2d::revertToLastCommit(void)
  {
    int retval= ProtoBeam2d::revertToLastCommit();
    retval+= theCoordTransf->revertToLastCommit();
    return retval;
  }

//! @brief Revert the element state to the start.
int XC::ElasticBeam2d::revertToStart()
  {
    int retval= ProtoBeam2d::revertToStart();
    retval+= theCoordTransf->revertToStart();
    return retval;
  }


//! @brief Returns the direction vector of element strong axis
//! expressed in the global coordinate system.
const XC::Vector &XC::ElasticBeam2d::getVDirStrongAxisGlobalCoord(bool initialGeometry) const
  {
    if(!initialGeometry)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; not implemented for deformed geometry." << std::endl;
    if(theCoordTransf)
      {
        const Vector eF= getVDirStrongAxisLocalCoord();
        return theCoordTransf->getVectorGlobalCoordFromLocal(eF);
      }
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; coordinate transformation not defined."
                  << std::endl;
        return P;
      }
  }

//! @brief Returns the direction vector of element weak axis
//! expressed in the global coordinate system.
const XC::Vector &XC::ElasticBeam2d::getVDirWeakAxisGlobalCoord(bool initialGeometry) const
  {
    if(!initialGeometry)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; not implemented for deformed geometry." << std::endl;
    if(theCoordTransf)
      {
        const Vector eD= getVDirWeakAxisLocalCoord();
        return theCoordTransf->getVectorGlobalCoordFromLocal(eD);
      }
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; coordinate transformation not defined."
                  << std::endl;
        return P;
      }
  }

//! @brief getTangent stiffness matrix.
const XC::Matrix &XC::ElasticBeam2d::getTangentStiff(void) const
  {
    const Vector &v= getSectionDeformation();
    const double L= theCoordTransf->getInitialLength();

    const CrossSectionProperties2d &sprop= getSectionProperties();
    const double EA= sprop.EA(); // EA
    const double EI2= 2.0*sprop.EI(); // 2EI
    const double EI3= 3.0*sprop.EI(); // 3EI
    const double EI4= 2.0*EI2; // 4EI
    const double EAoverL= EA/L;	// EA/L
    const double EIoverL2= EI2/L; // 2EI/L
    const double EIoverL4= EI4/L; // 4EI/L
    const double EIoverL3= EI3/L;

    // determine q= kv + q0
    q(0)= EA*v(0);
    kb.Zero();
    kb(0,0)= EAoverL;
    if(release==0)
      {
	q(1)= EI4*v(1) + EI2*v(2);
	q(2)= EI2*v(1) + EI4*v(2);
	kb(1,1)= kb(2,2)= EIoverL4;
	kb(2,1)= kb(1,2)= EIoverL2;    
      }
    else if(release==1)
      { // release I
	q(1)= 0.0;
	q(2)= EI3*v(2);
	kb(2,2)= EIoverL3;
      }
    else if(release==2)
      { // release J
	q(2)= 0.0;
	q(1)= EI3*v(1);
	kb(1,1)= EIoverL3;
      }
    else if(release==3)
      { // release both I and J
	q(1)= 0.0;
	q(2)= 0.0;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << " release value (" << release
	        << ") not valid. Must be between 0 and 3."
	        << std::endl;
    q(0)+= q0[0];
    q(1)+= q0[1];
    q(2)+= q0[2];

    static Matrix retval;
    retval= theCoordTransf->getGlobalStiffMatrix(kb,q);
    if(isDead())
      retval*=dead_srf;
    return retval;
  }

//! @brief Return initial stiffness matrix.
const XC::Matrix &XC::ElasticBeam2d::getInitialStiff(void) const
  {
    const double L= theCoordTransf->getInitialLength();
    const CrossSectionProperties2d &sprop= getSectionProperties();
    const double EoverL= sprop.E()/L;
    const double EAoverL= sprop.A()*EoverL; // EA/L
    const double EIoverL2= 2.0*sprop.I()*EoverL; // 2EI/L
    const double EIoverL4= 2.0*EIoverL2; // 4EI/L

    kb.Zero();
    kb(0,0)= EAoverL;
    if(release==0)
      {
        kb(1,1)= kb(2,2)= EIoverL4;
        kb(2,1)= kb(1,2)= EIoverL2;
      }
    else if(release==1)
      { // release I
        kb(2,2)= 3.0*sprop.I()*EoverL;
      }
    else if(release==2)
      { // release J
        kb(1,1)= 3.0*sprop.I()*EoverL;
      }
    else if(release==3)
      {}
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << " release value (" << release
	        << ") not valid. Must be between 0 and 3."
	        << std::endl;
    static Matrix retval;
    retval= theCoordTransf->getInitialGlobalStiffMatrix(kb);
    if(isDead())
      retval*=dead_srf;
    return retval;
  }

const XC::Matrix &XC::ElasticBeam2d::getMass(void) const
  {
    K.Zero();

    const double rho= getRho();
    if(rho>0.0)
      {
        const double L= theCoordTransf->getInitialLength();
        const double m= 0.5*rho*L;
        const double mpeq= m*1.0e-10;

        K(0,0)= m;
        K(1,1)= m;
        K(2,2)= mpeq; //Modified by LCPT to avoid a singular M matrix.

        K(3,3)= m;
        K(4,4)= m;
        K(5,5)= mpeq; //Modified by LCPT to avoid a singular M matrix.
      }
    if(isDead())
      K*=dead_srf;
    return K;
  }

void XC::ElasticBeam2d::zeroLoad(void)
  {
    ProtoBeam2d::zeroLoad();
    q0.zero();
    p0.zero();
  }

int XC::ElasticBeam2d::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(isDead())
      std::cerr << getClassName() << "::" << __FUNCTION__ 
                << "; load over inactive element: "
                << getTag()  
                << std::endl;
    else
      {
        const double L= theCoordTransf->getInitialLength();
        if(const BeamMecLoad *beamMecLoad= dynamic_cast<const BeamMecLoad *>(theLoad))
          {
            beamMecLoad->addReactionsInBasicSystem(L,loadFactor,p0); // Accumulate reactions in basic system
            beamMecLoad->addFixedEndForcesInBasicSystem(L,loadFactor,q0,release); // Fixed end forces in basic system
          }
        else if(const BeamStrainLoad *strainLoad= dynamic_cast<BeamStrainLoad *>(theLoad)) //Prescribed strains.
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
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; load type unknown for element with tag: "
		      << this->getTag() << std::endl;
            return -1;
          }
      }
    return 0;
  }

int XC::ElasticBeam2d::addInertiaLoadToUnbalance(const XC::Vector &accel)
  {
    const double rho= getRho();
    if(rho!=0.0)
      {
        // Get R * accel from the nodes
        const Vector &Raccel1= theNodes[0]->getRV(accel);
        const Vector &Raccel2= theNodes[1]->getRV(accel);

        if(3 != Raccel1.Size() || 3 != Raccel2.Size())
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; matrix and vector sizes are incompatible.\n";
            return -1;
          }

        // Want to add ( - fact * M R * accel ) to unbalance
        // Take advantage of lumped mass matrix
        const double L= theCoordTransf->getInitialLength();
        const double m= 0.5*rho*L;

        load(0)-= m * Raccel1(0);
        load(1)-= m * Raccel1(1);

        load(3)-= m * Raccel2(0);
        load(4)-= m * Raccel2(1);
      }
    return 0;
  }

//! @brief Return the element resisting force.
const XC::Vector &XC::ElasticBeam2d::getResistingForce(void) const
  {
    theCoordTransf->update();
    
    const Vector &v= getSectionDeformation();

    const CrossSectionProperties2d &sprop= getSectionProperties();
    const double EA= sprop.EA(); // EA
    const double EI2= 2.0*sprop.EI(); // 2EI
    const double EI4= 2.0*EI2; // 4EI


    // determine q= kv + q0
    q(0)= EA*v(0);
    if(release==0)
      {
        q(1)= EI4*v(1) + EI2*v(2);
        q(2)= EI2*v(1) + EI4*v(2);
      }
    else if(release==1)
      {
        q(1)= 0.0;
        q(2)= 3.0*sprop.EI()*v(2);
      }
    else if(release==2)
      {
        q(1)= 3.0*sprop.EI()*v(1);
        q(2)= 0.0;
      }
    else if(release==3)
      {
        q(1)= 0.0;
        q(2)= 0.0;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << " release value (" << release
	        << ") not valid. Must be between 0 and 3."
	        << std::endl;
    q(0)+= q0[0];
    q(1)+= q0[1];
    q(2)+= q0[2];
    
    if(isDead()) //Set internal forces to zero when element is dead.
      q*= dead_srf;

    // Vector for reactions in basic system
    Vector p0Vec= p0.getVector();

    P= theCoordTransf->getGlobalResistingForce(q, p0Vec);

    // P= P - load;
    P.addVector(1.0, load, -1.0);

    if(isDead())
      P*=dead_srf;
    return P;
  }

//! @brief Return the element resisting force including inertia effects.
const XC::Vector &XC::ElasticBeam2d::getResistingForceIncInertia(void) const
  {
    P= getResistingForce();

    // add the damping forces if rayleigh damping
    if(!rayFactors.nullValues())
      P+= this->getRayleighDampingForces();

    const double rho= getRho();
    if(rho!=0.0)
      {
        const Vector &accel1= theNodes[0]->getTrialAccel();
        const Vector &accel2= theNodes[1]->getTrialAccel();

        const double L= theCoordTransf->getInitialLength();
        const double m= 0.5*rho*L;

        P(0) += m * accel1(0);
        P(1) += m * accel1(1);

        P(3) += m * accel2(0);
        P(4) += m * accel2(1);
      }
    if(isDead())
      P*=dead_srf;
    return P;
  }


//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::ElasticBeam2d::getDbTagData(void) const
  {
    static DbTagData retval(13);
    return retval;
  }

//! @brief Send members through the communicator argument.
int XC::ElasticBeam2d::sendData(Communicator &comm)
  {
    int res= ProtoBeam2d::sendData(comm);
    res+= sendCoordTransf(8,9,10,comm);
    //res+= comm.sendVector(eInit,getDbTagData(),CommMetaData(11));
    res+= comm.sendInt(release,getDbTagData(),CommMetaData(12));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::ElasticBeam2d::recvData(const Communicator &comm)
  {
    int res= ProtoBeam2d::recvData(comm);
    theCoordTransf= recvCoordTransf2d(8,9,10,comm);
    //res+= comm.receiveVector(eInit,getDbTagData(),CommMetaData(11));
    res+= comm.receiveInt(release,getDbTagData(),CommMetaData(12));
    return res;
  }

int XC::ElasticBeam2d::sendSelf(Communicator &comm)
  {
    inicComm(13);
    int res= sendData(comm);

    const int dataTag= getDbTag();
    res= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; could not send data.\n";
    return res;
  }

int XC::ElasticBeam2d::recvSelf(const Communicator &comm)
  {
    const int dataTag= getDbTag();
    inicComm(13);
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ID data.\n";
    else
      res+= recvData(comm);
    return res;
  }

void XC::ElasticBeam2d::Print(std::ostream &s, int flag) const
  {
    if(flag==-1)
      {
        int eleTag= this->getTag();
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
        s << "\tmass density:  " << getRho() << std::endl;
        s << "\trelease code:  " << release << std::endl;
        const double P = q(0);
        const double M1= q(1);
        const double M2= q(2);
        const double L= theCoordTransf->getInitialLength();
        const double V= (M1+M2)/L;
        s << "\tEnd 1 Forces (P V M): " << -P+p0[0]
          << " " << V+p0[1] << " " << M1 << std::endl;
        s << "\tEnd 2 Forces (P V M): " << P
          << " " << -V+p0[2] << " " << M2 << std::endl;
      }
   }

XC::Response *XC::ElasticBeam2d::setResponse(const std::vector<std::string> &argv, Parameter &param)
  {
    // stiffness
    if(argv[0]=="stiffness")
        return new ElementResponse(this, 1, K);

    // global forces
    else
      if(argv[0]=="force" || argv[0]=="forces" ||
         argv[0]=="globalForce" || argv[0]=="globalForces")
        return new ElementResponse(this, 2, P);

    // local forces
      else
        if(argv[0]=="localForce" || argv[0]=="localForces")
          return new ElementResponse(this, 3, P);
        else
          return 0;
  }

int XC::ElasticBeam2d::getResponse(int responseID, Information &eleInfo)
  {
    double N, M1, M2, V;
    double L= theCoordTransf->getInitialLength();

    switch(responseID)
      {
      case 1: // stiffness
        return eleInfo.setMatrix(this->getTangentStiff());
      case 2: // global forces
        return eleInfo.setVector(this->getResistingForce());
      case 3: // local forces
        // Axial
        N= q(0);
        P(3)=  N;
        P(0)= -N+p0[0];
        // Moment
        M1= q(1);
        M2= q(2);
        P(2)= M1;
        P(5)= M2;
        // Shear
        V= (M1+M2)/L;
        P(1)=  V+p0[1];
        P(4)= -V+p0[2];
        return eleInfo.setVector(P);
      default:
        return -1;
      }
  }

int XC::ElasticBeam2d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    const size_t argc= argv.size();
    if(argc > 0)
      {
        const CrossSectionProperties2d &sprop= getSectionProperties();
        if(argv[0]=="E") // E of the beam interior
	  {
	    param.setValue(sprop.E());
	    return param.addObject(1, this);
	  }
	else if(argv[0]=="A") // A of the beam interior
	  {
	    param.setValue(sprop.A());
	    return param.addObject(2, this);
	  }
	else if(argv[0]=="I") // I of the beam interior
	  {
	    param.setValue(sprop.I());
	    return param.addObject(3, this);
	  }
	else if(argv[0]=="rho") // rho of the beam interior
	  {
	    param.setValue(sprop.getRho());
	    return param.addObject(4, this);
	  }
	else if(argv[0]=="release") // rho of the beam interior
	  {
	    param.setValue(release);
	    return param.addObject(5, this);
	  }
      }
    return -1;
  }

int XC::ElasticBeam2d::updateParameter(int parameterID, Information &info)
  {
    switch(parameterID)
      {
      case -1:
        return -1;
      case 5:
	release= static_cast<int>(info.theDouble);
	if(release < 0 || release > 3)
	  release= 0;
	  return 0;				
      default:
        return getSectionProperties().updateParameter(parameterID,info);
      }
  }

//! @brief Return a python list with the values of the argument property
//! at element nodes.
//!
//! When the property requested its located at the integration point this
//! function is responsible of the extrapolation of values from
//! Gauss points to nodes.
//! @param code: identifier of the requested value.
//! @param silent: if true don't complaint about non-existen property.
boost::python::list XC::ElasticBeam2d::getValuesAtNodes(const std::string &code, bool silent) const
  {
    boost::python::list retval;
    if(code=="N")
      {
	retval.append(getN1());
	retval.append(getN2());
      }
    else if((code=="M") || (code=="Mz"))
      {
	retval.append(getM1());
	retval.append(getM2());
      }
    else if((code=="V") || (code=="Vy") || (code=="Q") || (code=="Qy"))
      {
	retval.append(getV1());
	retval.append(getV2());
      }
    else
      retval= ProtoBeam2d::getValuesAtNodes(code, silent); 
    return retval;
  }
