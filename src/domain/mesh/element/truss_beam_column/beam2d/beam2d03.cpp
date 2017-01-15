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
// $Source: /usr/local/cvs/OpenSees/SRC/element/beam2d/beam2d03.cpp,v $


// File: ~/element/beam2d03.C
//
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the class definition for XC::beam2d03.
// beam2d03 is a 2d plane frame bending member. As such it can only
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


#include "domain/mesh/element/truss_beam_column/beam2d/beam2d03.h"
#include <domain/domain/Domain.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/mesh/node/Node.h>

#include <cmath>
#include <cstdlib>
#include "utility/actor/actor/MovableVector.h"
#include "utility/actor/actor/MovableMatrix.h"

// beam2d03(int tag, double A, double E, double I, int Nd1, int Nd2);
//        constructor which takes the unique element tag, the elements A,E and
//        I and the node XC::ID's of it's nodal end points.

XC::beam2d03::beam2d03(int tag)
  :beam2d(tag,ELE_TAG_beam2d03),
   k(6,6), rForce(6), trans(6,6) { load.reset(6); }

XC::beam2d03::beam2d03(int tag, double a, double e, double i, int Nd1, int Nd2)
  :beam2d(tag,ELE_TAG_beam2d03,a,e,i,Nd1,Nd2), k(6,6), rForce(6),
   trans(6,6) { load.reset(6); }


//! @brief Constructor virtual.
XC::Element* XC::beam2d03::getCopy(void) const
  { return new XC::beam2d03(*this); }

int XC::beam2d03::revertToLastCommit()
  {
    return 0; // linear element - nothing to commit
  }

void XC::beam2d03::setDomain(Domain *theDomain)
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

    double dx,dy;
    const XC::Vector &end1Crd = theNodes[0]->getCrds();
    const XC::Vector &end2Crd = theNodes[1]->getCrds();

    dx = end2Crd(0)-end1Crd(0);
    dy = end2Crd(1)-end1Crd(1);

    L = sqrt(dx*dx + dy*dy);
    double L2 = L*L;
    double L3 = L*L*L;
    if(L == 0.0) {
      std::cerr << "Element: " << this->getTag();
      std::cerr << " XC::beam2d03::getStiff: 0 length\n";
    }

    cs = dx/L;
    sn = dy/L;

    trans(0,0) = trans(3,3) = cs;
    trans(0,1) = trans(3,4) = sn;
    trans(1,0) = trans(4,3) = -sn;
    trans(1,1) = trans(4,4) = cs;
    trans(2,2) = trans(5,5) = 1.0;

    double oneEA = ctes_scc.EA()/L;
    double twoEI = 2*ctes_scc.EI()/L;
    double fourEI = 4*ctes_scc.EI()/L;
    double twelveEI = 12*ctes_scc.EI()/L3;
    double sixEI = 6*ctes_scc.EI()/L2;

    if(sn == 1.0) {
      k(0,0) = twelveEI;
      k(2,0) = -sixEI;
      k(3,0) = -twelveEI;;
      k(5,0) = -sixEI;;

      k(1,1) = oneEA;
      k(4,1) = -oneEA;

      k(0,2) = -sixEI;
      k(2,2) = fourEI;
      k(3,2) = sixEI;
      k(5,2) = twoEI;

      k(0,3) = -twelveEI;
      k(2,3) = sixEI;
      k(3,3) = twelveEI;
      k(5,3) = sixEI;

      k(1,4) = -oneEA;
      k(4,4) = oneEA;

      k(0,5) = -sixEI;
      k(2,5) = twoEI;
      k(3,5) = sixEI;
      k(5,5) = fourEI;
    }
    else {
      k(0,0) = oneEA;
      k(3,0) = -oneEA;

      k(1,1) = twelveEI;
      k(2,1)= sixEI;
      k(4,1) = -twelveEI;
      k(5,1) = sixEI;

      k(1,2) = sixEI;
      k(2,2) = fourEI;
      k(4,2) = -sixEI;
      k(5,2) = twoEI;

      k(0,3) = -oneEA;
      k(3,3) = oneEA;

      k(1,4) = -twelveEI;
      k(2,4) = -sixEI;
      k(4,4) = twelveEI;
      k(5,4)  = -sixEI;

      k(1,5) = sixEI;
      k(2,5) = twoEI;
      k(4,5) = -sixEI;
      k(5,5) = fourEI;

      if(cs != 1.0)
        k = trans^ k * trans;
    }
  }



const XC::Matrix &XC::beam2d03::getTangentStiff(void) const
  {
    static Matrix K;
    K= k;
    if(isDead())
      K*=dead_srf;
    return k;
  }


const XC::Matrix &XC::beam2d03::getInitialStiff(void) const
  { 
    static Matrix K;
    K= k;
    if(isDead())
      K*=dead_srf;
    return k;
  }

int XC::beam2d03::addLoad(ElementalLoad *theLoad, double loadFactor)
  {

    std::cerr << "XC::beam2d03::addLoad() - beam " << this->getTag()
           << ", load type unknown\n";
    return -1;
  }

int XC::beam2d03::addInertiaLoadToUnbalance(const XC::Vector &accel)
  { return 0; }

const XC::Vector &XC::beam2d03::getResistingForce(void) const
  {
    // compute the residual Res = k*uTrial
    const XC::Vector &end1Disp = theNodes[0]->getTrialDisp();
    const XC::Vector &end2Disp = theNodes[0]->getTrialDisp();
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

const XC::Vector &XC::beam2d03::getResistingForceIncInertia(void) const
  {
    this->getResistingForce();

    if(!rayFactors.KNulos())
        rForce += this->getRayleighDampingForces();

    if(isDead())
      rForce*=dead_srf; //XXX Se aplica 2 veces sobre getResistingForce: arreglar.
    return rForce;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::beam2d03::sendData(CommParameters &cp)
  {
    int res= beam2d::sendData(cp);
    res+= cp.sendMatrix(k,getDbTagData(),CommMetaData(10));
    res+= cp.sendVector(rForce,getDbTagData(),CommMetaData(11));
    res+= cp.sendMatrix(trans,getDbTagData(),CommMetaData(12));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::beam2d03::recvData(const CommParameters &cp)
  {
    int res= beam2d::recvData(cp);
    res+= cp.receiveMatrix(k,getDbTagData(),CommMetaData(10));
    res+= cp.receiveVector(rForce,getDbTagData(),CommMetaData(11));
    res+= cp.receiveMatrix(trans,getDbTagData(),CommMetaData(12));
    return res;
  }

int XC::beam2d03::sendSelf(CommParameters &cp)
  {
    inicComm(13);
    int res= sendData(cp);

    const int dataTag= getDbTag();
    res= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "beam2d03::sendSelf -- could not send data Vector\n";
    return res;
  }

int XC::beam2d03::recvSelf(const CommParameters &cp)
  {
    inicComm(13);
    const int dataTag= getDbTag();
    int res = cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "beam2d03::recvSelf() - failed to send ID data\n";
    else
      res+= recvData(cp);
    return res;
  }



void XC::beam2d03::Print(std::ostream &s, int flag)
  {
  //    s << "\nElement: " << this->getTag() << " Type: beam2d03 ";
  //    s << "\tConnected Nodes: " << connectedExternalNodes ;
//    s << "\tStiffness XC::Matrix:\n" << k;
  //    s << "\tResisting Force: " << rForce;
//    s << "\tElemt End Force: " << eForce;
  }




