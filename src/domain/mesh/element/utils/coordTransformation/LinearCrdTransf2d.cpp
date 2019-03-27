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
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO.
//  GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// junto a este programa.
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
// $Source: /usr/local/cvs/OpenSees/SRC/LinearCrdTransf2d.cpp,v $


// Written: Remo Magalhaes de Souza (rmsouza@ce.berkeley.edu)
// Created: 04/2000
// Revision: A
//
// Modified: May 2001 for matrix-multiply unrolling
// Modified: 04/2005 Andreas Schellenberg (getBasicTrialVel, getBasicTrialAccel)
//
// Purpose: This file contains the implementation for the
// LinearCrdTransf2d class. LinearCrdTransf2d is a linear
// transformation for a planar frame between the global
// and basic coordinate systems


#include "LinearCrdTransf2d.h"
#include <utility/matrix/ID.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <domain/mesh/node/Node.h>

// constructor:
XC::LinearCrdTransf2d::LinearCrdTransf2d(int tag)
  :XC::SmallDispCrdTransf2d(tag, CRDTR_TAG_LinearCrdTransf2d)
  {}


// constructor:
XC::LinearCrdTransf2d::LinearCrdTransf2d(int tag, const Vector &rigJntOffset1, const XC::Vector &rigJntOffset2)
  : XC::SmallDispCrdTransf2d(tag, CRDTR_TAG_LinearCrdTransf2d)
  {
    // check rigid joint offset for node I
    set_rigid_joint_offsetI(rigJntOffset1);

    // check rigid joint offset for node J
    set_rigid_joint_offsetJ(rigJntOffset2);
  }


// constructor:
// invoked by a FEM_ObjectBroker, recvSelf() needs to be invoked on this object.
XC::LinearCrdTransf2d::LinearCrdTransf2d()
  : XC::SmallDispCrdTransf2d(0, CRDTR_TAG_LinearCrdTransf2d)
  {}


int XC::LinearCrdTransf2d::commitState(void)
  { return 0; }


int XC::LinearCrdTransf2d::revertToLastCommit(void)
  { return 0; }


int XC::LinearCrdTransf2d::revertToStart(void)
  { return 0; }


int XC::LinearCrdTransf2d::update(void)
  { return 0; }


