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
// $Source: /usr/local/cvs/OpenSees/SRC/element/beam3d/beam3d02.cpp,v $


// File: ~/model/beam3d02.C
//
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the class definition for XC::beam3d02.
// beam3d02 is a 3d beam element. As such it can only
// connect to a node with 6-dof.

#include "beam3d02.h"
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>

#include <cmath>
#include <cstdlib>

//! @brief Constructor.
XC::beam3d02::beam3d02(int tag)
  :beam3dBase(tag,ELE_TAG_beam3d02) {}

//! @brief Constructor.
XC::beam3d02::beam3d02(int tag, double a, double e, double g,
                       double jx, double iy, double iz, int Nd1, int Nd2,
                       double Theta)

  :beam3dBase(tag,ELE_TAG_beam3d02,a,e,g,jx,iy,iz,Nd1,Nd2,Theta) {}

//! @brief Virtual constructor.
XC::Element* XC::beam3d02::getCopy(void) const
  { return new beam3d02(*this); }

//! @brief Method to return the stiffness matrix.
void XC::beam3d02::setDomain(Domain *theDomain)
  {
    beam3dBase::setDomain(theDomain);

    Node *end1Ptr= theNodes[0];
    Node *end2Ptr= theNodes[1];

    const XC::Vector &end1Crd= end1Ptr->getCrds();
    const XC::Vector &end2Crd= end2Ptr->getCrds();

    dx= end2Crd(0)-end1Crd(0);
    dy= end2Crd(1)-end1Crd(1);
    dz= end2Crd(2)-end1Crd(2);

    L= sqrt(dx*dx + dy*dy + dz*dz);
    const double L2 = L*L;
    const double L3 = L*L*L;
    if(L == 0.0)
      {
        std::cerr << "Element: " << this->getTag();
        std::cerr << " XC::beam3d02::getStiff: 0 length\n";
        exit(-1);
      }

    EA = ctes_scc.EA()/L;
    twoE = 2*ctes_scc.E()/L;
    fourE = 4*ctes_scc.E()/L;
    twelveE = 12*ctes_scc.E()/L3;
    sixE = 6*ctes_scc.E()/L2;
  }

