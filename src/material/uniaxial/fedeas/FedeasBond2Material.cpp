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
                                                                        
// $Revision: 1.6 $
// $Date: 2004/07/15 21:36:46 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/fedeas/FedeasBond2Material.cpp,v $
                                                                      
// Written: MHS
// Created: Jan 2001
//
// Description: This file contains the class definition for 
// FedeasBond2Material. FedeasBond2Material wraps the FEDEAS
// 1d material subroutine Bond_2.

#include <cstdlib>
#include <material/uniaxial/fedeas/FedeasBond2Material.h>
#include "utility/matrix/Vector.h"

XC::FedeasBond2Material::FedeasBond2Material(int tag,
	double u1p, double q1p, double u2p, double u3p, double q3p,
	double u1n, double q1n, double u2n, double u3n, double q3n,
	double s0, double bb, double alp, double aln)
  : FedeasBondMaterial(tag, MAT_TAG_FedeasBond2, 27, 15,u1p,q1p,u2p,u3p,q3p,u1n,q1n,u2n,u3n,q3n,s0,bb)// 27 history variables and 15 material parameters
  {

    matParams[12] = alp;
    matParams[13] = aln;

    const double E0p = q1p*u1p/(1.0+bb) + q1p*(u2p-u1p) + 0.5*(q1p+q3p)*(u3p-u2p);
    const double E0n = q1n*u1n/(1.0+bb) + q1n*(u2n-u1n) + 0.5*(q1n+q3n)*(u3n-u2n);

    matParams[14]= (E0p > E0n) ? E0p : E0n;

  }

XC::FedeasBond2Material::FedeasBond2Material(int tag, const Vector &d)
  : FedeasBondMaterial(tag, MAT_TAG_FedeasBond2, 27, 15,d) {} // 27 history variables and 15 material parameters

//@brief Constructor.
XC::FedeasBond2Material::FedeasBond2Material(int tag)
  : FedeasBondMaterial(tag, MAT_TAG_FedeasBond2, 27, 15) {}

XC::UniaxialMaterial *XC::FedeasBond2Material::getCopy(void) const
  { return new FedeasBond2Material(*this); }

