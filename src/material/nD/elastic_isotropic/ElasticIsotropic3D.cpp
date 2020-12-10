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
//===============================================================================
//# COPYRIGHT (C): Woody's license (by BJ):
//                 ``This    source  code is Copyrighted in
//                 U.S.,  for  an  indefinite  period,  and anybody
//                 caught  using it without our permission, will be
//                 mighty good friends of ourn, cause we don't give
//                 a  darn.  Hack it. Compile it. Debug it. Run it.
//                 Yodel  it.  Enjoy it. We wrote it, that's all we
//                 wanted to do.''
//
//# PROJECT:           Object Oriented Finite XC::Element Program
//# PURPOSE:           Elastic Isotropic XC::Material implementation:
//# CLASS:             ElasticIsotropic3D
//#
//# VERSION:           0.61803398874989 (golden section)
//# LANGUAGE:          C++
//# TARGET OS:         all...
//# DESIGN:            Zhaohui Yang, Boris Jeremic (jeremic@ucdavis.edu)
//# PROGRAMMER(S):     Zhaohui Yang, Boris Jeremic
//#
//#
//# DATE:              10Oct2000
//# UPDATE HISTORY:    22Nov2002 small fixes 
//#
//#
//===============================================================================
                                                                        
#include <material/nD/elastic_isotropic/ElasticIsotropic3D.h>

#include "utility/matrix/Matrix.h"
#include "material/nD/NDMaterialType.h"

XC::Matrix XC::ElasticIsotropic3D::D(6,6); // global for ElasticIsotropic3D only
XC::Vector XC::ElasticIsotropic3D::sigma(6); // global for ElasticIsotropic3D only

//! @brief Constructor.
XC::ElasticIsotropic3D::ElasticIsotropic3D(int tag)
  : ElasticIsotropicMaterial(tag, ND_TAG_ElasticIsotropic3D,6, 0.0, 0.0, 0.0)
  {}

//! @brief Constructor.
XC::ElasticIsotropic3D::ElasticIsotropic3D(int tag, double E, double nu, double rho)
  : ElasticIsotropicMaterial(tag, ND_TAG_ElasticIsotropic3D,6, E, nu, rho)
  {}


//! @brief Return the tangent stiffness matrix.
const XC::Matrix &XC::ElasticIsotropic3D::getTangent(void) const
  { return getInitialTangent(); }

//! @brief Return the tangent stiffness matrix.
const XC::Matrix &XC::ElasticIsotropic3D::getInitialTangent(void) const
  {
    double mu2 = E/(1.0+v);
    const double lam= v*mu2/(1.0-2.0*v);
    const double mu= 0.50*mu2;
    mu2 += lam;

    D(0,0) = D(1,1) = D(2,2) = mu2;
    D(0,1) = D(1,0) = lam;
    D(0,2) = D(2,0) = lam;
    D(1,2) = D(2,1) = lam;
    D(3,3) = mu;
    D(4,4) = mu;
    D(5,5) = mu;
    return D;
  }

const XC::Vector &XC::ElasticIsotropic3D::getStress(void) const
  {
    double mu2 = E/(1.0+v);
    const double lam = v*mu2/(1.0-2.0*v);
    const double mu  = 0.50*mu2;

    mu2 += lam;

    const Vector strain= getStrain();
    const double eps0= strain(0);
    const double eps1= strain(1);
    const double eps2= strain(2);
    
    D(0,0) = D(1,1) = D(2,2) = mu2;
    D(0,1) = D(1,0) = D(0,2) = D(2,0) = D(1,2) = D(2,1) = lam;
    D(3,3) = mu;
    D(4,4) = mu;
    D(5,5) = mu;

    sigma(0) = mu2*eps0 + lam*(eps1+eps2);
    sigma(1) = mu2*eps1 + lam*(eps2+eps0);
    sigma(2) = mu2*eps2 + lam*(eps0+eps1);

    sigma(3) = mu*strain(3);
    sigma(4) = mu*strain(4);
    sigma(5) = mu*strain(5);

    return sigma;
  }

//! @brief Commit the material state.
int XC::ElasticIsotropic3D::commitState(void)
  {
    return 0;
  }

//! @brief Return the material to its last committed state.
int XC::ElasticIsotropic3D::revertToLastCommit(void)
  {
    return 0;
  }

//! @brief Revert the material to its initial state.
int XC::ElasticIsotropic3D::revertToStart(void)
  {
    int retval= ElasticIsotropicMaterial::revertToStart();
    return retval;
  }

XC::NDMaterial *XC::ElasticIsotropic3D::getCopy(void) const
  { return new ElasticIsotropic3D(*this); }

const std::string &XC::ElasticIsotropic3D::getType(void) const
  { return strTypeThreeDimensional; }

int XC::ElasticIsotropic3D::getOrder(void) const
  { return 6; }

void XC::ElasticIsotropic3D::Print(std::ostream &s, int flag) const
  {
    s << strTypeElasticIsotropic3D << std::endl;
    s << "\ttag: " << this->getTag() << std::endl;
    s << "\tE: " << E << std::endl;
    s << "\tv: " << v << std::endl;
    s << "\trho: " << rho << std::endl;
  }




