//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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

// $Revision: 1.15 $
// $Date: 2004/10/30 00:05:32 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/elasticBeamColumn/ElasticBeam3d.cpp,v $


// File: ~/model/ElasticBeam3d.C
//
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the class definition for XC::ElasticBeam3d.
// ElasticBeam3d is a 3d beam element. As such it can only
// connect to a node with 6-dof.

#include <domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticBeam3d.h>
#include <domain/domain/Domain.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

#include <domain/mesh/element/utils/coordTransformation/CrdTransf3d.h>
#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/ElementResponse.h>
#include "domain/load/beam_loads/BeamMecLoad.h"
#include "domain/load/beam_loads/BeamStrainLoad.h"
#include <domain/mesh/node/Node.h>
#include <material/section/SectionForceDeformation.h>
#include <utility/matrix/ID.h>

#include "material/section/ResponseId.h"
#include "utility/actor/actor/MovableVector.h"

XC::Matrix XC::ElasticBeam3d::K(12,12);
XC::Vector XC::ElasticBeam3d::P(12);
XC::Matrix XC::ElasticBeam3d::kb(6,6);

//! @brief Default constructor.
//! @param tag: element identifier.
XC::ElasticBeam3d::ElasticBeam3d(int tag)
  :ElasticBeam3dBase(tag,ELE_TAG_ElasticBeam3d), releasez(0), releasey(0),
   q(), q0(), p0()
  { load.reset(12); }

//! @brief Constructor.
//! @param tag: element identifier.
//! @param m: element material.
//! @param trf: coordinate transformation for the element.
XC::ElasticBeam3d::ElasticBeam3d(int tag,const Material *m,const CrdTransf *trf)
  : ElasticBeam3dBase(tag,ELE_TAG_ElasticBeam3d, m, trf),
    releasez(0), releasey(0), q(), q0(), p0()
  { load.reset(12); }

//! @brief Constructor.
//! @param tag: element identifier.
//! @param a: area of the element cross section.
//! @param e: elastic modulus of the element material.
//! @param g: shear modulus of the element material.
//! @param jx: torsional section modulus of the element section.
//! @param iy: modulus of inertia the element section around the y axis.
//! @param iz: modulus of inertia the element section around the z axis.
//! @param Nd1: tag of the element I node.
//! @param Nd2: tag of the element J node.
//! @param coordTransf: coordinate transformation for the element.
//! @param r: density of the element material.
XC::ElasticBeam3d::ElasticBeam3d(int tag, double a, double e, double g,
                             double jx, double iy, double iz, int Nd1, int Nd2,
				 CrdTransf3d &coordTransf, double r, int relz, int rely)
  :ElasticBeam3dBase(tag,ELE_TAG_ElasticBeam3d,a,e,g,jx,iy,iz,Nd1,Nd2,coordTransf, r),
   releasez(relz), releasey(rely), q(), q0(), p0()
  {
    load.reset(12);
    // Make no release if input not 0, 1, 2, or 3
    if(releasez < 0 || releasez > 3)
      releasez = 0;
    if(releasey < 0 || releasey > 3)
      releasey = 0;
  }

//! @brief Constructor.
//! @param tag: element identifier.
//! @param Nd1: tag of the element I node.
//! @param Nd2: tag of the element J node.
//! @param section: SectionForceDeformation object to copy mechanical properties from.
//! @param coordTransf: coordinate transformation for the element.
//! @param r: density of the element material.
XC::ElasticBeam3d::ElasticBeam3d(int tag, int Nd1, int Nd2, SectionForceDeformation *section,CrdTransf3d &coordTransf, double r, int relz, int rely)
  :ElasticBeam3dBase(tag,ELE_TAG_ElasticBeam3d,Nd1,Nd2, section, coordTransf, r),
   releasez(relz), releasey(rely), q()
  {
    load.reset(12);
    
    // Make no release if input not 0, 1, 2, or 3
    if(releasez < 0 || releasez > 3)
      releasez = 0;
    if(releasey < 0 || releasey > 3)
      releasey = 0;

    q0.zero();
    p0.zero();
  }

//! @brief Virtual constructor.
XC::Element* XC::ElasticBeam3d::getCopy(void) const
  { return new ElasticBeam3d(*this); }


