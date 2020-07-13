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

// $Revision: 1.3 $
// $Date: 2007/10/13 00:16:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/CorotCrdTransf2d.cpp,v $

// Written: Remo Magalhaes de Souza (rmsouza@ce.berkeley.edu)
// Created: 05/2000
// Revision: rms 06/2000 (using Assemble, and AssembleTranspose)
//
// Modified: 04/2005 Andreas Schellenberg (getBasicTrialVel, getBasicTrialAccel)
// 
// Purpose: This file contains the implementation for the 
// CorotCrdTransf2d class. CorotCrdTransf2d is a Corot
// transformation for a planar frame between the global 
// and basic coordinate systems.

/*
 * References
 *

General Formulation and Analytical Response Sensitivity
---
Scott, M. H. and F. C. Filippou (2007).
"Response Gradients for Nonlinear Beam-Column Elements under Large Displacements."
Journal of Structural Engineering, 133(2):155-165.

 *
 */

#include "CorotCrdTransf2d.h"
#include <cmath>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <domain/mesh/node/Node.h>

#include "utility/matrix/ID.h"
#include "utility/actor/actor/MovableVector.h"

// initialize static variables
XC::Matrix XC::CorotCrdTransf2d::Tlg(6,6);
XC::Matrix XC::CorotCrdTransf2d::Tbl(3,6);
XC::Vector XC::CorotCrdTransf2d::uxg(3); 
XC::Vector XC::CorotCrdTransf2d::pg(6); 
XC::Vector XC::CorotCrdTransf2d::dub(3); 
XC::Vector XC::CorotCrdTransf2d::Dub(3); 
XC::Matrix XC::CorotCrdTransf2d::kg(6,6);


//! @brief Constructor.
XC::CorotCrdTransf2d::CorotCrdTransf2d(int tag)
  :CrdTransf2d(tag, CRDTR_TAG_CorotCrdTransf2d),
  cosAlpha(0.0), sinAlpha(0.0),Ln(0), ub(3), ubcommit(3), ubpr(3) {}

// constructor:
XC::CorotCrdTransf2d::CorotCrdTransf2d(int tag, const Vector &rigJntOffsetI,const Vector &rigJntOffsetJ)
  : CrdTransf2d(tag, CRDTR_TAG_CorotCrdTransf2d),cosAlpha(0.0), sinAlpha(0.0),Ln(0), ub(3), ubcommit(3), ubpr(3)
  {
    // check rigid joint offset for node I
    if(rigJntOffsetI.Size() != 2 )
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << ";  invalid rigid joint offset vector for node I.\n"
		  << "Size must be 2.\n";      
        nodeIOffset.Zero();      
      }
    else
      nodeIOffset = rigJntOffsetI;
    
    // check rigid joint offset for node J
    if(rigJntOffsetJ.Size() != 2 )
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; invalid rigid joint offset vector for node J.\n"
		  << "Size must be 2\n";      
        nodeJOffset.Zero(); 
      }
    else
      nodeJOffset = rigJntOffsetJ;
    
    // temporary
    if(nodeIOffset.Norm() != 0 || nodeJOffset.Norm() != 0)
      { nodeOffsets = true; }
    else
      nodeOffsets = false;
  }  

int XC::CorotCrdTransf2d::commitState(void)
  {
    ubcommit = ub;
    return 0;
  }


int XC::CorotCrdTransf2d::revertToLastCommit(void)
  {
    ub = ubcommit;  
    this->update();
    return 0;
  }


int XC::CorotCrdTransf2d::revertToStart(void)
  {
    ub.Zero();
    this->update();
    return 0;
  }


int XC::CorotCrdTransf2d::initialize(Node *nodeIPointer, Node *nodeJPointer)
  {       
    int error;
    
    nodeIPtr= nodeIPointer;
    nodeJPtr= nodeJPointer;
    
    if((!nodeIPtr) || (!nodeJPtr))
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; invalid pointers to the element nodes.\n";
        return -1;
      }
    
    // see if there is some initial displacements at nodes
    if(initialDispChecked == false)
      {
        const Vector &nodeIDisp = nodeIPtr->getDisp();
        for(int i=0; i<3; i++)
          if(nodeIDisp(i) != 0.0)
            {
              nodeIInitialDisp.resize(3);
              for(int j=0; j<3; j++)
                nodeIInitialDisp[j] = nodeIDisp(j);
              i= 3;
            }
            
        const Vector &nodeJDisp = nodeJPtr->getDisp();
        for(int j=0; j<3; j++)
          if(nodeJDisp(j) != 0.0)
            {
              nodeJInitialDisp.resize(3);
              for(int i=0; i<3; i++)
                nodeJInitialDisp[i] = nodeJDisp(i);
              j= 6;
            }
        initialDispChecked= true;
      }

    // get element length and orientation
    if((error = this->compElemtLengthAndOrient()))
      return error;
    return 0;
  }


int XC::CorotCrdTransf2d::update(void)
  {       
    // get global displacements 
    const Vector &dispI= nodeIPtr->getTrialDisp();
    const Vector &dispJ= nodeJPtr->getTrialDisp();
    
    static Vector ug(6);    
    for(int i = 0; i < 3; i++)
      {
        ug(i  ) = dispI(i);
        ug(i+3) = dispJ(i);
      }
    
    if(!nodeIInitialDisp.empty())
      {
        for(int j=0; j<3; j++)
          ug[j] -= nodeIInitialDisp[j];
      }
    
    if(!nodeJInitialDisp.empty())
      {
        for(int j=0; j<3; j++)
          ug[j+3] -= nodeJInitialDisp[j];
      }

    // account for rigid offsets
    if(nodeOffsets == true)
      {
        ug(0) = ug(0) - ug(2) * nodeIOffset(1);
        ug(1) = ug(1) + ug(2) * nodeIOffset(0);
      
        ug(3) = ug(3) - ug(5) * nodeJOffset(1);
        ug(4) = ug(4) + ug(5) * nodeJOffset(0);
      }
    
    // transform global end displacements to local coordinates
    static Vector ul(6);
    
    ul(0) = cosTheta*ug(0) + sinTheta*ug(1);
    ul(1) = cosTheta*ug(1) - sinTheta*ug(0);
    ul(2) = ug(2);
    ul(3) = cosTheta*ug(3) + sinTheta*ug(4);
    ul(4) = cosTheta*ug(4) - sinTheta*ug(3);
    ul(5) = ug(5);
    
    // get deformed element length and orientation with respect to the local system
    int error= this->compElemtLengthAndOrientWRTLocalSystem(ul);
    
    // determine displacements in the basic system eliminating rigid body modes 
    ubpr = ub;
    this->transfLocalDisplsToBasic(ul);
    
    // compute the transformation matrix from local to the basic system
    this->getTransfMatrixBasicLocal(Tbl);
    
    return error;
  }


