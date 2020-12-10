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
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/elastic_isotropic/ElasticIsotropicPlaneStress2D.cpp,v $
                                                                        
                                                                        

#include <material/nD/elastic_isotropic/ElasticIsotropicPlaneStress2D.h>           

#include <utility/matrix/Matrix.h>
#include "material/nD/NDMaterialType.h"

XC::Vector XC::ElasticIsotropicPlaneStress2D::sigma(3); //Stress vector: [sigma_xx, sigma_yy, tau_xy]

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

//! @brief Returns the material tangent stiffness matrix, \f$\D\f$.
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
  { return getInitialTangent(); }

//! @brief Returns the material tangent stiffness matrix, \f$\D\f$.
const XC::Matrix &XC::ElasticIsotropicPlaneStress2D::getInitialTangent(void) const
  {
    const double d00 = E/(1.0-v*v);
    const double d01 = v*d00;
    const double d22 = 0.5*(d00-d01);

    D(0,0) = D(1,1) = d00;
    D(1,0) = D(0,1) = d01;
    D(2,2) = d22;

    return D;
  }

//! Returns the material stress vector, \f$\sigma\f$, for the current
//! trial strain: [epsilon_xx, epsilon_yy, epsilon_xy]
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

    const Vector strain= getStrain();
    const double eps0= strain(0); //epsilon_xx
    const double eps1= strain(1);

    //sigma = D*epsilon;
    sigma(0)= d00*eps0 + d01*eps1;
    sigma(1)= d01*eps0 + d00*eps1;
    sigma(2)= d22*strain(2);
    return sigma;
  }

//! @brief return the Von Mises equivalent stress.
//!
//! <a href="https://en.wikipedia.org/wiki/Von_Mises_yield_criterion"> Von Mises yield criterion.</a>
double XC::ElasticIsotropicPlaneStress2D::getVonMisesStress(void) const
  {
    double retval= 0.0;
    const Vector sg= getStress();
    const size_t sz= sg.Size();
    //NDmaterial stress order = 11, 22, 33, 12, 23, 31 
    if(sz==3) // 2D material
      {
	const double sg11= sg[0]; const double sg22= sg[1]; 
	const double sg12= sg[2]; 
	retval= sqrt(sg11*sg11+sg11*sg22+sg22*sg22+3.0*sg12*sg12);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << ", wrong stress vector size (" << sz
	        << ")." << std::endl;
    return retval;
  }

//! @brief Accept the current material state
//! as being on the solution path. To return \f$0\f$ if
//! successful, a negative number if not.
int XC::ElasticIsotropicPlaneStress2D::commitState(void)
  { return 0; }

//! @brief Revert material to its last committed state.
int XC::ElasticIsotropicPlaneStress2D::revertToLastCommit(void)
  { return 0; }

//! @brief Virtual constructor.
XC::NDMaterial *XC::ElasticIsotropicPlaneStress2D::getCopy(void) const
  { return new ElasticIsotropicPlaneStress2D(*this); }

const std::string &XC::ElasticIsotropicPlaneStress2D::getType(void) const
  { return strTypePlaneStress; }

