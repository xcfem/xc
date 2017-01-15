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
// $Date: 2005/03/25 00:34:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/J2ThreeDimensional.cpp,v $

// Written: Ed "C++" Love
// Do not ask Prashant about this code.  He has no clue. 
//
// J2ThreeDimensional isotropic hardening material class
// 
//  Elastic Model
//  sigma = K*trace(epsilion_elastic) + (2*G)*dev(epsilon_elastic)
//
//  Yield Function
//  phi(sigma,q) = || dev(sigma) ||  - sqrt(2/3)*q(xi) 
//
//  Saturation Isotropic Hardening with linear term
//  q(xi) = simga_0 + (sigma_infty - sigma_0)*exp(-delta*xi) + H*xi 
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
//	                eps_22 		      
//                    2 eps_01   
//            	      2 eps_12   
//		      2 eps_20    }   <--- note the 2
// 
//  set eta := 0 for rate independent case
//

#include <material/nD/j2_plasticity/J2ThreeDimensional.h>
#include <cstdio> 
#include <cstdlib> 
#include <cmath> 

#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "material/nD/TipoMaterialND.h"

//static vectors and matrices
XC::Vector XC::J2ThreeDimensional::strain_vec(6) ;
XC::Vector XC::J2ThreeDimensional::stress_vec(6) ;
XC::Matrix XC::J2ThreeDimensional::tangent_matrix(6,6) ;

XC::J2ThreeDimensional::J2ThreeDimensional(int tag)
  : XC::J2Plasticity(tag,ND_TAG_J2ThreeDimensional) 
  {}


//null constructor
 XC::J2ThreeDimensional::J2ThreeDimensional( ) : 
 XC::J2Plasticity( ) 
{  }


//full constructor
 XC::J2ThreeDimensional::J2ThreeDimensional(   int    tag, 
                 double K,
                 double G,
                 double yield0,
                 double yield_infty,
                 double d,
                 double H,
                 double viscosity ) : 
 XC::J2Plasticity( tag, ND_TAG_J2ThreeDimensional, 
             K, G, yield0, yield_infty, d, H, viscosity )
{ 

}


//elastic constructor
 XC::J2ThreeDimensional::J2ThreeDimensional(   int    tag, 
                 double K, 
                 double G ) :
 XC::J2Plasticity( tag, ND_TAG_J2ThreeDimensional, K, G )
{ 

}



//make a clone of this material
XC::NDMaterial* XC::J2ThreeDimensional::getCopy(void) const 
  { return new J2ThreeDimensional(*this); }


//send back type of material
const std::string &XC::J2ThreeDimensional::getType( ) const 
  { return strTipoThreeDimensional; }


//send back order of strain in vector form
int XC::J2ThreeDimensional::getOrder( ) const 
  { return 6; } 


//get the strain and integrate plasticity equations
int XC::J2ThreeDimensional::setTrialStrain( const XC::Vector &strain_from_element) 
{
  strain.Zero( ) ;

  strain(0,0) =        strain_from_element(0) ;
  strain(1,1) =        strain_from_element(1) ;
  strain(2,2) =        strain_from_element(2) ;

  strain(0,1) = 0.50 * strain_from_element(3) ;
  strain(1,0) =        strain(0,1) ;

  strain(1,2) = 0.50 * strain_from_element(4) ;
  strain(2,1) =        strain(1,2) ;
  
  strain(2,0) = 0.50 * strain_from_element(5) ;
  strain(0,2) =        strain(2,0) ;

  this->plastic_integrator( ) ;

  return 0 ;
}


//unused trial strain functions
int XC::J2ThreeDimensional::setTrialStrain( const XC::Vector &v, const XC::Vector &r )
{ 
   return this->setTrialStrain( v ) ;
} 

int XC::J2ThreeDimensional::setTrialStrainIncr( const XC::Vector &v ) 
{
  static XC::Vector newStrain(6);
  newStrain(0) = strain(0,0) + v(0);
  newStrain(1) = strain(1,1) + v(1);
  newStrain(2) = strain(2,2) + v(2);
  newStrain(3) = 2.0*strain(0,1) + v(3);
  newStrain(4) = 2.0*strain(1,2) + v(4);
  newStrain(5) = 2.0*strain(2,0) + v(5);
  
  return this->setTrialStrain(newStrain);
}