int XC::CorotCrdTransf2d::compElemtLengthAndOrient(void)
  {
    // element projection
    static Vector dx(2);
    
    if(nodeOffsets) 
      dx = (nodeJPtr->getCrds() + nodeJOffset) - (nodeIPtr->getCrds() + nodeIOffset);  
    else
      dx = nodeJPtr->getCrds() - nodeIPtr->getCrds();  
    
    if(!nodeIInitialDisp.empty())
      {
        dx(0)-= nodeIInitialDisp[0];
        dx(1)-= nodeIInitialDisp[1];
      }
    
    if(!nodeJInitialDisp.empty())
      {
        dx(0)+= nodeJInitialDisp[0];
        dx(1)+= nodeJInitialDisp[1];
      }

    // calculate the element length
    L= dx.Norm();
    
    if(L == 0.0) 
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; 0 length.\n";
        return -2;
      }
    
    // calculate the element local x axis components (direction cosines)
    // wrt to the global coordinates 
    cosTheta = dx(0)/L;
    sinTheta = dx(1)/L;
    return 0;
  }


int XC::CorotCrdTransf2d::compElemtLengthAndOrientWRTLocalSystem(const Vector &ul)
  {
    // get length and chord orientation of the deformed element with respect 
    // to the local coordinate system 
    // (deformed chord corresponding to the basic system)   
    
    double dulx, duly;
    
    dulx = ul(3) - ul(0);           // horizontal relative displacement  
    duly = ul(4) - ul(1);           // vertical relative displacement
    
    Lx= L + dulx;                  // horizontal component of the deformed member
    Ly= duly;                      // vertical component of the deformed member
    
    Ln= sqrt (Lx*Lx + Ly*Ly);      // chord length of the deformed configuration
    
    if(Ln == 0.0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; 0 length.\n";
        return -2;  
      }
    
    cosAlpha= Lx / Ln;             // horizontal projection of the displaced chord 
    sinAlpha= Ly / Ln;             // vertical projection of the displaced chord

    return 0;
  } 


void XC::CorotCrdTransf2d::transfLocalDisplsToBasic(const Vector &ul)
  {
    // Eliminate rigid body modes, determining displacements
    // wrt the basic system    
    const double alpha = atan2(sinAlpha, cosAlpha);
    ub(0) = Ln - L;
    ub(1) = ul(2) - alpha;
    ub(2) = ul(5) - alpha;
  }


void XC::CorotCrdTransf2d::getTransfMatrixLocalGlobal(Matrix &Tlg) const
  {
    // setup transformation matrix
    Tlg.Zero();
    
    Tlg(0,0) = Tlg(3,3) =  cosTheta;           
    Tlg(0,1) = Tlg(3,4) =  sinTheta;
    Tlg(1,0) = Tlg(4,3) = -sinTheta;
    Tlg(1,1) = Tlg(4,4) =  cosTheta;
    Tlg(2,2) = Tlg(5,5) =  1.0;
  }


void XC::CorotCrdTransf2d::getTransfMatrixBasicLocal(Matrix &Tbl) const
  {
    // set up exact force transformation matrix from basic to local coordinates
    Tbl(0,0) = -cosAlpha;      
    Tbl(1,0) = -sinAlpha/Ln;
    Tbl(2,0) = -sinAlpha/Ln;
    
    Tbl(0,1) = -sinAlpha;
    Tbl(1,1) =  cosAlpha/Ln;
    Tbl(2,1) =  cosAlpha/Ln;
    
    Tbl(0,2) =  0;
    Tbl(1,2) =  1;  
    Tbl(2,2) =  0;  
    
    Tbl(0,3) =  cosAlpha;
    Tbl(1,3) =  sinAlpha/Ln;
    Tbl(2,3) =  sinAlpha/Ln;
    
    Tbl(0,4) =  sinAlpha;
    Tbl(1,4) = -cosAlpha/Ln;
    Tbl(2,4) = -cosAlpha/Ln;
    
    Tbl(0,5) =  0;
    Tbl(1,5) =  0; 
    Tbl(2,5) =  1;  
  }


const XC::Vector &XC::CorotCrdTransf2d::getBasicTrialDisp(void) const
  { return ub; }


const XC::Vector &XC::CorotCrdTransf2d::getBasicIncrDeltaDisp(void) const
  {
    // dub = ub - ubpr;
    dub = ub;
    dub.addVector (1.0, ubpr, -1.0);
    return dub;        
  }


const XC::Vector &XC::CorotCrdTransf2d::getBasicIncrDisp(void) const
  {
    // Dub = ub - ubcommit;
    Dub = ub;
    Dub.addVector(1.0, ubcommit, -1.0);
    return Dub;        
  }


const XC::Vector &XC::CorotCrdTransf2d::getBasicTrialVel(void) const
  {
    // determine global velocities
    const Vector &vel1 = nodeIPtr->getTrialVel();
    const Vector &vel2 = nodeJPtr->getTrialVel();
	
    static double vg[6];
    for(int i = 0; i < 3; i++)
      {
	vg[i]   = vel1(i);
	vg[i+3] = vel2(i);
      }
	
    // transform global end velocities to local coordinates
    static Vector vl(6);

    vl(0)= cosTheta*vg[0] + sinTheta*vg[1];
    vl(1)= cosTheta*vg[1] - sinTheta*vg[0];
    vl(2)= vg[2];
    vl(3)= cosTheta*vg[3] + sinTheta*vg[4];
    vl(4)= cosTheta*vg[4] - sinTheta*vg[3];
    vl(5)= vg[5];

    Lxdot= vl(3) - vl(0);
    Lydot= vl(4) - vl(1);

    // transform local velocities to basic coordinates
    static Vector vb(3);
	
    vb(0)= (Lx*Lxdot + Ly*Lydot)/Ln;
    vb(1)= vl(2) - (Lx*Lydot - Ly*Lxdot)/pow(Ln,2);
    vb(2)= vl(5) + vb(1) - vl(2);
    return vb;
  }


