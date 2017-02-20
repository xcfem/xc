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
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/drain/DrainClough1Material.cpp,v $

// Written: MHS
// Created: June 2001
//
// Description: This file contains the class definition for 
// DrainClough1Material.

#include <material/uniaxial/drain/DrainClough1Material.h>
#include <utility/matrix/Vector.h>

XC::DrainClough1Material::DrainClough1Material(int tag,
	double E, double fyp, double fyn, double alpha,
	double ecaps, double ecapk, double ecapa, double ecapd,
	double cs, double ck, double ca, double cd,
	double capSlope, double capDispP, double capDispN, double res,
	double b):
// 15 history variables and 16 material parameters
 XC::DrainMaterial(tag, MAT_TAG_DrainClough1, 15, 16, b)
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

XC::DrainClough1Material::DrainClough1Material(int tag, const XC::Vector &input, double b):
// 15 history variables and 16 material parameters
 XC::DrainMaterial(tag, MAT_TAG_DrainClough1, 15, 16, b)
{
	for (int i = 0; i < 16; i++)
		matParams[i] = input(i);

	// Initialize history variables
	this->revertToStart();
}

//! @brief Constructor.
XC::DrainClough1Material::DrainClough1Material(int tag)
  : XC::DrainMaterial(tag, MAT_TAG_DrainClough1, 15, 16)
  {}

XC::DrainClough1Material::DrainClough1Material(void):
 XC::DrainMaterial(0, MAT_TAG_DrainClough1, 15, 16)
{
	// Does nothing
}

int
XC::DrainClough1Material::revertToStart(void)
{
	double dyp = matParams[1]/matParams[0];	// fyp/E
	double dyn = matParams[2]/matParams[0];	// fyn/E

	hstv[0]  = matParams[0];		// E
	hstv[1]  = matParams[0];		// E
	hstv[2]  = dyp;
	hstv[3]  = dyn; 
	hstv[4]  = 0.0;
	hstv[5]  = dyp;
	hstv[6]  = dyn;
	hstv[7]  = matParams[1];		// fyp
	hstv[8]  = matParams[2];		// fyn
	hstv[9]  = matParams[13];	// capDispP
	hstv[10] = matParams[14];	// capDispN
	hstv[11] = 0.0;
	hstv[12] = 0.0;
	hstv[13] = 0.0;
	hstv[14] = matParams[0];		// E

	// Set trial history variables to committed values
	for (int i = 0; i < 15; i++)
		hstv[i+15] = hstv[i];

	return 0;
}

XC::UniaxialMaterial *XC::DrainClough1Material::getCopy(void) const
  { return new DrainClough1Material(*this); }
