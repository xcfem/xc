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
// $Date: 2002/12/05 22:49:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/ElasticIsotropicPlaneStress2D.cpp,v $
                                                                        
                                                                        

#include <material/nD/elastic_isotropic/ElasticIsotropicPlaneStress2D.h>           

#include <utility/matrix/Matrix.h>
#include "material/nD/TipoMaterialND.h"

XC::Vector XC::ElasticIsotropicPlaneStress2D::sigma(3);

//! @brief Constructor.
//! 
//! To construct an ElasticIsotropicPlaneStress2D whose unique integer tag
//! among NDMaterials in the domain is given by \p tag.  The material
//! model have Young's modulus \p E and Poisson's ratio \p v.
//!
//! @param tag: material identifier.
//! @param E: material Young's modulus.
//! @param nu: material Poisson's ratio.
//! @param rho: material density.
XC::ElasticIsotropicPlaneStress2D::ElasticIsotropicPlaneStress2D(int tag, double E, double nu, double rho)
  : ElasticIsotropic2D(tag, ND_TAG_ElasticIsotropicPlaneStress2d, E, nu, rho)
  {}

XC::ElasticIsotropicPlaneStress2D::ElasticIsotropicPlaneStress2D(int tag)
  : ElasticIsotropic2D(tag, ND_TAG_ElasticIsotropicPlaneStress2d, 0.0, 0.0, 0.0)
  {}

XC::ElasticIsotropicPlaneStress2D::ElasticIsotropicPlaneStress2D():
  ElasticIsotropic2D(0, ND_TAG_ElasticIsotropicPlaneStress2d,0.0, 0.0, 0.0)
  {}

int XC::ElasticIsotropicPlaneStress2D::setTrialStrainIncr(const Vector &strain)
  {
    epsilon += strain;
    return 0;
  }

int XC::ElasticIsotropicPlaneStress2D::setTrialStrainIncr(const Vector &strain, const Vector &rate)
  {
    epsilon += strain;
    return 0;
  }

//! Returns the material tangent stiffness matrix, \f$\D\f$.
//!
//! \f[
//! \begin{displaymath}
//! D := \frac{E}{1-\nu^2} \left[
//!    \begin{array}{ccc}
//!          1 & \nu &     0
//!        \nu &   1 &     0
//!          0 &   0 & 1-\nu
//!    \end{array} 
//!  \right]
//! \end{displaymath}
//! \f]
const XC::Matrix &XC::ElasticIsotropicPlaneStress2D::getTangent(void) const
  {
    const double d00= E/(1.0-v*v);
    const double d01= v*d00;
    const double d22= 0.5*(d00-d01);

    D(0,0)= D(1,1) = d00;
    D(1,0)= D(0,1) = d01;
    D(2,2)= d22;

    return D;
  }

const XC::Matrix &XC::ElasticIsotropicPlaneStress2D::getInitialTangent(void) const
  {
    double d00 = E/(1.0-v*v);
    double d01 = v*d00;
    double d22 = 0.5*(d00-d01);

    D(0,0) = D(1,1) = d00;
    D(1,0) = D(0,1) = d01;
    D(2,2) = d22;

    return D;
  }

//! Returns the material stress vector, \f$\mysigma\f$, for the current
//! trial strain.
//!
//! \f[
//! \begin{displaymath}
//! \sigma := \left[
//!    \begin{array}{c}
//!        \sigma_{xx}
//!        \sigma_{yy}
//!        \tau_{xy}   
//!   \end{array} 
//!  \right]
//! \end{displaymath}
//! \f]
const XC::Vector &XC::ElasticIsotropicPlaneStress2D::getStress(void) const
  {
    const double d00= E/(1.0-v*v);
    const double d01= v*d00;
    const double d22= 0.5*(d00-d01);

    const double eps0= epsilon(0);
    const double eps1= epsilon(1);

    //sigma = D*epsilon;
    sigma(0)= d00*eps0 + d01*eps1;
    sigma(1)= d01*eps0 + d00*eps1;
    sigma(2)= d22*epsilon(2);
    return sigma;
  }

//! @brief zeroes initial generalized strain
void XC::ElasticIsotropicPlaneStress2D::zeroInitialGeneralizedStrain(void)
  {
    //Initial strains not yet implemented so nothing to do.
  }

//! @brief Accept the current material state
//! as being on the solution path. To return \f$0\f$ if
//! successful, a negative number if not.
int XC::ElasticIsotropicPlaneStress2D::commitState(void)
  { return 0; }

//! @brief Revert material to its last commited state.
int XC::ElasticIsotropicPlaneStress2D::revertToLastCommit(void)
  { return 0; }

//! @brief Revert material to its initial state.
int XC::ElasticIsotropicPlaneStress2D::revertToStart(void)
  {
    epsilon.Zero();
    return 0;
  }

//! @brief Virtual constructor.
XC::NDMaterial *XC::ElasticIsotropicPlaneStress2D::getCopy(void) const
  { return new ElasticIsotropicPlaneStress2D(*this); }

const std::string &XC::ElasticIsotropicPlaneStress2D::getType(void) const
  { return strTipoPlaneStress; }

