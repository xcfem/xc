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

// $Revision: 1.2 $
// $Date: 2005/12/15 00:30:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/CrdTransf2d.cpp,v $


// File: ~/crdTransf/CrdTransf2d.C
//
// Written: Remo Magalhaes de Souza (rmsouza@ce.berkeley.edu)
// Created: 04/2000
// Revision: A
//
// Description: This file contains the implementation for the XC::CrdTransf2d class.
// CrdTransf2d provides the abstraction of a 2d frame 
// coordinate transformation. It is an abstract base class and 
// thus no objects of  it's type can be instatiated. 

#include "CrdTransf2d.h"
#include "domain/mesh/node/Node.h"
#include "utility/matrix/Matrix.h"

#include "xc_utils/src/geom/pos_vec/Pos3dArray.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/sis_ref/Ref2d2d.h"
#include "utility/actor/actor/MovableVector.h"

//! @brief constructor:
XC::CrdTransf2d::CrdTransf2d(int tag, int classTag)
  : CrdTransf(tag, classTag,2),cosTheta(0.0), sinTheta(0.0) {}

//! @brief check rigid joint offset for node I
void XC::CrdTransf2d::set_rigid_joint_offsetI(const Vector &rigJntOffset1)
  {
    if(rigJntOffset1.Size() != 2 )
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << ": invalid rigid joint offset vector for node I\n"
		  << "Size must be 2\n";
      }
    else
      if(rigJntOffset1.Norm() > 0.0)
        {
          nodeIOffset(0) = rigJntOffset1(0);
          nodeIOffset(1) = rigJntOffset1(1);
        }
  }

//! @brief check rigid joint offset for node J
void XC::CrdTransf2d::set_rigid_joint_offsetJ(const Vector &rigJntOffset2)
  {
    if(rigJntOffset2.Size() != 2 )
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << ": invalid rigid joint offset vector for node J\n"
		  << "Size must be 2\n";      
      }
    else
      if(rigJntOffset2.Norm() > 0.0)
        {
          nodeJOffset(0) = rigJntOffset2(0);
          nodeJOffset(1) = rigJntOffset2(1);
        }
  }


//! @brief Checks for initial displacements on nodes
//! and, if they exist, 
void XC::CrdTransf2d::nodes_init_disp(void)
  {
    // see if there is some initial displacements at nodes
    if(initialDispChecked == false)
      {
        
        const Vector &nodeIDisp= nodeIPtr->getDisp();
        const Vector &nodeJDisp= nodeJPtr->getDisp();
        for(int i=0; i<3; i++)
          if(nodeIDisp(i) != 0.0)
            {
              nodeIInitialDisp.resize(3);
              for(int j=0; j<3; j++)
                nodeIInitialDisp[j] = nodeIDisp(j);
              i = 3;
            }
        for(int j=0; j<3; j++)
          if(nodeJDisp(j) != 0.0)
            {
              nodeJInitialDisp.resize(3);
              for(int i=0; i<3; i++)
                nodeJInitialDisp[i] = nodeJDisp(i);
              j = 3;
            }
                
        initialDispChecked = true;
      }
  }

//! @brief Calculates initial displecements from those
//! of the nodes.
int XC::CrdTransf2d::initialize(Node *nodeIPointer, Node *nodeJPointer)
  {
    set_node_ptrs(nodeIPointer,nodeJPointer);
    
    // see if there is some initial displacements at nodes
    if(initialDispChecked == false)
      {
        
        const Vector &nodeIDisp= nodeIPtr->getDisp();
        const Vector &nodeJDisp= nodeJPtr->getDisp();
        for(int i=0; i<3; i++)
          if(nodeIDisp(i) != 0.0)
            {
              nodeIInitialDisp.resize(3);
              for(int j=0; j<3; j++)
                nodeIInitialDisp[j] = nodeIDisp(j);
              i = 3;
            }
        for(int j=0; j<3; j++)
          if(nodeJDisp(j) != 0.0)
            {
              nodeJInitialDisp.resize(3);
              for(int i=0; i<3; i++)
                nodeJInitialDisp[i] = nodeJDisp(i);
              j = 3;
            }
                
        initialDispChecked = true;
      }

    int error= 0;
    // get element length and orientation
    if((error = this->computeElemtLengthAndOrient()))
      return error;
    return 0;
  }

