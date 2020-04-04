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

// $Revision: 1.13 $
// $Date: 2005/12/15 00:30:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/PDeltaCrdTransf3d.cpp,v $


// Written: Remo Magalhaes de Souza (rmsouza@ce.berkeley.edu)
// Created: 04/2000
// Revision: A
//
// Modified: 04/2005 Andreas Schellenberg (getBasicTrialVel, getBasicTrialAccel)
// 
// Purpose: This file contains the implementation for the 
// PDeltaCrdTransf3d class. PDeltaCrdTransf3d is a linear
// transformation for a planar frame between the global 
// and basic coordinate systems

#include "PDeltaCrdTransf3d.h"

#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <domain/mesh/node/Node.h>
#include "utility/matrix/ID.h"
#include "utility/actor/actor/MovableVector.h"



//! @brief constructor.
XC::PDeltaCrdTransf3d::PDeltaCrdTransf3d(int tag, const XC::Vector &vecInLocXZPlane)
  : XC::SmallDispCrdTransf3d(tag, CRDTR_TAG_PDeltaCrdTransf3d,vecInLocXZPlane), ul17(0.0), ul28(0.0) {}

//! @brief constructor.
XC::PDeltaCrdTransf3d::PDeltaCrdTransf3d(int tag)
  : XC::SmallDispCrdTransf3d(tag, CRDTR_TAG_PDeltaCrdTransf3d), ul17(0), ul28(0) {}

//! @brief constructor.
XC::PDeltaCrdTransf3d::PDeltaCrdTransf3d(int tag, const XC::Vector &vecInLocXZPlane,const XC::Vector &rigJntOffset1,const XC::Vector &rigJntOffset2)
  : XC::SmallDispCrdTransf3d(tag, CRDTR_TAG_PDeltaCrdTransf3d,vecInLocXZPlane), ul17(0), ul28(0)
  {
    // check rigid joint offset for node I
    set_rigid_joint_offsetI(rigJntOffset1);

    // check rigid joint offset for node J
    set_rigid_joint_offsetJ(rigJntOffset2);
  }


// constructor.
// invoked by a FEM_ObjectBroker, recvSelf() needs to be invoked on this object.
XC::PDeltaCrdTransf3d::PDeltaCrdTransf3d(void)
  : XC::SmallDispCrdTransf3d(0, CRDTR_TAG_PDeltaCrdTransf3d), ul17(0), ul28(0) {}


int XC::PDeltaCrdTransf3d::commitState(void)
  { return 0; }


int XC::PDeltaCrdTransf3d::revertToLastCommit(void)
  { return 0; }


int XC::PDeltaCrdTransf3d::revertToStart(void)
  { return 0; }


int XC::PDeltaCrdTransf3d::update(void)
  {
    const XC::Vector &disp1 = nodeIPtr->getTrialDisp();
    const XC::Vector &disp2 = nodeJPtr->getTrialDisp();
    
    static double ug[12];
    inic_ug(disp1,disp2,ug);
    modif_ug_init_disp(ug);

    
    double ul1, ul7, ul2, ul8;
    
    ul1 = R(1,0)*ug[0] + R(1,1)*ug[1] + R(1,2)*ug[2];
    ul2 = R(2,0)*ug[0] + R(2,1)*ug[1] + R(2,2)*ug[2];
    
    ul7 = R(1,0)*ug[6] + R(1,1)*ug[7] + R(1,2)*ug[8];
    ul8 = R(2,0)*ug[6] + R(2,1)*ug[7] + R(2,2)*ug[8];
    
    static double Wu[3];
    
    Wu[0] =  nodeIOffset(2)*ug[4] - nodeIOffset(1)*ug[5];
    Wu[1] = -nodeIOffset(2)*ug[3] + nodeIOffset(0)*ug[5];
    Wu[2] =  nodeIOffset(1)*ug[3] - nodeIOffset(0)*ug[4];
        
    ul1 += R(1,0)*Wu[0] + R(1,1)*Wu[1] + R(1,2)*Wu[2];
    ul2 += R(2,0)*Wu[0] + R(2,1)*Wu[1] + R(2,2)*Wu[2];
    
    Wu[0] =  nodeJOffset(2)*ug[10] - nodeJOffset(1)*ug[11];
    Wu[1] = -nodeJOffset(2)*ug[9]  + nodeJOffset(0)*ug[11];
    Wu[2] =  nodeJOffset(1)*ug[9]  - nodeJOffset(0)*ug[10];
        
    ul7 += R(1,0)*Wu[0] + R(1,1)*Wu[1] + R(1,2)*Wu[2];
    ul8 += R(2,0)*Wu[0] + R(2,1)*Wu[1] + R(2,2)*Wu[2];
    
    ul17 = ul1-ul7;
    ul28 = ul2-ul8;
    
    return 0;
  }

