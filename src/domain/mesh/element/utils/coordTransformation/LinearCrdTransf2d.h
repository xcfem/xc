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

// $Revision: 1.8 $
// $Date: 2005/12/15 00:30:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/LinearCrdTransf2d.h,v $

// Written: Remo Magalhaes de Souza (rmsouza@ce.berkeley.edu)
// Created: 04/2000
// Revision: A
//
// Description: This file contains the class definition for
// LinearCrdTransf2d.h. LinearCrdTransf2d provides the
// abstraction of a linear transformation for a spatial frame
// between the global and basic coordinate systems

// What: "@(#) LinearCrdTransf2d.h, revA"

#ifndef LinearCrdTransf2d_h
#define LinearCrdTransf2d_h

#include "SmallDispCrdTransf2d.h"


namespace XC {
//! @ingroup ElemCT
//
//! @brief LinearCrdTransf2d provides the
//! abstraction of a linear transformation for a spatial frame
//! between the global and basic coordinate systems
class LinearCrdTransf2d: public SmallDispCrdTransf2d
  {
  public:
    LinearCrdTransf2d(int tag);
    LinearCrdTransf2d(int tag,const Vector &rigJntOffsetI,const Vector &rigJntOffsetJ);
    LinearCrdTransf2d(void);
    
    int update(void);
    
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
    
    // AddingSensitivity:BEGIN //////////////////////////////////
    const Vector &getBasicDisplSensitivity(int gradNumber);
    const Vector &getGlobalResistingForceShapeSensitivity(const Vector &basicForce, const Vector &p0);
    const Vector &getGlobalResistingForceShapeSensitivity(const Vector &pb, const Vector &p0, int gradNumber);
    const Vector &getBasicTrialDispShapeSensitivity(void);
    bool isShapeSensitivity(void);
    double getdLdh(void);
    double getd1overLdh(void);
    // AddingSensitivity:END //////////////////////////////////

    const Vector &getGlobalResistingForce(const Vector &basicForce, const Vector &p0) const;
    const Matrix &getGlobalStiffMatrix(const Matrix &basicStiff, const Vector &basicForce) const;
    const Matrix &getInitialGlobalStiffMatrix(const Matrix &basicStiff) const;
    
    CrdTransf2d *getCopy(void) const;
    
    void Print(std::ostream &s, int flag = 0);
  };
} // end of XC namespace

#endif

