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
// $Source: /usr/local/cvs/OpenSees/SRC/element/beam2d/beam2d04.cpp,v $


// File: ~/element/beam2d04.C
//
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the class definition for XC::beam2d04.
// beam2d04 is a 2d plane frame bending member. As such it can only
// connect to a node with 3-dof. It only uses the x and y coordinates
// at the nodes, a z coordinate is not used.
//
//                                      5
//        2                             |<
//        |                   .=========+-)-4
//       3|      .=============         | 6
// 1 ---(-+======
//       >|
//
// The interface:
//


#include "domain/mesh/element/truss_beam_column/beam2d/beam2d04.h"
#include <domain/domain/Domain.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/mesh/node/Node.h>

#include <cmath>
#include <cstdlib>

 XC::Matrix XC::beam2d04::k(6,6);
 XC::Matrix XC::beam2d04::trans(6,6);

// beam2d04(int tag, double A, double E, double I, int Nd1, int Nd2);
//        constructor which takes the unique element tag, the elements A,E and
//        I and the node XC::ID's of it's nodal end points.

XC::beam2d04::beam2d04(int tag)
  :beam2d(tag,ELE_TAG_beam2d04),
   rForce(6), isStiffFormed(0) { load.reset(6); }

XC::beam2d04::beam2d04(int tag, double a, double e, double i, int Nd1, int Nd2)
  :beam2d(tag,ELE_TAG_beam2d04,a,e,i,Nd1,Nd2),
   rForce(6), isStiffFormed(0) { load.reset(6); }

//! @brief Constructor virtual.
XC::Element* XC::beam2d04::getCopy(void) const
  { return new XC::beam2d04(*this); }

void XC::beam2d04::formVar(void) const
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

        double dx,dy;
        const XC::Vector &end1Crd = end1Ptr->getCrds();
        const XC::Vector &end2Crd = end2Ptr->getCrds();

        dx = end2Crd(0)-end1Crd(0);
        dy = end2Crd(1)-end1Crd(1);

        L = sqrt(dx*dx + dy*dy);
        double L2 = L*L;
        double L3 = L*L*L;
        if(L == 0.0) {
            std::cerr << "Element: " << this->getTag();
            std::cerr << " XC::beam2d04::formVar: 0 length\n";
            exit(-1);
        }

        cs = dx/L;
        sn = dy/L;

        oneEA = ctes_scc.EA()/L;
        twoEI = 2*ctes_scc.EI()/L;
        fourEI = 4*ctes_scc.EI()/L;
        twelveEI = 12*ctes_scc.EI()/L3;
        sixEI = 6*ctes_scc.EI()/L2;
    }
    isStiffFormed = 1;
  }


int XC::beam2d04::revertToLastCommit()
  {
    return 0; // linear element - nothing to commit
  }

const XC::Matrix &XC::beam2d04::getTangentStiff(void) const
  { return getStiff(); }

const XC::Matrix &XC::beam2d04::getInitialStiff(void) const
  { return getStiff(); }

// const XC::Matrix &getStiff():
//        Method to return the stiffness matrix.

