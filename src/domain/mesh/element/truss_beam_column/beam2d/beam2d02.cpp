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

// $Revision: 1.5 $
// $Date: 2003/02/14 23:01:05 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/beam2d/beam2d02.cpp,v $


// File: ~/element/beam2d02.C
//
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the implementation for the XC::beam2d02 class.

#include "domain/mesh/element/truss_beam_column/beam2d/beam2d02.h"
#include <domain/domain/Domain.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/mesh/node/Node.h>

#include <domain/mesh/element/utils/coordTransformation/CrdTransf2d.h>

#include "utility/actor/actor/MovableVector.h"

XC::beam2d02::beam2d02(int tag)
  :beam2d(tag,ELE_TAG_beam2d02),M(0.0),
    Kd(3,3), m(6,6),q(3), rForce(6), theCoordTransf(nullptr)
  { load.reset(6); }

// beam2d02(int tag, double A, double E, double I, int Nd1, int Nd2);
//        constructor which takes the unique element tag, the elements A,E and
//        I and the node XC::ID's of it's nodal end points.
XC::beam2d02::beam2d02(int tag, double a, double e, double i, int Nd1, int Nd2,
                   CrdTransf2d &theTrans, double rho)
  :beam2d(tag,ELE_TAG_beam2d02,a,e,i,Nd1,Nd2),
    M(rho), Kd(3,3), m(6,6),
    q(3), rForce(6), theCoordTransf(nullptr)
  {
    load.reset(6);
    theCoordTransf = theTrans.getCopy();
  }


//! @brief Virtual constructor.
XC::Element* XC::beam2d02::getCopy(void) const
  { return new beam2d02(*this); }

//! @brief Destructor.
XC::beam2d02::~beam2d02(void)
  { if(theCoordTransf) delete theCoordTransf; }

//! @brief Returns (if possible) a pointer to the coordinate transformation.
XC::CrdTransf *XC::beam2d02::getCoordTransf(void)
  { return theCoordTransf; }

//! @brief Returns (if possible) a pointer to the coordinate transformation.
const XC::CrdTransf *XC::beam2d02::getCoordTransf(void) const
  { return theCoordTransf; }

void XC::beam2d02::setDomain(Domain *theDomain)
  {
    beam2d::setDomain(theDomain);

    // now verify the number of dof at node ends
    const int Nd1 = theNodes.getTagNode(0);
    const int Nd2 = theNodes.getTagNode(1);
    int dofNd1 = theNodes[0]->getNumberDOF();
    int dofNd2 = theNodes[1]->getNumberDOF();
    if(dofNd1 != 3 && dofNd2 != 3) {
        std::cerr << "WARNING XC::beam2d02::setDomain(): node " << Nd1;
        std::cerr << " and/or node " << Nd2 << " have/has incorrect number ";
        std::cerr << "of dof's at end for beam\n " << *this;
        return;
    }

    // determine length and direction cosines
    double dx,dy;
    const XC::Vector &end1Crd = theNodes[0]->getCrds();
    const XC::Vector &end2Crd = theNodes[1]->getCrds();

    dx = end2Crd(0)-end1Crd(0);
    dy = end2Crd(1)-end1Crd(1);

    L = sqrt(dx*dx + dy*dy);
    if(L == 0.0)
      {
        std::cerr << "WARNING XC::beam2d02::setDomain(): beam " << this->getTag();
        std::cerr << " has zero length for beam\n" << *this;
        return;
      }

    Kd(0,0) = ctes_scc.EA()/L;
    Kd(0,1) = 0.0;
    Kd(0,2) = 0.0;

    const double EIL= ctes_scc.EI()/L;
    Kd(1,0) = 0.0;
    Kd(1,1) = 4.0*EIL;
    Kd(1,2) = 2.0*EIL;

    Kd(2,0) = 0.0;
    Kd(2,1) = 2.0*EIL;
    Kd(2,2) = 4.0*EIL;

    cs = dx/L;
    sn = dy/L;

    // set the mass variable equal to 1/2 the mass of the beam = 0.5 * rho*A*L
    M = 0.5*M*ctes_scc.A()*L;
  }


int XC::beam2d02::commitState()
  {
    int retVal = 0;

    // call element commitState to do any base class stuff
    if((retVal = this->XC::Element::commitState()) != 0) {
      std::cerr << "XC::beam2d02::commitState () - failed in base class";
    }

    retVal = theCoordTransf->commitState();
    return retVal;
  }

int XC::beam2d02::revertToLastCommit()
  { return theCoordTransf->revertToLastCommit(); }

