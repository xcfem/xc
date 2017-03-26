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

// $Revision: 1.10 $
// $Date: 2005/12/15 00:30:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/PDeltaCrdTransf2d.cpp,v $


// Written: Remo Magalhaes de Souza (rmsouza@ce.berkeley.edu)
// Created: 04/2000
// Revision: A
//
// Modified: 04/2005 Andreas Schellenberg (getBasicTrialVel, getBasicTrialAccel)
// 
// Purpose: This file contains the implementation for the 
// PDeltaCrdTransf2d class. PDeltaCrdTransf2d is a linear
// transformation for a planar frame between the global 
// and basic coordinate systems


#include "PDeltaCrdTransf2d.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "utility/matrix/ID.h"
#include <domain/mesh/node/Node.h>
#include "utility/actor/actor/MovableVector.h"



// constructor:
XC::PDeltaCrdTransf2d::PDeltaCrdTransf2d(int tag)
  :SmallDispCrdTransf2d(tag, CRDTR_TAG_PDeltaCrdTransf2d)
  {}


// constructor:
XC::PDeltaCrdTransf2d::PDeltaCrdTransf2d(int tag, const XC::Vector &rigJntOffset1, const XC::Vector &rigJntOffset2)
  :SmallDispCrdTransf2d(tag, CRDTR_TAG_PDeltaCrdTransf2d), ul14(0)
  {
    // check rigid joint offset for node I
    set_rigid_joint_offsetI(rigJntOffset1);

    // check rigid joint offset for node J
    set_rigid_joint_offsetJ(rigJntOffset2);
  }


// constructor:
// invoked by a FEM_ObjectBroker, recvSelf() needs to be invoked on this object.
XC::PDeltaCrdTransf2d::PDeltaCrdTransf2d(void)
  :SmallDispCrdTransf2d(0, CRDTR_TAG_PDeltaCrdTransf2d), ul14(0)
  {}


int XC::PDeltaCrdTransf2d::commitState(void)
  { return 0; }


int XC::PDeltaCrdTransf2d::revertToLastCommit(void)
  { return 0; }


int XC::PDeltaCrdTransf2d::revertToStart(void)
  { return 0; }

int XC::PDeltaCrdTransf2d::update(void)
  {
    static Vector nodeIDisp(3);
    static Vector nodeJDisp(3);
    nodeIDisp = nodeIPtr->getTrialDisp();
    nodeJDisp = nodeJPtr->getTrialDisp();
    
    if(!nodeIInitialDisp.empty())
      {
        for(register int j=0; j<3; j++)
          nodeIDisp(j)-= nodeIInitialDisp[j];
      }
    
    if(!nodeJInitialDisp.empty())
      {
        for(int j=0; j<3; j++)
          nodeJDisp(j)-= nodeJInitialDisp[j];
      }
    
    double ul1;
    double ul4;
    
    ul1 = -sinTheta*nodeIDisp(0) + cosTheta*nodeIDisp(1);
    ul4 = -sinTheta*nodeJDisp(0) + cosTheta*nodeJDisp(1);
    
    const double t12= T12();
    ul1+= t12*nodeIDisp(2);
    
    const double t45= T45();
    ul4+= t45*nodeJDisp(2);
    
    ul14= ul1-ul4;
    
    return 0;
  }


const XC::Vector &XC::PDeltaCrdTransf2d::getGlobalResistingForce(const XC::Vector &pb, const XC::Vector &p0) const
  {
    // transform resisting forces from the basic system to local coordinates
    static double pl[6];
    
    double q0 = pb(0);
    double q1 = pb(1);
    double q2 = pb(2);
    
    double oneOverL = 1.0/L;
    
    double V = oneOverL*(q1+q2);
    pl[0] = -q0;
    pl[1] =  V;
    pl[2] =  q1;
    pl[3] =  q0;
    pl[4] = -V;
    pl[5] =  q2;
    
    // add end forces due to element p0 loads
    pl[0] += p0(0);
    pl[1] += p0(1);
    pl[4] += p0(2);
    
    // Include leaning column effects (P-Delta)
    double NoverL = ul14*q0*oneOverL;             
    pl[1] += NoverL;
    pl[4] -= NoverL;
    
    // transform resisting forces  from local to global coordinates
    static XC::Vector pg(6);
    
    pg(0) = cosTheta*pl[0] - sinTheta*pl[1];
    pg(1) = sinTheta*pl[0] + cosTheta*pl[1];
    
    pg(3) = cosTheta*pl[3] - sinTheta*pl[4];
    pg(4) = sinTheta*pl[3] + cosTheta*pl[4];
    
    pg(2) = pl[2];
    pg(5) = pl[5];        
    
    const double t02= T02();
    const double t12= T12();
        
    pg(2) += t02*pl[0] + t12*pl[1];

    const double t35= T35();
    const double t45= T45();
       
    pg(5) += t35*pl[3] + t45*pl[4];
    
    return pg;
  }


