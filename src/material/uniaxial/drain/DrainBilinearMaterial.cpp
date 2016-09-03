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
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/drain/DrainBilinearMaterial.cpp,v $

// Written: MHS
// Created: June 2001
//
// Description: This file contains the class definition for 
// DrainBilinearMaterial.

#include <material/uniaxial/drain/DrainBilinearMaterial.h>
#include <utility/matrix/Vector.h>

XC::DrainBilinearMaterial::DrainBilinearMaterial(int tag,
	double E, double fyp, double fyn, double alpha,
	double ecaps, double ecapk, double ecapa, double ecapd,
	double cs, double ck, double ca, double cd,
	double capSlope, double capDispP, double capDispN, double res,
	double b):
// 17 history variables and 16 material parameters
 XC::DrainMaterial(tag, MAT_TAG_DrainBilinear, 17, 16, b)
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

XC::DrainBilinearMaterial::DrainBilinearMaterial(int tag, const XC::Vector &input, double b):
// 17 history variables and 16 material parameters
 XC::DrainMaterial(tag, MAT_TAG_DrainBilinear, 17, 16, b)
{
	for (int i = 0; i < 16; i++)
		data[i] = input(i);

	// Initialize history variables
	this->revertToStart();
}

//! @brief Constructor.
XC::DrainBilinearMaterial::DrainBilinearMaterial(int tag)
  :XC::DrainMaterial(tag, MAT_TAG_DrainBilinear, 17, 16)
  {}

XC::DrainBilinearMaterial::DrainBilinearMaterial(void)
: XC::DrainMaterial(0, MAT_TAG_DrainBilinear, 17, 16)
{}

int XC::DrainBilinearMaterial::revertToStart(void)
{
	hstv[0]  = 0.0;
	hstv[1]  = 0.0;
	hstv[2]  = data[1];		// fyp
	hstv[3]  = data[1];		// fyp
	hstv[4]  = data[2];		// fyn
	hstv[5]  = data[2];		// fyn
	hstv[6]  = data[0];		// E
	hstv[7]  = data[0];		// E
	hstv[8]  = data[13];	// capDispP
	hstv[9]  = data[13];	// capDispP
	hstv[10] = data[14];	// capDispN
	hstv[11] = data[14];	// capDispN
	hstv[12] = 0.0;
	hstv[13] = 0.0;
	hstv[14] = 0.0;
	hstv[15] = data[0];		// E
	hstv[16] = data[15];	// res

	// Set trial history variables to committed values
	for (int i = 0; i < 17; i++)
		hstv[i+17] = hstv[i];

	return 0;
}

XC::UniaxialMaterial *XC::DrainBilinearMaterial::getCopy(void) const
  { return new DrainBilinearMaterial(*this); }