const XC::Vector &XC::CorotCrdTransf2d::getBasicTrialAccel(void) const
  {
    // determine global velocities
    const Vector &vel1 = nodeIPtr->getTrialVel();
    const Vector &vel2 = nodeJPtr->getTrialVel();
    
    static double vg[6];
    int i;
    for(i = 0; i < 3; i++)
      {
    	vg[i]   = vel1(i);
    	vg[i+3] = vel2(i);
      }
    
    // transform global end velocities to local coordinates
    static Vector vl(6);

    vl(0) = cosTheta*vg[0] + sinTheta*vg[1];
    vl(1) = cosTheta*vg[1] - sinTheta*vg[0];
    vl(2) = vg[2];
    vl(3) = cosTheta*vg[3] + sinTheta*vg[4];
    vl(4) = cosTheta*vg[4] - sinTheta*vg[3];
    vl(5) = vg[5];

    Lxdot = vl(3) - vl(0);
    Lydot = vl(4) - vl(1);
    
    // determine global accelerations
    const Vector &accel1 = nodeIPtr->getTrialAccel();
    const Vector &accel2 = nodeJPtr->getTrialAccel();
    
    static double ag[6];
    for(i = 0; i < 3; i++)
      {
    	ag[i]   = accel1(i);
    	ag[i+3] = accel2(i);
      }
    
    // transform global end accelerations to local coordinates
    static Vector al(6);

    al(0) = cosTheta*ag[0] + sinTheta*ag[1];
    al(1) = cosTheta*ag[1] - sinTheta*ag[0];
    al(2) = ag[2];
    al(3) = cosTheta*ag[3] + sinTheta*ag[4];
    al(4) = cosTheta*ag[4] - sinTheta*ag[3];
    al(5) = ag[5];

    Lxdotdot = al(3) - al(0);
    Lydotdot = al(4) - al(1);

    // transform local accelerations to basic coordinates
    static Vector ab(3);
    
    ab(0) = (Lxdot*Lxdot + Lx*Lxdotdot + Ly*Lydotdot + Lydot*Lydot)/Ln
          - pow(Lx*Lxdot + Ly*Lydot,2)/pow(Ln,3);
    ab(1) = al(2) - (Lx*Lydotdot - Ly*Lxdotdot)/pow(Ln,2)
          + 2*(Lx*Lydot - Ly*Lxdot)*(Lx*Lxdot + Ly*Lydot)/pow(Ln,4);
    ab(2) = al(5) + ab(1) - al(2);
    
    return ab;
  }


const XC::Vector &XC::CorotCrdTransf2d::getGlobalResistingForce(const Vector &pb, const Vector &unifLoad) const
  {
    
    // transform resisting forces from the basic system to local coordinates
    this->getTransfMatrixBasicLocal(Tbl);
    static Vector pl(6);
    pl.addMatrixTransposeVector(0.0, Tbl, pb, 1.0);    // pl = Tbl ^ pb;
    
    // add end forces due to element unifLoad loads
    // This assumes member loads are in local system
    pl(0) += unifLoad(0);
    pl(1) += unifLoad(1);
    pl(4) += unifLoad(2);

    /*     // This assumes member loads are in basic system
    pl(0) += unifLoad(0)*cosAlpha - unifLoad(1)*sinAlpha;
    pl(1) += unifLoad(0)*sinAlpha + unifLoad(1)*cosAlpha;
    pl(3) -= unifLoad(2)*sinAlpha;
    pl(4) += unifLoad(2)*cosAlpha;
    */
    
    // transform resisting forces  from local to global coordinates
    //this->getTransfMatrixLocalGlobal(Tlg);     // OPTIMIZE LATER
    //pg.addMatrixTransposeVector(0.0, Tlg, pl, 1.0);   // pg = Tlg ^ pl; residual

    pg(0)= cosTheta*pl[0] - sinTheta*pl[1];
    pg(1)= sinTheta*pl[0] + cosTheta*pl[1];
    
    pg(3)= cosTheta*pl[3] - sinTheta*pl[4];
    pg(4)= sinTheta*pl[3] + cosTheta*pl[4];
    
    pg(2)= pl[2];
    pg(5)= pl[5];

    // account for rigid offsets
    if(nodeOffsets == true)
      {
        pg(2) += -pg(0) * nodeIOffset(1) + pg(1) * nodeIOffset(0);
        pg(5) += -pg(3) * nodeJOffset(1) + pg(4) * nodeJOffset(0);
      }
    return pg;
  }