int XC::beam2d02::revertToStart(void)
  { return theCoordTransf->revertToStart(); }

const XC::Matrix &XC::beam2d02::getTangentStiff(void) const
  { return getStiff(); }

const XC::Matrix &XC::beam2d02::getInitialStiff(void) const
  { return getStiff(); }


// const XC::Matrix &getStiff():
//        Method to return the stiffness matrix.

const XC::Matrix &XC::beam2d02::getStiff(void) const
  {
    const XC::Vector &v = theCoordTransf->getBasicTrialDisp();
    q.addMatrixVector(0.0,Kd,v,1.0);
    static Matrix K;
    K= theCoordTransf->getGlobalStiffMatrix(Kd, q);
    if(isDead())
      K*=dead_srf;
    return K;
  }

const XC::Matrix &XC::beam2d02::getMass(void) const
  {
    // lumped mass matrix
    m(0,0)= M;
    m(1,1)= M;
    m(3,3)= M;
    m(4,4)= M;
    if(isDead())
      m*=dead_srf;
    return m;
  }


int XC::beam2d02::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    std::cerr << "XC::beam2d02::addLoad() - beam " << this->getTag()
           << ", does not handle ele loads\n";
    return -1;
  }


int XC::beam2d02::addInertiaLoadToUnbalance(const XC::Vector &accel)
  {
    if(M == 0.0)
      return 0;

    // Get R * accel from the nodes
    const XC::Vector &Raccel1 = theNodes[0]->getRV(accel);
    const XC::Vector &Raccel2 = theNodes[1]->getRV(accel);

    // Want to add ( - fact * M R * accel ) to unbalance
    // Take advantage of lumped mass matrix
    load(0) -= M * Raccel1(0);
    load(1) -= M * Raccel1(1);

    load(3) -= M * Raccel2(0);
    load(4) -= M * Raccel2(1);

    return 0;
  }


const XC::Vector &XC::beam2d02::getResistingForce(void) const
  {
    // compute the residual Res = k*uTrial
    const XC::Vector &v = theCoordTransf->getBasicTrialDisp();
    q.addMatrixVector(0.0,Kd,v,1.0);

    static XC::Vector uniLoad(2);

    rForce = theCoordTransf->getGlobalResistingForce(q, uniLoad);

    // add any applied load
    rForce-= load;
    if(isDead())
      rForce*=dead_srf;
    return rForce;
  }


const XC::Vector &XC::beam2d02::getResistingForceIncInertia(void) const
  {
    this->getResistingForce();

    if(M != 0.0)
      {
        const Vector &end1Accel = theNodes[0]->getTrialAccel();
        const Vector &end2Accel = theNodes[1]->getTrialAccel();
        Vector inertia(6);
        rForce(0) += M*end1Accel(0);
        rForce(1) += M*end1Accel(1);
        rForce(3) += M*end2Accel(0);
        rForce(4) += M*end2Accel(1);
      }

    // add rayleigh damping forces
    if(!rayFactors.nullValues())
      rForce+= this->getRayleighDampingForces();

    if(isDead())
      rForce*=dead_srf; //XXX Applied twice over getResistingForce: to fix.
    return rForce;
  }



//! @brief Send members through the channel being passed as parameter.
int XC::beam2d02::sendData(CommParameters &cp)
  {
    int res= beam2d::sendData(cp);
    res+= cp.sendDoubles(L,sn,cs,getDbTagData(),CommMetaData(10));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::beam2d02::recvData(const CommParameters &cp)
  {
    int res= beam2d::recvData(cp);
    res+= cp.receiveDoubles(L,sn,cs,getDbTagData(),CommMetaData(10));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::beam2d02::sendSelf(CommParameters &cp)
  {
    static ID data(10);
    int res= sendData(cp);

    const int dataTag= getDbTag();
    res= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "beam2d02::sendSelf -- could not send data Vector\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::beam2d02::recvSelf(const CommParameters &cp)
  {
    static ID data(10);
    const int dataTag= getDbTag();
    int res = cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "beam2d02::recvSelf() - failed to send ID data\n";
    else
      res+= recvData(cp);
    return res;
  }

void XC::beam2d02::Print(std::ostream &s, int flag)
  {
    // compute current state
    this->getResistingForce();

    s << "Element: " << this->getTag();
    s << " type: beam2d02  iNode: " << theNodes.getTagNode(0);
    s << " jNode: " << theNodes.getTagNode(1);
    s << " section : " << ctes_scc << std::endl;
    s << " resisting Force: " << rForce;
  }







