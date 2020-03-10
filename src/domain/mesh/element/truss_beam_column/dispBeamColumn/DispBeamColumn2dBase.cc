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

// $Revision: 1.25 $
// $Date: 2004/10/30 00:05:32 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/dispBeamColumn/DispBeamColumn2dBase.cpp,v $

// Written: MHS
// Created: Feb 2001
//
// Description: This file contains the class definition for XC::DispBeamColumn2dBase.

#include "DispBeamColumn2dBase.h"
#include "domain/component/Parameter.h"
#include <domain/mesh/node/Node.h>
#include <material/section/PrismaticBarCrossSection.h>
#include <domain/mesh/element/utils/coordTransformation/CrdTransf2d.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <domain/domain/Domain.h>
#include <cstring>
#include <domain/mesh/element/utils/Information.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/recorder/response/ElementResponse.h>
#include "domain/load/beam_loads/BeamMecLoad.h"
#include "material/section/ResponseId.h"
#include "utility/actor/actor/MovableVector.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/BeamIntegration.h"

 XC::Matrix XC::DispBeamColumn2dBase::K(6,6);
 XC::Vector XC::DispBeamColumn2dBase::P(6);
double XC::DispBeamColumn2dBase::workArea[100];

void XC::DispBeamColumn2dBase::free_mem(void)
  {
    if(beamIntegration)
      delete beamIntegration;
    beamIntegration= nullptr;
  }

void XC::DispBeamColumn2dBase::alloc(const BeamIntegration &bi)
  {
    free_mem();
    beamIntegration= bi.getCopy();
  }

//! @brief Default constructor.
XC::DispBeamColumn2dBase::DispBeamColumn2dBase(int tag, int classTag)
  :BeamColumnWithSectionFDTrf2d(tag, classTag,1), beamIntegration(nullptr), q(3)
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