const XC::Vector &XC::LinearCrdTransf2d::getBasicTrialDispShapeSensitivity(void)
  {
    // Want to return dAdh * u

    // determine global displacements
    const Vector &disp1 = nodeIPtr->getTrialDisp();
    const Vector &disp2 = nodeJPtr->getTrialDisp();

    static double ug[6];
    for(int i = 0; i < 3; i++)
      {
        ug[i]   = disp1(i);
        ug[i+3] = disp2(i);
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

    static Vector ub(3);
    ub.Zero();

    static ID nodeParameterID(2);
    nodeParameterID(0)= nodeIPtr->getCrdsSensitivity();
    nodeParameterID(1)= nodeJPtr->getCrdsSensitivity();

    if(nodeParameterID(0) != 0 || nodeParameterID(1) != 0)
      {

        if(nodeIOffset.Norm2() > 0.0 || nodeJOffset.Norm2() > 0.0)
          {
            std::cerr << "ERROR: Currently a node offset cannot be used in " << std::endl
                << " conjunction with random nodal coordinates." << std::endl;
          }

        double dcosdh=0.0, dsindh=0.0, dsldh=0.0, dcldh=0.0;

        const double dx = cosTheta*L;
        const double dy = sinTheta*L;

        if(nodeParameterID(0) == 1)
          { // here x1 is random
            dcosdh = (-L+dx*dx/L)/(L*L);
            dsindh = dx*dy/(L*L*L);
            dcldh = (-L*L+dx*dx*2)/(L*L*L*L);
            dsldh = 2*dx*dy/(L*L*L*L);
          }
        if(nodeParameterID(0) == 2)
          { // here y1 is random
            dsindh = (-L+dy*dy/L)/(L*L);
            dcosdh = dx*dy/(L*L*L);
            dsldh = (-L*L+dy*dy*2)/(L*L*L*L);
            dcldh = 2*dx*dy/(L*L*L*L);
          }

        if(nodeParameterID(1) == 1)
          { // here x2 is random
            dcosdh = (L-dx*dx/L)/(L*L);
            dsindh = -dx*dy/(L*L*L);
            dcldh = (L*L-dx*dx*2)/(L*L*L*L);
            dsldh = -2*dx*dy/(L*L*L*L);
          }
        if(nodeParameterID(1) == 2)
          { // here y2 is random
            dsindh = (L-dy*dy/L)/(L*L);
            dcosdh = -dx*dy/(L*L*L);
            dsldh = (L*L-dy*dy*2)/(L*L*L*L);
            dcldh = -2*dx*dy/(L*L*L*L);
          }

        ub(0) = -dcosdh*ug[0] - dsindh*ug[1] + dcosdh*ug[3] + dsindh*ug[4];

        ub(1) = -dsldh*ug[0] + dcldh*ug[1] + dsldh*ug[3] - dcldh*ug[4];

        ub(2) = ub(1);
    }

    return ub;
}


const XC::Vector &XC::LinearCrdTransf2d::getGlobalResistingForce(const XC::Vector &pb, const XC::Vector &p0) const
  {
    const Vector &pl= basic_to_local_resisting_force(pb,p0);
    return local_to_global_resisting_force(pl);
  }


const XC::Vector &XC::LinearCrdTransf2d::getGlobalResistingForceShapeSensitivity(const Vector &pb, const Vector &p0)
  {
    // transform resisting forces from the basic system to local coordinates
    static double pl[6];

    double q0 = pb(0);
    double q1 = pb(1);
    double q2 = pb(2);

    const double oneOverL= 1.0/L;

    double V = oneOverL*(q1+q2);
    pl[0] = -q0;
    pl[1] =  V;
    pl[2] =  q1;
    pl[3] =  q0;
    pl[4] = -V;
    pl[5] =  q2;

    // add end forces due to element p0 loads
    //    pl[0] += p0(0);
    //    pl[1] += p0(1);
    //    pl[4] += p0(2);

    // transform resisting forces  from local to global coordinates
    static Vector pg(6);
    pg.Zero();

    static ID nodeParameterID(2);
    nodeParameterID(0) = nodeIPtr->getCrdsSensitivity();
    nodeParameterID(1) = nodeJPtr->getCrdsSensitivity();

    if(nodeParameterID(0) != 0 || nodeParameterID(1) != 0)
      {

        if(nodeIOffset.Norm2() > 0 || nodeJOffset.Norm2() > 0)
          {
            std::cerr << "ERROR: Currently a node offset cannot be used in " << std::endl
                << " conjunction with random nodal coordinates." << std::endl;
          }

        double dcosdh= 0.0, dsindh= 0.0, d1oLdh= 0.0;

        double dx = cosTheta*L;
        double dy = sinTheta*L;

        if(nodeParameterID(0) == 1)
          { // here x1 is random
            dcosdh = (-L+dx*dx/L)/(L*L);
            dsindh = dx*dy/(L*L*L);
            d1oLdh = dx/(L*L*L);
          }
        if(nodeParameterID(0) == 2)
          { // here y1 is random
            dsindh = (-L+dy*dy/L)/(L*L);
            dcosdh = dx*dy/(L*L*L);
            d1oLdh = dy/(L*L*L);
          }

        if(nodeParameterID(1) == 1)
          { // here x2 is random
            dcosdh = (L-dx*dx/L)/(L*L);
            dsindh = -dx*dy/(L*L*L);
            d1oLdh = -dx/(L*L*L);
          }
        if(nodeParameterID(1) == 2)
          { // here y2 is random
            dsindh = (L-dy*dy/L)/(L*L);
            dcosdh = -dx*dy/(L*L*L);
            d1oLdh = -dy/(L*L*L);
          }

        pg(0) = dcosdh*pl[0] - dsindh*pl[1] - sinTheta*d1oLdh*(q1+q2);
        pg(1) = dsindh*pl[0] + dcosdh*pl[1] + cosTheta*d1oLdh*(q1+q2);

        pg(3) = dcosdh*pl[3] - dsindh*pl[4] + sinTheta*d1oLdh*(q1+q2);
        pg(4) = dsindh*pl[3] + dcosdh*pl[4] - cosTheta*d1oLdh*(q1+q2);

        pg(2) = 0.0;
        pg(5) = 0.0;
    }

    return pg;
  }

const XC::Vector &XC::LinearCrdTransf2d::getGlobalResistingForceShapeSensitivity(const Vector &pb, const Vector &p0, int gradNumber)
  {
    // transform resisting forces from the basic system to local coordinates
    static double pl[6];

    const double q0 = pb(0);
    const double q1 = pb(1);
    const double q2 = pb(2);

    const double oneOverL = 1.0/L;

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

    // transform resisting forces  from local to global coordinates
    static Vector pg(6);
    pg.Zero();

    static ID nodeParameterID(2);
    nodeParameterID(0) = nodeIPtr->getCrdsSensitivity();
    nodeParameterID(1) = nodeJPtr->getCrdsSensitivity();

    if (nodeParameterID(0) != 0 || nodeParameterID(1) != 0)
      {

	if(nodeIOffset.Norm2() != 0.0 || nodeJOffset.Norm2() != 0.0)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; currently a node offset cannot be used in "
		      << " conjunction with random nodal coordinates."
		      << std::endl;
	  }

        double dcosdh=0.0, dsindh=0.0, d1oLdh=0.0;

	      double dx = cosTheta*L;
	      double dy = sinTheta*L;

	      if (nodeParameterID(0) == 1) { // here x1 is random
		dcosdh = (-L+dx*dx/L)/(L*L);
		dsindh = dx*dy/(L*L*L);
		d1oLdh = dx/(L*L*L);
	      }
	      if (nodeParameterID(0) == 2) { // here y1 is random
		dsindh = (-L+dy*dy/L)/(L*L);
		dcosdh = dx*dy/(L*L*L);
		d1oLdh = dy/(L*L*L);
	      }

	      if (nodeParameterID(1) == 1) { // here x2 is random
		dcosdh = (L-dx*dx/L)/(L*L);
		dsindh = -dx*dy/(L*L*L);
		d1oLdh = -dx/(L*L*L);
	      }
	      if (nodeParameterID(1) == 2) { // here y2 is random
		dsindh = (L-dy*dy/L)/(L*L);
		dcosdh = -dx*dy/(L*L*L);
		d1oLdh = -dy/(L*L*L);
	      }

	      pg(0) = dcosdh*pl[0] - dsindh*pl[1] - sinTheta*d1oLdh*(q1+q2);
	      pg(1) = dsindh*pl[0] + dcosdh*pl[1] + cosTheta*d1oLdh*(q1+q2);

	      pg(3) = dcosdh*pl[3] - dsindh*pl[4] + sinTheta*d1oLdh*(q1+q2);
	      pg(4) = dsindh*pl[3] + dcosdh*pl[4] - cosTheta*d1oLdh*(q1+q2);

	      pg(2) = 0.0;
	      pg(5) = 0.0;
	  }

	  return pg;
  }