//! @brief Returns the load vector expressed in global coordinates.
const XC::Vector &XC::PDeltaCrdTransf3d::getGlobalResistingForce(const Vector &pb, const Vector &unifLoad) const
  {
    // transform resisting forces from the basic system to local coordinates
    Vector &pl= basic_to_local_resisting_force(pb,unifLoad);

    const double oneOverL = 1.0/L;
    
    // Include leaning column effects (P-Delta)
    double NoverL;
    NoverL= ul17*pb(0)*oneOverL;             
    pl[1] += NoverL;
    pl[7] -= NoverL;
    NoverL= ul28*pb(0)*oneOverL;
    pl[2] += NoverL;
    pl[8] -= NoverL;
    
    // transform resisting forces from local to global coordinates
    return local_to_global_resisting_force(pl);
  }

//! @brief Returns the stiffness matrix expressed in global coordinates.
const XC::Matrix &XC::PDeltaCrdTransf3d::getGlobalStiffMatrix(const XC::Matrix &KB, const XC::Vector &pb) const
  {
    double oneOverL = 1.0/L;
        
    Matrix &kl= basic_to_local_stiff_matrix(KB); // Local stiffness
    // Include geometric stiffness effects in local system
    const double NoverL = pb(0)*oneOverL;
    kl(1,1)+= NoverL;
    kl(2,2)+= NoverL;
    kl(7,7)+= NoverL;
    kl(8,8)+= NoverL;
    kl(1,7)-= NoverL;
    kl(7,1)-= NoverL;
    kl(2,8)-= NoverL;
    kl(8,2)-= NoverL;
    return local_to_global_stiff_matrix(kl);
  }

//! @brief Virtual constructor.
XC::CrdTransf3d *XC::PDeltaCrdTransf3d::getCopy(void) const
  { return new PDeltaCrdTransf3d(*this); }