const XC::Matrix &XC::PDeltaCrdTransf2d::getGlobalStiffMatrix(const XC::Matrix &kb, const XC::Vector &pb) const
  {
    static XC::Matrix kg(6,6);
    
    const double oneOverL = 1.0/L;
    
    // Transform basic stiffness to local system
    static Matrix kl(6,6);
    kl(0,0)=  kb(0,0);
    kl(1,0)= -oneOverL*(kb(1,0)+kb(2,0));
    kl(2,0)= -kb(1,0);
    kl(3,0)= -kb(0,0);
    kl(4,0)= -kl(1,0);
    kl(5,0)= -kb(2,0);
    
    kl(0,1)= -oneOverL*(kb(0,1)+kb(0,2));
    kl(1,1)=  oneOverL*oneOverL*(kb(1,1)+kb(1,2)+kb(2,1)+kb(2,2));
    kl(2,1)=  oneOverL*(kb(1,1)+kb(1,2));
    kl(3,1)= -kl(0,1);
    kl(4,1)= -kl(1,1);
    kl(5,1)=  oneOverL*(kb(2,1)+kb(2,2));
    
    kl(0,2)= -kb(0,1);
    kl(1,2)=  oneOverL*(kb(1,1)+kb(2,1));
    kl(2,2)=  kb(1,1);
    kl(3,2)=  kb(0,1);
    kl(4,2)= -kl(1,2);
    kl(5,2)=  kb(2,1);
    
    kl(0,3)= -kl(0,0);
    kl(1,3)= -kl(1,0);
    kl(2,3)= -kl(2,0);
    kl(3,3)= -kl(3,0);
    kl(4,3)= -kl(4,0);
    kl(5,3)= -kl(5,0);
    
    kl(0,4)= -kl(0,1);
    kl(1,4)= -kl(1,1);
    kl(2,4)= -kl(2,1);
    kl(3,4)= -kl(3,1);
    kl(4,4)= -kl(4,1);
    kl(5,4)= -kl(5,1);
    
    kl(0,5)= -kb(0,2);
    kl(1,5)=  oneOverL*(kb(1,2)+kb(2,2));
    kl(2,5)=  kb(1,2);
    kl(3,5)=  kb(0,2);
    kl(4,5)= -kl(1,5);
    kl(5,5)=  kb(2,2);
    
    // Include geometric stiffness effects in local system
    double NoverL = pb(0)*oneOverL;
    kl(1,1)+= NoverL;
    kl(4,4)+= NoverL;
    kl(1,4)-= NoverL;
    kl(4,1)-= NoverL;
    
    const double t02= T02();
    const double t12= T12();
    
    const double t35= T35();
    const double t45= T45();
    
    // Now transform from local to global ... compute kl*T
    static Matrix tmp(6,6);
    tmp(0,0) = kl(0,0)*cosTheta - kl(0,1)*sinTheta;
    tmp(1,0) = kl(1,0)*cosTheta - kl(1,1)*sinTheta;
    tmp(2,0) = kl(2,0)*cosTheta - kl(2,1)*sinTheta;
    tmp(3,0) = kl(3,0)*cosTheta - kl(3,1)*sinTheta;
    tmp(4,0) = kl(4,0)*cosTheta - kl(4,1)*sinTheta;
    tmp(5,0) = kl(5,0)*cosTheta - kl(5,1)*sinTheta;
    
    tmp(0,1) = kl(0,0)*sinTheta + kl(0,1)*cosTheta;
    tmp(1,1) = kl(1,0)*sinTheta + kl(1,1)*cosTheta;
    tmp(2,1) = kl(2,0)*sinTheta + kl(2,1)*cosTheta;
    tmp(3,1) = kl(3,0)*sinTheta + kl(3,1)*cosTheta;
    tmp(4,1) = kl(4,0)*sinTheta + kl(4,1)*cosTheta;
    tmp(5,1) = kl(5,0)*sinTheta + kl(5,1)*cosTheta;
    
    tmp(0,2) = kl(0,0)*t02 + kl(0,1)*t12 + kl(0,2);
    tmp(1,2) = kl(1,0)*t02 + kl(1,1)*t12 + kl(1,2);
    tmp(2,2) = kl(2,0)*t02 + kl(2,1)*t12 + kl(2,2);
    tmp(3,2) = kl(3,0)*t02 + kl(3,1)*t12 + kl(3,2);
    tmp(4,2) = kl(4,0)*t02 + kl(4,1)*t12 + kl(4,2);
    tmp(5,2) = kl(5,0)*t02 + kl(5,1)*t12 + kl(5,2);

    tmp(0,3) = kl(0,3)*cosTheta - kl(0,4)*sinTheta;
    tmp(1,3) = kl(1,3)*cosTheta - kl(1,4)*sinTheta;
    tmp(2,3) = kl(2,3)*cosTheta - kl(2,4)*sinTheta;
    tmp(3,3) = kl(3,3)*cosTheta - kl(3,4)*sinTheta;
    tmp(4,3) = kl(4,3)*cosTheta - kl(4,4)*sinTheta;
    tmp(5,3) = kl(5,3)*cosTheta - kl(5,4)*sinTheta;
    
    tmp(0,4) = kl(0,3)*sinTheta + kl(0,4)*cosTheta;
    tmp(1,4) = kl(1,3)*sinTheta + kl(1,4)*cosTheta;
    tmp(2,4) = kl(2,3)*sinTheta + kl(2,4)*cosTheta;
    tmp(3,4) = kl(3,3)*sinTheta + kl(3,4)*cosTheta;
    tmp(4,4) = kl(4,3)*sinTheta + kl(4,4)*cosTheta;
    tmp(5,4) = kl(5,3)*sinTheta + kl(5,4)*cosTheta;
    
    tmp(0,5) = kl(0,3)*t35 + kl(0,4)*t45 + kl(0,5);
    tmp(1,5) = kl(1,3)*t35 + kl(1,4)*t45 + kl(1,5);
    tmp(2,5) = kl(2,3)*t35 + kl(2,4)*t45 + kl(2,5);
    tmp(3,5) = kl(3,3)*t35 + kl(3,4)*t45 + kl(3,5);
    tmp(4,5) = kl(4,3)*t35 + kl(4,4)*t45 + kl(4,5);
    tmp(5,5) = kl(5,3)*t35 + kl(5,4)*t45 + kl(5,5);
    
    // Now compute T'*(kl*T)
    kg(0,0) = cosTheta*tmp(0,0) - sinTheta*tmp(1,0);
    kg(0,1) = cosTheta*tmp(0,1) - sinTheta*tmp(1,1);
    kg(0,2) = cosTheta*tmp(0,2) - sinTheta*tmp(1,2);
    kg(0,3) = cosTheta*tmp(0,3) - sinTheta*tmp(1,3);
    kg(0,4) = cosTheta*tmp(0,4) - sinTheta*tmp(1,4);
    kg(0,5) = cosTheta*tmp(0,5) - sinTheta*tmp(1,5);
    
    kg(1,0) = sinTheta*tmp(0,0) + cosTheta*tmp(1,0);
    kg(1,1) = sinTheta*tmp(0,1) + cosTheta*tmp(1,1);
    kg(1,2) = sinTheta*tmp(0,2) + cosTheta*tmp(1,2);
    kg(1,3) = sinTheta*tmp(0,3) + cosTheta*tmp(1,3);
    kg(1,4) = sinTheta*tmp(0,4) + cosTheta*tmp(1,4);
    kg(1,5) = sinTheta*tmp(0,5) + cosTheta*tmp(1,5);
    
    kg(2,0) = t02*tmp(0,0) + t12*tmp(1,0) + tmp(2,0);
    kg(2,1) = t02*tmp(0,1) + t12*tmp(1,1) + tmp(2,1);
    kg(2,2) = t02*tmp(0,2) + t12*tmp(1,2) + tmp(2,2);
    kg(2,3) = t02*tmp(0,3) + t12*tmp(1,3) + tmp(2,3);
    kg(2,4) = t02*tmp(0,4) + t12*tmp(1,4) + tmp(2,4);
    kg(2,5) = t02*tmp(0,5) + t12*tmp(1,5) + tmp(2,5);
    
    kg(3,0) = cosTheta*tmp(3,0) - sinTheta*tmp(4,0);
    kg(3,1) = cosTheta*tmp(3,1) - sinTheta*tmp(4,1);
    kg(3,2) = cosTheta*tmp(3,2) - sinTheta*tmp(4,2);
    kg(3,3) = cosTheta*tmp(3,3) - sinTheta*tmp(4,3);
    kg(3,4) = cosTheta*tmp(3,4) - sinTheta*tmp(4,4);
    kg(3,5) = cosTheta*tmp(3,5) - sinTheta*tmp(4,5);
    
    kg(4,0) = sinTheta*tmp(3,0) + cosTheta*tmp(4,0);
    kg(4,1) = sinTheta*tmp(3,1) + cosTheta*tmp(4,1);
    kg(4,2) = sinTheta*tmp(3,2) + cosTheta*tmp(4,2);
    kg(4,3) = sinTheta*tmp(3,3) + cosTheta*tmp(4,3);
    kg(4,4) = sinTheta*tmp(3,4) + cosTheta*tmp(4,4);
    kg(4,5) = sinTheta*tmp(3,5) + cosTheta*tmp(4,5);
    
    kg(5,0) = t35*tmp(3,0) + t45*tmp(4,0) + tmp(5,0);
    kg(5,1) = t35*tmp(3,1) + t45*tmp(4,1) + tmp(5,1);
    kg(5,2) = t35*tmp(3,2) + t45*tmp(4,2) + tmp(5,2);
    kg(5,3) = t35*tmp(3,3) + t45*tmp(4,3) + tmp(5,3);
    kg(5,4) = t35*tmp(3,4) + t45*tmp(4,4) + tmp(5,4);
    kg(5,5) = t35*tmp(3,5) + t45*tmp(4,5) + tmp(5,5);

    return kg;
  }