//! @brief Computes the length and the orientation of the associated element.
int XC::CrdTransf2d::computeElemtLengthAndOrient(void) const
  {
    // element projection
    static Vector dx(2);
    if(nodeIPtr && nodeJPtr)
      {
        const Vector &ndICoords= nodeIPtr->getCrds();
        const Vector &ndJCoords= nodeJPtr->getCrds();
    
        dx(0)= ndJCoords(0) - ndICoords(0);
        dx(1)= ndJCoords(1) - ndICoords(1);
    
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
    
        dx(0)+= nodeJOffset(0);
        dx(1)+= nodeJOffset(1);
    
        dx(0)-= nodeIOffset(0);
        dx(1)-= nodeIOffset(1);
    
        // calculate the element length
        L = dx.Norm();
    
        if(L == 0.0) 
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
	              << ": zero length. "
                      << std::endl
                      << " Node I: " << nodeIPtr->getTag()
		      << " coords: " << ndICoords
                      << std::endl
                      << " node J: " << nodeJPtr->getTag()
		      << " coords: " << ndJCoords
                      << std::endl;
            return -2;  
          }
    
        // calculate the element local x axis components (direction cossines)
        // wrt to the global coordinates 
        cosTheta= dx(0)/L;
        sinTheta= dx(1)/L;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; pointer to node not set." << std::endl;
    return 0;
  }

//! @brief Return the displacements expresados on the basic system.
const XC::Vector &XC::CrdTransf2d::getBasicTrialDisp(void) const
  {
    // determine global displacements
    const Vector &disp1 = nodeIPtr->getTrialDisp();
    const Vector &disp2 = nodeJPtr->getTrialDisp();
    
    static double ug[6];
    for(register int i= 0;i<3;i++)
      {
        ug[i]   = disp1(i);
        ug[i+3] = disp2(i);
      }
    
    if(!nodeIInitialDisp.empty())
      {
        for(register int j=0;j<3;j++)
          ug[j]-= nodeIInitialDisp[j];
      }
    
    if(!nodeJInitialDisp.empty())
      {
        for(register int j=0;j<3;j++)
          ug[j+3]-= nodeJInitialDisp[j];
      }
    
    static Vector ub(3);
    // ub(0)= dx2-dx1: Element elongation.
    // ub(1)= (dy1-dy2)/L+gz1: Rotation about z axis.
    // ub(2)= (dy1-dy2)/L+gz2: Rotation about z axis.
    
    const double oneOverL = 1.0/L;
    const double sl = sinTheta*oneOverL;
    const double cl = cosTheta*oneOverL;
    
    ub(0)= -cosTheta*ug[0] - sinTheta*ug[1] + cosTheta*ug[3] + sinTheta*ug[4];
    
    ub(1)= -sl*ug[0] + cl*ug[1] + ug[2] + sl*ug[3] - cl*ug[4];
    
    const double t02= T02();
    const double t12= T12();
    ub(0)-= t02*ug[2];
    ub(1)+= oneOverL*t12*ug[2];
    
    const double t35= T35();
    const double t45= T45();
    ub(0)+= t35*ug[5];
    ub(1)-= oneOverL*t45*ug[5];
    
    ub(2)= ub(1)+ug[5]-ug[2];
    return ub;
  }

//! @brief Return the incrementos de displacement expresados on the basic system.
const XC::Vector &XC::CrdTransf2d::getBasicIncrDisp(void) const
  {
    // determine global displacements
    const Vector &disp1 = nodeIPtr->getIncrDisp();
    const Vector &disp2 = nodeJPtr->getIncrDisp();
    
    static double dug[6];
    for(register int i= 0;i<3;i++)
      {
        dug[i]   = disp1(i);
        dug[i+3] = disp2(i);
      }
    
    static XC::Vector dub(3);
    
    const double oneOverL = 1.0/L;
    const double sl = sinTheta*oneOverL;
    const double cl = cosTheta*oneOverL;
    
    dub(0) = -cosTheta*dug[0] - sinTheta*dug[1] + cosTheta*dug[3] + sinTheta*dug[4];
    
    dub(1) = -sl*dug[0] + cl*dug[1] + dug[2] + sl*dug[3] - cl*dug[4];
    
    const double t02= T02();
    const double t12= T12();
    dub(0)-= t02*dug[2];
    dub(1)+= oneOverL*t12*dug[2];
    
    const double t35= T35();
    const double t45= T45();
    dub(0) += t35*dug[5];
    dub(1) -= oneOverL*t45*dug[5];

    dub(2) = dub(1) + dug[5] - dug[2];
    return dub;
  }