bool XC::LinearCrdTransf2d::isShapeSensitivity(void)
  {
    const int nodeParameterI= nodeIPtr->getCrdsSensitivity();
    const int nodeParameterJ= nodeJPtr->getCrdsSensitivity();
    return (nodeParameterI != 0 || nodeParameterJ != 0);
  }


double XC::LinearCrdTransf2d::getdLdh(void)
  {
    const int nodeParameterI = nodeIPtr->getCrdsSensitivity();
    const int nodeParameterJ = nodeJPtr->getCrdsSensitivity();

    if (nodeParameterI != 0 || nodeParameterJ != 0)
      {
        if(nodeIOffset.Norm2() != 0 || nodeJOffset.Norm2() != 0)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; ERROR: Currently a node offset cannot be used in "
		      << " conjunction with random nodal coordinates."
		      << std::endl;
          }

        if(nodeParameterI == 1) // here x1 is random
          return -cosTheta;
        if(nodeParameterI == 2) // here y1 is random
          return -sinTheta;
        if(nodeParameterJ == 1) // here x2 is random
          return cosTheta;
        if(nodeParameterJ == 2) // here y2 is random
          return sinTheta;
      }
    return 0.0;
  }


double XC::LinearCrdTransf2d::getd1overLdh(void)
  {
    const int nodeParameterI= nodeIPtr->getCrdsSensitivity();
    const int nodeParameterJ= nodeJPtr->getCrdsSensitivity();

    if(nodeParameterI != 0 || nodeParameterJ != 0)
      {
        if(nodeIOffset.Norm2() != 0 || nodeJOffset.Norm2() != 0)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; ERROR: Currently a node offset cannot be used in "
		      << " conjunction with random nodal coordinates."
		      << std::endl;
          }

        if(nodeParameterI == 1) // here x1 is random
          return cosTheta/(L*L);
        if(nodeParameterI == 2) // here y1 is random
          return sinTheta/(L*L);
        if(nodeParameterJ == 1) // here x2 is random
          return -cosTheta/(L*L);
        if(nodeParameterJ == 2) // here y2 is random
          return -sinTheta/(L*L);
      }
    return 0.0;
  }


