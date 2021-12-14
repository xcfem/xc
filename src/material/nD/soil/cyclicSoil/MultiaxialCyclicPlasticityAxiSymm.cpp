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
** ****************************************************************** */
                                                                        
/*----+----+----+----+----+----+----+----+----+----+----+----+----+----+----*
 |                                                                          | 
 |              MultiaxialCyclicPlasticity  NDMaterial                      |
 +                                                                          +
 |--------------------------------------------------------------------------|
 |                                                                          |
 +             Authors: Gang Wang  AND  Professor Nicholas Sitar            +
 |                                                                          |
 |             Department of Civil and Environmental Engineering            |
 +             University of California, Berkeley, CA 94720, USA            +
 |                                                                          |
 |             Email: wang@ce.berkeley.edu (G.W.)                           |
 |                                                                          | 
 +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----*/

#include <material/nD/soil/cyclicSoil/MultiaxialCyclicPlasticityAxiSymm.h>
#include <material/nD/NDMaterialType.h>

using namespace XC;
//static vectors and matrices
XC::Vector XC::MultiaxialCyclicPlasticityAxiSymm::strain_vec(4) ;
XC::Vector XC::MultiaxialCyclicPlasticityAxiSymm::stress_vec(4) ;
XC::Matrix XC::MultiaxialCyclicPlasticityAxiSymm::tangent_matrix(4,4) ;

XC::MultiaxialCyclicPlasticityAxiSymm::MultiaxialCyclicPlasticityAxiSymm(int tag)
  : MultiaxialCyclicPlasticity(tag, ND_TAG_MultiaxialCyclicPlasticityAxiSymm) 
  {}

//null constructor
XC::MultiaxialCyclicPlasticityAxiSymm::MultiaxialCyclicPlasticityAxiSymm( )
  : MultiaxialCyclicPlasticity( ) 
  {}


//full constructor
XC::MultiaxialCyclicPlasticityAxiSymm::MultiaxialCyclicPlasticityAxiSymm(       int    tag, 
                             double rho,
                 double K,
                 double G,
                             double Su,
                              double Ho_kin,
                 double Parameter_h,
                 double Parameter_m,
                 double Parameter_beta,
                 double Kcoeff,
                 double viscosity ) : 
 XC::MultiaxialCyclicPlasticity( tag, ND_TAG_MultiaxialCyclicPlasticityAxiSymm, rho, K, G, 
      Su, Ho_kin, Parameter_h, Parameter_m, Parameter_beta, Kcoeff, viscosity)
{ 

}



//elastic constructor
XC::MultiaxialCyclicPlasticityAxiSymm::MultiaxialCyclicPlasticityAxiSymm(   int    tag, double rho,    // add density by Gang Wang
                 double K, 
                 double G ) :
 XC::MultiaxialCyclicPlasticity( tag, ND_TAG_MultiaxialCyclicPlasticityAxiSymm, rho, K, G )
{ 

}

//! @brief Virtual constructor.
XC::NDMaterial* XC::MultiaxialCyclicPlasticityAxiSymm::getCopy(void) const 
  { return new MultiaxialCyclicPlasticityAxiSymm(*this); }


//send back type of material
const std::string &XC::MultiaxialCyclicPlasticityAxiSymm::getType( ) const 
  { return strTypeAxiSymmetric ; }


//send back order of strain in vector form
int XC::MultiaxialCyclicPlasticityAxiSymm::getOrder( ) const 
  { return 4; } 


//get the strain and integrate plasticity equations
int XC::MultiaxialCyclicPlasticityAxiSymm::setTrialStrain( const XC::Vector &strain_from_element) 
{
  strain.Zero( ) ;

  strain(0,0) =        strain_from_element(0) ;
  strain(1,1) =        strain_from_element(1) ;
  strain(2,2) =        strain_from_element(2) ;

  strain(0,1) = 0.50 * strain_from_element(3) ;
  strain(1,0) =        strain(0,1) ;

//  this->plastic_integrator( ) ;

   //std::cerr<<"MCP:integrator"<<std::endl;

   if(this->MaterialStageID ==1) { 
           this->elastic_integrator( ) ;
  } else if(this->MaterialStageID ==2) {          
           this->plastic_integrator( ) ;
  }

  return 0 ;
}


//unused trial strain functions
int XC::MultiaxialCyclicPlasticityAxiSymm::setTrialStrain( const XC::Vector &v, const XC::Vector &r )
{ 
  //std::cerr<<"MCP::setTrialStrain v"<<v<<std::endl;
   return this->setTrialStrain( v ) ;
} 