const XC::Vector &XC::CrdTransf2d::getBasicIncrDeltaDisp(void) const
  {
    // determine global displacements
    const Vector &disp1 = nodeIPtr->getIncrDeltaDisp();
    const Vector &disp2 = nodeJPtr->getIncrDeltaDisp();
    
    static double Dug[6];
    for(register int i = 0; i < 3; i++)
      {
        Dug[i]   = disp1(i);
        Dug[i+3] = disp2(i);
      }
    
    static XC::Vector Dub(3);
    
    const double oneOverL = 1.0/L;
    const double sl = sinTheta*oneOverL;
    const double cl = cosTheta*oneOverL;
    
    Dub(0)= -cosTheta*Dug[0] - sinTheta*Dug[1] + cosTheta*Dug[3] + sinTheta*Dug[4];
    
    Dub(1) = -sl*Dug[0] + cl*Dug[1] + Dug[2] + sl*Dug[3] - cl*Dug[4];
    
    const double t02= T02();
    const double t12= T12();
    Dub(0) -= t02*Dug[2];
    Dub(1) += oneOverL*t12*Dug[2];
    
    const double t35= T35();
    const double t45= T45();
    Dub(0) += t35*Dug[5];
    Dub(1) -= oneOverL*t45*Dug[5];
    
    Dub(2) = Dub(1) + Dug[5] - Dug[2];    
    return Dub;
  }

const XC::Vector &XC::CrdTransf2d::getBasicTrialVel(void) const
  {
    // determine global velocities
    const XC::Vector &vel1 = nodeIPtr->getTrialVel();
    const XC::Vector &vel2 = nodeJPtr->getTrialVel();
	
    static double vg[6];
    for(int i = 0; i < 3; i++)
      {
	vg[i]   = vel1(i);
	vg[i+3] = vel2(i);
      }
	
    static XC::Vector vb(3);
	
    const double oneOverL = 1.0/L;
    const double sl = sinTheta*oneOverL;
    const double cl = cosTheta*oneOverL;
	
    vb(0) = -cosTheta*vg[0] - sinTheta*vg[1] + cosTheta*vg[3] + sinTheta*vg[4];
	
    vb(1) = -sl*vg[0] + cl*vg[1] + vg[2] + sl*vg[3] - cl*vg[4];
	
    const double t02= T02();
    const double t12= T12();
    vb(0) -= t02*vg[2];
    vb(1) += oneOverL*t12*vg[2];
	
    const double t35= T35();
    const double t45= T45();
    vb(0)+= t35*vg[5];
    vb(1)-= oneOverL*t45*vg[5];
	
    vb(2) = vb(1) + vg[5] - vg[2];	
    return vb;
  }


const XC::Vector &XC::CrdTransf2d::getBasicTrialAccel(void) const
  {
    // determine global accelerations
    const XC::Vector &accel1 = nodeIPtr->getTrialAccel();
    const XC::Vector &accel2 = nodeJPtr->getTrialAccel();
    
    static double ag[6];
    for(int i = 0; i < 3; i++)
      {
        ag[i]   = accel1(i);
        ag[i+3] = accel2(i);
      }
    
    static Vector ab(3);
    
    const double oneOverL = 1.0/L;
    const double sl = sinTheta*oneOverL;
    const double cl = cosTheta*oneOverL;
    
    ab(0) = -cosTheta*ag[0] - sinTheta*ag[1] + cosTheta*ag[3] + sinTheta*ag[4];
    
    ab(1) = -sl*ag[0] + cl*ag[1] + ag[2] + sl*ag[3] - cl*ag[4];
    
    const double t02= T02();
    const double t12= T12();
    ab(0) -= t02*ag[2];
    ab(1) += oneOverL*t12*ag[2];
    
    const double t35= T35();
    const double t45= T45();
    ab(0) += t35*ag[5];
    ab(1) -= oneOverL*t45*ag[5];
    
    ab(2) = ab(1) + ag[5] - ag[2];    
    return ab;
  }

//! @brief Return the $\vec{i}$ unit vector of the local axis
//! expressed in global coordinates for the initial (undeformed) geometry.
const XC::Vector &XC::CrdTransf2d::getInitialI(void) const
  {
    computeElemtLengthAndOrient();
    static Vector vectorI(2);
    vectorI(0)= cosTheta;
    vectorI(1)= sinTheta;
    return vectorI;
  }

