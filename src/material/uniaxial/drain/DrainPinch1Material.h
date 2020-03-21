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
                                                                        
// $Revision: 1.1 $
// $Date: 2001/08/18 21:39:05 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/drain/DrainPinch1Material.h,v $
                                                                      
// Written: MHS
// Created: June 2001
//
// Description: This file contains the class definition for 
// DrainPinch1Material.

#ifndef DrainPinch1Material_h
#define DrainPinch1Material_h

#include "material/uniaxial/drain/DrainMaterial.h"

namespace XC {
//! @ingroup Drain2DX
//
//! @brief Wrapper for <a href="https://nisee.berkeley.edu/elibrary/getpkg?id=DRAIN2DX">DRAIN-2DX</a> pinching effect material.
class DrainPinch1Material: public DrainMaterial
  {
  public:
    DrainPinch1Material(int tag,
		double E, double fyp, double fyn, double alpha,
		double ecaps, double ecapk, double ecapa, double ecapd,
		double cs, double ck, double ca, double cd,
		double capSlope, double capDispP, double capDispN,
		double fpp, double fpn, double pinch, double res, double beto = 0.0);
    DrainPinch1Material(int tag, const Vector &input, double beto = 0.0);
    DrainPinch1Material(int tag);
    DrainPinch1Material(void);

    int revertToStart(void);

    UniaxialMaterial *getCopy(void) const;
  };
} // end of XC namespace


#endif

