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
// $Source: /usr/local/cvs/OpenSees/SRC/element/beam3d/beam3d01.cpp,v $


// File: ~/model/beam3d01.C
//
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the class definition for XC::beam3d01.
// beam3d01 is a 3d beam element. As such it can only
// connect to a node with 6-dof.

#include <domain/mesh/element/truss_beam_column/beam3d/beam3d01.h>
#include <domain/domain/Domain.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/mesh/node/Node.h>

#include <cmath>
#include <cstdlib>

XC::beam3d01::beam3d01(int tag)
  :beam3dBase(tag,ELE_TAG_beam3d01){}


XC::beam3d01::beam3d01(int tag, double a, double e, double g,
           double jx, double iy, double iz, int Nd1, int Nd2,
           double Theta)

  :beam3dBase(tag,ELE_TAG_beam3d01,a,e,g,jx,iy,iz,Nd1,Nd2,Theta) {}

//! @brief Virtual constructor.
XC::Element* XC::beam3d01::getCopy(void) const
  { return new beam3d01(*this); }

//! @brief Method to return the stiffness matrix.
const XC::Matrix &XC::beam3d01::getStiff(void) const
  {
    // compute the stiffness
    if(isStiffFormed == 0)
      {
//         // first check element has correct number of DOF at attached nodes
//         Domain *theDomain = this->getDomain();
        assert(theNodes[0]);
        assert(theNodes[1]);
        const Node *end1Ptr= theNodes[0];
        const Node *end2Ptr= theNodes[1];

        double dx,dy,dz;
        const XC::Vector &end1Crd = end1Ptr->getCrds();
        const XC::Vector &end2Crd = end2Ptr->getCrds();

        dx = end2Crd(0)-end1Crd(0);
        dy = end2Crd(1)-end1Crd(1);
        dz = end2Crd(2)-end1Crd(2);

        L= sqrt(dx*dx + dy*dy + dz*dz);
        const double L2 = L*L;
        const double L3 = L*L*L;
        if(L == 0.0)
          {
            std::cerr << "Element: " << this->getTag();
            std::cerr << " beam3d01::getStiff: 0 length\n";
            return k;
          }

        const double EA = ctes_scc.EA()/L;
        const double twoE = 2*ctes_scc.E()/L;
        const double fourE = 4*ctes_scc.E()/L;
        const double twelveE = 12*ctes_scc.E()/L3;
        const double sixE = 6*ctes_scc.E()/L2;

        if(dy == 0.0 && dz == 0.0 && dx > 0.0 && theta == 90)
          {
            // local y in y and local z in z
            k(0,0) = EA;
            k(6,0) = -EA;

            k(1,1) = twelveE*ctes_scc.Iz();
            k(5,1) = sixE*ctes_scc.Iz();
            k(7,1) = -twelveE*ctes_scc.Iz();
            k(11,1) = sixE*ctes_scc.Iz();

            k(2,2) = twelveE*ctes_scc.Iy();
            k(4,2) = -sixE*ctes_scc.Iy();
            k(8,2) = -twelveE*ctes_scc.Iy();
            k(10,2) = -sixE*ctes_scc.Iy();

            k(3,3) = ctes_scc.GJ()/L;
            k(9,3) = -ctes_scc.GJ()/L;

            k(2,4) = -sixE*ctes_scc.Iy();
            k(4,4) = fourE*ctes_scc.Iy();
            k(8,4) = sixE*ctes_scc.Iy();
            k(10,4) = twoE*ctes_scc.Iy();

            k(1,5) = sixE*ctes_scc.Iz();
            k(5,5) = fourE*ctes_scc.Iz();
            k(7,5) = -sixE*ctes_scc.Iz();
            k(11,5) = twoE*ctes_scc.Iz();

            k(0,6) = -EA;
            k(6,6) = EA;

            k(1,7) = -twelveE*ctes_scc.Iz();
            k(5,7) = -sixE*ctes_scc.Iz();
            k(7,7) = twelveE*ctes_scc.Iz();
            k(11,7) = -sixE*ctes_scc.Iz();

            k(2,8) = -twelveE*ctes_scc.Iy();
            k(4,8) = sixE*ctes_scc.Iy();
            k(8,8) = twelveE*ctes_scc.Iy();
            k(10,8) = sixE*ctes_scc.Iy();

            k(3,9) = -ctes_scc.GJ()/L;
            k(9,9) = ctes_scc.GJ()/L;

            k(2,10) = -sixE*ctes_scc.Iy();
            k(4,10) = twoE*ctes_scc.Iy();
            k(8,10) = sixE*ctes_scc.Iy();
            k(10,10) = fourE*ctes_scc.Iy();

            k(1,11) = sixE*ctes_scc.Iz();
            k(5,11) = twoE*ctes_scc.Iz();
            k(7,11) = -sixE*ctes_scc.Iz();
            k(11,11) = fourE*ctes_scc.Iz();
          }
        else
          if(dx == 0.0 && dz == 0.0 && dy > 0.0 && theta == 90)
            {
              k(0,0) = twelveE*ctes_scc.Iz();
              k(5,0) = -sixE*ctes_scc.Iz();
              k(6,0) = -twelveE*ctes_scc.Iz();
              k(11,0) = -sixE*ctes_scc.Iz();

              k(1,1) = EA;
              k(7,1) = -EA;

              k(2,2) = twelveE*ctes_scc.Iy();
              k(3,2) = sixE*ctes_scc.Iy();
              k(8,2) = -twelveE*ctes_scc.Iy();
              k(9,2) = sixE*ctes_scc.Iy();

              k(2,3) = sixE*ctes_scc.Iy();
              k(3,3) = fourE*ctes_scc.Iy();
              k(8,3) = -sixE*ctes_scc.Iy();
              k(9,3) = twoE*ctes_scc.Iy();

              k(4,4) = ctes_scc.GJ()/L;
              k(10,4) = -ctes_scc.GJ()/L;

              k(0,5) = -sixE*ctes_scc.Iz();
              k(5,5) = fourE*ctes_scc.Iz();
              k(6,5) = sixE*ctes_scc.Iz();
              k(11,5) = twoE*ctes_scc.Iz();

              k(0,6) = -twelveE*ctes_scc.Iz();
              k(5,6) = sixE*ctes_scc.Iz();
              k(6,6) = twelveE*ctes_scc.Iz();
              k(11,6) = sixE*ctes_scc.Iz();

              k(1,7) = -EA;
              k(7,7) = EA;

              k(2,8) = -twelveE*ctes_scc.Iy();
              k(3,8) = -sixE*ctes_scc.Iy();
              k(8,8) = twelveE*ctes_scc.Iy();
              k(9,8) = -sixE*ctes_scc.Iy();

              k(2,9) = sixE*ctes_scc.Iy();
              k(3,9) = twoE*ctes_scc.Iy();
              k(8,9) = -sixE*ctes_scc.Iy();
              k(9,9) = fourE*ctes_scc.Iy();

              k(4,10) = -ctes_scc.GJ()/L;
              k(10,10) = ctes_scc.GJ()/L;

              k(0,11) = -sixE*ctes_scc.Iz();
              k(5,11) = twoE*ctes_scc.Iz();
              k(6,11) = sixE*ctes_scc.Iz();
              k(11,11) = fourE*ctes_scc.Iz();
            }
          else
            if(dx == 0.0 && dy == 0.0 && dz > 0.0 && theta == 90)
              { // local y of columns in x dirn, local z in y dirn
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
            else
              {
                std::cerr << "XC::beam3d01::getStiff - NOT FINISHED";
                std::cerr << " members not located along global axis directions\n";
                exit(0);
              }
            isStiffFormed = 1;
      }
    if(isDead())
      k*=dead_srf;
    return k;
  }


//! @brief Sends object through the channel being passed as parameter.
int XC::beam3d01::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::beam3d01::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }








