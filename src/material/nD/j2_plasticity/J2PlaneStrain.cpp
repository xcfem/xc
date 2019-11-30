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
** ****************************************************************** */
                                                                        
// $Revision: 1.6 $
// $Date: 2004/02/24 22:53:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/J2PlaneStrain.cpp,v $

// Written: Ed "C++" Love
//
// J2PlaneStrain isotropic hardening material class
// 
//  Elastic Model
//  sigma = K*trace(epsilion_elastic) + (2*G)*dev(epsilon_elastic)
//
//  Yield Function
//  phi(sigma,q) = || dev(sigma) ||  - sqrt(2/3)*q(xi) 
//
//  Saturation Isotropic Hardening with linear term
//  q(xi) = simga_infty + (sigma_0 - sigma_infty)*exp(-delta*xi) + H*xi 
//
//  Flow Rules
//  \dot{epsilon_p} =  gamma * d_phi/d_sigma
//  \dot{xi}        = -gamma * d_phi/d_q 
//
//  Linear Viscosity 
//  gamma = phi / eta  ( if phi > 0 ) 
//
//  Backward Euler Integration Routine 
//  Yield condition enforced at time n+1 
//
//  Send strains in following format :
// 
//     strain_vec = {   eps_00
//                      eps_11
//                    2 eps_01   }   <--- note the 2
// 
//  set eta := 0 for rate independent case
//

#include <material/nD/j2_plasticity/J2PlaneStrain.h>

#include  <utility/actor/objectBroker/FEM_ObjectBroker.h>

#include <cstdio> 
#include <cstdlib> 
#include <cmath> 
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "material/nD/NDMaterialType.h"

//static vectors and matrices
XC::Vector XC::J2PlaneStrain::strain_vec(3) ;
XC::Vector XC::J2PlaneStrain::stress_vec(3) ;
XC::Matrix XC::J2PlaneStrain::tangent_matrix(3,3) ;


//null constructor
XC::J2PlaneStrain::J2PlaneStrain(void)
  :  XC::J2Plasticity( ) 
  {}

//! @brief Constructor.
XC::J2PlaneStrain::J2PlaneStrain(int tag)
  :  XC::J2Plasticity(tag, ND_TAG_J2PlaneStrain) 
  {}

//full constructor
 XC::J2PlaneStrain::J2PlaneStrain(   int    tag, 
                 double K,
                 double G,
                 double yield0,
                 double yield_infty,
                 double d,
                 double H,
                 double viscosity ) : 
 XC::J2Plasticity( tag, ND_TAG_J2PlaneStrain, 
             K, G, yield0, yield_infty, d, H, viscosity )
{ 

}


//elastic constructor
 XC::J2PlaneStrain::J2PlaneStrain(   int    tag, 
                 double K, 
                 double G ) :
 XC::J2Plasticity( tag, ND_TAG_J2PlaneStrain, K, G )
{ 

}



//make a clone of this material
XC::NDMaterial *XC::J2PlaneStrain::getCopy(void) const 
  { return new J2PlaneStrain(*this); }



//send back type of material
const std::string &XC::J2PlaneStrain::getType(void) const 
  { return strTypePlaneStrain; }


//send back order of strain in vector form
int XC::J2PlaneStrain::getOrder( ) const 
  { return 3; } 


//get the strain and integrate plasticity equations
int XC::J2PlaneStrain::setTrialStrain( const XC::Vector &strain_from_element) 
{
  strain.Zero( ) ;

  strain(0,0) =        strain_from_element(0) ;
  strain(1,1) =        strain_from_element(1) ;
  strain(0,1) = 0.50 * strain_from_element(2) ;
  strain(1,0) =        strain(0,1) ;

  this->plastic_integrator( ) ;

  return 0 ;
}


//unused trial strain functions
int XC::J2PlaneStrain::setTrialStrain( const XC::Vector &v, const XC::Vector &r )
{ 
   return this->setTrialStrain( v ) ;
} 

int XC::J2PlaneStrain::setTrialStrainIncr( const XC::Vector &v ) 
{
  static XC::Vector newStrain(3);
  newStrain(0) = strain(0,0) + v(0);
  newStrain(1) = strain(1,1) + v(1);
  newStrain(2) = 2.0 * strain(0,1) + v(2);

  return this->setTrialStrain(newStrain);  
}

int XC::J2PlaneStrain::setTrialStrainIncr( const XC::Vector &v, const XC::Vector &r ) 
  { return this->setTrialStrainIncr(v); }


//send back the strain
const XC::Vector &XC::J2PlaneStrain::getStrain(void) const
{
  strain_vec(0) =       strain(0,0) ;
  strain_vec(1) =       strain(1,1) ;
  strain_vec(2) = 2.0 * strain(0,1) ;

  return strain_vec ;
} 


//send back the stress 
const XC::Vector &XC::J2PlaneStrain::getStress(void) const
{
  stress_vec(0) = stress(0,0) ;
  stress_vec(1) = stress(1,1) ;
  stress_vec(2) = stress(0,1) ;

  return stress_vec ;
}

//send back the tangent 
const XC::Matrix &XC::J2PlaneStrain::getTangent(void) const
{
  // matrix to tensor mapping
  //  Matrix      Tensor
  // -------     -------
  //   0           0 0
  //   1           1 1
  //   2           0 1  ( or 1 0 ) 
  // 
       
  tangent_matrix(0,0) = tangent [0][0] [0][0] ;
  tangent_matrix(1,1) = tangent [1][1] [1][1] ;
  tangent_matrix(2,2) = tangent [0][1] [0][1] ;

  tangent_matrix(0,1) = tangent [0][0] [1][1] ;
  tangent_matrix(1,0) = tangent [1][1] [0][0] ;

  tangent_matrix(0,2) = tangent [0][0] [0][1] ;
  tangent_matrix(2,0) = tangent [0][1] [0][0] ;

  tangent_matrix(1,2) = tangent [1][1] [0][1] ;
  tangent_matrix(2,1) = tangent [0][1] [1][1] ;

  return tangent_matrix ;
} 


//send back the tangent 
const XC::Matrix &XC::J2PlaneStrain::getInitialTangent(void) const
{
  // matrix to tensor mapping
  //  Matrix      Tensor
  // -------     -------
  //   0           0 0
  //   1           1 1
  //   2           0 1  ( or 1 0 ) 
  // 

  this->doInitialTangent();

  tangent_matrix(0,0) = initialTangent [0][0] [0][0] ;
  tangent_matrix(1,1) = initialTangent [1][1] [1][1] ;
  tangent_matrix(2,2) = initialTangent [0][1] [0][1] ;

  tangent_matrix(0,1) = initialTangent [0][0] [1][1] ;
  tangent_matrix(1,0) = initialTangent [1][1] [0][0] ;

  tangent_matrix(0,2) = initialTangent [0][0] [0][1] ;
  tangent_matrix(2,0) = initialTangent [0][1] [0][0] ;

  tangent_matrix(1,2) = initialTangent [1][1] [0][1] ;
  tangent_matrix(2,1) = initialTangent [0][1] [1][1] ;

  return tangent_matrix ;
} 

int XC::J2PlaneStrain::commitState( ) 
{
  epsilon_p_n = epsilon_p_nplus1;
  xi_n        = xi_nplus1;

  return 0;
}

int XC::J2PlaneStrain::revertToLastCommit( )
  { return 0; }


int XC::J2PlaneStrain::revertToStart( ) 
  {
    this->zero();
    return 0;
  }








