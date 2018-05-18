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
// $Date: 2001/10/01 17:08:54 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/drain/DrainBilinearMaterial.cpp,v $

// Written: MHS
// Created: June 2001
//
// Description: This file contains the class definition for 
// DrainBilinearMaterial.

#include <material/uniaxial/drain/DrainBilinearMaterial.h>
#include <utility/matrix/Vector.h>

// 17 history variables and 16 material parameters
const int numHistoryVariables= 17;
const int numMaterialParameters= 16;

XC::DrainBilinearMaterial::DrainBilinearMaterial(int tag,
	double E, double fyp, double fyn, double alpha,
	double ecaps, double ecapk, double ecapa, double ecapd,
	double cs, double ck, double ca, double cd,
	double capSlope, double capDispP, double capDispN, double res,
	double b):
// 17 history variables and 16 material parameters
 XC::DrainMaterial(tag, MAT_TAG_DrainBilinear, numHistoryVariables, numMaterialParameters, b)
{
	matParams[0]  = E;
	matParams[1]  = fyp;
	matParams[2]  = fyn;
	matParams[3]  = alpha;
	matParams[4]  = ecaps;
	matParams[5]  = ecapk;
	matParams[6]  = ecapa;
	matParams[7]  = ecapd;
	matParams[8]  = cs;
	matParams[9]  = ck;
	matParams[10] = ca;
	matParams[11] = cd;
	matParams[12] = capSlope;
	matParams[13] = capDispP;
	matParams[14] = capDispN;
	matParams[15] = res;

	// Initialize history variables
	this->revertToStart();
}

XC::DrainBilinearMaterial::DrainBilinearMaterial(int tag, const XC::Vector &input, double b):
// 17 history variables and 16 material parameters
 XC::DrainMaterial(tag, MAT_TAG_DrainBilinear, numHistoryVariables, numMaterialParameters, b)
  {
    for(int i = 0; i < numMaterialParameters; i++)
	    matParams[i] = input(i);

    // Initialize history variables
    this->revertToStart();
  }

//! @brief Constructor.
XC::DrainBilinearMaterial::DrainBilinearMaterial(int tag)
  :XC::DrainMaterial(tag, MAT_TAG_DrainBilinear, numHistoryVariables, numMaterialParameters)
  {}

XC::DrainBilinearMaterial::DrainBilinearMaterial(void)
: XC::DrainMaterial(0, MAT_TAG_DrainBilinear, numHistoryVariables, numMaterialParameters)
{}

int XC::DrainBilinearMaterial::revertToStart(void)
  {
    hstv[0]  = 0.0;
    hstv[1]  = 0.0;
    hstv[2]  = matParams[1];		// fyp
    hstv[3]  = matParams[1];		// fyp
    hstv[4]  = matParams[2];		// fyn
    hstv[5]  = matParams[2];		// fyn
    hstv[6]  = matParams[0];		// E
    hstv[7]  = matParams[0];		// E
    hstv[8]  = matParams[13];	// capDispP
    hstv[9]  = matParams[13];	// capDispP
    hstv[10] = matParams[14];	// capDispN
    hstv[11] = matParams[14];	// capDispN
    hstv[12] = 0.0;
    hstv[13] = 0.0;
    hstv[14] = 0.0;
    hstv[15] = matParams[0];		// E
    hstv[16] = matParams[15];	// res

    // Set trial history variables to committed values
    for(int i = 0; i < numHistoryVariables; i++)
      hstv[i+numHistoryVariables] = hstv[i];
    return 0;
  }

//! @brief Virtual constructor.
XC::UniaxialMaterial *XC::DrainBilinearMaterial::getCopy(void) const
  { return new DrainBilinearMaterial(*this); }