const XC::Matrix &XC::CorotCrdTransf2d::getGlobalStiffMatrix(const Matrix &kb, const Vector &pb) const
  {
    // transform tangent stiffness matrix from the basic system to local coordinates
    static Matrix kl(6,6);
    this->getTransfMatrixBasicLocal(Tbl);
    kl.addMatrixTripleProduct(0.0, Tbl, kb, 1.0);      // kl = Tbl ^ kb * Tbl;
    
    // add geometric stiffness matrix
    kl.addMatrix(1.0, this->getGeomStiffMatrix(pb), 1.0);
    
    // transform tangent  stiffness matrix from local to global coordinates
    
    // kg.addMatrixTripleProduct(0.0, Tlg, kl, 1.0);
    double s2, c2, cs;  
    
    s2 = sinTheta*sinTheta;
    c2 = cosTheta*cosTheta;
    cs = sinTheta*cosTheta;
    
    double k11, k12, k13, k21, k22, k23, k31, k32, k33;
    
    k11 = kl(0,0);    k12 = kl(0,1);    k13 = kl(0,2);
    k21 = kl(1,0);    k22 = kl(1,1);    k23 = kl(1,2);
    k31 = kl(2,0);    k32 = kl(2,1);    k33 = kl(2,2);
    
    kg(0,0) = c2*k11+s2*k22-cs*(k21+k12); 
    kg(1,0) = c2*k21-s2*k12+cs*(k11-k22);
    kg(2,0) = cosTheta*k31-sinTheta*k32;
    
    kg(0,1) = c2*k12-s2*k21+cs*(k11-k22);
    kg(1,1) = c2*k22+s2*k11+cs*(k21+k12);
    kg(2,1) = sinTheta*k31+cosTheta*k32;
    
    kg(0,2) = cosTheta*k13 - sinTheta*k23;
    kg(1,2) = sinTheta*k13 + cosTheta*k23;
    kg(2,2) = k33;
    
    k11 = kl(0,3);    k12 = kl(0,4);    k13 = kl(0,5);
    k21 = kl(1,3);    k22 = kl(1,4);    k23 = kl(1,5);
    k31 = kl(2,3);    k32 = kl(2,4);    k33 = kl(2,5);
    
    kg(0,3) = c2*k11+s2*k22-cs*(k21+k12); 
    kg(1,3) = c2*k21-s2*k12+cs*(k11-k22);
    kg(2,3) = cosTheta*k31-sinTheta*k32;
    
    kg(0,4) = c2*k12-s2*k21+cs*(k11-k22);
    kg(1,4) = c2*k22+s2*k11+cs*(k21+k12);
    kg(2,4) = sinTheta*k31+cosTheta*k32;
    
    kg(0,5) = cosTheta*k13 - sinTheta*k23;
    kg(1,5) = sinTheta*k13 + cosTheta*k23;
    kg(2,5) = k33;
    
    k11 = kl(3,0);    k12 = kl(3,1);    k13 = kl(3,2);
    k21 = kl(4,0);    k22 = kl(4,1);    k23 = kl(4,2);
    k31 = kl(5,0);    k32 = kl(5,1);    k33 = kl(5,2);
    
    kg(3,0) = c2*k11+s2*k22-cs*(k21+k12); 
    kg(4,0) = c2*k21-s2*k12+cs*(k11-k22);
    kg(5,0) = cosTheta*k31-sinTheta*k32;
    
    kg(3,1) = c2*k12-s2*k21+cs*(k11-k22);
    kg(4,1) = c2*k22+s2*k11+cs*(k21+k12);
    kg(5,1) = sinTheta*k31+cosTheta*k32;
    
    kg(3,2) = cosTheta*k13 - sinTheta*k23;
    kg(4,2) = sinTheta*k13 + cosTheta*k23;
    kg(5,2) = k33;
    
    k11 = kl(3,3);    k12 = kl(3,4);    k13 = kl(3,5);
    k21 = kl(4,3);    k22 = kl(4,4);    k23 = kl(4,5);
    k31 = kl(5,3);    k32 = kl(5,4);    k33 = kl(5,5);
    
    kg(3,3) = c2*k11+s2*k22-cs*(k21+k12); 
    kg(4,3) = c2*k21-s2*k12+cs*(k11-k22);
    kg(5,3) = cosTheta*k31-sinTheta*k32;
    
    kg(3,4) = c2*k12-s2*k21+cs*(k11-k22);
    kg(4,4) = c2*k22+s2*k11+cs*(k21+k12);
    kg(5,4) = sinTheta*k31+cosTheta*k32;
    
    kg(3,5) = cosTheta*k13 - sinTheta*k23;
    kg(4,5) = sinTheta*k13 + cosTheta*k23;
    kg(5,5) = k33;


    if(nodeOffsets == true)
      {
        const double &X1= nodeIOffset(0);
        const double &Y1= nodeIOffset(1);
        const double &X2= nodeJOffset(0);
        const double &Y2= nodeJOffset(1);

        const double &k11= kg(0,0);

        const double &k12= kg(0,1);
        const double &k22= kg(1,1);

        const double &k13= kg(0,2);
        const double &k23= kg(1,2);
        const double &k33= kg(2,2);

        const double &k14= kg(0,3);
        const double &k24= kg(1,3);
        const double &k34= kg(2,3);
        const double &k44= kg(3,3);

        const double &k15= kg(0,4);
        const double &k25= kg(1,4);
        const double &k35= kg(2,4);
        const double &k45= kg(3,4);
        const double &k55= kg(3,4);

        const double &k16= kg(0,5);
        const double &k26= kg(1,5);
        const double &k36= kg(2,5);
        const double &k46= kg(3,5);
        const double &k56= kg(4,5);
        const double &k66= kg(5,5);


        const double &K13= -k11*Y1 + k12*X1 + k13;
        const double &K23= -k12*Y1 + k22*X1 + k23;
        kg(0,2)= K13;
        kg(2,0)= K13;
        kg(1,2)= K23;
        kg(2,1)= K23;
        kg(2,2)= -Y1*K13 + X1*K23 -k13*Y1 + k23*X1 + k33;
      

        const double &K16= -k14*Y2 + k15*X2 + k16;
        const double &K26= -k24*Y2 + k25*X2 + k26;
        kg(0,5)= K16;
        kg(5,0)= K16;
        kg(1,5)= K26;
        kg(5,1)= K26;
        kg(2,5)= -Y2*K16 + X2*K26 -k16*Y1 + k26*X1 + k36;      
        kg(5,2)= kg(2,5);

        const double &K46= -k44*Y2 + k45*X2 + k46;
        const double &K56= -k45*Y2 + k55*X2 + k56;
        kg(3,5)= K46;
        kg(5,3)= K46;
        kg(4,5)= K56;
        kg(5,4)= K56;
        kg(5,5)= -Y2*K46 + X2*K56 -k46*Y2 + k56*X2 + k66;      

        const double &K34= -k14*Y1 + k24*X1 + k34;
        const double &K35= -k15*Y1 + k25*X1 + k35;
        kg(2,3)= K34;
        kg(3,2)= K34;
        kg(2,4)= K35;
        kg(4,2)= K35;
      }
    return kg;
  }


