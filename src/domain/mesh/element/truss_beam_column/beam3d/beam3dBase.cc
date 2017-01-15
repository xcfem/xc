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
// $Date: 2003/02/14 23:Base:05 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/beam3d/beam3dBase.cpp,v $


// File: ~/model/beam3dBase.C
//
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the class definition for XC::beam3dBase.
// beam3dBase is a 3d beam element. As such it can only
// connect to a node with 6-dof.

#include <domain/mesh/element/truss_beam_column/beam3d/beam3dBase.h>
#include <domain/domain/Domain.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/mesh/node/Node.h>

#include <cmath>
#include <cstdlib>

XC::Matrix XC::beam3dBase::k(12,12);
XC::Matrix XC::beam3dBase::m(12,12);  // these beam members have no mass or damping matrices.
XC::Matrix XC::beam3dBase::d(12,12);


XC::beam3dBase::beam3dBase(int tag, int classTag)
  :ProtoBeam3d(tag,classTag), theta(0),
   L(0.0), rForce(12), isStiffFormed(false)
  { load.reset(12); }


XC::beam3dBase::beam3dBase(int tag, int classTag, double a, double e, double g,
           double jx, double iy, double iz, int Nd1, int Nd2,
           double Theta)

  :ProtoBeam3d(tag,classTag,a,e,g,jx,iy,iz,Nd1,Nd2),
   theta(Theta), L(0.0), rForce(12), isStiffFormed(false)
  {load.reset(12);}

//! @brief Vuelve al último estado consumado del elemento.
int XC::beam3dBase::revertToLastCommit(void)
  {
    return 0; // linear element - nothing to commit
  }

//! @brief Vuelve la matriz de rigidez tangente.
const XC::Matrix &XC::beam3dBase::getTangentStiff(void) const
  { return getStiff(); }

//! @brief Vuelve el valor inicial de la matriz de rigidez tangente.
const XC::Matrix &XC::beam3dBase::getInitialStiff(void) const
  { return getStiff(); }

//! @brief Agrega la carga being passed as parameter.
int XC::beam3dBase::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    std::cerr << "beam3dBase::addLoad() - beam " << this->getTag() << "load type unknown\n";
    return -1;
  }

int XC::beam3dBase::addInertiaLoadToUnbalance(const XC::Vector &accel)
  { return 0; }


const XC::Vector &XC::beam3dBase::getResistingForceIncInertia(void) const
  {
    this->getResistingForce();

    // add rayleigh damping force if factors present
    if(!rayFactors.KNulos())
    rForce += this->getRayleighDampingForces();

    if(isDead()) //XXX Se aplica 2 veces sobre getResistingForce: arreglar.
      rForce*=dead_srf;
    return rForce;
  }

const XC::Vector &XC::beam3dBase::getResistingForce(void) const
  {
    if(!isStiffFormed)
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
    rForce(3) = end1Disp(3);
    rForce(4) = end1Disp(4);
    rForce(5) = end1Disp(5);
    rForce(6) = end2Disp(0);
    rForce(7) = end2Disp(1);
    rForce(8) = end2Disp(2);
    rForce(9) = end2Disp(3);
    rForce(10) = end2Disp(4);
    rForce(11) = end2Disp(5);

    rForce = k * rForce;

    // add any applied load
    rForce -= load;
    if(isDead())
      rForce*=dead_srf;
    return rForce;
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::beam3dBase::sendData(CommParameters &cp)
  {
    int res= ProtoBeam3d::sendData(cp);
    res+= cp.sendDoubles(theta,L,getDbTagData(),CommMetaData(8));
    res+= cp.sendMatrix(k,getDbTagData(),CommMetaData(9));
    res+= cp.sendVector(rForce,getDbTagData(),CommMetaData(10));
    res+= cp.sendBool(isStiffFormed,getDbTagData(),CommMetaData(11));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::beam3dBase::recvData(const CommParameters &cp)
  {
    int res= ProtoBeam3d::recvData(cp);
    res+= cp.receiveDoubles(theta,L,getDbTagData(),CommMetaData(8));
    res+= cp.receiveMatrix(k,getDbTagData(),CommMetaData(9));
    res+= cp.receiveVector(rForce,getDbTagData(),CommMetaData(10));
    res+= cp.receiveBool(isStiffFormed,getDbTagData(),CommMetaData(11));
    return res;
  }

void XC::beam3dBase::Print(std::ostream &s, int flag)
  {
    s << "Element: " << this->getTag();
    s << " type: beam3dBase  iNode: " << theNodes.getTagNode(0);
    s << " jNode: " << theNodes.getTagNode(1) << " Length: " << L << std::endl;
//    s << "\tStiffness XC::Matrix:\n" << k;
    s << "\tResisting Force: " << rForce;
//    s << "\tElemt End Force: " << eForce;
  }






