//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/fedeas/FedeasHardeningMaterial.cpp,v $
                                                                      
// Written: MHS
// Created: Jan 2001
//
// Description: This file contains the class definition for 
// FedeasHardeningMaterial. FedeasHardeningMaterial wraps the FEDEAS
// 1d material subroutine Hard_1.

#include <cstdlib>
#include <material/uniaxial/fedeas/FedeasHardeningMaterial.h>
#include "utility/matrix/Vector.h"

XC::FedeasHardeningMaterial::FedeasHardeningMaterial(int tag,
					 double E, double sigmaY, double Hiso, double Hkin):
// 3 history variables and 4 material parameters
 XC::FedeasMaterial(tag, MAT_TAG_FedeasHardening, 3, 4)
  {
	// Fill in material parameters
	matParams[0] = E;
	matParams[1] = sigmaY;
	matParams[2] = Hiso;
	matParams[3] = Hkin;

    trial.Tangent()=  E;
    converged.Tangent()= trial.getTangent();
  }

XC::FedeasHardeningMaterial::FedeasHardeningMaterial(int tag, const Vector &d):
// 3 history variables and 4 material parameters
 XC::FedeasMaterial(tag, MAT_TAG_FedeasHardening, 3, 4)
{
  if (d.Size() != numData) {
    std::cerr << "XC::FedeasHardeningMaterial::FedeasHardeningMaterial -- not enough input arguments\n";
    exit(-1);	
  }

  for (int i = 0; i < numData; i++)
    matParams[i] = d(i);

    trial.Tangent()= matParams[0];
    converged.Tangent()= trial.getTangent();
  }

XC::FedeasHardeningMaterial::FedeasHardeningMaterial(int tag)
  : XC::FedeasMaterial(tag, MAT_TAG_FedeasHardening, 3, 4) {}

XC::UniaxialMaterial* XC::FedeasHardeningMaterial::getCopy(void) const
  { return new FedeasHardeningMaterial(*this); }

double XC::FedeasHardeningMaterial::getInitialTangent(void) const
  { 
    //return E;
    return matParams[0];
  }