const XC::Matrix &XC::CorotCrdTransf2d::getInitialGlobalStiffMatrix(const Matrix &kb) const
  {
    // transform tangent stiffness matrix from the basic system to local coordinates
    static Matrix kl(6,6);
    static Matrix T(3,6);
    
    T(0,0)= -1.0;
    T(1,0)= 0;
    T(2,0)= 0;
    
    T(0,1)= 0;
    T(1,1)= 1/L;
    T(2,1)= 1/L;
    
    T(0,2)=  0;
    T(1,2)=  1;  
    T(2,2)=  0;  
    
    T(0,3)=  1;
    T(1,3)=  0;
    T(2,3)=  0;
    
    T(0,4)=  0;
    T(1,4)= -1/L;
    T(2,4)= -1/L;
    
    T(0,5)=  0;
    T(1,5)=  0; 
    T(2,5)=  1;  
    
    kl.addMatrixTripleProduct(0.0, T, kb, 1.0);      // kl= Tbl ^ kb * Tbl;
    
    // add geometric stiffness matrix
    // kl.addMatrix(1.0, this->getGeomStiffMatrix(pb), 1.0);
    
    // transform tangent  stiffness matrix from local to global coordinates
    
    // kg.addMatrixTripleProduct(0.0, Tlg, kl, 1.0);
    double s2, c2, cs;  
    
    s2= sinTheta*sinTheta;
    c2= cosTheta*cosTheta;
    cs= sinTheta*cosTheta;
    
    double k11, k12, k13, k21, k22, k23, k31, k32, k33;
    
    k11= kl(0,0);    k12= kl(0,1);    k13= kl(0,2);
    k21= kl(1,0);    k22= kl(1,1);    k23= kl(1,2);
    k31= kl(2,0);    k32= kl(2,1);    k33= kl(2,2);
    
    kg(0,0)= c2*k11+s2*k22-cs*(k21+k12); 
    kg(1,0)= c2*k21-s2*k12+cs*(k11-k22);
    kg(2,0)= cosTheta*k31-sinTheta*k32;
    
    kg(0,1)= c2*k12-s2*k21+cs*(k11-k22);
    kg(1,1)= c2*k22+s2*k11+cs*(k21+k12);
    kg(2,1)= sinTheta*k31+cosTheta*k32;
    
    kg(0,2)= cosTheta*k13 - sinTheta*k23;
    kg(1,2)= sinTheta*k13 + cosTheta*k23;
    kg(2,2)= k33;
    
    k11= kl(0,3);    k12= kl(0,4);    k13= kl(0,5);
    k21= kl(1,3);    k22= kl(1,4);    k23= kl(1,5);
    k31= kl(2,3);    k32= kl(2,4);    k33= kl(2,5);
    
    kg(0,3)= c2*k11+s2*k22-cs*(k21+k12); 
    kg(1,3)= c2*k21-s2*k12+cs*(k11-k22);
    kg(2,3)= cosTheta*k31-sinTheta*k32;
    
    kg(0,4)= c2*k12-s2*k21+cs*(k11-k22);
    kg(1,4)= c2*k22+s2*k11+cs*(k21+k12);
    kg(2,4)= sinTheta*k31+cosTheta*k32;
    
    kg(0,5)= cosTheta*k13 - sinTheta*k23;
    kg(1,5)= sinTheta*k13 + cosTheta*k23;
    kg(2,5)= k33;
    
    k11= kl(3,0);    k12= kl(3,1);    k13= kl(3,2);
    k21= kl(4,0);    k22= kl(4,1);    k23= kl(4,2);
    k31= kl(5,0);    k32= kl(5,1);    k33= kl(5,2);
    
    kg(3,0)= c2*k11+s2*k22-cs*(k21+k12); 
    kg(4,0)= c2*k21-s2*k12+cs*(k11-k22);
    kg(5,0)= cosTheta*k31-sinTheta*k32;
    
    kg(3,1)= c2*k12-s2*k21+cs*(k11-k22);
    kg(4,1)= c2*k22+s2*k11+cs*(k21+k12);
    kg(5,1)= sinTheta*k31+cosTheta*k32;
    
    kg(3,2)= cosTheta*k13 - sinTheta*k23;
    kg(4,2)= sinTheta*k13 + cosTheta*k23;
    kg(5,2)= k33;
    
    k11= kl(3,3);    k12= kl(3,4);    k13= kl(3,5);
    k21= kl(4,3);    k22= kl(4,4);    k23= kl(4,5);
    k31= kl(5,3);    k32= kl(5,4);    k33= kl(5,5);
    
    kg(3,3)= c2*k11+s2*k22-cs*(k21+k12); 
    kg(4,3)= c2*k21-s2*k12+cs*(k11-k22);
    kg(5,3)= cosTheta*k31-sinTheta*k32;
    
    kg(3,4)= c2*k12-s2*k21+cs*(k11-k22);
    kg(4,4)= c2*k22+s2*k11+cs*(k21+k12);
    kg(5,4)= sinTheta*k31+cosTheta*k32;
    
    kg(3,5)= cosTheta*k13 - sinTheta*k23;
    kg(4,5)= sinTheta*k13 + cosTheta*k23;
    kg(5,5)= k33;

    if(nodeOffsets == true)
      {
        const double &X1= nodeIOffset(0);
        const double &Y1= nodeIOffset(1);
        const double &X2= nodeJOffset(0);
        const double &Y2= nodeJOffset(1);

        const double &k11= kg(0,0);

        const double &k12= kg(0,1);
        const double &k22= kg(1,1);

        const double &k13= kg(0,2);
        const double &k23= kg(1,2);
        const double &k33= kg(2,2);

        const double &k14= kg(0,3);
        const double &k24= kg(1,3);
        const double &k34= kg(2,3);
        const double &k44= kg(3,3);

        const double &k15= kg(0,4);
        const double &k25= kg(1,4);
        const double &k35= kg(2,4);
        const double &k45= kg(3,4);
        const double &k55= kg(3,4);

        const double &k16= kg(0,5);
        const double &k26= kg(1,5);
        const double &k36= kg(2,5);
        const double &k46= kg(3,5);
        const double &k56= kg(4,5);
        const double &k66= kg(5,5);


        const double &K13= -k11*Y1 + k12*X1 + k13;
        const double &K23= -k12*Y1 + k22*X1 + k23;
        kg(0,2)= K13;
        kg(2,0)= K13;
        kg(1,2)= K23;
        kg(2,1)= K23;
        kg(2,2)= -Y1*K13 + X1*K23 -k13*Y1 + k23*X1 + k33;
      

        const double &K16= -k14*Y2 + k15*X2 + k16;
        const double &K26= -k24*Y2 + k25*X2 + k26;
        kg(0,5)= K16;
        kg(5,0)= K16;
        kg(1,5)= K26;
        kg(5,1)= K26;
        kg(2,5)= -Y2*K16 + X2*K26 -k16*Y1 + k26*X1 + k36;      
        kg(5,2)= kg(2,5);

        const double &K46= -k44*Y2 + k45*X2 + k46;
        const double &K56= -k45*Y2 + k55*X2 + k56;
        kg(3,5)= K46;
        kg(5,3)= K46;
        kg(4,5)= K56;
        kg(5,4)= K56;
        kg(5,5)= -Y2*K46 + X2*K56 -k46*Y2 + k56*X2 + k66;      

        const double &K34= -k14*Y1 + k24*X1 + k34;
        const double &K35= -k15*Y1 + k25*X1 + k35;
        kg(2,3)= K34;
        kg(3,2)= K34;
        kg(2,4)= K35;
        kg(4,2)= K35;
      }
    return kg;
  }


