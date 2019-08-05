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
// $Date: 2005/12/15 00:30:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/CrdTransf2d.h,v $


// File: ~/CrdTransf/CrdTransf2d.h
//
// Written: Remo Magalhaes de Souza (rmsouza@ce.berkeley.edu)
// Created: 04/2000
// Revision: A
//
// Description: This file contains the class definition for 
// CrdTransf2d. CrdTransf2d provides the abstraction of spatial 
// coordinate transformation for a 2d frame. 

//
// What: "@(#) CrdTransf2d.h, revA"

#ifndef CrdTransf2d_h
#define CrdTransf2d_h

#include "CrdTransf.h"

class Pos2d;
class Ref2d2d;

namespace XC {
//! @ingroup ElemCT
//
//! @brief Base class for 2D coordinate transformation.
class CrdTransf2d: public CrdTransf
  {
  protected:
    mutable double cosTheta, sinTheta; //!< direction cosines of undeformed element wrt to global system 
    void set_rigid_joint_offsetI(const Vector &rigJntOffsetI);
    void set_rigid_joint_offsetJ(const Vector &rigJntOffsetJ);
    inline double T02(void) const
      { return -cosTheta*nodeIOffset(1)+sinTheta*nodeIOffset(0); }
    inline double T12(void) const
      { return  sinTheta*nodeIOffset(1)+cosTheta*nodeIOffset(0); }
    inline double T35(void) const
      { return -cosTheta*nodeJOffset(1)+sinTheta*nodeJOffset(0); }
    inline double T45(void) const
      { return  sinTheta*nodeJOffset(1)+cosTheta*nodeJOffset(0); }

    void nodes_init_disp(void);
    int computeElemtLengthAndOrient(void) const;

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    CrdTransf2d(int tag, int classTag);
    inline virtual int getDimension(void) const
      { return 2; }
    
    int initialize(Node *node1Pointer, Node *node2Pointer);    
    virtual CrdTransf2d *getCopy(void) const= 0;
    //! @brief Return the initial length of the element.
    inline double getInitialLength(void) const
      { return L; }
    //! @brief Return the length of the deformed element.
    inline double getDeformedLength(void) const
      { return L; }
    const Vector &getBasicTrialDisp(void) const;
    const Vector &getBasicIncrDisp(void) const;
    const Vector &getBasicIncrDeltaDisp(void) const;
    const Vector &getBasicTrialVel(void) const;
    const Vector &getBasicTrialAccel(void) const;

    const Vector &getInitialI(void) const;
    const Vector &getI(void) const;
    const Vector &getInitialJ(void) const;
    const Vector &getJ(void) const;
    int getInitialLocalAxes(Vector &xAxis, Vector &yAxis) const; 
    int getLocalAxes(Vector &xAxis, Vector &yAxis) const;
    Matrix getLocalAxes(bool) const;
    Pos2d getPosNodeI(void) const;
    Pos2d getPosNodeJ(void) const;
    Ref2d2d getLocalReference(void) const;
    Vector getPointLocalCoordFromGlobal(const Vector &xg) const;
    const Vector &getPointGlobalCoordFromBasic(const double &xi) const;
    const Matrix &getPointsGlobalCoordFromBasic(const Vector &) const;
    const Vector &getVectorGlobalCoordFromLocal(const Vector &localCoords) const;
    const Matrix &getVectorGlobalCoordFromLocal(const Matrix &localCoords) const;
    const Vector &getVectorLocalCoordFromGlobal(const Vector &globalCoords) const;

    const Matrix &getCooNodes(void) const;
    const Matrix &getCooPoints(const size_t &ndiv) const;
    const Vector &getCooPoint(const double &xrel) const;
  };
} // end of XC namespace

#endif
