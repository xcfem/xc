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
// $Date: 2002/12/05 22:49:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/elastic_isotropic/ElasticIsotropicPlaneStrain2D.cpp,v $
                                                                        
                                                                        
#include <material/nD/elastic_isotropic/ElasticIsotropicPlaneStrain2D.h>                                                                        

#include <utility/matrix/Matrix.h>
#include "material/nD/NDMaterialType.h"

XC::Vector XC::ElasticIsotropicPlaneStrain2D::sigma(3);

//! @brief Constructor.
//!
//! To construct an ElasticIsotropicPlaneStrain2D whose unique integer tag
//! among NDMaterials in the domain is given by \p tag.  The material
//! model have Young's modulus \p E and Poisson's ratio \p v.
//!
//! @param tag: material identifier.
//! @param E: material Young's modulus.
//! @param nu: material Poisson's ratio.
XC::ElasticIsotropicPlaneStrain2D::ElasticIsotropicPlaneStrain2D(int tag, double E, double nu, double rho)
  : ElasticIsotropic2D(tag, ND_TAG_ElasticIsotropicPlaneStrain2d, E, nu, rho)
  {}

//! @brief Constructor.
//!
//! @param tag: material identifier.
XC::ElasticIsotropicPlaneStrain2D::ElasticIsotropicPlaneStrain2D(int tag)
  : ElasticIsotropic2D(tag, ND_TAG_ElasticIsotropicPlaneStrain2d, 0.0, 0.0, 0.0)
  {}

int XC::ElasticIsotropicPlaneStrain2D::setTrialStrainIncr(const XC::Vector &strain)
  {
    epsilon+= strain;
    return 0;
  }

int XC::ElasticIsotropicPlaneStrain2D::setTrialStrainIncr(const XC::Vector &strain, const XC::Vector &rate)
  {
    epsilon += strain;
    return 0;
  }

//! @brief Returns the material tangent stiffness matrix, \f$D\f$.
//!
//! \f[
//! \begin{displaymath}
//! D := \frac{E}{(1+\nu)(1-2\nu)} \left[
//!    \begin{array}{ccc}
//!          1-\nu &     \nu &      0
//!            \nu &   1-\nu &      0
//!              0 &       0 & 1-2\nu
//!    \end{array} 
//!  \right]
//! \end{displaymath}
//! \f]
const XC::Matrix &XC::ElasticIsotropicPlaneStrain2D::getTangent(void) const
  {
    const double mu2= E/(1.0+v);
    const double lam= v*mu2/(1.0-2.0*v);
    const double mu= 0.50*mu2;

    D(0,0) = D(1,1) = mu2+lam;
    D(0,1) = D(1,0) = lam;
    D(2,2) = mu;

    return D;
  }

const XC::Matrix &XC::ElasticIsotropicPlaneStrain2D::getInitialTangent(void) const
  {
    const double mu2= E/(1.0+v);
    const double lam= v*mu2/(1.0-2.0*v);
    const double mu= 0.50*mu2;

    D(0,0) = D(1,1) = mu2+lam;
    D(0,1) = D(1,0) = lam;
    D(2,2) = mu;

    return D;
  }

//! @brief Returns the material stress vector, \f$\mysigma\f$, for the current
//! trial strain.
//!
//! \f[
//! \begin{displaymath}
//! \mysigma := \left[
//!    \begin{array}{c}
//!        \sigma_{xx}
//!        \sigma_{yy}
//!        \tau_{xy}   
//!    \end{array} 
//!  \right]
//! \end{displaymath}
//! \f]
const XC::Vector &XC::ElasticIsotropicPlaneStrain2D::getStress(void) const
  {
    double mu2 = E/(1.0+v);
    const double lam = v*mu2/(1.0-2.0*v);
    const double mu = 0.50*mu2;

    const double eps0 = epsilon(0);
    const double eps1 = epsilon(1);

    mu2+= lam;

    //sigma = D*epsilon;
    sigma(0) = mu2*eps0 + lam*eps1;
    sigma(1) = lam*eps0 + mu2*eps1;
    sigma(2) = mu*epsilon(2);
    return sigma;
  }

int XC::ElasticIsotropicPlaneStrain2D::commitState(void)
  { return 0; }

int XC::ElasticIsotropicPlaneStrain2D::revertToLastCommit(void)
  { return 0; }

int XC::ElasticIsotropicPlaneStrain2D::revertToStart(void)
  {
    epsilon.Zero();
    return 0;
  }

//ª @brief Virtual constructor.
XC::NDMaterial *XC::ElasticIsotropicPlaneStrain2D::getCopy(void) const
  { return new ElasticIsotropicPlaneStrain2D(*this); }

const std::string &XC::ElasticIsotropicPlaneStrain2D::getType(void) const
  { return strTypePlaneStrain; }