int XC::J2ThreeDimensional::setTrialStrainIncr( const XC::Vector &v, const XC::Vector &r ) 
{
  return this->setTrialStrainIncr(v);
}



//send back the strain
const XC::Vector &XC::J2ThreeDimensional::getStrain(void) const
{
  strain_vec(0) =       strain(0,0) ;
  strain_vec(1) =       strain(1,1) ;
  strain_vec(2) =       strain(2,2) ;

  strain_vec(3) = 2.0 * strain(0,1) ;

  strain_vec(4) = 2.0 * strain(1,2) ;

  strain_vec(5) = 2.0 * strain(2,0) ;

  return strain_vec ;
} 


//send back the stress 
const XC::Vector &XC::J2ThreeDimensional::getStress(void) const
{
  stress_vec(0) = stress(0,0) ;
  stress_vec(1) = stress(1,1) ;
  stress_vec(2) = stress(2,2) ;

  stress_vec(3) = stress(0,1) ;

  stress_vec(4) = stress(1,2) ;
  
  stress_vec(5) = stress(2,0) ;

  return stress_vec ;
}

//send back the tangent 
const XC::Matrix& XC::J2ThreeDimensional::getTangent(void) const
{
  // matrix to XC::BJtensor mapping
  //  XC::Matrix      XC::Tensor
  // -------     -------
  //   0           0 0
  //   1           1 1
  //   2           2 2   
  //   3           0 1  ( or 1 0 )
  //   4           1 2  ( or 2 1 )
  //   5           2 0  ( or 0 2 ) 
    
  int ii, jj ;
  int i, j, k, l ;

  for( ii = 0; ii < 6; ii++ ) {
    for( jj = 0; jj < 6; jj++ ) {

      index_map( ii, i, j ) ;
      index_map( jj, k, l ) ;

      tangent_matrix(ii,jj) = tangent[i][j][k][l] ;

    } //end for j
  } //end for i


  return tangent_matrix ;
} 

//send back the tangent 
const XC::Matrix& XC::J2ThreeDimensional::getInitialTangent(void) const
{
  // matrix to XC::BJtensor mapping
  //  XC::Matrix      XC::Tensor
  // -------     -------
  //   0           0 0
  //   1           1 1
  //   2           2 2   
  //   3           0 1  ( or 1 0 )
  //   4           1 2  ( or 2 1 )
  //   5           2 0  ( or 0 2 ) 
    
  int ii, jj ;
  int i, j, k, l ;

  this->doInitialTangent();

  for( ii = 0; ii < 6; ii++ ) {
    for( jj = 0; jj < 6; jj++ ) {

      index_map( ii, i, j ) ;
      index_map( jj, k, l ) ;

      tangent_matrix(ii,jj) = initialTangent[i][j][k][l] ;

    } //end for j
  } //end for i

  return tangent_matrix ;
} 

//this is mike's problem
int XC::J2ThreeDimensional::setTrialStrain(const XC::Tensor &v) 
{
  return -1 ;
}

int XC::J2ThreeDimensional::setTrialStrain(const XC::Tensor &v, const XC::Tensor &r)     
{
  return -1 ;
}

int XC::J2ThreeDimensional::setTrialStrainIncr(const XC::Tensor &v) 
{
  return -1 ;
}

int XC::J2ThreeDimensional::setTrialStrainIncr(const XC::Tensor &v, const XC::Tensor &r) 
{
  return -1 ;
}

const XC::Tensor &XC::J2ThreeDimensional::getTangentTensor(void) const
  { return rank4; }

//jeremic@ucdavis.edu 22jan2001const XC::Tensor& J2ThreeDimensional::getStressTensor( ) 
//jeremic@ucdavis.edu 22jan2001{
//jeremic@ucdavis.edu 22jan2001  return rank2 ;
//jeremic@ucdavis.edu 22jan2001}
//jeremic@ucdavis.edu 22jan2001
//jeremic@ucdavis.edu 22jan2001const XC::Tensor& J2ThreeDimensional::getStrainTensor( ) 
//jeremic@ucdavis.edu 22jan2001{
//jeremic@ucdavis.edu 22jan2001  return rank2 ;
//jeremic@ucdavis.edu 22jan2001}







