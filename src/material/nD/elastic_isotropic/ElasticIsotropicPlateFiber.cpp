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
// $Date: 2002/12/05 22:49:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/elastic_isotropic/ElasticIsotropicPlateFiber.cpp,v $
                                                                        
                                                                        

#include <material/nD/elastic_isotropic/ElasticIsotropicPlateFiber.h>           

#include "utility/matrix/Matrix.h"
#include "material/nD/NDMaterialType.h"

XC::Vector XC::ElasticIsotropicPlateFiber::sigma(ElasticIsotropicPlateFiber::order);
XC::Matrix XC::ElasticIsotropicPlateFiber::D(ElasticIsotropicPlateFiber::order, ElasticIsotropicPlateFiber::order);

//! @brief Default constructor.
XC::ElasticIsotropicPlateFiber::ElasticIsotropicPlateFiber(int tag)
  : ElasticIsotropicMaterial(tag, ND_TAG_ElasticIsotropicPlateFiber,5, 0.0, 0.0)
  {}

//! @brief Default constructor.
//! @param tag: object identifier.
//! @param E: elastic modulus.
//! @param nu: Poisson's coefficient.
//! @param rho: material density.
XC::ElasticIsotropicPlateFiber::ElasticIsotropicPlateFiber(int tag, double E, double nu, double rho)
  : ElasticIsotropicMaterial(tag, ND_TAG_ElasticIsotropicPlateFiber, ElasticIsotropicPlateFiber::order, E, nu, rho)
  {}

//! @brief Return the tangent stiffness matrix.
const XC::Matrix &XC::ElasticIsotropicPlateFiber::getTangent(void) const
  {
    double d00 = E/(1.0-v*v);
    double d01 = v*d00;
    double d22 = 0.5*(d00-d01);
    
    D(0,0) = D(1,1) = d00;
    D(0,1) = D(1,0) = d01;
    D(2,2) = d22;
    D(3,3) = d22;
    D(4,4) = d22;

    return D;
  }

//! @brief Return the initial tangent stiffness matrix.
const XC::Matrix &XC::ElasticIsotropicPlateFiber::getInitialTangent(void) const
  {
    double d00 = E/(1.0-v*v);
    double d01 = v*d00;
    double d22 = 0.5*(d00-d01);
    
    D(0,0) = D(1,1) = d00;
    D(0,1) = D(1,0) = d01;
    D(2,2) = d22;
    D(3,3) = d22;
    D(4,4) = d22;

    return D;
  }

//! @brief Return the material stresses.
const XC::Vector &XC::ElasticIsotropicPlateFiber::getStress(void) const
  {
    const double d00 = E/(1.0-v*v);
    const double d01 = v*d00;
    const double d22 = 0.5*(d00-d01);

    const Vector strain= getStrain();
    const double eps0= strain(0);
    const double eps1= strain(1);

    //sigma = D*epsilon;
    sigma(0)= d00*eps0 + d01*eps1;
    sigma(1)= d01*eps0 + d00*eps1;

    sigma(2)= d22*strain(2);
    sigma(3)= d22*strain(3);
    sigma(4)= d22*strain(4);
	
    return sigma;
  }

//! @brief return the Von Mises equivalent stress.
//!
//! <a href="https://en.wikipedia.org/wiki/Von_Mises_yield_criterion"> Von Mises yield criterion.</a>
double XC::ElasticIsotropicPlateFiber::getVonMisesStress(void) const
  {
    double retval= 0.0;
    const Vector sg= getStress();
    const size_t sz= sg.Size();
    //NDmaterial stress order= 11, 22, 33, 12, 23, 31 
    //PlateFiberMaterial stress order= 11, 22, 12, 23, 31, (33) 
    if(sz==5) // plate fiber material
      {
	const double sg11= sg[0]; //11
	const double sg22= sg[1]; //22
	const double sg12= sg[2]; //12
	const double sg23= sg[3]; //23
	const double sg31= sg[4]; //31
	
	retval= sqrt(0.5*(pow(sg11-sg22,2)+(sg22*sg22)+(sg11*sg11)+6*(sg12*sg12+sg23+sg23+sg31+sg31)));
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << ", wrong stress vector size (" << sz
	        << ")." << std::endl;
    return retval;
  }

//! @brief Commit the material state.
int XC::ElasticIsotropicPlateFiber::commitState(void)
  { return 0; }

//! @brief Revert the material state to its last commit.
int XC::ElasticIsotropicPlateFiber::revertToLastCommit(void)
  { return 0; }

//! @brief Virtual constructor.
XC::NDMaterial *XC::ElasticIsotropicPlateFiber::getCopy(void) const
  { return new ElasticIsotropicPlateFiber(*this); }

//! @brief Return a string that identifies the material type.
const std::string &XC::ElasticIsotropicPlateFiber::getType(void) const
  { return strTypePlateFiber; }

//! @brief ??
int XC::ElasticIsotropicPlateFiber::getOrder(void) const
  { return ElasticIsotropicPlateFiber::order; }
