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
                                                                        
// $Revision: 1.6 $
// $Date: 2010-09-11 00:50:53 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/CompressionOnlyMaterial.cpp,v $

// Written: MHS
// Created: Aug 2001
//
// Description: This file contains the class definition for 
// CompressionOnlyMaterial.  CompressionOnlyMaterial wraps a UniaxialMaterial
// and removes the positive part of its stress-strain diagram.

#include "CompressionOnlyMaterial.h"
#include "utility/matrix/ID.h"

//! @brief Constructor.
XC::CompressionOnlyMaterial::CompressionOnlyMaterial(int tag, UniaxialMaterial &material)
  :HalfDiagramMaterial(tag,MAT_TAG_CompressionOnly, material)
  {}

XC::CompressionOnlyMaterial::CompressionOnlyMaterial(int tag)
  :HalfDiagramMaterial(tag,MAT_TAG_CompressionOnly)
  {}

//! @brief Return the material stress.
double XC::CompressionOnlyMaterial::getStress(void) const
  {
    const double f= this->getMaterial()->getStress();
    if(f > 0.0)
      return factor*f;
    else
      return f;
  }

//! @brief Return the tangent stiffness.
double XC::CompressionOnlyMaterial::getTangent(void) const
  {
    const UniaxialMaterial *tmp= this->getMaterial();
    const double E= tmp->getTangent();
    const double f= tmp->getStress();
    if(f > 0.0)
      return factor*E;
    else
      return E;
  }

double XC::CompressionOnlyMaterial::getDampTangent(void) const
  {
    const UniaxialMaterial *tmp= this->getMaterial();
    const double D= tmp->getDampTangent();
    const double f= tmp->getStress();
    if (f > 0.0)
      return factor*D;
    else
      return D;
  }


int XC::CompressionOnlyMaterial::commitState(void)
  {
    UniaxialMaterial *tmp= this->getMaterial();
    const double f= tmp->getStress();
    if(f <= 0.0)
      return tmp->commitState();
    else
      return 0;
  }

XC::UniaxialMaterial *XC::CompressionOnlyMaterial::getCopy(void) const
  { return new CompressionOnlyMaterial(*this); }

double XC::CompressionOnlyMaterial::getStressSensitivity(int gradIndex, bool conditional)
  {
    UniaxialMaterial *tmp= this->getMaterial();
    const double f = tmp->getStress();
    if (f > 0.0)
      return 0.0;
    else 
      return tmp->getStressSensitivity(gradIndex, conditional);
  }

double XC::CompressionOnlyMaterial::getDampTangentSensitivity(int gradIndex)
  {
    UniaxialMaterial *tmp= this->getMaterial();
    const double f = tmp->getStress();
    if(f > 0.0)
      return 0.0;
    else
      return tmp->getDampTangentSensitivity(gradIndex);
  }

int XC::CompressionOnlyMaterial::commitSensitivity(double strainGradient,
				       int gradIndex, int numGrads)
  {
    UniaxialMaterial *tmp= this->getMaterial();
    double f = tmp->getStress();
    if(f <= 0.0)
      return tmp->commitSensitivity(strainGradient, gradIndex, numGrads);
    else
      return 0;
  }