const XC::Matrix &XC::LinearCrdTransf2d::getGlobalStiffMatrix(const XC::Matrix &kb, const XC::Vector &pb) const
  {
    const double oneOverL= 1.0/L;

    const double t02= -T02();
    const double t12= oneOverL*T12()+1.0;
    const double t22= oneOverL*T12();

    const double t05= T35();
    const double t45= T45();
    const double t15= -oneOverL*(t45);
    const double t25= -oneOverL*(t45) + 1.0;

    const double sl = sinTheta*oneOverL;
    const double cl = cosTheta*oneOverL;

    const bool nodeIOffsetNotZero= (nodeIOffset.Norm2()>0.0);
    const bool nodeJOffsetNotZero= (nodeJOffset.Norm2()>0.0);

    static Matrix tmp(6,6);
    tmp(0,0) = -cosTheta*kb(0,0) - sl*(kb(0,1)+kb(0,2));
    tmp(0,1) = -sinTheta*kb(0,0) + cl*(kb(0,1)+kb(0,2));
    tmp(0,2) = (nodeIOffsetNotZero) ? t02*kb(0,0) + t12*kb(0,1) + t22*kb(0,2) : kb(0,1);
    tmp(0,3) = -tmp(0,0);
    tmp(0,4) = -tmp(0,1);
    tmp(0,5) = (nodeJOffsetNotZero) ? t05*kb(0,0) + t15*kb(0,1) + t25*kb(0,2) : kb(0,2);

    tmp(1,0) = -cosTheta*kb(1,0) - sl*(kb(1,1)+kb(1,2));
    tmp(1,1) = -sinTheta*kb(1,0) + cl*(kb(1,1)+kb(1,2));
    tmp(1,2) = (nodeIOffsetNotZero) ? t02*kb(1,0) + t12*kb(1,1) + t22*kb(1,2) : kb(1,1);
    tmp(1,3) = -tmp(1,0);
    tmp(1,4) = -tmp(1,1);
    tmp(1,5) = (nodeJOffsetNotZero) ? t05*kb(1,0) + t15*kb(1,1) + t25*kb(1,2) : kb(1,2);

    tmp(2,0) = -cosTheta*kb(2,0) - sl*(kb(2,1)+kb(2,2));
    tmp(2,1) = -sinTheta*kb(2,0) + cl*(kb(2,1)+kb(2,2));
    tmp(2,2) = (nodeIOffsetNotZero) ? t02*kb(2,0) + t12*kb(2,1) + t22*kb(2,2) : kb(2,1);
    tmp(2,3) = -tmp(2,0);
    tmp(2,4) = -tmp(2,1);
    tmp(2,5) = (nodeJOffsetNotZero) ? t05*kb(2,0) + t15*kb(2,1) + t25*kb(2,2) : kb(2,2);

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

    if(nodeIOffsetNotZero)
      {
        kg(2,0) =  t02*tmp(0,0) + t12*tmp(1,0) + t22*tmp(2,0);
        kg(2,1) =  t02*tmp(0,1) + t12*tmp(1,1) + t22*tmp(2,1);
        kg(2,2) =  t02*tmp(0,2) + t12*tmp(1,2) + t22*tmp(2,2);
        kg(2,3) =  t02*tmp(0,3) + t12*tmp(1,3) + t22*tmp(2,3);
        kg(2,4) =  t02*tmp(0,4) + t12*tmp(1,4) + t22*tmp(2,4);
        kg(2,5) =  t02*tmp(0,5) + t12*tmp(1,5) + t22*tmp(2,5);
      }
    else
      {
        kg(2,0) = tmp(1,0);
        kg(2,1) = tmp(1,1);
        kg(2,2) = tmp(1,2);
        kg(2,3) = tmp(1,3);
        kg(2,4) = tmp(1,4);
        kg(2,5) = tmp(1,5);
      }

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

    if(nodeJOffsetNotZero)
      {
        kg(5,0) =  t05*tmp(0,0) + t15*tmp(1,0) + t25*tmp(2,0);
        kg(5,1) =  t05*tmp(0,1) + t15*tmp(1,1) + t25*tmp(2,1);
        kg(5,2) =  t05*tmp(0,2) + t15*tmp(1,2) + t25*tmp(2,2);
        kg(5,3) =  t05*tmp(0,3) + t15*tmp(1,3) + t25*tmp(2,3);
        kg(5,4) =  t05*tmp(0,4) + t15*tmp(1,4) + t25*tmp(2,4);
        kg(5,5) =  t05*tmp(0,5) + t15*tmp(1,5) + t25*tmp(2,5);
      }
    else
      {
        kg(5,0) =  tmp(2,0);
        kg(5,1) =  tmp(2,1);
        kg(5,2) =  tmp(2,2);
        kg(5,3) =  tmp(2,3);
        kg(5,4) =  tmp(2,4);
        kg(5,5) =  tmp(2,5);
      }
    return kg;
  }