const XC::Matrix &XC::PDeltaCrdTransf2d::getInitialGlobalStiffMatrix (const XC::Matrix &kb) const
  {
    const double oneOverL = 1.0/L;
    
    const double t02= -T02();
    const double t12= oneOverL*T12() + 1.0;
    const double t22= oneOverL*T12();
    
    const double t05= T35();
    const double t45= T45();
    const double t15= -oneOverL*(t45);
    const double t25= -oneOverL*(t45) + 1.0;
    
    const double sl= sinTheta*oneOverL;
    const double cl= cosTheta*oneOverL;
    
    const bool nodeIOffsetNoNulo= (nodeIOffset.Norm2()>0.0);
    const bool nodeJOffsetNoNulo= (nodeJOffset.Norm2()>0.0);

    static Matrix tmp(6,6);
    tmp(0,0) = -cosTheta*kb(0,0) - sl*(kb(0,1)+kb(0,2));
    tmp(0,1) = -sinTheta*kb(0,0) + cl*(kb(0,1)+kb(0,2));
    tmp(0,2) = (nodeIOffsetNoNulo) ? t02*kb(0,0) + t12*kb(0,1) + t22*kb(0,2) : kb(0,1);
    tmp(0,3) = -tmp(0,0);
    tmp(0,4) = -tmp(0,1);
    tmp(0,5) = (nodeJOffsetNoNulo) ? t05*kb(0,0) + t15*kb(0,1) + t25*kb(0,2) : kb(0,2);
    
    tmp(1,0) = -cosTheta*kb(1,0) - sl*(kb(1,1)+kb(1,2));
    tmp(1,1) = -sinTheta*kb(1,0) + cl*(kb(1,1)+kb(1,2));
    tmp(1,2) = (nodeIOffsetNoNulo) ? t02*kb(1,0) + t12*kb(1,1) + t22*kb(1,2) : kb(1,1);
    tmp(1,3) = -tmp(1,0);
    tmp(1,4) = -tmp(1,1);
    tmp(1,5) = (nodeJOffsetNoNulo) ? t05*kb(1,0) + t15*kb(1,1) + t25*kb(1,2) : kb(1,2);
    
    tmp(2,0) = -cosTheta*kb(2,0) - sl*(kb(2,1)+kb(2,2));
    tmp(2,1) = -sinTheta*kb(2,0) + cl*(kb(2,1)+kb(2,2));
    tmp(2,2) = (nodeIOffsetNoNulo) ? t02*kb(2,0) + t12*kb(2,1) + t22*kb(2,2) : kb(2,1);
    tmp(2,3) = -tmp(2,0);
    tmp(2,4) = -tmp(2,1);
    tmp(2,5) = (nodeJOffsetNoNulo) ? t05*kb(2,0) + t15*kb(2,1) + t25*kb(2,2) : kb(2,2);
    
    static Matrix kg(6,6);
    kg(0,0) = -cosTheta*tmp(0,0) - sl*(tmp(1,0)+tmp(2,0));
    kg(0,1) = -cosTheta*tmp(0,1) - sl*(tmp(1,1)+tmp(2,1));
    kg(0,2) = -cosTheta*tmp(0,2) - sl*(tmp(1,2)+tmp(2,2));
    kg(0,3) = -cosTheta*tmp(0,3) - sl*(tmp(1,3)+tmp(2,3));
    kg(0,4) = -cosTheta*tmp(0,4) - sl*(tmp(1,4)+tmp(2,4));
    kg(0,5) = -cosTheta*tmp(0,5) - sl*(tmp(1,5)+tmp(2,5));
    
    kg(1,0) = -sinTheta*tmp(0,0) + cl*(tmp(1,0)+tmp(2,0));
    kg(1,1) = -sinTheta*tmp(0,1) + cl*(tmp(1,1)+tmp(2,1));
    kg(1,2) = -sinTheta*tmp(0,2) + cl*(tmp(1,2)+tmp(2,2));
    kg(1,3) = -sinTheta*tmp(0,3) + cl*(tmp(1,3)+tmp(2,3));
    kg(1,4) = -sinTheta*tmp(0,4) + cl*(tmp(1,4)+tmp(2,4));
    kg(1,5) = -sinTheta*tmp(0,5) + cl*(tmp(1,5)+tmp(2,5));
    
    kg(2,0) =  t02*tmp(0,0) + t12*tmp(1,0) + t22*tmp(2,0);
    kg(2,1) =  t02*tmp(0,1) + t12*tmp(1,1) + t22*tmp(2,1);
    kg(2,2) =  t02*tmp(0,2) + t12*tmp(1,2) + t22*tmp(2,2);
    kg(2,3) =  t02*tmp(0,3) + t12*tmp(1,3) + t22*tmp(2,3);
    kg(2,4) =  t02*tmp(0,4) + t12*tmp(1,4) + t22*tmp(2,4);
    kg(2,5) =  t02*tmp(0,5) + t12*tmp(1,5) + t22*tmp(2,5);

    
    kg(3,0) = -kg(0,0);
    kg(3,1) = -kg(0,1);
    kg(3,2) = -kg(0,2);
    kg(3,3) = -kg(0,3);
    kg(3,4) = -kg(0,4);
    kg(3,5) = -kg(0,5);
    
    kg(4,0) = -kg(1,0);
    kg(4,1) = -kg(1,1);
    kg(4,2) = -kg(1,2);
    kg(4,3) = -kg(1,3);
    kg(4,4) = -kg(1,4);
    kg(4,5) = -kg(1,5);
    
    kg(5,0) =  t05*tmp(0,0) + t15*tmp(1,0) + t25*tmp(2,0);
    kg(5,1) =  t05*tmp(0,1) + t15*tmp(1,1) + t25*tmp(2,1);
    kg(5,2) =  t05*tmp(0,2) + t15*tmp(1,2) + t25*tmp(2,2);
    kg(5,3) =  t05*tmp(0,3) + t15*tmp(1,3) + t25*tmp(2,3);
    kg(5,4) =  t05*tmp(0,4) + t15*tmp(1,4) + t25*tmp(2,4);
    kg(5,5) =  t05*tmp(0,5) + t15*tmp(1,5) + t25*tmp(2,5);
    
    return kg;
  }