const XC::Matrix &XC::beam3d02::getStiff(void) const
  {
    if(dy == 0.0 && dz == 0.0 && dx > 0.0 && theta == 90)
      {
        k(0,0) = EA;
        k(6,0) = -EA;
        k(1,0) = 0;
        k(2,0) = 0;
        k(3,0) = 0;
        k(4,0) = 0;
        k(5,0) = 0;
        k(7,0) = 0;
        k(8,0) = 0;
        k(9,0) = 0;
        k(10,0) = 0;
        k(11,0) = 0;

        k(1,1) = twelveE*ctes_scc.Iz();
        k(5,1) = sixE*ctes_scc.Iz();
        k(7,1) = -twelveE*ctes_scc.Iz();
        k(11,1) = sixE*ctes_scc.Iz();
        k(0,1) = 0;
        k(2,1) = 0;
        k(3,1) = 0;
        k(4,1) = 0;
        k(6,1) = 0;
        k(8,1) = 0;
        k(9,1) = 0;
        k(10,1) = 0;

            k(2,2) = twelveE*ctes_scc.Iy();
            k(4,2) = -sixE*ctes_scc.Iy();
            k(8,2) = -twelveE*ctes_scc.Iy();
            k(10,2) = -sixE*ctes_scc.Iy();
            k(0,2) = 0;
            k(1,2) = 0;
            k(3,2) = 0;
            k(5,2) = 0;
            k(6,2) = 0;
            k(7,2) = 0;
            k(9,2) = 0;
            k(11,2) = 0;

            k(3,3) = ctes_scc.GJ()/L;
            k(9,3) = -ctes_scc.GJ()/L;
            k(0,3) = 0;
            k(1,3) = 0;
            k(2,3) = 0;
            k(4,3) = 0;
            k(5,3) = 0;
            k(6,3) = 0;
            k(7,3) = 0;
            k(8,3) = 0;
            k(10,3) = 0;
            k(11,3) = 0;

            k(2,4) = -sixE*ctes_scc.Iy();
            k(4,4) = fourE*ctes_scc.Iy();
            k(8,4) = sixE*ctes_scc.Iy();
            k(10,4) = twoE*ctes_scc.Iy();
            k(0,4) = 0;
            k(1,4) = 0;
            k(3,4) = 0;
            k(5,4) = 0;
            k(6,4) = 0;
            k(7,4) = 0;
            k(9,4) = 0;
            k(11,4) = 0;

            k(1,5) = sixE*ctes_scc.Iz();
            k(5,5) = fourE*ctes_scc.Iz();
            k(7,5) = -sixE*ctes_scc.Iz();
            k(11,5) = twoE*ctes_scc.Iz();
            k(0,5) = 0;
            k(2,5) = 0;
            k(3,5) = 0;
            k(4,5) = 0;
            k(6,5) = 0;
            k(8,5) = 0;
            k(9,5) = 0;
            k(10,5) = 0;

            k(0,6) = -EA;
            k(6,6) = EA;
            k(1,6) = 0;
            k(2,6) = 0;
            k(3,6) = 0;
            k(4,6) = 0;
            k(5,6) = 0;
            k(7,6) = 0;
            k(8,6) = 0;
            k(9,6) = 0;
            k(10,6) = 0;
            k(11,6) = 0;

            k(1,7) = -twelveE*ctes_scc.Iz();
            k(5,7) = -sixE*ctes_scc.Iz();
            k(7,7) = twelveE*ctes_scc.Iz();
            k(11,7) = -sixE*ctes_scc.Iz();
            k(0,7) = 0;
            k(2,7) = 0;
            k(3,7) = 0;
            k(4,7) = 0;
            k(6,7) = 0;
            k(8,7) = 0;
            k(9,7) = 0;
            k(10,7) = 0;

            k(2,8) = -twelveE*ctes_scc.Iy();
            k(4,8) = sixE*ctes_scc.Iy();
            k(8,8) = twelveE*ctes_scc.Iy();
            k(10,8) = sixE*ctes_scc.Iy();
            k(0,8) = 0;
            k(1,8) = 0;
            k(3,8) = 0;
            k(5,8) = 0;
            k(6,8) = 0;
            k(7,8) = 0;
            k(9,8) = 0;
            k(11,8) = 0;

            k(3,9) = -ctes_scc.GJ()/L;
            k(9,9) = ctes_scc.GJ()/L;
            k(0,9) = 0;
            k(1,9) = 0;
            k(2,9) = 0;
            k(4,9) = 0;
            k(5,9) = 0;
            k(6,9) = 0;
            k(7,9) = 0;
            k(8,9) = 0;
            k(10,9) = 0;
            k(11,9) = 0;

            k(2,10) = -sixE*ctes_scc.Iy();
            k(4,10) = twoE*ctes_scc.Iy();
            k(8,10) = sixE*ctes_scc.Iy();
            k(10,10) = fourE*ctes_scc.Iy();
            k(0,10) = 0;
            k(1,10) = 0;
            k(3,10) = 0;
            k(5,10) = 0;
            k(6,10) = 0;
            k(7,10) = 0;
            k(9,10) = 0;
            k(11,10) = 0;

            k(1,11) = sixE*ctes_scc.Iz();
            k(5,11) = twoE*ctes_scc.Iz();
            k(7,11) = -sixE*ctes_scc.Iz();
            k(11,11) = fourE*ctes_scc.Iz();
            k(0,11) = 0;
            k(2,11) = 0;
            k(3,11) = 0;
            k(4,11) = 0;
            k(6,11) = 0;
            k(8,11) = 0;
            k(9,11) = 0;
            k(10,11) = 0;
        }

        else if(dx == 0.0 && dz == 0.0 && dy > 0.0 && theta == 90) {

            k(0,0) = twelveE*ctes_scc.Iz();
            k(5,0) = -sixE*ctes_scc.Iz();
            k(6,0) = -twelveE*ctes_scc.Iz();
            k(11,0) = -sixE*ctes_scc.Iz();
            k(1,0) = 0;
            k(2,0) = 0;
            k(3,0) = 0;
            k(4,0) = 0;
            k(7,0) = 0;
            k(8,0) = 0;
            k(9,0) = 0;
            k(10,0) = 0;

            k(1,1) = EA;
            k(7,1) = -EA;
            k(0,1) = 0;
            k(2,1) = 0;
            k(3,1) = 0;
            k(4,1) = 0;
            k(5,1) = 0;
            k(6,1) = 0;
            k(8,1) = 0;
            k(9,1) = 0;
            k(10,1) = 0;
            k(11,1) = 0;

            k(2,2) = twelveE*ctes_scc.Iy();
            k(3,2) = sixE*ctes_scc.Iy();
            k(8,2) = -twelveE*ctes_scc.Iy();
            k(9,2) = sixE*ctes_scc.Iy();
            k(0,2) = 0;
            k(1,2) = 0;
            k(4,2) = 0;
            k(5,2) = 0;
            k(6,2) = 0;
            k(7,2) = 0;
            k(10,2) = 0;
            k(11,2) = 0;

            k(2,3) = sixE*ctes_scc.Iy();
            k(3,3) = fourE*ctes_scc.Iy();
            k(8,3) = -sixE*ctes_scc.Iy();
            k(9,3) = twoE*ctes_scc.Iy();
            k(0,3) = 0;
            k(1,3) = 0;
            k(4,3) = 0;
            k(5,3) = 0;
            k(6,3) = 0;
            k(7,3) = 0;
            k(10,3) = 0;
            k(11,3) = 0;

            k(4,4) = ctes_scc.GJ()/L;
            k(10,4) = -ctes_scc.GJ()/L;
            k(0,4) = 0;
            k(1,4) = 0;
            k(2,4) = 0;
            k(3,4) = 0;
            k(5,4) = 0;
            k(6,4) = 0;
            k(7,4) = 0;
            k(8,4) = 0;
            k(9,4) = 0;
            k(11,4) = 0;

            k(0,5) = -sixE*ctes_scc.Iz();
            k(5,5) = fourE*ctes_scc.Iz();
            k(6,5) = sixE*ctes_scc.Iz();
            k(11,5) = twoE*ctes_scc.Iz();
            k(1,5) = 0;
            k(2,5) = 0;
            k(3,5) = 0;
            k(4,5) = 0;
            k(7,5) = 0;
            k(8,5) = 0;
            k(9,5) = 0;
            k(10,5) = 0;

            k(0,6) = -twelveE*ctes_scc.Iz();
            k(5,6) = sixE*ctes_scc.Iz();
            k(6,6) = twelveE*ctes_scc.Iz();
            k(11,6) = sixE*ctes_scc.Iz();
            k(1,6) = 0;
            k(2,6) = 0;
            k(3,6) = 0;
            k(4,6) = 0;
            k(7,6) = 0;
            k(8,6) = 0;
            k(9,6) = 0;
            k(10,6) = 0;

            k(1,7) = -EA;
            k(7,7) = EA;
            k(0,7) = 0;
            k(2,7) = 0;
            k(3,7) = 0;
            k(4,7) = 0;
            k(5,7) = 0;
            k(6,7) = 0;
            k(8,7) = 0;
            k(9,7) = 0;
            k(10,7) = 0;
            k(11,7) = 0;

            k(2,8) = -twelveE*ctes_scc.Iy();
            k(3,8) = -sixE*ctes_scc.Iy();
            k(8,8) = twelveE*ctes_scc.Iy();
            k(9,8) = -sixE*ctes_scc.Iy();
            k(0,8) = 0;
            k(1,8) = 0;
            k(4,8) = 0;
            k(5,8) = 0;
            k(6,8) = 0;
            k(7,8) = 0;
            k(10,8) = 0;
            k(11,8) = 0;

            k(2,9) = sixE*ctes_scc.Iy();
            k(3,9) = twoE*ctes_scc.Iy();
            k(8,9) = -sixE*ctes_scc.Iy();
            k(9,9) = fourE*ctes_scc.Iy();
            k(0,9) = 0;
            k(1,9) = 0;
            k(4,9) = 0;
            k(5,9) = 0;
            k(6,9) = 0;
            k(7,9) = 0;
            k(10,9) = 0;
            k(11,9) = 0;

            k(4,10) = -ctes_scc.GJ()/L;
            k(10,10) = ctes_scc.GJ()/L;
            k(0,10) = 0;
            k(1,10) = 0;
            k(2,10) = 0;
            k(3,10) = 0;
            k(5,10) = 0;
            k(6,10) = 0;
            k(7,10) = 0;
            k(8,10) = 0;
            k(9,10) = 0;
            k(11,10) = 0;

            k(0,11) = -sixE*ctes_scc.Iz();
            k(5,11) = twoE*ctes_scc.Iz();
            k(6,11) = sixE*ctes_scc.Iz();
            k(11,11) = fourE*ctes_scc.Iz();
            k(1,11) = 0;
            k(2,11) = 0;
            k(3,11) = 0;
            k(4,11) = 0;
            k(7,11) = 0;
            k(8,11) = 0;
            k(9,11) = 0;
            k(10,11) = 0;
        }

        else if(dx == 0.0 && dy == 0.0 && dz > 0.0 && theta == 90) {
            // local y of columns in x dirn, local z in y dirn
            k.Zero();
            k(0,0) = twelveE*ctes_scc.Iz();
            k(4,0) = sixE*ctes_scc.Iz();
            k(6,0) = -twelveE*ctes_scc.Iz();
            k(10,0) = sixE*ctes_scc.Iz();

            k(1,1) = twelveE*ctes_scc.Iy();
            k(3,1) = -sixE*ctes_scc.Iy();
            k(7,1) = -twelveE*ctes_scc.Iy();
            k(9,1) = -sixE*ctes_scc.Iy();

            k(2,2) = EA;
            k(8,2) = -EA;

            k(1,3) = -sixE*ctes_scc.Iy();
            k(3,3) = fourE*ctes_scc.Iy();
            k(7,3) = sixE*ctes_scc.Iy();
            k(9,3) = twoE*ctes_scc.Iy();

            k(0,4) = sixE*ctes_scc.Iz();
            k(4,4) = fourE*ctes_scc.Iz();
            k(6,4) = -sixE*ctes_scc.Iz();
            k(10,4) = twoE*ctes_scc.Iz();

            k(5,5) = ctes_scc.GJ()/L;
            k(11,5) = -ctes_scc.GJ()/L;

            k(0,6) = -twelveE*ctes_scc.Iz();
            k(4,6) = -sixE*ctes_scc.Iz();
            k(6,6) = twelveE*ctes_scc.Iz();
            k(10,6) = -sixE*ctes_scc.Iz();

            k(1,7) = -twelveE*ctes_scc.Iy();
            k(3,7) = sixE*ctes_scc.Iy();
            k(7,7) = twelveE*ctes_scc.Iy();
            k(9,7) = sixE*ctes_scc.Iy();

            k(2,8) = -EA;
            k(8,8) = EA;

            k(1,9) = -sixE*ctes_scc.Iy();
            k(3,9) = twoE*ctes_scc.Iy();
            k(7,9) = sixE*ctes_scc.Iy();
            k(9,9) = fourE*ctes_scc.Iy();

            k(0,10) = sixE*ctes_scc.Iz();
            k(4,10) = twoE*ctes_scc.Iz();
            k(6,10) = -sixE*ctes_scc.Iz();
            k(10,10) = fourE*ctes_scc.Iz();

            k(5,11) = -ctes_scc.GJ()/L;
            k(11,11) = ctes_scc.GJ()/L;
        }

        else {

        int Nd1, Nd2;
        Nd1 = theNodes.getTagNode(0);
        Nd2 = theNodes.getTagNode(1);
        Domain *theDomain = this->getDomain();
        Node *end1Ptr = theDomain->getNode(Nd1);
        Node *end2Ptr = theDomain->getNode(Nd2);

        if(end1Ptr == 0) {
            std::cerr << "XC::beam3d02::getStiff: Nd1: ";
            std::cerr << Nd1 << "does not exist in model\n";
            exit(0);
        }
        if(end2Ptr == 0) {
            std::cerr << "XC::beam3d02::getStiff: Nd2: ";
            std::cerr << Nd2 << "does not exist in model\n";
            exit(0);
        }

        std::cerr << "XC::beam3d02::getStiff - NOT FINISHED";
        std::cerr << " members not located along global axis directions\n";
        exit(0);

        }
    if(isDead())
      k*=dead_srf;
    return k;
  }

//! @brief Send object members through the communicator argument.
int XC::beam3d02::sendData(Communicator &comm)
  {
    int res= beam3dBase::sendData(comm);
    res+= comm.sendDoubles(dx,dy,dz,EA,twoE,fourE,getDbTagData(),CommMetaData(13));
    res+= comm.sendDoubles(twelveE,sixE,getDbTagData(),CommMetaData(14));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::beam3d02::recvData(const Communicator &comm)
  {
    int res= beam3dBase::recvData(comm);
    res+= comm.receiveDoubles(dx,dy,dz,EA,twoE,fourE,getDbTagData(),CommMetaData(13));
    res+= comm.receiveDoubles(twelveE,sixE,getDbTagData(),CommMetaData(14));
    return res;
  }

int XC::beam3d02::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(15);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

int XC::beam3d02::recvSelf(const Communicator &comm)
  {
    inicComm(15);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

