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

// $Revision: 1.19 $
// $Date: 2004/10/30 00:05:32 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/dispBeamColumn/DispBeamColumn3d.cpp,v $

// Written: MHS
// Created: Feb 2001
//
// Description: This file contains the class definition for XC::DispBeamColumn3d.

#include <domain/mesh/element/truss_beam_column/dispBeamColumn/DispBeamColumn3d.h>
#include <domain/mesh/node/Node.h>
#include "domain/component/Parameter.h"
#include <material/section/PrismaticBarCrossSection.h>
#include <domain/mesh/element/utils/coordTransformation/CrdTransf3d.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/element/utils/Information.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/recorder/response/ElementResponse.h>
#include "domain/load/beam_loads/BeamMecLoad.h"
#include "material/section/ResponseId.h"
#include "utility/actor/actor/MovableVector.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/BeamIntegration.h"

XC::Matrix XC::DispBeamColumn3d::K(12,12);
XC::Vector XC::DispBeamColumn3d::P(12);
double XC::DispBeamColumn3d::workArea[200];

void XC::DispBeamColumn3d::free_mem(void)
  {
    if(beamIntegration)
      delete beamIntegration;
    beamIntegration= nullptr;
  }

void XC::DispBeamColumn3d::alloc(const BeamIntegration &bi)
  {
    free_mem();
    beamIntegration= bi.getCopy();
  }

XC::DispBeamColumn3d::DispBeamColumn3d(int tag, int nd1, int nd2, int numSec, const std::vector<PrismaticBarCrossSection *> &s, const CrdTransf3d &coordTransf, const BeamIntegration &bi, double r)
  :BeamColumnWithSectionFDTrf3d(tag, ELE_TAG_DispBeamColumn3d,numSec),
   beamIntegration(nullptr), q(6), q0(), p0(), parameterID(0)
  {
    alloc(bi);
    setRho(r);
    load.reset(12);
    setSections(s);
    set_transf(&coordTransf);

    // Set connected external node IDs
    theNodes.set_id_nodes(nd1,nd2);
  }

XC::DispBeamColumn3d::DispBeamColumn3d(int tag, int numSec, const Material *m, const CrdTransf *trf, const BeamIntegration *bi)
  :BeamColumnWithSectionFDTrf3d(tag, ELE_TAG_DispBeamColumn3d,numSec,m,trf),
   beamIntegration(nullptr), q(6), q0(), p0(), parameterID(0)
  {
    if(bi) alloc(*bi);
    load.reset(12);
  }

XC::DispBeamColumn3d::DispBeamColumn3d(int tag)
  :BeamColumnWithSectionFDTrf3d(tag, ELE_TAG_DispBeamColumn3d,0),
   beamIntegration(nullptr), q(6), q0(), p0()
  {
    load.reset(12);
  }

//! @brief Copy constructor.
XC::DispBeamColumn3d::DispBeamColumn3d(const DispBeamColumn3d &other):
  BeamColumnWithSectionFDTrf3d(other), beamIntegration(nullptr), q(other.q), q0(other.q0), p0(other.p0), parameterID(other.parameterID)
  
  {
    if(other.beamIntegration)
      alloc(*other.beamIntegration);
  }

XC::DispBeamColumn3d &XC::DispBeamColumn3d::operator=(const DispBeamColumn3d &other)
  {
    //BeamColumnWithSectionFDTrf3d::operator=(other);
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
XC::DispBeamColumn3d::~DispBeamColumn3d(void)
  { free_mem(); }

//! @brief Virtual constructor.
XC::Element* XC::DispBeamColumn3d::getCopy(void) const
  { return new DispBeamColumn3d(*this); }

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
    int retVal= BeamColumnWithSectionFDTrf3d::revertToLastCommit();

    // Loop over the integration points and revert to last committed state
  const size_t numSections= getNumSections();
    for(size_t i= 0;i<numSections;i++)
      retVal += theSections[i]->revertToLastCommit();
    retVal += theCoordTransf->revertToLastCommit();
    return retVal;
  }