//! @brief Compute the current strain.
const XC::Vector &XC::ElasticBeam3d::computeCurrentStrain(void) const
  {
    static Vector retval(5);
    theCoordTransf->update();
    const double L= theCoordTransf->getInitialLength();
    retval= theCoordTransf->getBasicTrialDisp()/L;
    return retval;
  }

//! @brief Update element state.
int XC::ElasticBeam3d::update(void)
  {
    int retval= ElasticBeam3dBase::update();
    retval+= theCoordTransf->update();
    return retval;
  }

//! @brief Commit the element state.
int XC::ElasticBeam3d::commitState(void)
  {
    int retVal = 0;
    // call element commitState to do any base class stuff
    if((retVal = this->ElasticBeam3dBase::commitState()) != 0)
      { std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed in base class."; }
    retVal += theCoordTransf->commitState();
    return retVal;
  }

//! @brief Revert the element to the its last commited state.
int XC::ElasticBeam3d::revertToLastCommit()
  {
    int retval= ElasticBeam3dBase::revertToLastCommit();
    retval+= theCoordTransf->revertToLastCommit();
    return retval;
  }

//! @brief Revert the the element to the its start state.
int XC::ElasticBeam3d::revertToStart()
  {
    int retval= ElasticBeam3dBase::revertToStart();
    retval+= theCoordTransf->revertToStart();
    return retval;
  }

//! @brief Return the tangent stiffness matrix in global coordinates.
const XC::Matrix &XC::ElasticBeam3d::getTangentStiff(void) const
  {
    const Vector &v= getSectionDeformation();
    //Ignore sections with product moment
    //of inertia not zero.
    const CrossSectionProperties3d &sprop= getSectionProperties();
    const double eiyz= sprop.EIyz();
    const double eimax= std::max(sprop.EIz(),sprop.EIy());
    if(std::abs(eiyz/eimax)>1e-5) //Product of inertia not null.
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; this element must not"
                << " be used with section that have a non-zero"
                << " product of inertia."
                << std::endl;

    const double E= sprop.E();
    const double EA= sprop.A()*E; // EA
    
    const double EIz2= 2.0*sprop.Iz()*E; // 2EIz
    const double EIz4= 2.0*EIz2; // 4EIz
    const double EIy2= 2.0*sprop.Iy()*E; // 2EIy
    const double EIy4= 2.0*EIy2; // 4EIy
    
    const double GJ= sprop.GJ(); // GJ
    
    if(releasez!= 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; release z not implemented yet." << std::endl;
    if(releasey!= 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; release y not implemented yet." << std::endl;

    //Element internal forces due to the actions of the domain on its nodes.
    q.N()= EA*v(0); //axial force in the element.
    q.Mz1()= EIz4*v(1) + EIz2*v(2); //z bending moment in node 1: 4EI/L*v(1)+2EI/L*v(2).
    q.Mz2()= EIz2*v(1) + EIz4*v(2); //z bending moment in node 2: 2EI/L*v(1)+4EI/L*v(2).
    q.My1()= EIy4*v(3) + EIy2*v(4); //y bending moment in node 1: 4EI/L*v(3)+2EI/L*v(4).
    q.My2()= EIy2*v(3) + EIy4*v(4); //y bending moment in node 2: 2EI/L*v(3)+4EI/L*v(4).
    q.T() = GJ*v(5); //Torsion in the element.

    q.N()+= q0[0];
    q.Mz1()+= q0[1];
    q.Mz2()+= q0[2];
    q.My1()+= q0[3];
    q.My2()+= q0[4];

    const double L = theCoordTransf->getInitialLength();
    const double EAoverL= EA/L;	// EA/L
    const double GJoverL= GJ/L; // GJ/L
    
    kb(0,0) = EAoverL;
    kb(1,1) = kb(2,2)= EIz4/L;
    kb(2,1) = kb(1,2)= EIz2/L;
    kb(3,3) = kb(4,4)= EIy4/L;
    kb(4,3) = kb(3,4)= EIy2/L;
    kb(5,5) = GJoverL;

    static Matrix retval;
    retval= theCoordTransf->getGlobalStiffMatrix(kb,q);
    if(isDead())
      retval*=dead_srf;

    return retval;
  }

//! @brief Return the initial stiffness matrix.
const XC::Matrix &XC::ElasticBeam3d::getInitialStiff(void) const
  {
    //Ignore sections with product moment
    //of inertia not zero.
    const CrossSectionProperties3d &sprop= getSectionProperties();
    const double eiyz= sprop.EIyz();
    const double eimax= std::max(sprop.EIz(),sprop.EIy());
    if(std::abs(eiyz/eimax)>1e-5) //Product of inertia not null.
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; this element must not"
                << " be used with sections that has "
                << " a non-zero product of inertia."
                << std::endl;

    const double L = theCoordTransf->getInitialLength();
    const double oneOverL = 1.0/L;
    const double EoverL   = sprop.E()*oneOverL;
    const double EAoverL  = sprop.A()*EoverL; // EA/L
    const double EIzoverL2 = 2.0*sprop.Iz()*EoverL; // 2EIz/L
    const double EIzoverL4 = 2.0*EIzoverL2; // 4EIz/L
    const double EIyoverL2 = 2.0*sprop.Iy()*EoverL; // 2EIy/L
    const double EIyoverL4 = 2.0*EIyoverL2; // 4EIy/L
    const double GJoverL = sprop.GJ()*oneOverL; // GJ/L

    if(releasez!= 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; release z not implemented yet." << std::endl;
    if(releasey!= 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; release y not implemented yet." << std::endl;
    
    kb(0,0) = EAoverL;
    kb(1,1) = kb(2,2) = EIzoverL4;
    kb(2,1) = kb(1,2) = EIzoverL2;
    kb(3,3) = kb(4,4) = EIyoverL4;
    kb(4,3) = kb(3,4) = EIyoverL2;
    kb(5,5) = GJoverL;

    static Matrix retval;
    retval= theCoordTransf->getInitialGlobalStiffMatrix(kb);
    if(isDead())
      retval*=dead_srf;

    return retval;
  }

//! @brief Return the mass matrix of the element.
const XC::Matrix &XC::ElasticBeam3d::getMass(void) const
  {
    K.Zero();

    const double rho= getRho();
    if(rho > 0.0)
      {
        const double L= theCoordTransf->getInitialLength();
        const double m= 0.5*rho*L;
        const double mpeq= m*1.0e-10;

        K(0,0)= m;
        K(1,1)= m;
        K(2,2)= m;
        K(3,3)= mpeq; //Modified by LCPT to avoid a singular M matrix.
        K(4,4)= mpeq; //Modified by LCPT to avoid a singular M matrix.
        K(5,5)= mpeq; //Modified by LCPT to avoid a singular M matrix.

        K(6,6)= m;
        K(7,7)= m;
        K(8,8)= m;
        K(9,9)= mpeq; //Modified by LCPT to avoid a singular M matrix.
        K(10,10)= mpeq; //Modified by LCPT to avoid a singular M matrix.
        K(11,11)= mpeq; //Modified by LCPT to avoid a singular M matrix.
      }
    if(isDead())
      K*=dead_srf;
    return K;
  }

void XC::ElasticBeam3d::zeroLoad(void)
  {
    ElasticBeam3dBase::zeroLoad();
    q0.zero();
    p0.zero();
    return;
  }

int XC::ElasticBeam3d::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(isDead())
      std::cerr << getClassName() << "::" << __FUNCTION__ 
                << "; load over inactive element: "
                << getTag()  
                << std::endl;
    else
      {
	if(releasez!= 0)
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; release z not implemented yet." << std::endl;
	if(releasey!= 0)
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; release y not implemented yet." << std::endl;
	
        if(const BeamMecLoad *beamMecLoad= dynamic_cast<const BeamMecLoad *>(theLoad))
          {
            const double L = theCoordTransf->getInitialLength();
            beamMecLoad->addReactionsInBasicSystem({L},loadFactor,p0); // Accumulate reactions in basic system
            beamMecLoad->addFixedEndForcesInBasicSystem({L},loadFactor,q0); // Fixed end forces in basic system
          }
        else if(const BeamStrainLoad *strainLoad= dynamic_cast<const BeamStrainLoad *>(theLoad)) //Prescribed strains.
          {
            const int order= 3;
            const RespPMzMy code;
            const Vector &e1= strainLoad->getSection1Deformation(order,code)*loadFactor;
            const Vector &e2= strainLoad->getSection2Deformation(order,code)*loadFactor;
            Vector ezero= getInitialSectionDeformation();
            ezero+= (e2+e1)/2;
            setInitialSectionDeformation(ezero); 
          }
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; load type unknown for element with tag: " << this->getTag() << std::endl;
            return -1;
          }
      }
    return 0;
  }


int XC::ElasticBeam3d::addInertiaLoadToUnbalance(const XC::Vector &accel)
  {
    const double rho= getRho();
    if(rho == 0.0) return 0;

    // Get R * accel from the nodes
    const XC::Vector &Raccel1 = theNodes[0]->getRV(accel);
    const XC::Vector &Raccel2 = theNodes[1]->getRV(accel);

    if(6 != Raccel1.Size() || 6 != Raccel2.Size())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; matrix and vector sizes are incompatible.\n";
        return -1;
      }

    // Want to add ( - fact * M R * accel ) to unbalance
    // Take advantage of lumped mass matrix
    const double L = theCoordTransf->getInitialLength();
    const double m = 0.5*rho*L;

    load(0)-= m * Raccel1(0);
    load(1)-= m * Raccel1(1);
    load(2)-= m * Raccel1(2);

    load(6)-= m * Raccel2(0);
    load(7)-= m * Raccel2(1);
    load(8)-= m * Raccel2(2);

    return 0;
  }


const XC::Vector &XC::ElasticBeam3d::getResistingForceIncInertia(void) const
  {
    P = this->getResistingForce();

    // add the damping forces if rayleigh damping
    if(!rayFactors.nullValues())
      P+= this->getRayleighDampingForces();

    const double rho= getRho();
    if(rho == 0.0)
      return P;
    else
      {
        const Vector &accel1= theNodes[0]->getTrialAccel();
        const Vector &accel2= theNodes[1]->getTrialAccel();

        const double L= theCoordTransf->getInitialLength();
        const double m= 0.5*rho*L;

        P(0) += m * accel1(0);
        P(1) += m * accel1(1);
        P(2) += m * accel1(2);

        P(6) += m * accel2(0);
        P(7) += m * accel2(1);
        P(8) += m * accel2(2);
        return P;
      }
    if(isDead())
      P*=dead_srf;
  }

//! @brief Return the element resisting force.
const XC::Vector &XC::ElasticBeam3d::getResistingForce(void) const
  {
    const Vector &v= getSectionDeformation();
    const CrossSectionProperties3d &sprop= getSectionProperties();
    const double E= sprop.E();
    const double EA= sprop.A()*E; // EA
    const double EIz2= 2.0*sprop.Iz()*E; // 2EIz
    const double EIz4= 2.0*EIz2; // 4EIz
    const double EIy2= 2.0*sprop.Iy()*E; // 2EIy
    const double EIy4= 2.0*EIy2; // 4EIy
    const double GJ= sprop.GJ(); // GJ

    if(releasez!= 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; release z not implemented yet." << std::endl;
    if(releasey!= 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; release y not implemented yet." << std::endl;
    //Element internal forces due to the actions of the structure over its nodes.
    q.N()= EA*v(0); //axial force in the element.
    q.Mz1()= EIz4*v(1) + EIz2*v(2); //z bending moment in node 1: 4EI/L*v(1)+2EI/L*v(2).
    q.Mz2()= EIz2*v(1) + EIz4*v(2); //z bending moment in node 2: 2EI/L*v(1)+4EI/L*v(2).
    q.My1()= EIy4*v(3) + EIy2*v(4); //y bending moment in node 1: 4EI/L*v(3)+2EI/L*v(4).
    q.My2()= EIy2*v(3) + EIy4*v(4); //y bending moment in node 2: 2EI/L*v(3)+4EI/L*v(4).
    q.T()= GJ*v(5); //Torsor in the element.

    q.N()+= q0[0];
    q.Mz1()+= q0[1];
    q.Mz2()+= q0[2];
    q.My1()+= q0[3];
    q.My2()+= q0[4];
    
    if(isDead()) //Set internal forces to zero when element is dead.
      q*= dead_srf;

    const Vector p0Vec= p0.getVector();

    //  std::cerr << q;

    P= theCoordTransf->getGlobalResistingForce(q, p0Vec);

    // std::cerr << P;

    // P = P - load;
    P.addVector(1.0, load, -1.0);

    if(isDead())
      P*= dead_srf;
    return P;
  }


//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::ElasticBeam3d::getDbTagData(void) const
  {
    static DbTagData retval(17);
    return retval;
  }

//! @brief Send members through the communicator argument.
int XC::ElasticBeam3d::sendData(Communicator &comm)
  {
    DbTagData &dt= getDbTagData();
    int res= ElasticBeam3dBase::sendData(comm);
    //res+= comm.sendVector(eInic,dt,CommMetaData(11));
    res+= comm.sendInts(releasez,releasey,dt,CommMetaData(12));
    res+= sendEsfBeamColumn3d(q,13,dt,comm);
    res+= p0.sendData(comm,dt,CommMetaData(14));
    res+= q0.sendData(comm,dt,CommMetaData(15));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::ElasticBeam3d::recvData(const Communicator &comm)
  {
    DbTagData &dt= getDbTagData();
    int res= ElasticBeam3dBase::recvData(comm);
    //res+= comm.receiveVector(eInic,dt,CommMetaData(11));
    res+= comm.receiveInts(releasez, releasey,dt,CommMetaData(12));
    res+= receiveEsfBeamColumn3d(q,13,dt,comm);
    res+= p0.receiveData(comm,dt,CommMetaData(14));
    res+= q0.receiveData(comm,dt,CommMetaData(15));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::ElasticBeam3d::sendSelf(Communicator &comm)
  {
    inicComm(17);
    int res= sendData(comm);

    const int dataTag= getDbTag();
    res= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; could not send data Vector\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::ElasticBeam3d::recvSelf(const Communicator &comm)
  {
    inicComm(17);
    const int dataTag= getDbTag();
    int res = comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send ID data\n";
    else
      res+= recvData(comm);
    return res;
  }

void XC::ElasticBeam3d::Print(std::ostream &s, int flag) const
  {
   if(flag == -1)
     {
       int eleTag = this->getTag();
       s << "EL_BEAM\t" << eleTag << "\t";
       //s << sectionTag << "\t" << sectionTag;
       s  << "\t" << theNodes.getTagNode(0) << "\t" << theNodes.getTagNode(1);
       s << "\t0\t0.0000000\n";
     }
   else
     {
       this->getResistingForce(); // in case linear algo
       const double L = theCoordTransf->getInitialLength();
       if(flag < -1)
         {
           int counter = (flag + 1) * -1;
           int eleTag = this->getTag();
           s << "FORCE\t" << eleTag << "\t" << counter << "\t0";
           s << "\t" << q.AN1()+p0[0] << "\t"  <<  q.Vy(L)+p0[1] << "\t"  << -q.Vz(L)+p0[3]  << std::endl;
           s << "FORCE\t" << eleTag << "\t" << counter << "\t1";
           s << "\t"  << q.AN2() << ' '  << -q.Vy(L)+p0[2] << ' ' << q.Vz(L)+p0[4] << std::endl;
           s << "MOMENT\t" << eleTag << "\t" << counter << "\t0";
           s << "\t" << q.T1() << "\t"  << q.My1() << "\t" << q.Mz1() << std::endl;
           s << "MOMENT\t" << eleTag << "\t" << counter << "\t1";
           s << "\t" << q.T2() << ' ' << q.My2() << ' '  <<  q.Mz2() << std::endl;
         }
       else if(flag == 2)
         {
           static Vector xAxis(3);
           static Vector yAxis(3);
           static Vector zAxis(3);

           theCoordTransf->getLocalAxes(xAxis, yAxis, zAxis);

           s << "#ElasticBeamColumn3D\n";
           s << "#LocalAxis " << xAxis(0) << " " << xAxis(1) << " " << xAxis(2);
           s << " " << yAxis(0) << " " << yAxis(1) << " " << yAxis(2) << " ";
           s << zAxis(0) << " " << zAxis(1) << " " << zAxis(2) << std::endl;

           const Vector &node1Crd = theNodes[0]->getCrds();
           const Vector &node2Crd = theNodes[1]->getCrds();
           const Vector &node1Disp = theNodes[0]->getDisp();
           const Vector &node2Disp = theNodes[1]->getDisp();

           s << "#NODE " << node1Crd(0) << " " << node1Crd(1) << " " << node1Crd(2)
             << " " << node1Disp(0) << " " << node1Disp(1) << " " << node1Disp(2)
             << " " << node1Disp(3) << " " << node1Disp(4) << " " << node1Disp(5) << std::endl;

           s << "#NODE " << node2Crd(0) << " " << node2Crd(1) << " " << node2Crd(2)
             << " " << node2Disp(0) << " " << node2Disp(1) << " " << node2Disp(2)
             << " " << node2Disp(3) << " " << node2Disp(4) << " " << node2Disp(5) << std::endl;


           s << "#END_FORCES " << q.AN1()+p0[0] << ' ' <<  q.Vy(L)+p0[1] << ' ' << q.Vz(L)+p0[3] << ' '
             << q.T1() << ' ' << q.My1() << ' ' <<  q.Mz1() << std::endl;
           s << "#END_FORCES " <<  q.AN2() << ' ' << -q.Vy(L)+p0[2] << ' ' << -q.Vz(L)+p0[4] << ' '
             << q.T2() << ' ' << q.My2() << ' ' << q.Mz2() << std::endl;
          }
        else
          {

            s << "\nElasticBeam3d: " << this->getTag() << std::endl;
            s << "\tConnected Nodes: " << theNodes;
            s << "\tCoordTransf: " << theCoordTransf->getTag() << std::endl;


            s << "\tEnd 1 Forces (N Mz Vy My Vz T): "
              << q.AN1()+p0[0] << ' ' << q.Mz1() << ' ' <<  q.Vy(L)+p0[1] << ' ' << q.My1() << ' ' <<  q.Vz(L)+p0[3] << ' ' << q.T1() << std::endl;
            s << "\tEnd 2 Forces (N Mz Vy My Vz T): "
              <<  q.AN2() << ' ' << q.Mz2() << ' ' << -q.Vy(L)+p0[2] << ' ' << q.My2() << ' ' << -q.Vz(L)+p0[4] << ' ' <<  q.T2() << std::endl;
          }
     }
  }

XC::Response *XC::ElasticBeam3d::setResponse(const std::vector<std::string> &argv, Information &info)
  {
    // stiffness
    if(argv[0] == "stiffness")
                return new ElementResponse(this, 1, K);

    // global forces
    else if(argv[0] == "force" || argv[0] == "forces" ||
                argv[0] == "globalForce" || argv[0] == "globalForces")
                return new ElementResponse(this, 2, P);

        // local forces
    else if(argv[0] == "localForce" || argv[0] == "localForces")
                return new ElementResponse(this, 3, P);

    else
                return 0;
  }

int XC::ElasticBeam3d::getResponse(int responseID, Information &eleInfo)
  {
    double V;
    const double L = theCoordTransf->getInitialLength();

    switch(responseID)
      {
      case 1: // stiffness
        return eleInfo.setMatrix(getTangentStiff());
      case 2: // global forces
        return eleInfo.setVector(getResistingForce());
      case 3: // local forces
        // Axial
        P(6) =  q.AN2();
        P(0) = q.AN1()+p0[0];

        // Torsion
        P(9) = q.T2();
        P(3) = q.T1();

        // Moments about z and shears along y
        P(5)  = q.Mz1();
        P(11) = q.Mz2();
        V = q.Vy(L);
        P(1) =  V+p0[1];
        P(7) = -V+p0[2];

        // Moments about y and shears along z
        P(4)  = q.My1();
        P(10) = q.My2();
        V = q.Vz(L);
        P(2) = -V+p0[3];
        P(8) =  V+p0[4];
        return eleInfo.setVector(P);
      default:
        return -1;
      }
  }

