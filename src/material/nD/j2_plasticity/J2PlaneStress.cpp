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
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/J2PlaneStress.cpp,v $

// Written: Ed "C++" Love
//
// J2PlaneStress isotropic hardening material class
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

#include <material/nD/j2_plasticity/J2PlaneStress.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <cstdio> 
#include <cstdlib> 
#include <cmath> 
#include "material/nD/NDMaterialType.h"

XC::Vector XC::J2PlaneStress::strain_vec(3);
XC::Vector XC::J2PlaneStress::stress_vec(3);
XC::Matrix XC::J2PlaneStress::tangent_matrix(3,3);

XC::J2PlaneStress:: J2PlaneStress(int tag)
  : XC::J2Plasticity(tag, ND_TAG_J2PlaneStress) 
  {}

//null constructor
 XC::J2PlaneStress:: J2PlaneStress( ) : 
 XC::J2Plasticity( ) 
{  }


//full constructor
 XC::J2PlaneStress::J2PlaneStress(   int    tag, 
                 double K,
                 double G,
                 double yield0,
                 double yield_infty,
                 double d,
                 double H,
                 double viscosity ) : 
 XC::J2Plasticity(tag, ND_TAG_J2PlaneStress, 
             K, G, yield0, yield_infty, d, H, viscosity )
{ 

}


//elastic constructor
 XC::J2PlaneStress::J2PlaneStress(   int    tag, 
                 double K, 
                 double G ) :
 XC::J2Plasticity(tag, ND_TAG_J2PlaneStress, K, G )
{ 

}


//make a clone of this material
XC::NDMaterial *XC::J2PlaneStress::getCopy(void) const 
  { return new J2PlaneStress(*this); }


//send back type of material
const std::string &XC::J2PlaneStress::getType(void) const 
  { return strTypePlaneStress;}


//send back order of strain in vector form
int XC::J2PlaneStress::getOrder( ) const 
  { return 3; } 

//get the strain and integrate plasticity equations
int XC::J2PlaneStress::setTrialStrain( const Vector &strain_from_element ) 
  {
    const double tolerance= (1.0e-8)*sigma_0;

    const int max_iterations= 25;
    int iteration_counter= 0;

    int i, j, k, l;

    double eps22= strain(2,2);
    strain.Zero( );

    strain(0,0)= strain_from_element(0);
    strain(1,1)= strain_from_element(1);
    strain(0,1)= 0.50 * strain_from_element(2);
    strain(1,0)= strain(0,1);

    strain(2,2)= eps22; 

    //enforce the plane stress condition sigma_22= 0 
    //solve for epsilon_22 
    iteration_counter= 0;  
    do
      {

       this->plastic_integrator( );

       strain(2,2) -= stress(2,2) / tangent[2][2][2][2];

       iteration_counter++;
       if(iteration_counter > max_iterations)
	 {
	   std::cerr << getClassName() << "::" << __FUNCTION__
		     << "; more than " << max_iterations;
	   std::cerr << " iterations in setTrialStrain.\n";
	   break;
         }// end if 

      }
    while( fabs(stress(2,2)) > tolerance );

    //modify tangent for plane stress 
    for(int ii= 0; ii < 3; ii++ )
      {
        for(int jj= 0; jj < 3; jj++ )
	  {
	    index_map( ii, i, j );
	    index_map( jj, k, l );

	    tangent[i][j][k][l] -=   tangent[i][j][2][2] 
				   * tangent[2][2][k][l] 
				   / tangent[2][2][2][2];

	    //minor symmetries 
	    tangent [j][i][k][l]= tangent[i][j][k][l];
	    tangent [i][j][l][k]= tangent[i][j][k][l];
	    tangent [j][i][l][k]= tangent[i][j][k][l];

          } // end for jj
        } // end for ii 

    return 0;
  }


//unused trial strain functions
int XC::J2PlaneStress::setTrialStrain( const XC::Vector &v, const XC::Vector &r )
  { return this->setTrialStrain( v ); } 

int XC::J2PlaneStress::setTrialStrainIncr( const XC::Vector &v ) 
  {
    static Vector newStrain(3);
    newStrain(0)= strain(0,0) + v(0);
    newStrain(1)= strain(1,1) + v(1);
    newStrain(2)= 2.0 * strain(0,1) + v(2);

    return this->setTrialStrain(newStrain);  
  }

int XC::J2PlaneStress::setTrialStrainIncr( const XC::Vector &v, const XC::Vector &r ) 
{
    return this->setTrialStrainIncr(v);
}