XC::CrdTransf2d *XC::PDeltaCrdTransf2d::getCopy(void) const
  { return new PDeltaCrdTransf2d(*this); }

//! @brief Send object members through the channel being passed as parameter.
int XC::PDeltaCrdTransf2d::sendData(CommParameters &cp)
  {
    int res= SmallDispCrdTransf2d::sendData(cp);
    res+= cp.sendDouble(ul14,getDbTagData(),CommMetaData(10));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::PDeltaCrdTransf2d::recvData(const CommParameters &cp)
  {
    int res= SmallDispCrdTransf2d::recvData(cp);
    res+= cp.receiveDouble(ul14,getDbTagData(),CommMetaData(10));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::PDeltaCrdTransf2d::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(11);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::PDeltaCrdTransf2d::recvSelf(const CommParameters &cp)
  {
    inicComm(11);
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

void XC::PDeltaCrdTransf2d::Print(std::ostream &s, int flag)
  {
    s << "\nCrdTransf: " << this->getTag() << " Type: PDeltaCrdTransf2d";
    s << "\tnodeI Offset: " << nodeIOffset(0) << ' ' << nodeIOffset(1) << std::endl;
    s << "\tnodeJ Offset: " << nodeJOffset(0) << ' ' << nodeJOffset(1) << std::endl;
  }