const XC::Matrix &XC::beam2d04::getStiff(void) const
  {
    if(isStiffFormed == 0)
        this->formVar();

    if(cs == 1.0) {
        k(0,0) = oneEA;
        k(1,0) = 0;
        k(2,0) = 0;
        k(3,0) = -oneEA;
        k(4,0) = 0;
        k(5,0) = 0;

        k(0,1) = 0;
        k(1,1) = twelveEI;
        k(2,1)= sixEI;
        k(3,1) = 0;
        k(4,1) = -twelveEI;
        k(5,1) = sixEI;

        k(0,2) = 0;
        k(1,2) = sixEI;
        k(2,2) = fourEI;
        k(3,2) = 0;
        k(4,2) = -sixEI;
        k(5,2) = twoEI;

        k(0,3) = -oneEA;
        k(1,3) = 0;
        k(2,3) = 0;
        k(3,3) = oneEA;
        k(4,3) = 0;
        k(5,3) = 0;

        k(0,4) = 0;
        k(1,4) = -twelveEI;
        k(2,4) = -sixEI;
        k(3,4) = 0;
        k(4,4) = twelveEI;
        k(5,4)  = -sixEI;

        k(0,5) = 0;
        k(1,5) = sixEI;
        k(2,5) = twoEI;
        k(3,5) = 0;
        k(4,5) = -sixEI;
        k(5,5) = fourEI;
    }
    else if(sn == 1.0) {
        k(0,0) = twelveEI;
        k(1,0) = 0;
        k(2,0) = -sixEI;
        k(3,0) = -twelveEI;;
        k(4,0) = 0;
        k(5,0) = -sixEI;;

        k(0,1) = 0;
        k(1,1) = oneEA;
        k(2,1) = 0;
        k(3,1) = 0;
        k(4,1) = -oneEA;
        k(5,1) = 0;

        k(0,2) = -sixEI;
        k(1,2) = 0;
        k(2,2) = fourEI;
        k(3,2) = sixEI;
        k(4,2) = 0;
        k(5,2) = twoEI;

        k(0,3) = -twelveEI;
        k(1,3) = 0;
        k(2,3) = sixEI;
        k(3,3) = twelveEI;
        k(4,3) = 0;
        k(5,3) = sixEI;

        k(0,4) = 0;
        k(1,4) = -oneEA;
        k(2,4) = 0;
        k(3,4) = 0;
        k(4,4) = oneEA;
        k(5,4) = 0;

        k(0,5) = -sixEI;
        k(1,5) = 0;
        k(2,5) = twoEI;
        k(3,5) = sixEI;
        k(4,5) = 0;
        k(5,5) = fourEI;
    }
    else {
        std::cerr << "XC::beam2d04::getStiff - more WORK \n";
        exit(0);
    }

    if(isDead())
      k*=dead_srf;
    return k;
  }



int XC::beam2d04::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    std::cerr << "XC::beam2d04::addLoad() - beam " << this->getTag()
           << ", load type unknown\n";
    return -1;
  }

int XC::beam2d04::addInertiaLoadToUnbalance(const XC::Vector &accel)
  { return 0; }

const XC::Vector &XC::beam2d04::getResistingForceIncInertia(void) const
  {
    this->getResistingForce();

    if(!rayFactors.KNulos())
      rForce += this->getRayleighDampingForces();

    if(isDead())
      rForce*=dead_srf;
    return rForce;
  }




const XC::Vector &XC::beam2d04::getResistingForce(void) const
  {
    this->getStiff();

    // compute the residual Res = k*uTrial
    int Nd1, Nd2;
    Nd1 = theNodes.getTagNode(0);
    Nd2 = theNodes.getTagNode(1);
    Domain *theDomain = this->getDomain();
    Node *end1Ptr = theDomain->getNode(Nd1);
    Node *end2Ptr = theDomain->getNode(Nd2);

    const XC::Vector &end1Disp = end1Ptr->getTrialDisp();
    const XC::Vector &end2Disp = end2Ptr->getTrialDisp();
    rForce(0) = end1Disp(0);
    rForce(1) = end1Disp(1);
    rForce(2) = end1Disp(2);
    rForce(3) = end2Disp(0);
    rForce(4) = end2Disp(1);
    rForce(5) = end2Disp(2);

    rForce = k * rForce;

    // add any applied load
    rForce -= load;

    if(isDead())
      rForce*=dead_srf;
    return rForce;
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::beam2d04::sendData(CommParameters &cp)
  {
    int res= beam2d::sendData(cp);
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::beam2d04::recvData(const CommParameters &cp)
  {
    int res= beam2d::recvData(cp);
    return res;
  }

int XC::beam2d04::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(10);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

int XC::beam2d04::recvSelf(const CommParameters &cp)
  {
    inicComm(10);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::beam2d04::Print(std::ostream &s, int flag)
  {
    s << "\nElement: " << this->getTag() << " Type: beam2d04 ";
    s << "\tConnected Nodes: " << theNodes;
//    s << "\tStiffness XC::Matrix:\n" << k;
    s << "\tResisting Force: " << rForce;
//    s << "\tElemt End Force: " << load;
//    s << "\tElemt End Force: " << eForce;
  }