int XC::DispBeamColumn3d::revertToStart(void)
  {
    int retVal= BeamColumnWithSectionFDTrf3d::revertToStart();

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
    const Matrix &pts= beamIntegration->getIntegrPointCoords(numSections,L);

    // Loop over the integration points
    for(size_t i = 0; i < numSections; i++)
      {
        int order= theSections[i]->getOrder();
        const ID &code = theSections[i]->getType();

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

    const double L = theCoordTransf->getInitialLength();
    const Matrix &pts = beamIntegration->getIntegrPointCoords(numSections, L);
    const Vector &wts= beamIntegration->getIntegrPointWeights(numSections, L);
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
    const double L = theCoordTransf->getInitialLength();
    const Matrix &pts= beamIntegration->getIntegrPointCoords(numSections,L);
    const Vector &wts= beamIntegration->getIntegrPointWeights(numSections,L);
    double oneOverL = 1.0/L;

    // Loop over the integration points
    for(size_t i = 0; i < numSections; i++)
      {

      int order = theSections[i]->getOrder();
      const XC::ID &code = theSections[i]->getType();

      Matrix ka(workArea, order, 6);
      ka.Zero();

      double xi6 = 6.0*pts(i,0);

      // Get the section tangent stiffness and stress resultant
      const Matrix &ks = theSections[i]->getInitialTangent();

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

    const double rho= getRho();
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
      std::cerr << getClassName() 
                << "; load over inactive element: "
                << getTag()  
                << std::endl;
    else
      {
        if(BeamMecLoad *beamMecLoad= dynamic_cast<BeamMecLoad *>(theLoad))
          {
            const double L = theCoordTransf->getInitialLength();
            beamMecLoad->addReactionsInBasicSystem({L},loadFactor,p0); // Accumulate reactions in basic system
            beamMecLoad->addFixedEndForcesInBasicSystem({L},loadFactor,q0); // Fixed end forces in basic system
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
    // Check for a quick return
    const double rho= getRho();
    if(rho == 0.0)
      return 0;

    // Get R * accel from the nodes
    const Vector &Raccel1 = theNodes[0]->getRV(accel);
    const Vector &Raccel2 = theNodes[1]->getRV(accel);

    if(6 != Raccel1.Size() || 6 != Raccel2.Size()) {
      std::cerr << "XC::DispBeamColumn3d::addInertiaLoadToUnbalance matrix and vector sizes are incompatible\n";
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
    const double L = theCoordTransf->getInitialLength();
    const size_t numSections= getNumSections();
    const Matrix &pts= beamIntegration->getIntegrPointCoords(numSections,L);
    const Vector &wts= beamIntegration->getIntegrPointWeights(numSections,L);

    // Zero for integration
    q.Zero();

    // Loop over the integration points
    for(size_t i = 0; i < numSections; i++) {

      int order = theSections[i]->getOrder();
      const XC::ID &code = theSections[i]->getType();

      double xi6 = 6.0*pts(i,0);

      // Get section stress resultant
      const Vector &s = theSections[i]->getStressResultant();

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

    const double rho= getRho();
    if(rho != 0.0)
      {
	const Vector &accel1 = theNodes[0]->getTrialAccel();
	const Vector &accel2 = theNodes[1]->getTrialAccel();

	// Compute the current resisting force
	this->getResistingForce();

	const double L = theCoordTransf->getInitialLength();
	const double m = 0.5*rho*L;

	P(0) += m*accel1(0);
	P(1) += m*accel1(1);
	P(2) += m*accel1(2);
	P(6) += m*accel2(0);
	P(7) += m*accel2(1);
	P(8) += m*accel2(2);

	// add the damping forces if rayleigh damping
	if(!rayFactors.nullValues())
	  P+= this->getRayleighDampingForces();
      }
    else
      {
	// add the damping forces if rayleigh damping
	if(!rayFactors.nullKValues())
	  P+= this->getRayleighDampingForces();
      }
    if(isDead())
      P*=dead_srf; //XXX Se aplica 2 veces sobre getResistingForce: arreglar.
    return P;
  }

//! @brief Send members through the communicator argument.
int XC::DispBeamColumn3d::sendData(Communicator &comm)
  {
    int res= BeamColumnWithSectionFDTrf3d::sendData(comm);
    res+= sendBeamIntegrationPtr(beamIntegration,13,14,getDbTagData(),comm);
    res+= comm.sendVector(q,getDbTagData(),CommMetaData(15));
    res+= p0.sendData(comm,getDbTagData(),CommMetaData(16));
    res+= q0.sendData(comm,getDbTagData(),CommMetaData(17));
    res+= comm.sendInt(parameterID,getDbTagData(),CommMetaData(18));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::DispBeamColumn3d::recvData(const Communicator &comm)
  {
    int res= BeamColumnWithSectionFDTrf3d::recvData(comm);
    beamIntegration= receiveBeamIntegrationPtr(beamIntegration,13,14,getDbTagData(),comm);
    res+= comm.receiveVector(q,getDbTagData(),CommMetaData(15));
    res+= p0.receiveData(comm,getDbTagData(),CommMetaData(16));
    res+= q0.receiveData(comm,getDbTagData(),CommMetaData(17));
    res+= comm.receiveInt(parameterID,getDbTagData(),CommMetaData(18));
    return res;
  }

int XC::DispBeamColumn3d::sendSelf(Communicator &comm)
  {
    inicComm(19);
    int res= sendData(comm);

    const int dataTag= getDbTag(comm);
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "NLBeamColumn3d::sendSelf -- failed to send ID data\n";
    return res;
  }

int XC::DispBeamColumn3d::recvSelf(const Communicator &comm)
  {
    inicComm(19);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "NLBeamColumn3d::recvSelf() - failed to recv ID data";
    else
      res+= recvData(comm);
    return res;
  }

void XC::DispBeamColumn3d::Print(std::ostream &s, int flag) const
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
                return new ElementResponse(this, 1, P);

    // local force -
    else if(argv[0] == "localForce" || argv[0] == "localForces")
                return new ElementResponse(this, 2, P);

    // chord rotation -
    else if(argv[0] == "chordRotation" || argv[0] == "chordDeformation"
             || argv[0] == "basicDeformation")
      return new ElementResponse(this, 3, Vector(6));

    // plastic rotation -
    else if(argv[0] == "plasticRotation" || argv[0] == "plasticDeformation")
      return new ElementResponse(this, 4, Vector(6));

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

// AddingSensitivity:BEGIN ///////////////////////////////////

//! @ Set value for parameter.
//!
//! From the parameterID value it should be possible to extract
//! information about:
//!  1) Which parameter is in question. The parameter could
//!     be at element, section, or material level.
//!  2) Which section and material number (tag) it belongs to.
//!
//! To accomplish this the parameterID is given the following value:
//!     parameterID = type + 1000*matrTag + 100000*sectionTag
//! ...where 'type' is an integer in the range (1-99) and added 100
//! for each level (from material to section to element).
//!
//! Example:
//!    If 'E0' (case 2) is random in material #3 of section #5
//!    the value of the parameterID at this (element) level would be:
//!    parameterID = 2 + 1000*3 + 100000*5 = 503002
//!    As seen, all given information can be extracted from this number.
int XC::DispBeamColumn3d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    // Initial declarations
    int ok = -1;
    const size_t numSections= getNumSections();
    const size_t argc= argv.size();

        // If the parameter belongs to the element itself
        if(argv[0] == "rho")
	  {
	    param.setValue(rho);
            return param.addObject(1, this);
	  }
        // If the parameter is belonging to a section or lower
        else if(argv[0] == "section" || argv[0] == "-section")
          {
	    // For now, no parameters of the section itself:
	    if(argc<5)
	      {
		std::cerr << getClassName() << "::" << __FUNCTION__
		          << "For now cannot handle parameters "
		          << "of the section itself." << std::endl;
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
	    if(ok < 0)
	      {
	        std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; could not set parameter. "
			  << std::endl;
	        return -1;
	      }
	    else
	      {
	        // Return the ok value (according to the above comments)
	        return ok;
	      }
	  }
        else if(argv[0] == "integration")
	  {
	    if(argc<2)
	      return -1;
	    else
	      {
		std::vector<std::string> argv2(argv);
		argv2.erase(argv2.begin(),argv2.begin()+1);
	        return beamIntegration->setParameter(argv2, param);
	      }
	  }
        else // Otherwise parameter is unknown for this class
	  { return -1; }
  }

int XC::DispBeamColumn3d::updateParameter(int parameterID, Information &info)
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
                        std::cerr << "XC::DispBeamColumn3d::updateParameter() - could not update parameter. " << std::endl;
                        return ok;
                }
                else {
                        return ok;
                }
        }
        else {
                std::cerr << "XC::DispBeamColumn3d::updateParameter() - could not update parameter. " << std::endl;
                return -1;
        }
}




int
XC::DispBeamColumn3d::activateParameter(int passedParameterID)
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



const XC::Matrix &XC::DispBeamColumn3d::getKiSensitivity(int gradNumber)
  {
        K.Zero();
        return K;
  }

const XC::Matrix &XC::DispBeamColumn3d::getMassSensitivity(int gradNumber)
  {
        K.Zero();
        return K;
  }



const XC::Vector &XC::DispBeamColumn3d::getResistingForceSensitivity(int gradNumber)
  {
    const size_t numSections= getNumSections();
    const double L = theCoordTransf->getInitialLength();
    const Matrix &pts= beamIntegration->getIntegrPointCoords(numSections,L);
    const Vector &wts= beamIntegration->getIntegrPointWeights(numSections,L);
    const double oneOverL = 1.0/L;

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
int XC::DispBeamColumn3d::commitSensitivity(int gradNumber, int numGrads)
{
    // Get basic deformation and sensitivities
        const Vector &v = theCoordTransf->getBasicTrialDisp();

        static Vector vsens(3);
        vsens = theCoordTransf->getBasicDisplSensitivity(gradNumber);

        double L = theCoordTransf->getInitialLength();
        double oneOverL = 1.0/L;
  const size_t numSections= getNumSections();
        const Matrix &pts =  beamIntegration->getIntegrPointCoords(numSections,L);

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