//! @brief Return the $\vec{i}$ unit vector of the local axis
//! expressed in global coordinates for the current geometry.
const XC::Vector &XC::CrdTransf2d::getI(void) const
  { return getInitialI(); }


//! @brief Return the $\vec{j}$ unit vector of the local axis
//! expressed in global coordinates for the initial (undeformed) geometry.
const XC::Vector &XC::CrdTransf2d::getInitialJ(void) const
  {
    computeElemtLengthAndOrient();
    static Vector vectorJ(2);
    vectorJ(0)= -sinTheta;
    vectorJ(1)= cosTheta;
    return vectorJ;
  }

//! @brief Return the $\vec{j}$ unit vector of the local axis
//! expressed in global coordinates for the current geometry.
const XC::Vector &XC::CrdTransf2d::getJ(void) const
  { return getInitialJ(); }

//| @brief Return the $\vec{i}$ and $\vec{j}$ unit vector of the local
//! axis for the initial (undeformed) geometry.
int XC::CrdTransf2d::getInitialLocalAxes(Vector &XAxis, Vector &YAxis) const
  {
    XAxis(0)= cosTheta; XAxis(1)= sinTheta;
    YAxis(0)= -sinTheta; YAxis(1)= cosTheta;
    return 0;
  }

//| @brief Return the direction vectors of local axis in the element
//! current position.
int XC::CrdTransf2d::getLocalAxes(Vector &XAxis, Vector &YAxis) const
  { return getInitialLocalAxes(XAxis,YAxis); }

//! @brief Returns a matrix with the axes of the element as matrix rows
//! [[x1,y1,z1],[x2,y2,z2],...·]
XC::Matrix XC::CrdTransf2d::getLocalAxes(bool initialGeometry) const
  {
    Vector vectorI(2);
    Vector vectorJ(2);
    if(initialGeometry)
      getInitialLocalAxes(vectorI,vectorJ);
    else
      getLocalAxes(vectorI,vectorJ);
    Matrix retval(2,2);
    retval(0,0)= vectorI(0); retval(0,1)= vectorI(1);
    retval(1,0)= vectorJ(0); retval(1,1)= vectorJ(1);
    return retval;
  }

//! @brief Return the global coordinates of the point.
const XC::Vector &XC::CrdTransf2d::getPointGlobalCoordFromBasic(const double &xi) const
  {
    static Vector local_coord(2),global_coord(2);
    local_coord.Zero();
    local_coord[0]= xi*getDeformedLength();
    global_coord= getPointGlobalCoordFromLocal(local_coord);
    return global_coord;
  }

//! @brief Return the position of the node I.
Pos2d XC::CrdTransf2d::getPosNodeI(void) const
  {
    Pos2d retval= nodeIPtr->getCurrentPosition2d();
    retval+= Vector2d(nodeIOffset(0),nodeIOffset(1));
    if(!nodeIInitialDisp.empty())
      retval-= Vector2d(nodeIInitialDisp[0],nodeIInitialDisp[1]);
    return retval;
  }

//! @brief Return the position of the node J.
Pos2d XC::CrdTransf2d::getPosNodeJ(void) const
  {
    Pos2d retval= nodeJPtr->getCurrentPosition2d();
    retval+= Vector2d(nodeJOffset(0),nodeJOffset(1));
    if(!nodeJInitialDisp.empty())
      retval-= Vector2d(nodeJInitialDisp[0],nodeJInitialDisp[1]);
    return retval;
  }

//! @brief Return the local reference system.
Ref2d2d XC::CrdTransf2d::getLocalReference(void) const
  {
    const Vector vI= getI();
    return Ref2d2d(getPosNodeI(),Vector2d(vI[0],vI[1]));
  }

//! @brief Return the local coordinates of the point from the local ones.
XC::Vector XC::CrdTransf2d::getPointLocalCoordFromGlobal(const Vector &xg) const
  {
    Ref2d2d ref= getLocalReference();
    Pos2d pl= ref.GetPosLocal(Pos2d(xg[0],xg[1]));
    Vector retval(2);
    retval[0]= pl.x(); retval[1]= pl.y();
    return retval;  
  }