const XC::Matrix &XC::LinearCrdTransf2d::getInitialGlobalStiffMatrix(const XC::Matrix &kb) const
  {
    const double oneOverL = 1.0/L;

    const double t02= -T02();
    const double t12= oneOverL*T12() + 1.0;
    const double t22= oneOverL*T12();

    const double t05 = T35();
    const double t45= T45();
    const double t15 = -oneOverL*(t45);
    const double t25 = -oneOverL*(t45) + 1.0;

    const double sl = sinTheta*oneOverL;
    const double cl = cosTheta*oneOverL;

    const bool nodeIOffsetNotZero= (nodeIOffset.Norm2()>0.0);
    const bool nodeJOffsetNotZero= (nodeJOffset.Norm2()>0.0);

    static Matrix tmp(6,6);
    tmp(0,0)= -cosTheta*kb(0,0) - sl*(kb(0,1)+kb(0,2));
    tmp(0,1)= -sinTheta*kb(0,0) + cl*(kb(0,1)+kb(0,2));
    tmp(0,2)= (nodeIOffsetNotZero) ? t02*kb(0,0) + t12*kb(0,1) + t22*kb(0,2) : kb(0,1);
    tmp(0,3)= -tmp(0,0);
    tmp(0,4)= -tmp(0,1);
    tmp(0,5)= (nodeJOffsetNotZero) ? t05*kb(0,0) + t15*kb(0,1) + t25*kb(0,2) : kb(0,2);

    tmp(1,0)= -cosTheta*kb(1,0) - sl*(kb(1,1)+kb(1,2));
    tmp(1,1)= -sinTheta*kb(1,0) + cl*(kb(1,1)+kb(1,2));
    tmp(1,2)= (nodeIOffsetNotZero) ? t02*kb(1,0) + t12*kb(1,1) + t22*kb(1,2) : kb(1,1);
    tmp(1,3)= -tmp(1,0);
    tmp(1,4)= -tmp(1,1);
    tmp(1,5)= (nodeJOffsetNotZero) ? t05*kb(1,0) + t15*kb(1,1) + t25*kb(1,2) : kb(1,2);

    tmp(2,0)= -cosTheta*kb(2,0) - sl*(kb(2,1)+kb(2,2));
    tmp(2,1)= -sinTheta*kb(2,0) + cl*(kb(2,1)+kb(2,2));
    tmp(2,2)= (nodeIOffsetNotZero) ? t02*kb(2,0) + t12*kb(2,1) + t22*kb(2,2) : kb(2,1);
    tmp(2,3)= -tmp(2,0);
    tmp(2,4)= -tmp(2,1);
    tmp(2,5)= (nodeJOffsetNotZero) ? t05*kb(2,0) + t15*kb(2,1) + t25*kb(2,2) : kb(2,2);

    static Matrix kg(6,6);
    kg(0,0)= -cosTheta*tmp(0,0) - sl*(tmp(1,0)+tmp(2,0));
    kg(0,1)= -cosTheta*tmp(0,1) - sl*(tmp(1,1)+tmp(2,1));
    kg(0,2)= -cosTheta*tmp(0,2) - sl*(tmp(1,2)+tmp(2,2));
    kg(0,3)= -cosTheta*tmp(0,3) - sl*(tmp(1,3)+tmp(2,3));
    kg(0,4)= -cosTheta*tmp(0,4) - sl*(tmp(1,4)+tmp(2,4));
    kg(0,5)= -cosTheta*tmp(0,5) - sl*(tmp(1,5)+tmp(2,5));

    kg(1,0)= -sinTheta*tmp(0,0) + cl*(tmp(1,0)+tmp(2,0));
    kg(1,1)= -sinTheta*tmp(0,1) + cl*(tmp(1,1)+tmp(2,1));
    kg(1,2)= -sinTheta*tmp(0,2) + cl*(tmp(1,2)+tmp(2,2));
    kg(1,3)= -sinTheta*tmp(0,3) + cl*(tmp(1,3)+tmp(2,3));
    kg(1,4)= -sinTheta*tmp(0,4) + cl*(tmp(1,4)+tmp(2,4));
    kg(1,5)= -sinTheta*tmp(0,5) + cl*(tmp(1,5)+tmp(2,5));

    if(nodeIOffsetNotZero)
      {
        kg(2,0)=  t02*tmp(0,0) + t12*tmp(1,0) + t22*tmp(2,0);
        kg(2,1)=  t02*tmp(0,1) + t12*tmp(1,1) + t22*tmp(2,1);
        kg(2,2)=  t02*tmp(0,2) + t12*tmp(1,2) + t22*tmp(2,2);
        kg(2,3)=  t02*tmp(0,3) + t12*tmp(1,3) + t22*tmp(2,3);
        kg(2,4)=  t02*tmp(0,4) + t12*tmp(1,4) + t22*tmp(2,4);
        kg(2,5)=  t02*tmp(0,5) + t12*tmp(1,5) + t22*tmp(2,5);
      }
    else
      {
        kg(2,0)= tmp(1,0);
        kg(2,1)= tmp(1,1);
        kg(2,2)= tmp(1,2);
        kg(2,3)= tmp(1,3);
        kg(2,4)= tmp(1,4);
        kg(2,5)= tmp(1,5);
      }

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

    if(nodeJOffsetNotZero)
      {
        kg(5,0) =  t05*tmp(0,0) + t15*tmp(1,0) + t25*tmp(2,0);
        kg(5,1) =  t05*tmp(0,1) + t15*tmp(1,1) + t25*tmp(2,1);
        kg(5,2) =  t05*tmp(0,2) + t15*tmp(1,2) + t25*tmp(2,2);
        kg(5,3) =  t05*tmp(0,3) + t15*tmp(1,3) + t25*tmp(2,3);
        kg(5,4) =  t05*tmp(0,4) + t15*tmp(1,4) + t25*tmp(2,4);
        kg(5,5) =  t05*tmp(0,5) + t15*tmp(1,5) + t25*tmp(2,5);
      }
    else
      {
        kg(5,0) =  tmp(2,0);
        kg(5,1) =  tmp(2,1);
        kg(5,2) =  tmp(2,2);
        kg(5,3) =  tmp(2,3);
        kg(5,4) =  tmp(2,4);
        kg(5,5) =  tmp(2,5);
      }
    return kg;
  }


