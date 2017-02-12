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
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/fedeas/FedeasSteel2Material.cpp,v $
                                                                      
// Written: MHS
// Created: Jan 2001
//
// Description: This file contains the class definition for 
// FedeasSteel2Material. FedeasSteel2Material wraps the FEDEAS
// 1d material subroutine Steel_2.

#include <cstdlib>
#include <material/uniaxial/fedeas/FedeasSteel2Material.h>
#include "utility/matrix/Vector.h"

XC::FedeasSteel2Material::FedeasSteel2Material(int tag,
					 double fy, double E0, double b,
					 double R0, double cR1, double cR2,
					 double a1, double a2, double a3, double a4):
// 8 history variables and 10 material parameters
 XC::FedeasMaterial(tag, MAT_TAG_FedeasSteel2, 8, 10)
{
	data[0]  = fy;
	data[1]  = E0;
	data[2]  = b;
	data[3]  = R0;
	data[4]  = cR1;
	data[5]  = cR2;
	data[6]  = a1;
	data[7]  = a2;
	data[8]  = a3;
	data[9]  = a4;

	trial.Tangent()= E0;
	converged.Tangent()= E0;
}

XC::FedeasSteel2Material::FedeasSteel2Material(int tag,
					 double fy, double E0, double b,
					 double R0, double cR1, double cR2):
// 8 history variables and 10 material parameters
 XC::FedeasMaterial(tag, MAT_TAG_FedeasSteel2, 8, 10)
{
	data[0]  = fy;
	data[1]  = E0;
	data[2]  = b;
	data[3]  = R0;
	data[4]  = cR1;
	data[5]  = cR2;

	// Default values for no isotropic hardening
	data[6]  = 0.0;
	data[7]  = 1.0;
	data[8]  = 0.0;
	data[9]  = 1.0;

	trial.Tangent()= E0;
	converged.Tangent()= E0;
}

XC::FedeasSteel2Material::FedeasSteel2Material(int tag,
					 double fy, double E0, double b):
// 8 history variables and 10 material parameters
 XC::FedeasMaterial(tag, MAT_TAG_FedeasSteel2, 8, 10)
{
	data[0]  = fy;
	data[1]  = E0;
	data[2]  = b;

	// Default values for elastic to hardening transitions
	data[3]  = 15.0;
	data[4]  = 0.925;
	data[5]  = 0.15;

	// Default values for no isotropic hardening
	data[6]  = 0.0;
	data[7]  = 1.0;
	data[8]  = 0.0;
	data[9]  = 1.0;

	trial.Tangent()= E0;
	converged.Tangent()= E0;
}

XC::FedeasSteel2Material::FedeasSteel2Material(int tag, const XC::Vector &d):
// 8 history variables and 10 material parameters
 XC::FedeasMaterial(tag, MAT_TAG_FedeasSteel2, 8, 10)
  {
    if(d.Size() != numData)
      {
        std::cerr << "FedeasSteel2Material::FedeasSteel2Material -- not enough input arguments\n";
        exit(-1);
      }

  for(int i = 0; i < numData; i++)
    data[i] = d(i);

  trial.Tangent()= data[1];
  converged.Tangent()= data[1];
}

XC::FedeasSteel2Material::FedeasSteel2Material(int tag)
 : XC::FedeasMaterial(tag, MAT_TAG_FedeasSteel2, 8, 10)
  {}

//! @brief Virtual constructor.
XC::UniaxialMaterial* XC::FedeasSteel2Material::getCopy(void) const
  { return  new FedeasSteel2Material(*this); }

double XC::FedeasSteel2Material::getInitialTangent(void) const
  {
    //return E;
    return data[1];
  }