//! @brief Return the global coordinates of the points.
const XC::Matrix &XC::CrdTransf2d::getPointsGlobalCoordFromBasic(const Vector &basicCoords) const
  {
    static Matrix retval;
    const size_t numPts= basicCoords.Size(); //Number of points to transform.
    retval.resize(numPts,2);
    Vector xg(2);
    for(size_t i= 0;i<numPts;i++)
      {
        xg= getPointGlobalCoordFromBasic(basicCoords(i));
        for(size_t j= 0;j<2;j++)
          retval(i,j)= xg[j];
      }
    return retval;
  }

//! @brief Return the vector expressed in global coordinates.
const XC::Vector &XC::CrdTransf2d::getVectorGlobalCoordFromLocal(const Vector &localCoords) const
  {
    static XC::Vector retval(2);
    // retval = Rlj'*localCoords (Multiplica el vector por R traspuesta).
    retval(0)= cosTheta*localCoords(0) - sinTheta*localCoords(1);
    retval(1)= sinTheta*localCoords(0) + cosTheta*localCoords(1);
    return retval;
  }

//! @brief Return the vectors expressed in global coordinates.
const XC::Matrix &XC::CrdTransf2d::getVectorGlobalCoordFromLocal(const Matrix &localCoords) const
  {
    static Matrix retval;
    const size_t numPts= localCoords.noRows(); //Number of vectors to transform.
    retval.resize(numPts,2);
    for(size_t i= 0;i<numPts;i++)
      {
        // retval = Rlj'*localCoords (Multiplica el vector por R traspuesta).
        retval(i,0)= cosTheta*localCoords(i,0) - sinTheta*localCoords(i,1);
        retval(i,1)= sinTheta*localCoords(i,0) + cosTheta*localCoords(i,1);
      }
    return retval;
  }

//! @brief Return the vector expressed in local coordinates.
const XC::Vector &XC::CrdTransf2d::getVectorLocalCoordFromGlobal(const Vector &globalCoords) const
  {
    static XC::Vector retval(2);
    retval(0)=  cosTheta*globalCoords(0) + sinTheta*globalCoords(1);
    retval(1)= -sinTheta*globalCoords(0) + cosTheta*globalCoords(1);
    return retval;
  }

//! @brief Return the coordinates of the nodes as rows of the returned matrix.
const XC::Matrix &XC::CrdTransf2d::getCooNodes(void) const
  {
    static Matrix retval;
    retval= Matrix(2,2);

    retval(0,0)= nodeIPtr->getCrds()[0];
    retval(0,1)= nodeIPtr->getCrds()[1];

    retval(1,0)= nodeJPtr->getCrds()[0];
    retval(1,1)= nodeJPtr->getCrds()[1];

    return retval;
  }

//! @brief Return points distributed between the nodes as a matrix with the coordinates as rows.
const XC::Matrix &XC::CrdTransf2d::getCooPoints(const size_t &ndiv) const
  {
    const Pos3d p0= nodeIPtr->getInitialPosition3d();
    const Pos3d p1= nodeJPtr->getInitialPosition3d();
    Pos3dArray linea(p0,p1,ndiv);
    static Matrix retval;
    retval= Matrix(ndiv+1,2);
    Pos3d tmp;
    for(size_t i= 0;i<ndiv+1;i++)
      {
        tmp= linea(i+1,1);
        retval(i,0)= tmp.x();
        retval(i,1)= tmp.y();
      }
    return retval;
  }

//! @brief Return the point that correspond to the relative coordinate 0<=xrel<=1.
const XC::Vector &XC::CrdTransf2d::getCooPoint(const double &xrel) const
  {
    const Pos3d p0= nodeIPtr->getInitialPosition3d();
    const Pos3d p1= nodeJPtr->getInitialPosition3d();
    const Vector3d v= p1-p0;
    static Vector retval(2);
    const Pos3d tmp= p0+xrel*v;
    retval(0)= tmp.x();
    retval(1)= tmp.y();
    return retval;
  }

//! @brief Sends object members through the communicator argument.
int XC::CrdTransf2d::sendData(Communicator &comm)
  {
    int res= CrdTransf::sendData(comm);
    res+= comm.sendDoubles(cosTheta,sinTheta,getDbTagData(),CommMetaData(9));
    if(res<0)
      std::cerr << "CrdTransf2d::sendData - failed to send data.\n";
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::CrdTransf2d::recvData(const Communicator &comm)
  {
    int res= CrdTransf::recvData(comm);
    res+= comm.receiveDoubles(cosTheta,sinTheta,getDbTagData(),CommMetaData(9));
    if(res<0)
      std::cerr << "CrdTransf2d::recvData - failed to receive data.\n";
    return res;    
  }

