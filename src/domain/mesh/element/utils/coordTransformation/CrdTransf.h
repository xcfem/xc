// -*-c++-*-
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

// $Revision: 1.4 $
// $Date: 2005/12/15 00:30:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/coordTransformation/CrdTransf.h,v $


// File: ~/crdTransf/CrdTransf.h
//
// Written: Remo Magalhaes de Souza (rmsouza@ce.berkeley.edu)
// Created: 04/2000
// Revision: A
//
// Description: This file contains the class definition for 
//
// What: "@(#) CrdTransf.h, revA"

#ifndef CrdTransf_h
#define CrdTransf_h

#include "utility/actor/actor/MovableObject.h"
#include <utility/tagged/TaggedObject.h>
#include <utility/matrix/Vector.h>

class Pos3dArray3d;

namespace XC {
class Matrix;
class Node;
class TransfCooHandler;


//! @ingroup FEMisc
//!
//! @defgroup ElemCT Element coordinate transformations.
//
//! @ingroup ElemCT
//
//! @brief CrdTransf provides the abstraction of a frame 
//! coordinate transformation. It is an abstract base class and 
//! thus no objects of  it's type can be instatiated. It has pure 
//! virtual functions which  must be implemented in it's derived classes.
class CrdTransf: public TaggedObject, public MovableObject
  {
  protected:
    Node *nodeIPtr, *nodeJPtr; //!< pointers to the end nodes of the element.
    mutable double L; //!< element length
    Vector nodeIOffset,nodeJOffset; //!< rigid joint offsets
    std::vector<double> nodeIInitialDisp; //!< Initial displacement for I node.
    std::vector<double> nodeJInitialDisp; //!< Initial displacement for J node.
    bool initialDispChecked;

    int set_node_ptrs(Node *nodeIPointer, Node *nodeJPointer);

    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);
    virtual void set_rigid_joint_offsetI(const Vector &rigJntOffsetI)= 0;
    virtual void set_rigid_joint_offsetJ(const Vector &rigJntOffsetJ)= 0;
  public:
    CrdTransf(int tag, int classTag, int dim_joint_offset);
    CrdTransf(void);
    virtual ~CrdTransf(void);
    
    const TransfCooHandler *GetTransfCooHandler(void) const;
    TransfCooHandler *GetTransfCooHandler(void);
    std::string getName(void) const;
    virtual int getDimension(void) const= 0;

    virtual int initialize(Node *node1Pointer, Node *node2Pointer) = 0;
    virtual int update(void) = 0;
    virtual double getInitialLength(void) const= 0;
    virtual double getDeformedLength(void) const= 0;
    double getLength(bool initialGeometry= true) const;
    
    virtual int commitState(void) = 0;
    virtual int revertToLastCommit(void) = 0;        
    virtual int revertToStart(void) = 0;
    
    virtual const Vector &getBasicTrialDisp(void) const= 0;
    virtual const Vector &getBasicIncrDisp(void) const= 0;
    virtual const Vector &getBasicIncrDeltaDisp(void) const= 0;
    virtual const Vector &getBasicTrialVel(void) const= 0;
    virtual const Vector &getBasicTrialAccel(void) const= 0;
    
    // AddingSensitivity:BEGIN //////////////////////////////////
    virtual const Vector &getBasicDisplSensitivity(int gradNumber);
    virtual const Vector &getGlobalResistingForceShapeSensitivity(const Vector &, const Vector &);
    virtual const Vector &getGlobalResistingForceShapeSensitivity(const Vector &pb, const Vector &p0, int gradNumber);
    virtual const Vector &getBasicTrialDispShapeSensitivity(void);
    virtual bool isShapeSensitivity(void) {return false;}
    virtual double getdLdh(void) {return 0.0;}
    virtual double getd1overLdh(void) {return 0.0;}
    // AddingSensitivity:END //////////////////////////////////
    
    virtual const Vector &getGlobalResistingForce(const Vector &basicForce, const Vector &uniformLoad) const= 0;
    virtual const Matrix &getGlobalStiffMatrix(const Matrix &basicStiff, const Vector &basicForce) const= 0;
    virtual const Matrix &getInitialGlobalStiffMatrix(const Matrix &basicStiff) const= 0;
    
    virtual const Vector &getI(void) const= 0;
    virtual const Vector &getJ(void) const= 0;
    virtual Matrix getLocalAxes(bool) const= 0;

    virtual const Vector &getPointGlobalCoordFromLocal(const Vector &localCoords) const= 0;
    virtual const Vector &getPointGlobalCoordFromBasic(const double &xi) const= 0;
    virtual Vector getPointLocalCoordFromGlobal(const Vector &globalCoords) const= 0;
    double getPointBasicCoordFromGlobal(const Vector &globalCoords) const;
    const Matrix &getPointsGlobalCoordFromLocal(const Matrix &localCoords) const;
    virtual const Matrix &getPointsGlobalCoordFromBasic(const Vector &) const= 0;
    virtual const Vector &getPointGlobalDisplFromBasic(double xi, const Vector &basicDisps) const= 0;

    virtual const Vector &getVectorGlobalCoordFromLocal(const Vector &localCoords) const= 0;
    virtual const Matrix &getVectorGlobalCoordFromLocal(const Matrix &localCoords) const= 0;
    virtual const Vector &getVectorLocalCoordFromGlobal(const Vector &globalCoords) const= 0;

    virtual const Matrix &getCooNodes(void) const= 0;
    virtual const Matrix &getCooPoints(const size_t &ndiv) const= 0;
    virtual const Vector &getCooPoint(const double &xrel) const= 0;

  };
} // end of XC namespace

#endif