int XC::MultiaxialCyclicPlasticityAxiSymm::setTrialStrainIncr( const XC::Vector &v ) 
{
    // implemented Dec. 5, 2003, Gang Wang
    static XC::Vector newStrain(4);
    newStrain(0) = strain(0,0) + v(0);
    newStrain(1) = strain(1,1) + v(1);
    newStrain(2) = strain(2,2) + v(2);
    newStrain(3) = 2.0*strain(0,1) + v(3);
    
    //std::cerr<<"MCP::setTrialStrainIncr "<<newStrain<<std::endl;
    return this->setTrialStrain(newStrain);
    // return -1 ;  
}

int XC::MultiaxialCyclicPlasticityAxiSymm::setTrialStrainIncr( const XC::Vector &v, const XC::Vector &r ) 
{
    // implemented Dec. 5, 2003, Gang Wang
    //return this->setTrialStrainIncr(v);

   // implemented April. 4, 2004, Gang Wang
    static XC::Vector newStrain(4);
    newStrain(0) = strain(0,0) + v(0);
    newStrain(1) = strain(1,1) + v(1);
    newStrain(2) = strain(2,2) + v(2);
    newStrain(3) = 2.0*strain(0,1) + v(3);
            
    std::cerr<<"MCP::setTrialStrainIncr"<<strain; 

    //this->ResidStress(0,0)= r(0);
    //this->ResidStress(1,1)= r(1);
    //this->ResidStress(2,2)= r(2);
    //this->ResidStress(0,1)= r(3);
    //this->ResidStress(1,0)= r(3);

    return this->setTrialStrain(newStrain);

    // return -1 ;
}



//send back the strain
const XC::Vector& XC::MultiaxialCyclicPlasticityAxiSymm::getStrain(void) const
{
  strain_vec(0) =       strain(0,0) ;
  strain_vec(1) =       strain(1,1) ;
  strain_vec(2) =       strain(2,2) ;

  strain_vec(3) = 2.0 * strain(0,1) ;

  //std::cerr<<"MCP::getStrain() "<<strain_vec;
  return strain_vec ;
  
} 


//send back the stress 
const XC::Vector& XC::MultiaxialCyclicPlasticityAxiSymm::getStress(void) const
{
  stress_vec(0) = stress(0,0) ;
  stress_vec(1) = stress(1,1) ;
  stress_vec(2) = stress(2,2) ;

  stress_vec(3) = stress(0,1) ;

  //std::cerr<<"MCP::getStress()= "<<stress_vec;
  return stress_vec ;
}

//send back the tangent 
const XC::Matrix& XC::MultiaxialCyclicPlasticityAxiSymm::getTangent(void) const
{
  // matrix to XC::BJtensor mapping
  //  XC::Matrix      XC::Tensor
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

  //std::cerr<<"MCP::return tangent"<<std::endl;
  //std::cerr<<"MCP:"<<tangent_matrix<<std::endl;

  return tangent_matrix ;
} 

//! @brief Return the material initial stiffness.
const XC::Matrix &XC::MultiaxialCyclicPlasticityAxiSymm::getInitialTangent(void) const
  {
  // matrix to XC::BJtensor mapping
  //  XC::Matrix      XC::Tensor
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

//this is mike's problem
int XC::MultiaxialCyclicPlasticityAxiSymm::setTrialStrain(const XC::Tensor &v) 
{
  return -1 ;
}

int XC::MultiaxialCyclicPlasticityAxiSymm::setTrialStrain(const XC::Tensor &v, const XC::Tensor &r)     
{
  return -1 ;
}

int XC::MultiaxialCyclicPlasticityAxiSymm::setTrialStrainIncr(const XC::Tensor &v) 
{
  return -1 ;
}

int XC::MultiaxialCyclicPlasticityAxiSymm::setTrialStrainIncr(const XC::Tensor &v, const XC::Tensor &r) 
{
  return -1 ;
}

const XC::Tensor& XC::MultiaxialCyclicPlasticityAxiSymm::getTangentTensor( ) 
{
  return rank4 ;
}


//swap history variables
/* 
int XC::MultiaxialCyclicPlasticityAxiSymm::commitState( )  
{

  e_p_n        = e_p_nplus1 ;       // dev plastic part
  theta_p_n   = theta_p_nplus1 ;  // add vol plastic part
  xi_n        = xi_nplus1 ;

  return 0;

}


//revert to last saved state
int XC::MultiaxialCyclicPlasticityAxiSymm::revertToLastCommit( )
{ 
  return 0 ;
} 

//revert to start
int XC::MultiaxialCyclicPlasticityAxiSymm::revertToStart( ) 

{  
  int retval= MultiaxialCyclicPlasticity::revertToStart();
  this->zero( ) ;
  return retval;
}

 


int
XC::MultiaxialCyclicPlasticityAxiSymm::sendSelf(Communicator &comm)
{
  return -1;
}

int
XC::MultiaxialCyclicPlasticityAxiSymm::recvSelf(const Communicator &comm)
{
  return -1;
}


*/