const XC::Matrix &XC::CorotCrdTransf2d::getGeomStiffMatrix(const Vector &pb) const
  {
    // get  geometric stiffness matrix present in the transformation 
    // from basic to local system
    double s2, c2, cs;  
    
    s2= sinAlpha*sinAlpha;
    c2= cosAlpha*cosAlpha;
    cs= sinAlpha*cosAlpha;
    
    static Matrix kg0(6,6), kg12(6,6);
    kg0.Zero();
    
    kg12.Zero();
    
    kg0(0,0)= kg0(3,3)=  s2;
    kg0(0,1)= kg0(3,4)= -cs;
    kg0(1,0)= kg0(4,3)= -cs;
    kg0(1,1)= kg0(4,4)=  c2;
    
    kg0(0,3)= kg0(3,0)= -s2;
    kg0(0,4)= kg0(3,1)=  cs;
    kg0(1,3)= kg0(4,0)=  cs;
    kg0(1,4)= kg0(4,1)= -c2;
    
    kg0 *= pb(0)/Ln;
    
    kg12(0,0)= kg12(3,3)= -2*cs;
    kg12(0,1)= kg12(3,4)=  c2-s2;
    kg12(1,0)= kg12(4,3)=  c2-s2;
    kg12(1,1)= kg12(4,4)=  2*cs;
    
    kg12(0,3)= kg12(3,0)=  2*cs;
    kg12(0,4)= kg12(3,1)= -c2+s2;
    kg12(1,3)= kg12(4,0)= -c2+s2;
    kg12(1,4)= kg12(4,1)= -2*cs;
    
    kg12 *= (pb(1)+pb(2))/(Ln*Ln);
    
    static Matrix kg(6,6);
    // kg= kg0 + kg12;
    kg= kg0;
    kg.addMatrix(1.0, kg12, 1.0);
    
    return kg;
  }


double XC::CorotCrdTransf2d::getInitialLength(void) const
  { return L; }


double XC::CorotCrdTransf2d::getDeformedLength(void) const
  { return Ln; }


XC::CrdTransf2d *XC::CorotCrdTransf2d::getCopy(void) const
  { return new CorotCrdTransf2d(*this); }

//! @brief Sends object members through the communicator argument.
int XC::CorotCrdTransf2d::sendData(Communicator &comm)
  {
    int res= CrdTransf2d::sendData(comm);
    res+=comm.sendDoubles(cosAlpha,sinAlpha,Ln,Lx,Ly,getDbTagData(),CommMetaData(10));
    res+=comm.sendDoubles(Lxdot,Lydot,Lxdotdot,Lydotdot,getDbTagData(),CommMetaData(11));
    res+=comm.sendVector(ub,getDbTagData(),CommMetaData(12));
    res+=comm.sendVector(ubcommit,getDbTagData(),CommMetaData(13));
    res+=comm.sendVector(ubpr,getDbTagData(),CommMetaData(14));
    res+= comm.sendBool(nodeOffsets,getDbTagData(),CommMetaData(15));
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data.\n";
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::CorotCrdTransf2d::recvData(const Communicator &comm)
  {
    int res= CrdTransf2d::recvData(comm);
    res+=comm.receiveDoubles(cosAlpha,sinAlpha,Ln,Lx,Ly,getDbTagData(),CommMetaData(10));
    res+=comm.receiveDoubles(Lxdot,Lydot,Lxdotdot,Lydotdot,getDbTagData(),CommMetaData(11));
    res+= comm.receiveVector(ub,getDbTagData(),CommMetaData(12));
    res+= comm.receiveVector(ubcommit,getDbTagData(),CommMetaData(13));
    res+= comm.receiveVector(ubpr,getDbTagData(),CommMetaData(14));
    res+= comm.receiveBool(nodeOffsets,getDbTagData(),CommMetaData(15));
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive data.\n";
    return res;    
  }

//! @brief Send the object through the communicator argument.
int XC::CorotCrdTransf2d::sendSelf(Communicator &comm)
  {
    static ID data(16);
    int res= sendData(comm);

    const int dataTag= getDbTag();
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; data could not be sent.\n" ;
    return res;
  }


//! @brief Receives object through the communicator argument.
int XC::CorotCrdTransf2d::recvSelf(const Communicator &comm)
  {
    static ID data(16);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; data could not be received.\n" ;
    else
      res+= recvData(comm);
    return res;
  }


const XC::Vector &XC::CorotCrdTransf2d::getPointGlobalCoordFromLocal(const Vector &xl) const
  {
    static Vector xg(3);
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented yet." << std::endl;
    
    return xg;  
  }


const XC::Vector &XC::CorotCrdTransf2d::getPointGlobalDisplFromBasic(double xi, const Vector &uxb) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented yet." << std::endl;
    return uxg;  
  }

//! @brief Returns the $\vec{i}$ unit vector of the local axis
//! expressed in global coordinates for the current geometry.
const XC::Vector &XC::CorotCrdTransf2d::getI(void)
  {
    static Vector vectorI(2);
    vectorI(0)= cosAlpha;
    vectorI(1)= sinAlpha;
    return vectorI;
  }

//! @brief Returns the $\vec{j}$ unit vector of the local axis
//! expressed in global coordinates for the current geometry.
const XC::Vector &XC::CorotCrdTransf2d::getJ(void)
  {
    static Vector vectorJ(2);
    vectorJ(0)= -sinAlpha;
    vectorJ(1)= cosAlpha;
    return vectorJ;
  }

//| @brief Returns the direction vectors of the local axis in the current
//! element position.
int XC::CorotCrdTransf2d::getLocalAxes(Vector &XAxis, Vector &YAxis) const
  {
    XAxis(0)= cosAlpha; XAxis(1)= sinAlpha;
    YAxis(0)= -sinAlpha; YAxis(1)= cosAlpha;
    return 0;
  }

//! @brief Printing
void XC::CorotCrdTransf2d::Print(std::ostream &s, int flag) const
  {
    s << "\nCrdTransf: " << this->getTag() << " Type: LinearCrdTransf2d";
    s << "\tnodeI Offset: " << nodeIOffset;
    s << "\tnodeJ Offset: " << nodeJOffset;
  }

