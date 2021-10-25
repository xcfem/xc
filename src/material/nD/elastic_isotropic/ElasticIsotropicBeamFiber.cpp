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

// $Revision: 1.3 $
// $Date: 2002/12/05 22:49:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/elastic_isotropic/ElasticIsotropicBeamFiber.cpp,v $

// Written: MHS
// Created: Aug 2001
//
// Description: Elastic isotropic model where stress components 22, 33, and 23
// are condensed out.

#include <material/nD/elastic_isotropic/ElasticIsotropicBeamFiber.h>           

#include <utility/matrix/Matrix.h>
#include "material/nD/NDMaterialType.h"

XC::Vector XC::ElasticIsotropicBeamFiber::sigma(3);
XC::Matrix XC::ElasticIsotropicBeamFiber::D(3,3);

XC::ElasticIsotropicBeamFiber::ElasticIsotropicBeamFiber(int tag, double E, double nu, double rho)
  : ElasticIsotropicMaterial(tag, ND_TAG_ElasticIsotropicBeamFiber,3, E, nu, rho)
  {}

XC::ElasticIsotropicBeamFiber::ElasticIsotropicBeamFiber(int tag)
  : ElasticIsotropicMaterial(tag, ND_TAG_ElasticIsotropicBeamFiber,3, 0.0, 0.0)
  {}

XC::ElasticIsotropicBeamFiber::ElasticIsotropicBeamFiber()
  : ElasticIsotropicMaterial(0, ND_TAG_ElasticIsotropicBeamFiber,3, 0.0, 0.0)
  {}

int XC::ElasticIsotropicBeamFiber::setTrialStrainIncr(const XC::Vector &strain)
{
	return 0;
}

int
XC::ElasticIsotropicBeamFiber::setTrialStrainIncr(const XC::Vector &strain, const XC::Vector &rate)
{
	return 0;
}

//! @brief Return the material tangent stiffness.
const XC::Matrix &XC::ElasticIsotropicBeamFiber::getTangent(void) const
{
	double mu2 = E/(1.0+v);
	double lam = v*mu2/(1.0-2.0*v);
	double mu = 0.50*mu2;

	D(0,0) = mu*(mu2+3.0*lam)/(mu+lam);
	D(1,1) = mu;
	D(2,2) = mu;

	return D;
}

//! @brief Return the material initial stiffness.
const XC::Matrix &XC::ElasticIsotropicBeamFiber::getInitialTangent(void) const
{
	double mu2 = E/(1.0+v);
	double lam = v*mu2/(1.0-2.0*v);
	double mu = 0.50*mu2;

	D(0,0) = mu*(mu2+3.0*lam)/(mu+lam);
	D(1,1) = mu;
	D(2,2) = mu;

	return D;
}

const XC::Vector &XC::ElasticIsotropicBeamFiber::getStress(void) const
  {
    const double mu2 = E/(1.0+v);
    const double lam = v*mu2/(1.0-2.0*v);
    const double mu = 0.50*mu2;

    const Vector strain= getStrain();
    sigma(0) = mu*(mu2+3.0*lam)/(mu+lam)*strain(0);
    sigma(1) = mu*strain(1);
    sigma(2) = mu*strain(2);
    return sigma;
  }

int XC::ElasticIsotropicBeamFiber::commitState(void)
{
	return 0;
}

int XC::ElasticIsotropicBeamFiber::revertToLastCommit(void)
  { return 0; }

int XC::ElasticIsotropicBeamFiber::revertToStart(void)
  { return ElasticIsotropicMaterial::revertToStart(); }

XC::NDMaterial *XC::ElasticIsotropicBeamFiber::getCopy(void) const
  { return new ElasticIsotropicBeamFiber(*this); }

const std::string &XC::ElasticIsotropicBeamFiber::getType(void) const
  { return strTypeBeamFiber; }

int XC::ElasticIsotropicBeamFiber::getOrder(void) const
  { return 3; }

