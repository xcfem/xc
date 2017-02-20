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
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/fedeas/FedeasHyster2Material.cpp,v $
                                                                      
// Written: MHS
// Created: Jan 2001
//
// Description: This file contains the class definition for 
// FedeasHyster2Material. FedeasHyster2Material wraps the FEDEAS
// 1d material subroutine Hyster_2.

#include <cstdlib>
#include <material/uniaxial/fedeas/FedeasHyster2Material.h>
#include "utility/matrix/Vector.h"

XC::FedeasHyster2Material::FedeasHyster2Material(int tag,
		double mom1p, double rot1p, double mom2p, double rot2p,
		double mom3p, double rot3p, double mom1n, double rot1n,
		double mom2n, double rot2n, double mom3n, double rot3n,
		double pinchX, double pinchY, double damfc1, double damfc2):
// 6 history variables and 16 material parameters
 XC::FedeasMaterial(tag, MAT_TAG_FedeasHysteretic2, 6, 16)
{
	matParams[0]  = mom1p;
	matParams[1]  = rot1p;
	matParams[2]  = mom2p;
	matParams[3]  = rot2p;
	matParams[4]  = mom3p;
	matParams[5]  = rot3p;

	matParams[6]  = mom1n;
	matParams[7]  = rot1n;
	matParams[8]  = mom2n;
	matParams[9]  = rot2n;
	matParams[10] = mom3n;
	matParams[11] = rot3n;

	matParams[12] = pinchX;
	matParams[13] = pinchY;
	matParams[14] = damfc1;
	matParams[15] = damfc2;

    trial.Tangent() =  matParams[0]/matParams[1];
    converged.Tangent()= trial.getTangent();
  }

XC::FedeasHyster2Material::FedeasHyster2Material(int tag,
	double mom1p, double rot1p, double mom2p, double rot2p,
	double mom1n, double rot1n, double mom2n, double rot2n,
	double pinchX, double pinchY, double damfc1, double damfc2):
// 6 history variables and 16 material parameters
 XC::FedeasMaterial(tag, MAT_TAG_FedeasHysteretic2, 6, 16)
{
	matParams[0]  = mom1p;
	matParams[1]  = rot1p;
	matParams[2]  = 0.5*(mom1p+mom2p);
	matParams[3]  = 0.5*(rot1p+rot2p);
	matParams[4]  = mom2p;
	matParams[5]  = rot2p;

	matParams[6]  = mom1n;
	matParams[7]  = rot1n;
	matParams[8]  = 0.5*(mom1n+mom2n);
	matParams[9]  = 0.5*(rot1n+rot2n);
	matParams[10] = mom2n;
	matParams[11] = rot2n;

	matParams[12] = pinchX;
	matParams[13] = pinchY;
	matParams[14] = damfc1;
	matParams[15] = damfc2;

    trial.Tangent() =  matParams[0]/matParams[1];
    converged.Tangent()= trial.getTangent();
  }

XC::FedeasHyster2Material::FedeasHyster2Material(int tag, const Vector &d):
// 6 history variables and 16 material parameters
 XC::FedeasMaterial(tag, MAT_TAG_FedeasHysteretic2, 6, 16)
{
  if (d.Size() != numData) {
    std::cerr << "XC::FedeasHyster2Material::FedeasHyster2Material -- not enough input arguments\n";
    exit(-1);
  }

  for (int i = 0; i < numData; i++)
    matParams[i] = d(i);
}

XC::FedeasHyster2Material::FedeasHyster2Material(int tag)
  : XC::FedeasMaterial(tag, MAT_TAG_FedeasHysteretic2, 6, 16)
  {}

XC::UniaxialMaterial* XC::FedeasHyster2Material::getCopy(void) const
  { return new FedeasHyster2Material(*this); }

double XC::FedeasHyster2Material::getInitialTangent(void) const
  {
    //return mom1p/rot1p;
    return matParams[0]/matParams[1];
  }
