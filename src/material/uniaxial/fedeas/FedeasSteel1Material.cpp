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
                                                                        
// $Revision: 1.5 $
// $Date: 2004/07/15 21:36:46 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/fedeas/FedeasSteel1Material.cpp,v $
                                                                      
// Written: MHS
// Created: Jan 2001
//
// Description: This file contains the class definition for 
// FedeasSteel1Material. FedeasSteel1Material wraps the FEDEAS
// 1d material subroutine Steel_1.

#include <cstdlib>
#include <material/uniaxial/fedeas/FedeasSteel1Material.h>
#include "utility/matrix/Vector.h"

XC::FedeasSteel1Material::FedeasSteel1Material(int tag,
					 double fy, double E0, double b,
					 double a1, double a2, double a3, double a4):
// 7 history variables and 7 material parameters
 XC::FedeasMaterial(tag, MAT_TAG_FedeasSteel1, 7, 7)
{
	matParams[0]  = fy;
	matParams[1]  = E0;
	matParams[2]  = b;
	matParams[3]  = a1;
	matParams[4]  = a2;
	matParams[5]  = a3;
	matParams[6]  = a4;

	trial.Tangent()= E0;
	converged.Tangent()= E0;
}

XC::FedeasSteel1Material::FedeasSteel1Material(int tag,
					 double fy, double E0, double b):
// 7 history variables and 7 material parameters
 XC::FedeasMaterial(tag, MAT_TAG_FedeasSteel1, 7, 7)
{
	matParams[0]  = fy;
	matParams[1]  = E0;
	matParams[2]  = b;

	// Default values for no isotropic hardening
	matParams[3]  = 0.0;
	matParams[4]  = 1.0;
	matParams[5]  = 0.0;
	matParams[6]  = 1.0;

	trial.Tangent()= E0;
	converged.Tangent()= E0;
  }

XC::FedeasSteel1Material::FedeasSteel1Material(int tag, const XC::Vector &d):
// 7 history variables and 7 material parameters
 XC::FedeasMaterial(tag, MAT_TAG_FedeasSteel1, 7, 7)
{
  if (d.Size() != numData) {
    std::cerr << "XC::FedeasSteel1Material::FedeasSteel1Material -- not enough input arguments\n";
    exit(-1);
  }

  for (int i = 0; i < numData; i++)
    matParams[i] = d(i);
  trial.Tangent()= matParams[1];
  converged.Tangent()= matParams[1];
}

XC::FedeasSteel1Material::FedeasSteel1Material(int tag)
  : XC::FedeasMaterial(tag, MAT_TAG_FedeasSteel1, 7, 7)
  {}

XC::UniaxialMaterial *XC::FedeasSteel1Material::getCopy(void) const
  { return new FedeasSteel1Material(*this); }

double XC::FedeasSteel1Material::getInitialTangent(void) const
  {
    //return E;
    return matParams[1];
  }
