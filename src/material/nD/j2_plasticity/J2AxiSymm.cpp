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
// $Date: 2003/02/14 23:01:25 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/J2AxiSymm.cpp,v $

// Written: Ed "C++" Love
//
// J2AxiSymmetric isotropic hardening material class
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
//                        eps_22                       
//                    2 eps_01   }   <--- note the 2
// 
//  set eta := 0 for rate independent case
//

#include <material/nD/j2_plasticity/J2AxiSymm.h>
#include "material/nD/NDMaterialType.h"

//static vectors and matrices
XC::Vector XC::J2AxiSymm::strain_vec(4) ;
XC::Vector XC::J2AxiSymm::stress_vec(4) ;
XC::Matrix XC::J2AxiSymm::tangent_matrix(4,4) ;


//null constructor
 XC::J2AxiSymm::J2AxiSymm( )
  :J2Plasticity( ) {}


//full constructor
 XC::J2AxiSymm::J2AxiSymm(   int    tag, 
                 double K,
                 double G,
                 double yield0,
                 double yield_infty,
                 double d,
                 double H,
                 double viscosity )
  : XC::J2Plasticity( tag, ND_TAG_J2AxiSymm, K, G, yield0, yield_infty, d, H, viscosity )
  {}


//elastic constructor
XC::J2AxiSymm::J2AxiSymm(int tag)
  : XC::J2Plasticity(tag, ND_TAG_J2AxiSymm)
  {}

//elastic constructor
XC::J2AxiSymm::J2AxiSymm(int tag, double K, double G )
  : XC::J2Plasticity( tag, ND_TAG_J2AxiSymm, K, G )
  {}

//make a clone of this material
XC::NDMaterial *XC::J2AxiSymm::getCopy(void) const 
  { return new J2AxiSymm(*this); }


//send back type of material
const std::string &XC::J2AxiSymm::getType(void) const 
  { return strTypeAxiSymmetric2D ; }


//send back order of strain in vector form
int XC::J2AxiSymm::getOrder(void) const 
  { return 4; } 


//get the strain and integrate plasticity equations
int XC::J2AxiSymm::setTrialStrain( const XC::Vector &strain_from_element) 
{
  strain.Zero( ) ;

  strain(0,0) =        strain_from_element(0) ;
  strain(1,1) =        strain_from_element(1) ;
  strain(2,2) =        strain_from_element(2) ;

  strain(0,1) = 0.50 * strain_from_element(3) ;
  strain(1,0) =        strain(0,1) ;

  this->plastic_integrator( ) ;

  return 0 ;
}


//unused trial strain functions
int XC::J2AxiSymm::setTrialStrain( const XC::Vector &v, const XC::Vector &r )
{ 
   return this->setTrialStrain( v ) ;
} 

int XC::J2AxiSymm::setTrialStrainIncr( const XC::Vector &v ) 
{
    return -1 ;
}

int XC::J2AxiSymm::setTrialStrainIncr( const XC::Vector &v, const XC::Vector &r ) 
{
    return -1 ;
}



//send back the strain
const XC::Vector& XC::J2AxiSymm::getStrain(void) const 
{
  strain_vec(0) =       strain(0,0) ;
  strain_vec(1) =       strain(1,1) ;
  strain_vec(2) =       strain(2,2) ;

  strain_vec(3) = 2.0 * strain(0,1) ;

  return strain_vec ;
} 


//send back the stress 
const XC::Vector& XC::J2AxiSymm::getStress(void) 
{
  stress_vec(0) = stress(0,0) ;
  stress_vec(1) = stress(1,1) ;
  stress_vec(2) = stress(2,2) ;

  stress_vec(3) = stress(0,1) ;

  return stress_vec ;
}

//send back the tangent 
const XC::Matrix& XC::J2AxiSymm::getTangent(void) const
{
  // matrix to tensor mapping
  //  Matrix      Tensor
  // -------     -------
  //   0           0 0
  //   1           1 1
  //   2           2 2   
  //   3           0 1  ( or 1 0 )
  
  int ii, jj ;
  int i, j, k, l ;

  for( ii = 0; ii < 4; ii++ ) {
    for( jj = 0; jj < 4; jj++ ) {

      index_map( ii, i, j ) ;
      index_map( jj, k, l ) ;

      tangent_matrix(ii,jj) = tangent[i][j][k][l] ;

    } //end for j
  } //end for i

  return tangent_matrix ;
} 

//send back the tangent 
const XC::Matrix& XC::J2AxiSymm::getInitialTangent(void) const
{
  // matrix to tensor mapping
  //  Matrix      Tensor
  // -------     -------
  //   0           0 0
  //   1           1 1
  //   2           2 2   
  //   3           0 1  ( or 1 0 )

  this->doInitialTangent();

  int ii, jj ;
  int i, j, k, l ;

  for( ii = 0; ii < 4; ii++ ) {
    for( jj = 0; jj < 4; jj++ ) {

      index_map( ii, i, j ) ;
      index_map( jj, k, l ) ;

      tangent_matrix(ii,jj) = initialTangent[i][j][k][l] ;

    } //end for j
  } //end for i

  return tangent_matrix ;
} 



//swap history variables
int XC::J2AxiSymm::commitState( )  
  {
    epsilon_p_n = epsilon_p_nplus1 ;
    xi_n        = xi_nplus1 ;
    return 0 ;
  }


//revert to last saved state
int XC::J2AxiSymm::revertToLastCommit( )
{ 
  return 0 ;
} 

//revert to start
int XC::J2AxiSymm::revertToStart( ) 
  {  
    this->zero( ) ;
    return 0 ;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::J2AxiSymm::sendData(Communicator &comm)
  {
    int res= J2Plasticity::sendData(comm);
    res+= comm.sendDoubles(commitEps00,commitEps11,commitEps01,commitEps22,getDbTagData(),CommMetaData(88));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::J2AxiSymm::recvData(const Communicator &comm)
  {
    int res= J2Plasticity::recvData(comm);
    res+= comm.receiveDoubles(commitEps00,commitEps11,commitEps01,commitEps22,getDbTagData(),CommMetaData(88));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::J2AxiSymm::sendSelf(Communicator &comm)
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

//! @brief Receives object through the channel being passed as parameter.
int XC::J2AxiSymm::recvSelf(const Communicator &comm)
  {
    inicComm(7);
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