//! @brief Send object members through the channel being passed as parameter.
int XC::PDeltaCrdTransf3d::sendData(Communicator &comm)
  {
    int res= SmallDispCrdTransf3d::sendData(comm);
    res+= comm.sendDoubles(ul17,ul28,getDbTagData(),CommMetaData(10));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::PDeltaCrdTransf3d::recvData(const Communicator &comm)
  {
    int res= SmallDispCrdTransf3d::recvData(comm);
    res+= comm.receiveDoubles(ul17,ul28,getDbTagData(),CommMetaData(10));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::PDeltaCrdTransf3d::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(11);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::PDeltaCrdTransf3d::recvSelf(const Communicator &comm)
  {
    inicComm(11);
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

const XC::Vector &XC::PDeltaCrdTransf3d::getPointGlobalCoordFromLocal(const Vector &xl) const
  {
    static Vector xg(3);
    
    //xg = nodeIPtr->getCrds() + nodeIOffset;
    xg= nodeIPtr->getCrds();
    
    xg(0)+= nodeIOffset(0);
    xg(1)+= nodeIOffset(1);
    xg(2)+= nodeIOffset(2);
    
    // xg = xg + Rlj'*xl
    //xg.addMatrixTransposeVector(1.0, Rlj, xl, 1.0);
    xg(0)+= R(0,0)*xl(0) + R(1,0)*xl(1) + R(2,0)*xl(2);
    xg(1)+= R(0,1)*xl(0) + R(1,1)*xl(1) + R(2,1)*xl(2);
    xg(2)+= R(0,2)*xl(0) + R(1,2)*xl(1) + R(2,2)*xl(2);
    
    return xg;  
  }


const XC::Vector &XC::PDeltaCrdTransf3d::getPointGlobalDisplFromBasic (double xi, const XC::Vector &uxb) const
  {
    // determine global displacements
    const Vector &disp1 = nodeIPtr->getTrialDisp();
    const Vector &disp2 = nodeJPtr->getTrialDisp();
    
    static double ug[12];
    inic_ug(disp1,disp2,ug);
    modif_ug_init_disp(ug);

    
    // transform global end displacements to local coordinates
    //ul.addMatrixVector(0.0, Tlg,  ug, 1.0);       //  ul = Tlg *  ug;
    static double ul[12];
    
    ul[0]  = R(0,0)*ug[0] + R(0,1)*ug[1] + R(0,2)*ug[2];
    ul[1]  = R(1,0)*ug[0] + R(1,1)*ug[1] + R(1,2)*ug[2];
    ul[2]  = R(2,0)*ug[0] + R(2,1)*ug[1] + R(2,2)*ug[2];
    
    ul[7]  = R(1,0)*ug[6] + R(1,1)*ug[7] + R(1,2)*ug[8];
    ul[8]  = R(2,0)*ug[6] + R(2,1)*ug[7] + R(2,2)*ug[8];
    
    static double Wu[3];
    Wu[0] =  nodeIOffset(2)*ug[4] - nodeIOffset(1)*ug[5];
    Wu[1] = -nodeIOffset(2)*ug[3] + nodeIOffset(0)*ug[5];
    Wu[2] =  nodeIOffset(1)*ug[3] - nodeIOffset(0)*ug[4];
        
    ul[0] += R(0,0)*Wu[0] + R(0,1)*Wu[1] + R(0,2)*Wu[2];
    ul[1] += R(1,0)*Wu[0] + R(1,1)*Wu[1] + R(1,2)*Wu[2];
    ul[2] += R(2,0)*Wu[0] + R(2,1)*Wu[1] + R(2,2)*Wu[2];
    
    Wu[0] =  nodeJOffset(2)*ug[10] - nodeJOffset(1)*ug[11];
    Wu[1] = -nodeJOffset(2)*ug[9]  + nodeJOffset(0)*ug[11];
    Wu[2] =  nodeJOffset(1)*ug[9]  - nodeJOffset(0)*ug[10];
        
    ul[7] += R(1,0)*Wu[0] + R(1,1)*Wu[1] + R(1,2)*Wu[2];
    ul[8] += R(2,0)*Wu[0] + R(2,1)*Wu[1] + R(2,2)*Wu[2];
    
    // compute displacements at point xi, in local coordinates
    static double uxl[3];
    static XC::Vector uxg(3);
    
    uxl[0] = uxb(0) +        ul[0];
    uxl[1] = uxb(1) + (1-xi)*ul[1] + xi*ul[7];
    uxl[2] = uxb(2) + (1-xi)*ul[2] + xi*ul[8];
    
    // rotate displacements to global coordinates
    // uxg = Rlj'*uxl
    //uxg.addMatrixTransposeVector(0.0, Rlj, uxl, 1.0);
    uxg(0) = R(0,0)*uxl[0] + R(1,0)*uxl[1] + R(2,0)*uxl[2];
    uxg(1) = R(0,1)*uxl[0] + R(1,1)*uxl[1] + R(2,1)*uxl[2];
    uxg(2) = R(0,2)*uxl[0] + R(1,2)*uxl[1] + R(2,2)*uxl[2];
    
    return uxg;  
  }


void XC::PDeltaCrdTransf3d::Print(std::ostream &s, int flag) const
  {
    s << "\nCrdTransf: " << this->getTag() << " Type: PDeltaCrdTransf3d" << std::endl;
    s << "\tNode I offset: " << nodeIOffset(0) << " " << nodeIOffset(1) << " "<< nodeIOffset(2) << std::endl;
    s << "\tNode J offset: " << nodeJOffset(0) << " " << nodeJOffset(1) << " "<< nodeJOffset(2) << std::endl;
  }
