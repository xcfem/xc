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
// $Source: /usr/local/cvs/OpenSees/SRC/CrdTransf3d.h,v $


// File: ~/CrdTransf/CrdTransf3d.h
//
// Written: Remo Magalhaes de Souza (rmsouza@ce.berkeley.edu)
// Created: 04/2000
// Revision: A
//
// Description: This file contains the class definition for 
// CrdTransf3d. CrdTransf3d provides the abstraction of spatial 
// coordinate transformation for a 3d frame. 

//
// What: "@(#) CrdTransf3d.h, revA"

#ifndef CrdTransf3d_h
#define CrdTransf3d_h

#include "CrdTransf.h"
#include "utility/matrix/Matrix.h"

class Pos3d;
class Ref3d3d;

namespace XC {
class Vector;

//! @ingroup ElemCT
//
//! @brief Base class for 3D coordinate transformation.
class CrdTransf3d: public CrdTransf
  {
  protected:
    mutable Matrix R; //!< Transformation matrix

    void set_rigid_joint_offsetI(const Vector &rigJntOffsetI);
    void set_rigid_joint_offsetJ(const Vector &rigJntOffsetJ);
    static void inic_ug(const Vector &d1,const Vector &d2,double *ug);
    void modif_ug_init_disp(double *ug) const;
    void global_to_local(const double *ug,double *ul) const;
    void calc_Wu(const double *ug,double *ul,double *Wu) const;
    const Vector &calc_ub(const double *ul,Vector &) const;

    static Vector vectorI;
    static Vector vectorJ;
    static Vector vectorK;
    static Vector vectorCoo;
    virtual int computeElemtLengthAndOrient(void) const= 0;
    virtual int computeLocalAxis(void) const= 0;

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    CrdTransf3d(int tag, int classTag);
    CrdTransf3d(int tag, int classTag, const Vector &vecInLocXZPlane);
    inline virtual int getDimension(void) const
      { return 3; }
    
    virtual CrdTransf3d *getCopy(void) const= 0;
    int initialize(Node *node1Pointer, Node *node2Pointer);
    virtual void set_xz_vector(const Vector &vecInLocXZPlane);
    Vector get_xz_vector(void) const;
    const Vector &getI(void) const;
    const Vector &getJ(void) const;
    const Vector &getK(void) const;
    Matrix getLocalAxes(bool) const;
    int getLocalAxes(Vector &xAxis, Vector &yAxis, Vector &zAxis) const; 
    Pos3d getPosNodeI(void) const;
    Pos3d getPosNodeJ(void) const;
    Ref3d3d getLocalReference(void) const;
    Vector getPointLocalCoordFromGlobal(const Vector &xg) const;
    const Vector &getPointGlobalCoordFromBasic(const double &xi) const;
    const Matrix &getPointsGlobalCoordFromBasic(const Vector &) const;
    const Vector &getVectorGlobalCoordFromLocal(const Vector &localCoords) const;
    const Matrix &getVectorGlobalCoordFromLocal(const Matrix &localCoords) const;
    const Vector &getVectorLocalCoordFromGlobal(const Vector &globalCoords) const;

    const Matrix &getCooNodes(void) const;
    const Matrix &getCooPoints(const size_t &ndiv) const;
    const Vector &getCooPoint(const double &xrel) const;

    void gira(const double &);
  };
} // end of XC namespace

#endif
