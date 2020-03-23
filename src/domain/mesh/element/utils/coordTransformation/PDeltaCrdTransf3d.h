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

// $Revision: 1.7 $
// $Date: 2005/12/15 00:30:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/PDeltaCrdTransf3d.h,v $

// Written: Remo Magalhaes de Souza (rmsouza@ce.berkeley.edu)
// Created: 04/2000
// Revision: A
//
// Description: This file contains the class definition for
// PDeltaCrdTransf3d.h. PDeltaCrdTransf3d provides the
// abstraction of a linear transformation for a spatial frame
// between the global and basic coordinate systems

// What: "@(#) PDeltaCrdTransf3d.h, revA"

#ifndef PDeltaCrdTransf3d_h
#define PDeltaCrdTransf3d_h

#include "SmallDispCrdTransf3d.h"

namespace XC {
//! @ingroup ElemCT
//
//! @brief PDeltaCrdTransf3d provides the
//! abstraction of a linear transformation for a spatial frame
//! between the global and basic coordinate systems
class PDeltaCrdTransf3d: public SmallDispCrdTransf3d
  {
  private:
    double ul17; //!< Transverse local displacement offsets of P-Delta
    double ul28;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    PDeltaCrdTransf3d(int tag);
    PDeltaCrdTransf3d(int tag, const Vector &vecInLocXZPlane);
    PDeltaCrdTransf3d(int tag, const Vector &vecInLocXZPlane, const Vector &rigJntOffsetI, const Vector &rigJntOffsetJ);
    
    PDeltaCrdTransf3d(void);
    
    int update(void);
    
    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);
    
    const Vector &getGlobalResistingForce(const Vector &basicForce, const Vector &p0) const;
    const Matrix &getGlobalStiffMatrix(const Matrix &basicStiff, const Vector &basicForce) const;
    
    CrdTransf3d *getCopy(void) const;
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag = 0) const;
    
    // functions used in post-processing only    
    const Vector &getPointGlobalCoordFromLocal(const Vector &localCoords) const;
    const Vector &getPointGlobalDisplFromBasic(double xi, const Vector &basicDisps) const;
  };
} // end of XC namespace

#endif

