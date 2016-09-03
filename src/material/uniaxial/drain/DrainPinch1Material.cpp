//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/drain/DrainPinch1Material.cpp,v $

// Written: MHS
// Created: June 2001
//
// Description: This file contains the class definition for 
// DrainPinch1Material.

#include <material/uniaxial/drain/DrainPinch1Material.h>
#include <utility/matrix/Vector.h>

XC::DrainPinch1Material::DrainPinch1Material(int tag,
	double E, double fyp, double fyn, double alpha,
	double ecaps, double ecapk, double ecapa, double ecapd,
	double cs, double ck, double ca, double cd,
	double capSlope, double capDispP, double capDispN,
	double fpp, double fpn, double pinch, double res,
	double b):
// 15 history variables and 19 material parameters
 XC::DrainMaterial(tag, MAT_TAG_DrainPinch1, 15, 19, b)
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
	data[15] = fpp;
	data[16] = fpn;
	data[17] = pinch;
	data[18] = res;

	// Initialize history variables
	this->revertToStart();
}

XC::DrainPinch1Material::DrainPinch1Material(int tag, const XC::Vector &input, double b):
// 15 history variables and 19 material parameters
 XC::DrainMaterial(tag, MAT_TAG_DrainPinch1, 15, 19, b)
{
	for (int i = 0; i < 19; i++)
		data[i] = input(i);

	// Initialize history variables
	this->revertToStart();
}

//! @brief Constructor.
XC::DrainPinch1Material::DrainPinch1Material(int tag)
  :XC::DrainMaterial(tag, MAT_TAG_DrainPinch1, 15, 19)
  {}

XC::DrainPinch1Material::DrainPinch1Material(void):
 XC::DrainMaterial(0, MAT_TAG_DrainPinch1, 15, 19)
{
	// Does nothing
}

int
XC::DrainPinch1Material::revertToStart(void)
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

	// Set trial history variables to committed values
	for (int i = 0; i < 15; i++)
		hstv[i+15] = hstv[i];

	return 0;
}

XC::UniaxialMaterial* XC::DrainPinch1Material::getCopy(void) const
  { return new DrainPinch1Material(*this); }