XC::CrdTransf2d *XC::LinearCrdTransf2d::getCopy(void) const
  { return new LinearCrdTransf2d(*this); }


void XC::LinearCrdTransf2d::Print(std::ostream &s, int flag)
  {
    s << "\nCrdTransf: " << this->getTag() << " Type: LinearCrdTransf2d";
    s << "\tnodeI Offset: " << nodeIOffset(0) << ' ' << nodeIOffset(1) << std::endl;
    s << "\tnodeJ Offset: " << nodeJOffset(0) << ' ' << nodeJOffset(1) << std::endl;
  }


// AddingSensitivity:BEGIN ///////////////////////////////
const XC::Vector &XC::LinearCrdTransf2d::getBasicDisplSensitivity(int gradNumber)
  {

    // This method is created by simply copying the
    // getBasicTrialDisp method. Instead of picking
    // up the nodal displacements we just pick up
    // the nodal displacement sensitivities.

    static double ug[6];
    for (int i = 0; i < 3; i++) {
        ug[i]   = nodeIPtr->getDispSensitivity((i+1),gradNumber);
        ug[i+3] = nodeJPtr->getDispSensitivity((i+1),gradNumber);
    }

    static Vector ub(3);

    const double oneOverL= 1.0/L;
    const double sl= sinTheta*oneOverL;
    const double cl= cosTheta*oneOverL;

    ub(0) = -cosTheta*ug[0] - sinTheta*ug[1] +
        cosTheta*ug[3] + sinTheta*ug[4];

    ub(1) = -sl*ug[0] + cl*ug[1] + ug[2] +
        sl*ug[3] - cl*ug[4];

    const double t02= T02();
    const double t12= T12();
    ub(0) -= t02*ug[2];
    ub(1) += oneOverL*t12*ug[2];

    const double t35= T35();
    const double t45= T45();
    ub(0) += t35*ug[5];
    ub(1) -= oneOverL*t45*ug[5];

    ub(2) = ub(1) + ug[5] - ug[2];
    return ub;
  }
// AddingSensitivity:END /////////////////////////////////////