//send back the strain
const XC::Vector &XC::J2PlaneStress::getStrain(void) const
{
  strain_vec(0)= strain(0,0);
  strain_vec(1)= strain(1,1);
  strain_vec(2)= 2.0 * strain(0,1);

  return strain_vec;
} 


//send back the stress 
const XC::Vector &XC::J2PlaneStress::getStress(void) const
{
  stress_vec(0)= stress(0,0);
  stress_vec(1)= stress(1,1);
  stress_vec(2)= stress(0,1);

  return stress_vec;
}

//send back the tangent 
const XC::Matrix &XC::J2PlaneStress::getTangent(void) const
  {
  // matrix to tensor mapping
  //  Matrix      Tensor
  // -------     -------
  //   0           0 0
  //   1           1 1
  //   2           0 1  ( or 1 0 ) 
  // 
       
  tangent_matrix(0,0)= tangent [0][0] [0][0];
  tangent_matrix(1,1)= tangent [1][1] [1][1];
  tangent_matrix(2,2)= tangent [0][1] [0][1];

  tangent_matrix(0,1)= tangent [0][0] [1][1];
  tangent_matrix(1,0)= tangent [1][1] [0][0];

  tangent_matrix(0,2)= tangent [0][0] [0][1];
  tangent_matrix(2,0)= tangent [0][1] [0][0];

  tangent_matrix(1,2)= tangent [1][1] [0][1];
  tangent_matrix(2,1)= tangent [0][1] [1][1];

  return tangent_matrix;
} 


//send back the tangent 
const XC::Matrix &XC::J2PlaneStress::getInitialTangent(void) const
{
  // matrix to tensor mapping
  //  Matrix      Tensor
  // -------     -------
  //   0           0 0
  //   1           1 1
  //   2           0 1  ( or 1 0 ) 
  // 

  this->doInitialTangent();
       
  tangent_matrix(0,0)= initialTangent [0][0] [0][0];
  tangent_matrix(1,1)= initialTangent [1][1] [1][1];
  tangent_matrix(2,2)= initialTangent [0][1] [0][1];

  tangent_matrix(0,1)= initialTangent [0][0] [1][1];
  tangent_matrix(1,0)= initialTangent [1][1] [0][0];

  tangent_matrix(0,2)= initialTangent [0][0] [0][1];
  tangent_matrix(2,0)= initialTangent [0][1] [0][0];

  tangent_matrix(1,2)= initialTangent [1][1] [0][1];
  tangent_matrix(2,1)= initialTangent [0][1] [1][1];

  return tangent_matrix;
} 

int XC::J2PlaneStress::commitState( ) 
{
  epsilon_p_n= epsilon_p_nplus1;
  xi_n= xi_nplus1;

  commitEps22= strain(2,2);

  return 0;
}

int XC::J2PlaneStress::revertToLastCommit( )
 {
   strain(2,2)= commitEps22;
   return 0;
  }


int XC::J2PlaneStress::revertToStart( )
  {
    commitEps22= 0.0;
    this->zero( );
    return 0;
  }

//! @brief Send object members through the communicator argument.
int XC::J2PlaneStress::sendData(Communicator &comm)
  {
    int res= J2Plasticity::sendData(comm);
    res+= comm.sendDouble(commitEps22,getDbTagData(),CommMetaData(88));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::J2PlaneStress::recvData(const Communicator &comm)
  {
    int res= J2Plasticity::recvData(comm);
    res+= comm.receiveDouble(commitEps22,getDbTagData(),CommMetaData(88));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::J2PlaneStress::sendSelf(Communicator &comm)
   {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(89);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::J2PlaneStress::recvSelf(const Communicator &comm)
  {
    inicComm(89);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }


//matrix_index --->tensor indices i,j
// plane stress different because of condensation on tangent
// case 3 switched to 1-2 and case 4 to 3-3 
void XC::J2PlaneStress::index_map(int matrix_index, int &i, int &j )
{
  switch( matrix_index+1 ) { //add 1 for standard tensor indices

    case 1 :
      i= 1; 
      j= 1;
      break;
 
    case 2 :
      i= 2;
      j= 2; 
      break;

    case 3 :
      i= 1;
      j= 2;
      break;

    case 4 :
      i= 3;
      j= 3;
      break;

    case 5 :
      i= 2;
      j= 3;
      break;

    case 6 :
      i= 3;
      j= 1;
      break;


    default :
      i= 1;
      j= 1;
      break;

  } //end switch

i--; //subtract 1 for C-indexing
j--;

return; 
}