const XC::Vector &XC::CorotCrdTransf2d::getGlobalResistingForceShapeSensitivity(const Vector &q,const Vector &p0,int gradNumber)
  {
    static Vector dpgdh(6);
    dpgdh.Zero();

    const int nodeIid= nodeIPtr->getCrdsSensitivity();
    const int nodeJid= nodeJPtr->getCrdsSensitivity();
  
    if(nodeIid == 0 && nodeJid == 0)
      return dpgdh;
    
    this->update();

    if(nodeIOffset.Norm2() != 0.0 || nodeJOffset.Norm2() != 0.0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
 	          << "; ERROR: currently a node offset cannot be used in "
		  << std::endl
                  << " conjunction with random nodal coordinates."
		  << std::endl;
      }
  
    double dcosThetadh= 0.0;
    double dsinThetadh= 0.0;

//   double dx= cosTheta*L;
//   double dy= sinTheta*L;	
  
    double dLdh= this->getdLdh();

    if(nodeIid == 1) { // here x1 is random
    //dcosThetadh= (-L+dx*dx/L)/(L*L);
    //dsinThetadh= dx*dy/(L*L*L);
    dcosThetadh= -1/L-cosTheta/L*dLdh;
    dsinThetadh= -sinTheta/L*dLdh;
  }
  if(nodeIid == 2) { // here y1 is random
    //dsinThetadh= (-L+dy*dy/L)/(L*L);
    //dcosThetadh= dx*dy/(L*L*L);
    dcosThetadh= -cosTheta/L*dLdh;
    dsinThetadh= -1/L-sinTheta/L*dLdh;
  }
  
  if(nodeJid == 1) { // here x2 is random
    //dcosThetadh= (L-dx*dx/L)/(L*L);
    //dsinThetadh= -dx*dy/(L*L*L);
    dcosThetadh= 1/L-cosTheta/L*dLdh;
    dsinThetadh= -sinTheta/L*dLdh;
  }
  if(nodeJid == 2) { // here y2 is random
    //dsinThetadh= (L-dy*dy/L)/(L*L);
    //dcosThetadh= -dx*dy/(L*L*L);
    dcosThetadh= -cosTheta/L*dLdh;
    dsinThetadh= 1/L-sinTheta/L*dLdh;
  }
  
  const Vector &disp1= nodeIPtr->getTrialDisp();
  const Vector &disp2= nodeJPtr->getTrialDisp();

  static Vector U(6);
  for(int i= 0; i < 3; i++) {
    U(i)  = disp1(i);
    U(i+3)= disp2(i);
  }
  
  static Vector u(6);

//   double dux=  cosTheta*(U(3)-U(0)) + sinTheta*(U(4)-U(1));
//   double duy= -sinTheta*(U(3)-U(0)) + cosTheta*(U(4)-U(1));

  //double dLdh= this->getdLdh();

  double dcosAlphadh=  sinAlpha*sinAlpha/Ln;
  double dsinAlphadh= -cosAlpha*sinAlpha/Ln;

  double dcosAlphaOverLndh= (2*sinAlpha*sinAlpha-1.0)/(Ln*Ln) ;
  double dsinAlphaOverLndh= -2*cosAlpha*sinAlpha/(Ln*Ln);

  double q0= q(0);
  double q1= q(1);
  double q2= q(2);

  static Vector dpldh(6);
  dpldh.Zero();

  dpldh(0)= (-dcosAlphadh*q0 - dsinAlphaOverLndh*(q1+q2) )*dLdh;
  dpldh(1)= (-dsinAlphadh*q0 + dcosAlphaOverLndh*(q1+q2) )*dLdh;
  dpldh(2)= 0.0;
  dpldh(3)= ( dcosAlphadh*q0 + dsinAlphaOverLndh*(q1+q2) )*dLdh;
  dpldh(4)= ( dsinAlphadh*q0 - dcosAlphaOverLndh*(q1+q2) )*dLdh;
  dpldh(5)= 0.0;

  this->getTransfMatrixLocalGlobal(Tlg);     // OPTIMIZE LATER
  dpgdh.addMatrixTransposeVector(0.0, Tlg, dpldh, 1.0);   // pg= Tlg ^ pl; residual

  static Vector pl(6);
  pl.Zero();

  static Matrix Abl(3,6);
  this->getTransfMatrixBasicLocal(Abl);

  pl.addMatrixTransposeVector(0.0, Abl, q, 1.0); // OPTIMIZE LATER

  dpgdh(0) += dcosThetadh*pl(0)-dsinThetadh*pl(1);
  dpgdh(1) += dsinThetadh*pl(0)+dcosThetadh*pl(1);
  dpgdh(2) += 0.0;
  dpgdh(3) += dcosThetadh*pl(3)-dsinThetadh*pl(4);
  dpgdh(4) += dsinThetadh*pl(3)+dcosThetadh*pl(4);
  dpgdh(5) += 0.0;

  return dpgdh;
}

const XC::Vector &XC::CorotCrdTransf2d::getBasicDisplSensitivity(int gradNumber)
{
  static Vector dvdh(3);
  dvdh.Zero();

  int nodeIid= nodeIPtr->getCrdsSensitivity();
  int nodeJid= nodeJPtr->getCrdsSensitivity();
  
  this->update();

  double dcosThetadh= 0.0;
  double dsinThetadh= 0.0;

//   double dx= cosTheta*L;
//   double dy= sinTheta*L;	
  
  double dLdh= this->getdLdh();

  if(nodeIid == 1) { // here x1 is random
    //dcosThetadh= (-L+dx*dx/L)/(L*L);
    //dsinThetadh= dx*dy/(L*L*L);
    dcosThetadh= -1/L-cosTheta/L*dLdh;
    dsinThetadh= -sinTheta/L*dLdh;
  }
  if(nodeIid == 2) { // here y1 is random
    //dsinThetadh= (-L+dy*dy/L)/(L*L);
    //dcosThetadh= dx*dy/(L*L*L);
    dcosThetadh= -cosTheta/L*dLdh;
    dsinThetadh= -1/L-sinTheta/L*dLdh;
  }
  
  if(nodeJid == 1) { // here x2 is random
    //dcosThetadh= (L-dx*dx/L)/(L*L);
    //dsinThetadh= -dx*dy/(L*L*L);
    dcosThetadh= 1/L-cosTheta/L*dLdh;
    dsinThetadh= -sinTheta/L*dLdh;
  }
  if(nodeJid == 2) { // here y2 is random
    //dsinThetadh= (L-dy*dy/L)/(L*L);
    //dcosThetadh= -dx*dy/(L*L*L);
    dcosThetadh= -cosTheta/L*dLdh;
    dsinThetadh= 1/L-sinTheta/L*dLdh;
  }
  
  static Vector U(6);
  static Vector dUdh(6);

  const Vector &disp1= nodeIPtr->getTrialDisp();
  const Vector &disp2= nodeJPtr->getTrialDisp();
  for(int i= 0; i < 3; i++) {
    U(i)  = disp1(i);
    U(i+3)= disp2(i);
    dUdh(i)  = nodeIPtr->getDispSensitivity((i+1),gradNumber);
    dUdh(i+3)= nodeJPtr->getDispSensitivity((i+1),gradNumber);
  }

  static Vector dudh(6);

  dudh(0)=  cosTheta*dUdh(0) + sinTheta*dUdh(1);
  dudh(1)= -sinTheta*dUdh(0) + cosTheta*dUdh(1);
  dudh(2)=  dUdh(2);
  dudh(3)=  cosTheta*dUdh(3) + sinTheta*dUdh(4);
  dudh(4)= -sinTheta*dUdh(3) + cosTheta*dUdh(4);
  dudh(5)=  dUdh(5);

  if(nodeIid != 0 || nodeJid != 0) {
    dudh(0) +=  dcosThetadh*U(0) + dsinThetadh*U(1);
    dudh(1) += -dsinThetadh*U(0) + dcosThetadh*U(1);
    dudh(3) +=  dcosThetadh*U(3) + dsinThetadh*U(4);
    dudh(4) += -dsinThetadh*U(3) + dcosThetadh*U(4);
  }

  double duxdh= dudh(3)-dudh(0);
  double duydh= dudh(4)-dudh(1);

  //double dLdh = this->getdLdh();
  double dLndh= cosAlpha*(dLdh+duxdh) + sinAlpha*duydh;

  double dalphadh= (cosAlpha*duydh - sinAlpha*(dLdh+duxdh))/Ln;

  // direct differentiation of v(u) wrt theta
  dvdh(0)= dLndh-dLdh;
  dvdh(1)= dudh(2)-dalphadh;
  dvdh(2)= dudh(5)-dalphadh;

  return dvdh;
}

