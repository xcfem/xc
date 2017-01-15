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
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/drain/DrainClough2Material.cpp,v $

// Written: MHS
// Created: June 2001
//
// Description: This file contains the class definition for 
// DrainClough2Material.

#include <material/uniaxial/drain/DrainClough2Material.h>
#include <utility/matrix/Vector.h>

XC::DrainClough2Material::DrainClough2Material(int tag,
	double E, double fyp, double fyn, double alpha,
	double ecaps, double ecapk, double ecapa, double ecapd,
	double cs, double ck, double ca, double cd,
	double capSlope, double capDispP, double capDispN, double res,
	double b):
// 19 history variables and 16 material parameters
 XC::DrainMaterial(tag, MAT_TAG_DrainClough2, 19, 16, b)
{
	data[0]  = E;
	data[1]  = fyp;
	data[2]  = fyn;
	data[3]  = alpha;
	data[4]  = ecaps;
	data[5]  = ecapk;
	data[6]  = ecapa;
	data[7]  = ecapd;
	data[8]  = cs;
	data[9]  = ck;
	data[10] = ca;
	data[11] = cd;
	data[12] = capSlope;
	data[13] = capDispP;
	data[14] = capDispN;
	data[15] = res;

	// Initialize history variables
	this->revertToStart();
}

XC::DrainClough2Material::DrainClough2Material(int tag, const XC::Vector &input, double b):
// 19 history variables and 16 material parameters
 XC::DrainMaterial(tag, MAT_TAG_DrainClough2, 19, 16, b)
{
	for (int i = 0; i < 16; i++)
		data[i] = input(i);

	// Initialize history variables
	this->revertToStart();
}

//! @brief Constructor.
XC::DrainClough2Material::DrainClough2Material(int tag)
  :XC::DrainMaterial(tag, MAT_TAG_DrainClough2, 19, 16)
  {}

XC::DrainClough2Material::DrainClough2Material(void):
 XC::DrainMaterial(0, MAT_TAG_DrainClough2, 19, 16)
{
	// Does nothing
}

int
XC::DrainClough2Material::revertToStart(void)
{
	double dyp = data[1]/data[0];	// fyp/E
	double dyn = data[2]/data[0];	// fyn/E

	hstv[0]  = data[0];		// E
	hstv[1]  = data[0];		// E
	hstv[2]  = dyp;
	hstv[3]  = dyn; 
	hstv[4]  = 0.0;
	hstv[5]  = dyp;
	hstv[6]  = dyn;
	hstv[7]  = data[1];		// fyp
	hstv[8]  = data[2];		// fyn
	hstv[9]  = data[13];	// capDispP
	hstv[10] = data[14];	// capDispN
	hstv[11] = 0.0;
	hstv[12] = 0.0;
	hstv[13] = 0.0;
	hstv[14] = data[0];		// E
	hstv[15] = 0.0;
	hstv[16] = 0.0;
	hstv[17] = 0.0;
	hstv[18] = 0.0;

	// Set trial history variables to committed values
	for (int i = 0; i < 19; i++)
		hstv[i+19] = hstv[i];

	return 0;
}

XC::UniaxialMaterial *XC::DrainClough2Material::getCopy(void) const
  { return new DrainClough2Material(*this); }