XC::DispBeamColumn2dBase::DispBeamColumn2dBase(int tag, int classTag, int nd1, int nd2, int numSec,const std::vector<PrismaticBarCrossSection *> &s, const BeamIntegration &bi, const CrdTransf2d &coordTransf, double r)
  : BeamColumnWithSectionFDTrf2d(tag, classTag,numSec), beamIntegration(nullptr), q(3)
  {
    alloc(bi);
    setRho(r);
    load.reset(6);
    setSections(s);
    set_transf(&coordTransf);

    // Set connected external node IDs
    theNodes.set_id_nodes(nd1,nd2);

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

XC::DispBeamColumn2dBase::DispBeamColumn2dBase(int tag, int classTag, int numSec,const Material *m, const BeamIntegration *bi, const CrdTransf *trf)
  :BeamColumnWithSectionFDTrf2d(tag, classTag, numSec,m,trf), beamIntegration(nullptr), q(3)
  {
    if(bi) alloc(*bi);
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

//! @brief Copy constructor.
XC::DispBeamColumn2dBase::DispBeamColumn2dBase(const DispBeamColumn2dBase &other):BeamColumnWithSectionFDTrf2d(other), beamIntegration(nullptr), q(other.q), q0(other.q0), p0(other.p0), parameterID(other.parameterID)
  
  {
    if(other.beamIntegration)
      alloc(*other.beamIntegration);
  }

XC::DispBeamColumn2dBase &XC::DispBeamColumn2dBase::operator=(const DispBeamColumn2dBase &other)
  {
    //BeamColumnWithSectionFDTrf2d::operator=(other);
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; assignment operator must not be called."
              << std::endl;
    q= other.q;
    q0= other.q0;
    p0= other.p0;
    parameterID= other.parameterID;
    if(other.beamIntegration)
      alloc(*other.beamIntegration);
    return *this;
  }

//! @brief Destructor
XC::DispBeamColumn2dBase::~DispBeamColumn2dBase(void)
  { free_mem(); }

int XC::DispBeamColumn2dBase::getNumDOF(void) const
  { return 6; }

void XC::DispBeamColumn2dBase::setDomain(Domain *theDomain)
  {
    // Check XC::Domain is not null - invoked when object removed from a domain
    BeamColumnWithSectionFDTrf2d::setDomain(theDomain);
    if(theDomain)
      {
        int dofNd1 = theNodes[0]->getNumberDOF();
        int dofNd2 = theNodes[1]->getNumberDOF();

        if(dofNd1 != 3 || dofNd2 != 3)
          {
            //std::cerr << getClassName() << "::" << __FUNCTION__
	    //          << "FATAL ERROR (tag: %d), has differing number of DOFs at its nodes",
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

//! @brief Commit the state of the element.
int XC::DispBeamColumn2dBase::commitState()
  {
    int retVal = 0;

    // call element commitState to do any base class stuff
    if((retVal = this->XC::BeamColumnWithSectionFDTrf2d::commitState()) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; failed in base class." << std::endl;
      }

    retVal += theCoordTransf->commitState();

    return retVal;
  }

//! @brief Revert the state of the element to the last committed one.
int XC::DispBeamColumn2dBase::revertToLastCommit()
  {
    int retVal = 0;

  const size_t numSections= getNumSections();
    // Loop over the integration points and revert to last committed state
    for(size_t i = 0; i < numSections; i++)
         retVal += theSections[i]->revertToLastCommit();

    retVal += theCoordTransf->revertToLastCommit();

    return retVal;
  }

int XC::DispBeamColumn2dBase::revertToStart()
{
    int retVal = 0;

  const size_t numSections= getNumSections();
    // Loop over the integration points and revert states to start
    for(size_t i= 0;i<numSections; i++)
      retVal += theSections[i]->revertToStart();

    retVal += theCoordTransf->revertToStart();

    return retVal;
}


const XC::Matrix&XC::DispBeamColumn2dBase::getMass(void) const
  {
    K.Zero();
    const double rho= getRho();
    if(rho == 0.0)
      return K;
    const double L = theCoordTransf->getInitialLength();
    const double m = 0.5*rho*L;
    K(0,0) = K(1,1) = K(3,3) = K(4,4) = m;
    if(isDead())
      K*=dead_srf;
    return K;
  }

void XC::DispBeamColumn2dBase::zeroLoad(void)
  {
    BeamColumnWithSectionFDTrf2d::zeroLoad();

    q0[0] = 0.0;
    q0[1] = 0.0;
    q0[2] = 0.0;

    p0[0] = 0.0;
    p0[1] = 0.0;
    p0[2] = 0.0;
  }

int XC::DispBeamColumn2dBase::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(isDead())
      std::cerr << getClassName() << "::" << __FUNCTION__ 
                << "; load over inactive element: "
                << getTag()  
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
            std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; load type unknown for element with tag: "
                      << this->getTag() << std::endl;
            return -1;
          }
      }
    return 0;
  }

int XC::DispBeamColumn2dBase::addInertiaLoadToUnbalance(const XC::Vector &accel)
  {
    int retval= 0;
    // Check for a quick return
    const double rho= getRho();
    if(rho != 0.0)
      {
        // Get R * accel from the nodes
        const Vector &Raccel1 = theNodes[0]->getRV(accel);
        const Vector &Raccel2 = theNodes[1]->getRV(accel);

        if(3 != Raccel1.Size() || 3 != Raccel2.Size())
	  {
            std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; matrix and vector sizes are incompatible\n";
            retval= -1;
          }
        else
	  {
            const double L = theCoordTransf->getInitialLength();
            const double m = 0.5*rho*L;
            // Want to add ( - fact * M R * accel ) to unbalance
            // Take advantage of lumped mass matrix
            load(0) -= m*Raccel1(0);
            load(1) -= m*Raccel1(1);
            load(3) -= m*Raccel2(0);
            load(4) -= m*Raccel2(1);
	  }
      }
    return 0;
  }

//! @brief Get resisting force including inertia effects.
const XC::Vector &XC::DispBeamColumn2dBase::getResistingForceIncInertia(void) const
  {
    this->getResistingForce();
    const double rho= getRho();
    if(rho != 0.0)
      {
	const Vector &accel1= theNodes[0]->getTrialAccel();
	const Vector &accel2= theNodes[1]->getTrialAccel();

	// Compute the current resisting force
	this->getResistingForce();

	const double L = theCoordTransf->getInitialLength();
	const double m = 0.5*rho*L;

	P(0) += m*accel1(0);
	P(1) += m*accel1(1);
	P(3) += m*accel2(0);
	P(4) += m*accel2(1);

	// add the damping forces if rayleigh damping
	if(!rayFactors.nullValues())
	  P += this->getRayleighDampingForces();

      }
    else
      {
	// add the damping forces if rayleigh damping
	if(!rayFactors.nullKValues())
	  P += this->getRayleighDampingForces();
      }
    if(isDead())
      P*=dead_srf; //XXX Se aplica 2 veces sobre getResistingForce: arreglar.
    return P;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::DispBeamColumn2dBase::sendData(CommParameters &cp)
  {
    int res= BeamColumnWithSectionFDTrf2d::sendData(cp);
    res+= sendBeamIntegrationPtr(beamIntegration,13,14,getDbTagData(),cp);
    res+= cp.sendVector(q,getDbTagData(),CommMetaData(15));
    res+= p0.sendData(cp,getDbTagData(),CommMetaData(16));
    res+= q0.sendData(cp,getDbTagData(),CommMetaData(17));
    res+= cp.sendInt(parameterID,getDbTagData(),CommMetaData(18));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::DispBeamColumn2dBase::recvData(const CommParameters &cp)
  {
    int res= BeamColumnWithSectionFDTrf2d::recvData(cp);
    beamIntegration= receiveBeamIntegrationPtr(beamIntegration,13,14,getDbTagData(),cp);
    res+= cp.receiveVector(q,getDbTagData(),CommMetaData(15));
    res+= p0.receiveData(cp,getDbTagData(),CommMetaData(16));
    res+= q0.receiveData(cp,getDbTagData(),CommMetaData(17));
    res+= cp.receiveInt(parameterID,getDbTagData(),CommMetaData(18));
    return res;
  }

int XC::DispBeamColumn2dBase::sendSelf(CommParameters &cp)
  {
    inicComm(19);
    int res= sendData(cp);

    const int dataTag= getDbTag(cp);
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send ID data\n";
    return res;
  }

int XC::DispBeamColumn2dBase::recvSelf(const CommParameters &cp)
  {
    inicComm(19);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to recv ID data";
    else
      res+= recvData(cp);
    return res;
  }

void XC::DispBeamColumn2dBase::Print(std::ostream &s, int flag)
  {
    s << "\nDispBeamColumn2dBase, element id:  " << this->getTag() << std::endl;
    s << "\tConnected external nodes:  " << theNodes;
    s << "\tCoordTransf: " << theCoordTransf->getTag() << std::endl;
    s << "\tmass density:  " << getRho() << std::endl;

    double L = theCoordTransf->getInitialLength();
    double P  = q(0);
    double M1 = q(1);
    double M2 = q(2);
    double V = (M1+M2)/L;
    s << "\tEnd 1 Forces (P V M): " << -P+p0[0]
      << " " << V+p0[1] << " " << M1 << std::endl;
    s << "\tEnd 2 Forces (P V M): " << P
      << " " << -V+p0[2] << " " << M2 << std::endl;
    
    beamIntegration->Print(s, flag);
    const size_t numSections= getNumSections();
    for (int i = 0; i < numSections; i++)
      theSections[i]->Print(s,flag);
  }