const XC::Vector &XC::CorotCrdTransf2d::getBasicTrialDispShapeSensitivity(void)
  {
    static Vector dvdh(3);
    dvdh.Zero();

    int nodeIid= nodeIPtr->getCrdsSensitivity();
  int nodeJid= nodeJPtr->getCrdsSensitivity();
  
  if(nodeIid == 0 && nodeJid == 0)
    return dvdh;

  static Matrix Abl(3,6);

  this->update();
  this->getTransfMatrixBasicLocal(Abl);

  double dcosThetadh= 0.0;
  double dsinThetadh= 0.0;

//   double dx= cosTheta*L;
//   double dy= sinTheta*L;	
  
  double dLdh= this->getdLdh();

  if(nodeIid == 1) { // here x1 is random
    //dcosThetadh= (-L+dx*dx/L)/(L*L);
    //dsinThetadh= dx*dy/(L*L*L);
    dcosThetadh= -1/L-cosTheta/L*dLdh;
    dsinThetadh= -sinTheta/L*dLdh;
  }
  if(nodeIid == 2) { // here y1 is random
    //dsinThetadh= (-L+dy*dy/L)/(L*L);
    //dcosThetadh= dx*dy/(L*L*L);
    dcosThetadh= -cosTheta/L*dLdh;
    dsinThetadh= -1/L-sinTheta/L*dLdh;
  }
  
  if(nodeJid == 1) { // here x2 is random
    //dcosThetadh= (L-dx*dx/L)/(L*L);
    //dsinThetadh= -dx*dy/(L*L*L);
    dcosThetadh= 1/L-cosTheta/L*dLdh;
    dsinThetadh= -sinTheta/L*dLdh;
  }
  if(nodeJid == 2) { // here y2 is random
    //dsinThetadh= (L-dy*dy/L)/(L*L);
    //dcosThetadh= -dx*dy/(L*L*L);
    dcosThetadh= -cosTheta/L*dLdh;
    dsinThetadh= 1/L-sinTheta/L*dLdh;
  }
  
  const Vector &disp1= nodeIPtr->getTrialDisp();
  const Vector &disp2= nodeJPtr->getTrialDisp();

  static Vector U(6);
  for(int i= 0; i < 3; i++) {
    U(i)  = disp1(i);
    U(i+3)= disp2(i);
  }

  dvdh(0)= (cosAlpha-1.0)*dLdh;
  dvdh(1)=  (sinAlpha/Ln)*dLdh;
  dvdh(2)=  (sinAlpha/Ln)*dLdh;

  static Vector dAdh_U(6);
  // dAdh * U
  dAdh_U(0)=  dcosThetadh*U(0) + dsinThetadh*U(1);
  dAdh_U(1)= -dsinThetadh*U(0) + dcosThetadh*U(1);
  dAdh_U(2)= 0.0;
  dAdh_U(3)=  dcosThetadh*U(3) + dsinThetadh*U(4);
  dAdh_U(4)= -dsinThetadh*U(3) + dcosThetadh*U(4);
  dAdh_U(5)= 0.0;

  dvdh += Abl*dAdh_U;

  return dvdh;
}

bool XC::CorotCrdTransf2d::isShapeSensitivity(void)
  {
    const int nodeIid= nodeIPtr->getCrdsSensitivity();
    const int nodeJid= nodeJPtr->getCrdsSensitivity();
    return (nodeIid != 0 || nodeJid != 0);
  }

double XC::CorotCrdTransf2d::getdLdh(void)
  {
    int nodeIid= nodeIPtr->getCrdsSensitivity();
    int nodeJid= nodeJPtr->getCrdsSensitivity();
  
    if(nodeIid == 0 && nodeJid == 0) 
      return 0.0;
    
    if(nodeIOffset.Norm2() != 0.0 || nodeJOffset.Norm2() != 0.0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; ERROR: currently a node offset cannot be used in "
		  << std::endl
                  << " conjunction with random nodal coordinates."
		  << std::endl;
      }
  
    if(nodeIid == 1) // here x1 is random
      return -cosTheta;
    if(nodeIid == 2) // here y1 is random
      return -sinTheta;
  
    if(nodeJid == 1) // here x2 is random
      return cosTheta;
    if(nodeJid == 2) // here y2 is random
      return sinTheta;
    return 0.0;
  }

double XC::CorotCrdTransf2d::getd1overLdh(void)
  {
    int nodeIid= nodeIPtr->getCrdsSensitivity();
    int nodeJid= nodeJPtr->getCrdsSensitivity();
  
    if(nodeIid == 0 && nodeJid == 0)
      return 0.0;
    
    if(nodeIOffset.Norm2() != 0.0 || nodeJOffset.Norm2() != 0.0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; ERROR: currently a node offset cannot be used in "
		  << std::endl
                  << " conjunction with random nodal coordinates."
		  << std::endl;
      }
  
    if(nodeIid == 1) // here x1 is random
      return cosTheta/(L*L);
    if(nodeIid == 2) // here y1 is random
      return sinTheta/(L*L);
  
    if(nodeJid == 1) // here x2 is random
      return -cosTheta/(L*L);
    if(nodeJid == 2) // here y2 is random
      return -sinTheta/(L*L);

    return 0.0;
  }

