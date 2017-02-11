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
// $Date: 2005/08/22 20:50:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/NewUniaxialMaterial.cpp,v $

// Written: MHS
// Created: Aug 2001
//
// Description: This file contains the class implementation for 
// NewUniaxialMaterial.

#include <material/uniaxial/NewUniaxialMaterial.h>
#include <utility/matrix/Vector.h>

#include <cmath>
#include <cfloat>

//! @brief Constructor.
XC::NewUniaxialMaterial::NewUniaxialMaterial(int tag)
  :UniaxialMaterial(tag,MAT_TAG_NewUniaxialMaterial),
   Tstrain(0.0), Tstress(0.0), Ttangent(0.0)
  {}

//! @brief Default constructor.
XC::NewUniaxialMaterial::NewUniaxialMaterial()
  :UniaxialMaterial(0,MAT_TAG_NewUniaxialMaterial),
   Tstrain(0.0), Tstress(0.0), Ttangent(0.0)
  {}

//! @brief Asigna la deformación al material.
int XC::NewUniaxialMaterial::setTrialStrain(double strain, double strainRate)
  {
    // set the trial strain
    Tstrain = strain;

    // determine trial stress and tangent
    Tstress = 0.0;
    Ttangent = 0.0;
    return 0;
  }

//! @brief Devuelve la tensión del material.
double XC::NewUniaxialMaterial::getStress(void) const
  { return Tstress; }

//! @brief Devuelve el módulo de rigidez tangente del material.
double XC::NewUniaxialMaterial::getTangent(void) const
  { return Ttangent; }

//! @brief Devuelve el módulo de rigidez tangente inicial del material.
double XC::NewUniaxialMaterial::getInitialTangent(void) const
  {
    // return the initial tangent
    return 0.0;
  }

//! @brief Devuelve la deformación del material.
double XC::NewUniaxialMaterial::getStrain(void) const
  { return Tstrain; }

//! @brief Consuma los valores de las variables de estado del material.
int XC::NewUniaxialMaterial::commitState(void)
  { return 0; }

//! @brief Devuelve el estado del material al último consolidado.
int XC::NewUniaxialMaterial::revertToLastCommit(void)
  { return 0; }

//! @brief Devuelve los valores de las variables de estado del material al
//! los del estado inicial.
int XC::NewUniaxialMaterial::revertToStart(void)
  { return 0; }

//! @brief Constructor virtual.
XC::UniaxialMaterial *XC::NewUniaxialMaterial::getCopy(void) const
  { return new XC::NewUniaxialMaterial(this->getTag()); }

int XC::NewUniaxialMaterial::sendSelf(CommParameters &cp)
  { return -1; }

int XC::NewUniaxialMaterial::recvSelf(const CommParameters &cp)
  { return -1; }

void XC::NewUniaxialMaterial::Print(std::ostream &s, int flag)
  { return; }


