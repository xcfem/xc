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
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/elastic_isotropic/ElasticIsotropicAxiSymm.cpp,v $

#include "ElasticIsotropicAxiSymm.h"                                                                 

#include <utility/matrix/Matrix.h>
#include "material/nD/NDMaterialType.h"

XC::Vector XC::ElasticIsotropicAxiSymm::sigma(4);
XC::Matrix XC::ElasticIsotropicAxiSymm::D(4,4);

XC::ElasticIsotropicAxiSymm::ElasticIsotropicAxiSymm(int tag, double E, double nu, double rho) :
  ElasticIsotropicMaterial(tag, ND_TAG_ElasticIsotropicAxiSymm,4, E, nu, rho)
  {}

XC::ElasticIsotropicAxiSymm::ElasticIsotropicAxiSymm(int tag)
  : ElasticIsotropicMaterial(tag, ND_TAG_ElasticIsotropicAxiSymm,4, 0.0, 0.0)
  {}

XC::ElasticIsotropicAxiSymm::ElasticIsotropicAxiSymm(void)
  : ElasticIsotropicMaterial(0, ND_TAG_ElasticIsotropicAxiSymm,4, 0.0, 0.0)
  {}

int XC::ElasticIsotropicAxiSymm::setTrialStrainIncr(const XC::Vector &strain)
  {
    epsilon+=strain;
    return 0;
  }

int
XC::ElasticIsotropicAxiSymm::setTrialStrainIncr(const XC::Vector &strain, const XC::Vector &rate)
{
	epsilon+=strain;

	return 0;
}

const XC::Matrix &XC::ElasticIsotropicAxiSymm::getTangent(void) const
{
	double mu2 = E/(1.0+v);
	double lam = v*mu2/(1.0-2.0*v);
	double mu = 0.50*mu2;

	D(0,0) = D(1,1) = D(2,2) = mu2+lam;
	D(0,1) = D(1,0) = lam;
	D(0,2) = D(2,0) = lam;
	D(1,2) = D(2,1) = lam;
	D(3,3) = mu;

	return D;
}

const XC::Matrix &XC::ElasticIsotropicAxiSymm::getInitialTangent(void) const
{
	double mu2 = E/(1.0+v);
	double lam = v*mu2/(1.0-2.0*v);
	double mu = 0.50*mu2;

	D(0,0) = D(1,1) = D(2,2) = mu2+lam;
	D(0,1) = D(1,0) = lam;
	D(0,2) = D(2,0) = lam;
	D(1,2) = D(2,1) = lam;
	D(3,3) = mu;

	return D;
}

const XC::Vector &XC::ElasticIsotropicAxiSymm::getStress(void) const
{
  double mu2 = E/(1.0+v);
  double lam = v*mu2/(1.0-2.0*v);
  double mu = 0.50*mu2;

  double eps0 = epsilon(0);
  double eps1 = epsilon(1);
  double eps2 = epsilon(2);

  mu2 += lam;

  //sigma = D*epsilon;
  sigma(0) = mu2*eps0 + lam*(eps1+eps2);
  sigma(1) = mu2*eps1 + lam*(eps0+eps2);
  sigma(2) = mu2*eps2 + lam*(eps0+eps1);
  sigma(3) = mu*epsilon(3);
	
  return sigma;
}

int XC::ElasticIsotropicAxiSymm::commitState(void)
{
  return 0;
}

int
XC::ElasticIsotropicAxiSymm::revertToLastCommit(void)
{
  return 0;
}

int
XC::ElasticIsotropicAxiSymm::revertToStart(void)
{
  epsilon.Zero();
  return 0;
}

XC::NDMaterial *XC::ElasticIsotropicAxiSymm::getCopy(void) const
  { return new ElasticIsotropicAxiSymm(*this); }

const std::string &XC::ElasticIsotropicAxiSymm::getType(void) const
  { return strTypeAxiSymmetric; }

int XC::ElasticIsotropicAxiSymm::getOrder(void) const
  { return 4; }

