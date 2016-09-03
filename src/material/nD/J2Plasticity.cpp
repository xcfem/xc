//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
                                                                        
// $Revision: 1.9 $
// $Date: 2005/03/25 00:32:11 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/J2Plasticity.cpp,v $

// Written: Ed "C++" Love
//
// J2 isotropic hardening material class
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
//  set eta := 0 for rate independent case
//

#include "J2Plasticity.h"
#include "ProblemaEF.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "domain/domain/Domain.h"
#include <material/nD/j2_plasticity/J2PlaneStress.h>
#include <material/nD/j2_plasticity/J2PlaneStrain.h>
#include <material/nD/j2_plasticity/J2AxiSymm.h>
#include <material/nD/j2_plasticity/J2PlateFiber.h>
#include <material/nD/j2_plasticity/J2ThreeDimensional.h> 
#include "material/nD/TipoMaterialND.h"

//this is mike's problem
XC::Tensor XC::J2Plasticity::rank2(2, def_dim_2, 0.0 ) ;
XC::Tensor XC::J2Plasticity::rank4(2, def_dim_2, 0.0 ) ;

//parameters
const double XC::J2Plasticity::one3   = 1.0 / 3.0 ;
const double XC::J2Plasticity::two3   = 2.0 / 3.0 ;
const double XC::J2Plasticity::four3  = 4.0 / 3.0 ;
const double XC::J2Plasticity::root23 = sqrt( 2.0 / 3.0 ) ;

double XC::J2Plasticity::initialTangent[3][3][3][3] ;   //material tangent
double XC::J2Plasticity::IIdev[3][3][3][3] ; //rank 4 deviatoric 
double XC::J2Plasticity::IbunI[3][3][3][3] ; //rank 4 I bun I 

//zero internal variables
void XC::J2Plasticity::zero( ) 
{
  xi_n = 0.0 ;
  xi_nplus1 = 0.0 ;
  
  epsilon_p_n.Zero( ) ;
  epsilon_p_nplus1.Zero( ) ;

  stress.Zero();
  strain.Zero();
}


//null constructor
XC::J2Plasticity::J2Plasticity(void)
  : XC::NDMaterial(), epsilon_p_n(3,3),epsilon_p_nplus1(3,3),stress(3,3),strain(3,3)
  { 
    bulk        = 0.0 ;
    shear       = 0.0 ;
    sigma_0     = 0.0 ;
    sigma_infty = 0.0 ;
    delta       = 0.0 ;
    Hard        = 0.0 ;
    eta         = 0.0 ;

    this->zero( ) ;     // or (*this).zero( ) 

    int i, j, k, l ;

    //zero rank4 IIdev and IbunI 
    for(i = 0; i < 3; i++ )
      {
        for(j = 0; j < 3; j++ )
          {
            for(k = 0; k < 3; k++ )
              {
                for(l = 0; l < 3; l++)
                  { 
                    IbunI[i][j][k][l] = 0.0 ;
                    IIdev[i][j][k][l] = 0.0 ;
                  } // end for l
              } // end for k
          } // end for j
      } // end for i


    //form rank4 IbunI 

    IbunI [0][0] [0][0] = 1.0 ;
    IbunI [0][0] [1][1] = 1.0 ;
    IbunI [0][0] [2][2] = 1.0 ;
    IbunI [1][1] [0][0] = 1.0 ;
    IbunI [1][1] [1][1] = 1.0 ;
    IbunI [1][1] [2][2] = 1.0 ;
    IbunI [2][2] [0][0] = 1.0 ;
    IbunI [2][2] [1][1] = 1.0 ;
    IbunI [2][2] [2][2] = 1.0 ;

    //form rank4 IIdev

    IIdev [0][0] [0][0] =  two3 ; // 0.666667 
    IIdev [0][0] [1][1] = -one3 ; //-0.333333 
    IIdev [0][0] [2][2] = -one3 ; //-0.333333 
    IIdev [0][1] [0][1] = 0.5 ;
    IIdev [0][1] [1][0] = 0.5 ;
    IIdev [0][2] [0][2] = 0.5 ;
    IIdev [0][2] [2][0] = 0.5 ;
    IIdev [1][0] [0][1] = 0.5 ;
    IIdev [1][0] [1][0] = 0.5 ;
    IIdev [1][1] [0][0] = -one3 ; //-0.333333 
    IIdev [1][1] [1][1] =  two3 ; // 0.666667 
    IIdev [1][1] [2][2] = -one3 ; //-0.333333 
    IIdev [1][2] [1][2] = 0.5 ;
    IIdev [1][2] [2][1] = 0.5 ;
    IIdev [2][0] [0][2] = 0.5 ;
    IIdev [2][0] [2][0] = 0.5 ;
    IIdev [2][1] [1][2] = 0.5 ;
    IIdev [2][1] [2][1] = 0.5 ;
    IIdev [2][2] [0][0] = -one3 ; //-0.333333 
    IIdev [2][2] [1][1] = -one3 ; //-0.333333 
    IIdev [2][2] [2][2] =  two3 ; // 0.666667 
  }


//! @brief Constructor
XC::J2Plasticity::J2Plasticity(int tag,int classtag)
  : XC::NDMaterial(tag, classtag), epsilon_p_n(3,3),epsilon_p_nplus1(3,3),stress(3,3),strain(3,3)
  { 
    bulk        = 0.0 ;
    shear       = 0.0 ;
    sigma_0     = 0.0 ;
    sigma_infty = 0.0 ;
    delta       = 0.0 ;
    Hard        = 0.0 ;
    eta         = 0.0 ;

    this->zero( ) ;     // or (*this).zero( ) 

    int i, j, k, l ;

    //zero rank4 IIdev and IbunI 
    for(i = 0; i < 3; i++ )
      {
        for(j = 0; j < 3; j++ )
          {
            for(k = 0; k < 3; k++ )
              {
                for(l = 0; l < 3; l++)
                  { 
                    IbunI[i][j][k][l] = 0.0 ;
                    IIdev[i][j][k][l] = 0.0 ;
                  } // end for l
              } // end for k
          } // end for j
      } // end for i


    //form rank4 IbunI 

    IbunI [0][0] [0][0] = 1.0 ;
    IbunI [0][0] [1][1] = 1.0 ;
    IbunI [0][0] [2][2] = 1.0 ;
    IbunI [1][1] [0][0] = 1.0 ;
    IbunI [1][1] [1][1] = 1.0 ;
    IbunI [1][1] [2][2] = 1.0 ;
    IbunI [2][2] [0][0] = 1.0 ;
    IbunI [2][2] [1][1] = 1.0 ;
    IbunI [2][2] [2][2] = 1.0 ;

    //form rank4 IIdev

    IIdev [0][0] [0][0] =  two3 ; // 0.666667 
    IIdev [0][0] [1][1] = -one3 ; //-0.333333 
    IIdev [0][0] [2][2] = -one3 ; //-0.333333 
    IIdev [0][1] [0][1] = 0.5 ;
    IIdev [0][1] [1][0] = 0.5 ;
    IIdev [0][2] [0][2] = 0.5 ;
    IIdev [0][2] [2][0] = 0.5 ;
    IIdev [1][0] [0][1] = 0.5 ;
    IIdev [1][0] [1][0] = 0.5 ;
    IIdev [1][1] [0][0] = -one3 ; //-0.333333 
    IIdev [1][1] [1][1] =  two3 ; // 0.666667 
    IIdev [1][1] [2][2] = -one3 ; //-0.333333 
    IIdev [1][2] [1][2] = 0.5 ;
    IIdev [1][2] [2][1] = 0.5 ;
    IIdev [2][0] [0][2] = 0.5 ;
    IIdev [2][0] [2][0] = 0.5 ;
    IIdev [2][1] [1][2] = 0.5 ;
    IIdev [2][1] [2][1] = 0.5 ;
    IIdev [2][2] [0][0] = -one3 ; //-0.333333 
    IIdev [2][2] [1][1] = -one3 ; //-0.333333 
    IIdev [2][2] [2][2] =  two3 ; // 0.666667 
  }

//full constructor
 XC::J2Plasticity::J2Plasticity(int    tag,
                             int classTag,
                             double K,
                             double G,
                             double yield0,
                             double yield_infty,
                             double d,
                             double H,
                             double viscosity) 
: 
  XC::NDMaterial(tag, classTag),
  epsilon_p_n(3,3),
  epsilon_p_nplus1(3,3),
  stress(3,3),
  strain(3,3)
{
  bulk        = K ;
  shear       = G ;
  sigma_0     = yield0 ;
  sigma_infty = yield_infty ;
  delta       = d ;
  Hard        = H ;
  eta         = viscosity ;

  this->zero( ) ;

  int i, j, k, l ;

  //zero rank4 IIdev and IbunI 
  for( i = 0; i < 3; i++ ) {
    for( j = 0; j < 3; j++ )  {
      for( k = 0; k < 3; k++ ) {
        for( l = 0; l < 3; l++)  { 

          IbunI[i][j][k][l] = 0.0 ;

          IIdev[i][j][k][l] = 0.0 ;

        } // end for l
      } // end for k
    } // end for j
  } // end for i


  //form rank4 IbunI 

  IbunI [0][0] [0][0] = 1.0 ;
  IbunI [0][0] [1][1] = 1.0 ;
  IbunI [0][0] [2][2] = 1.0 ;
  IbunI [1][1] [0][0] = 1.0 ;
  IbunI [1][1] [1][1] = 1.0 ;
  IbunI [1][1] [2][2] = 1.0 ;
  IbunI [2][2] [0][0] = 1.0 ;
  IbunI [2][2] [1][1] = 1.0 ;
  IbunI [2][2] [2][2] = 1.0 ;

  //form rank4 IIdev

  IIdev [0][0] [0][0] =  two3 ; // 0.666667 
  IIdev [0][0] [1][1] = -one3 ; //-0.333333 
  IIdev [0][0] [2][2] = -one3 ; //-0.333333 
  IIdev [0][1] [0][1] = 0.5 ;
  IIdev [0][1] [1][0] = 0.5 ;
  IIdev [0][2] [0][2] = 0.5 ;
  IIdev [0][2] [2][0] = 0.5 ;
  IIdev [1][0] [0][1] = 0.5 ;
  IIdev [1][0] [1][0] = 0.5 ;
  IIdev [1][1] [0][0] = -one3 ; //-0.333333 
  IIdev [1][1] [1][1] =  two3 ; // 0.666667 
  IIdev [1][1] [2][2] = -one3 ; //-0.333333 
  IIdev [1][2] [1][2] = 0.5 ;
  IIdev [1][2] [2][1] = 0.5 ;
  IIdev [2][0] [0][2] = 0.5 ;
  IIdev [2][0] [2][0] = 0.5 ;
  IIdev [2][1] [1][2] = 0.5 ;
  IIdev [2][1] [2][1] = 0.5 ;
  IIdev [2][2] [0][0] = -one3 ; //-0.333333 
  IIdev [2][2] [1][1] = -one3 ; //-0.333333 
  IIdev [2][2] [2][2] =  two3 ; // 0.666667 
}


//elastic constructor
 XC::J2Plasticity::J2Plasticity(   int    tag, 
                int  classTag,
                double K, 
                double G ) :
XC::NDMaterial(tag, classTag),
epsilon_p_n(3,3),
epsilon_p_nplus1(3,3),
stress(3,3),
strain(3,3)
{
  bulk        = K ;
  shear       = G ; 
  sigma_0     = 1.0e16*shear ;
  sigma_infty = sigma_0 ;
  delta       = 0.0 ;
  Hard        = 0.0 ;
  eta         = 0.0 ;

  this->zero( ) ;

  int i, j, k, l ;

  //zero rank4 IIdev and IbunI 
  for( i = 0; i < 3; i++ ) {
    for( j = 0; j < 3; j++ )  {
      for( k = 0; k < 3; k++ ) {
        for( l = 0; l < 3; l++)  { 

          IbunI[i][j][k][l] = 0.0 ;

          IIdev[i][j][k][l] = 0.0 ;

        } // end for l
      } // end for k
    } // end for j
  } // end for i


  //form rank4 IbunI 

  IbunI [0][0] [0][0] = 1.0 ;
  IbunI [0][0] [1][1] = 1.0 ;
  IbunI [0][0] [2][2] = 1.0 ;
  IbunI [1][1] [0][0] = 1.0 ;
  IbunI [1][1] [1][1] = 1.0 ;
  IbunI [1][1] [2][2] = 1.0 ;
  IbunI [2][2] [0][0] = 1.0 ;
  IbunI [2][2] [1][1] = 1.0 ;
  IbunI [2][2] [2][2] = 1.0 ;

  //form rank4 IIdev

  IIdev [0][0] [0][0] =  two3 ; // 0.666667 
  IIdev [0][0] [1][1] = -one3 ; //-0.333333 
  IIdev [0][0] [2][2] = -one3 ; //-0.333333 
  IIdev [0][1] [0][1] = 0.5 ;
  IIdev [0][1] [1][0] = 0.5 ;
  IIdev [0][2] [0][2] = 0.5 ;
  IIdev [0][2] [2][0] = 0.5 ;
  IIdev [1][0] [0][1] = 0.5 ;
  IIdev [1][0] [1][0] = 0.5 ;
  IIdev [1][1] [0][0] = -one3 ; //-0.333333 
  IIdev [1][1] [1][1] =  two3 ; // 0.666667 
  IIdev [1][1] [2][2] = -one3 ; //-0.333333 
  IIdev [1][2] [1][2] = 0.5 ;
  IIdev [1][2] [2][1] = 0.5 ;
  IIdev [2][0] [0][2] = 0.5 ;
  IIdev [2][0] [2][0] = 0.5 ;
  IIdev [2][1] [1][2] = 0.5 ;
  IIdev [2][1] [2][1] = 0.5 ;
  IIdev [2][2] [0][0] = -one3 ; //-0.333333 
  IIdev [2][2] [1][1] = -one3 ; //-0.333333 
  IIdev [2][2] [2][2] =  two3 ; // 0.666667 
}


XC::NDMaterial *XC::J2Plasticity::getCopy(const std::string &type) const
  {
    NDMaterial *retval= nullptr;
    if((type==strTipoPlaneStress2D) || (type==strTipoPlaneStress))
      retval= new XC::J2PlaneStress(this->getTag(), bulk, shear, sigma_0,
                                  sigma_infty, delta, Hard, eta) ;
    else if((type==strTipoPlaneStrain2D) || (type==strTipoPlaneStrain))
      retval= new XC::J2PlaneStrain(this->getTag(), bulk, shear, sigma_0,
                                  sigma_infty, delta, Hard, eta) ;
    else if((type==strTipoAxiSymmetric2D) || (type==strTipoAxiSymmetric))
      retval= new XC::J2AxiSymm(this->getTag(), bulk, shear, sigma_0,
                              sigma_infty, delta, Hard, eta) ;
    else if(((type==strTipoThreeDimensional)) ||
             ((type==strTipo3D)))
      retval= new XC::J2ThreeDimensional(this->getTag(), bulk, shear, sigma_0,
                              sigma_infty, delta, Hard, eta) ;
    else if( ((type==strTipoPlateFiber)) )
      retval= new XC::J2PlateFiber(this->getTag(), bulk, shear, sigma_0,
                              sigma_infty, delta, Hard, eta) ;
    else // Handle other cases
      std::cerr << "J2Plasticity::getModel failed to get model: " << type << std::endl;
    return retval;
  }

//print out material data
void XC::J2Plasticity::Print( std::ostream &s, int flag )
  {
    s << std::endl; ;
    s << "J2-Plasticity : " ; 
    s << this->getType( ) << std::endl; ;
    s << "Bulk Modulus =   " << bulk        << std::endl; ;
    s << "Shear Modulus =  " << shear       << std::endl; ;
    s << "Sigma_0 =        " << sigma_0     << std::endl; ;
    s << "Sigma_infty =    " << sigma_infty << std::endl; ;
    s << "Delta =          " << delta       << std::endl; ;
    s << "H =              " << Hard        << std::endl; ;
    s << "Eta =            " << eta         << std::endl; ;
    s << std::endl; ;
  }


//--------------------Plasticity-------------------------------------

//! @brief Plasticity integration routine
void XC::J2Plasticity::plastic_integrator( )
  {
    const double tolerance = (1.0e-8)*sigma_0 ;
    const double dt= ProblemaEF::theActiveDomain->getTimeTracker().getDt(); //time step

    static XC::Matrix dev_strain(3,3) ; //deviatoric strain
    static XC::Matrix dev_stress(3,3) ; //deviatoric stress
    static XC::Matrix normal(3,3) ;     //normal to yield surface

    double NbunN ; //normal bun normal 

    double norm_tau = 0.0 ;   //norm of deviatoric stress 
    double inv_norm_tau = 0.0 ;

    double phi = 0.0 ; //trial value of yield function
    double trace = 0.0 ; //trace of strain
    double gamma = 0.0 ; //consistency parameter
    double resid = 1.0 ; 
    double tang  = 0.0 ;
  
  double theta = 0.0 ; 
  double theta_inv = 0.0 ;

  double c1 = 0.0 ; 
  double c2 = 0.0 ;
  double c3 = 0.0 ;

  int i,j,k,l;
  int ii, jj ; 

  int iteration_counter ;
  const int max_iterations = 25 ;

  //compute the deviatoric strains

  trace = strain(0,0) + strain(1,1) + strain(2,2) ;
 
  dev_strain = strain ;
  for( i = 0; i < 3; i++ )
    dev_strain(i,i) -= ( one3*trace ) ;
   
  //compute the trial deviatoric stresses

  //   dev_stress = (2.0*shear) * ( dev_strain - epsilon_p_n ) ;
  dev_stress = dev_strain;
  dev_stress -= epsilon_p_n;
  dev_stress *= 2.0 * shear;

  //compute norm of deviatoric stress

  norm_tau = 0.0 ;
  for( i = 0; i < 3; i++ ){
    for( j = 0; j < 3; j++ ) 
      norm_tau += dev_stress(i,j)*dev_stress(i,j) ;
  } //end for i 

  norm_tau = sqrt( norm_tau ) ;

  if( norm_tau > tolerance ) {
    inv_norm_tau = 1.0 / norm_tau ;
    normal =  inv_norm_tau * dev_stress ;
  }
  else {
    normal.Zero( ) ;
    inv_norm_tau = 0.0 ;
  } //end if 

  //compute trial value of yield function

  phi = norm_tau -  root23 * q(xi_n) ;

  // check if phi > 0 
  
  if( phi > 0.0 ) { //plastic

     //solve for gamma 
     gamma = 0.0 ;
     resid = 1.0 ;
     iteration_counter = 0 ;
     while( fabs(resid) > tolerance ) {

        resid = norm_tau 
              - (2.0*shear) * gamma 
              - root23 * q( xi_n + root23*gamma ) 
              - (eta/dt) * gamma ;

        tang =  - (2.0*shear)  
                - two3 * qprime( xi_n + root23*gamma )
                - (eta/dt) ;

        gamma -= ( resid / tang ) ;

        iteration_counter++ ;

        if( iteration_counter > max_iterations ) {
            std::cerr << "More than " << max_iterations ;
            std::cerr << " iterations in constituive subroutine J2-plasticity \n" ;
            break ;
        } //end if 
        
     } //end while resid

     gamma *= (1.0 - 1e-08) ;

     //update plastic internal variables

     epsilon_p_nplus1 = epsilon_p_n + gamma*normal ;

     xi_nplus1 = xi_n + root23*gamma ;

     //recompute deviatoric stresses 

     dev_stress = (2.0*shear) * ( dev_strain - epsilon_p_nplus1 ) ;

     //compute the terms for plastic part of tangent

     theta =  (2.0*shear)  
           +  two3 * qprime( xi_nplus1 )
           +  (eta/dt) ;

     theta_inv = 1.0/theta ;

  }
  else { //elastic 

    //update history variables -- they remain unchanged

    epsilon_p_nplus1 = epsilon_p_n ;

    xi_nplus1 = xi_n ;

    //no extra tangent terms to compute 
    
    gamma = 0.0 ; 
    theta = 0.0 ;
    theta_inv = 0.0 ;

  } //end if phi > 0


  //add on bulk part of stress

  stress = dev_stress ;
  for( i = 0; i < 3; i++ )
     stress(i,i) += bulk*trace ;

  //compute the tangent

  c1 = -4.0 * shear * shear ;
  c2 = c1 * theta_inv ;
  c3 = c1 * gamma * inv_norm_tau ;

  for( ii = 0; ii < 6; ii++ ) {
    for( jj = 0; jj < 6; jj++ )  {

          index_map( ii, i, j ) ;
          index_map( jj, k, l ) ;

          NbunN  = normal(i,j)*normal(k,l) ; 

          //elastic terms
          tangent[i][j][k][l]  = bulk * IbunI[i][j][k][l] ;

          tangent[i][j][k][l] += (2.0*shear) * IIdev[i][j][k][l] ;

          //plastic terms 
          tangent[i][j][k][l] += c2 * NbunN ;

          tangent[i][j][k][l] += c3 * (  IIdev[i][j][k][l] - NbunN ) ;

          //minor symmetries 
          tangent [j][i][k][l] = tangent[i][j][k][l] ;
          tangent [i][j][l][k] = tangent[i][j][k][l] ;
          tangent [j][i][l][k] = tangent[i][j][k][l] ;

    } // end for jj
  } // end for ii

  return ;
} 





// set up for initial elastic
void XC::J2Plasticity::doInitialTangent(void) const
{
  int ii,jj,i,j,k,l;

  //compute the deviatoric strains
  for( ii = 0; ii < 6; ii++ ) {
    for( jj = 0; jj < 6; jj++ )  {

          index_map( ii, i, j ) ;
          index_map( jj, k, l ) ;

          //elastic terms
          initialTangent[i][j][k][l]  = bulk * IbunI[i][j][k][l] ;
          initialTangent[i][j][k][l] += (2.0*shear) * IIdev[i][j][k][l] ;

          //minor symmetries 
          //minor symmetries 
          initialTangent [j][i][k][l] = initialTangent[i][j][k][l] ;
          initialTangent [i][j][l][k] = initialTangent[i][j][k][l] ;
          initialTangent [j][i][l][k] = initialTangent[i][j][k][l] ;

    } // end for jj
  } // end for ii

  return ;
} 



//hardening function
double XC::J2Plasticity::q( double xi ) 
{
//  q(xi) = simga_infty + (sigma_0 - sigma_infty)*exp(-delta*xi) + H*xi 

 return    sigma_infty
         + (sigma_0 - sigma_infty)*exp(-delta*xi)
         + Hard*xi ;
}


//hardening function derivative
double XC::J2Plasticity::qprime( double xi )
{
  return  (sigma_0 - sigma_infty) * (-delta) * exp(-delta*xi)
         + Hard ;
}


//matrix_index ---> BJtensor indices i,j
void XC::J2Plasticity::index_map( int matrix_index, int &i, int &j ) const
{
  switch( matrix_index+1 ) { //add 1 for standard XC::BJtensor indices

    case 1 :
      i = 1 ; 
      j = 1 ;
      break ;
 
    case 2 :
      i = 2 ;
      j = 2 ; 
      break ;

    case 3 :
      i = 3 ;
      j = 3 ;
      break ;

    case 4 :
      i = 1 ;
      j = 2 ;
      break ;

    case 5 :
      i = 2 ;
      j = 3 ;
      break ;

    case 6 :
      i = 3 ;
      j = 1 ;
      break ;


    default :
      i = 1 ;
      j = 1 ;
      break ;

  } //end switch

i-- ; //subtract 1 for C-indexing
j-- ;

return ; 
}


XC::NDMaterial *XC::J2Plasticity::getCopy(void) const
{
  std::cerr << "J2Plasticity::getCopy -- subclass responsibility\n"; 
  exit(-1);
  return 0;
}

const std::string &XC::J2Plasticity::getType(void) const
  {
    std::cerr << "J2Plasticity::getType -- subclass responsibility\n";
    exit(-1);
    static const std::string error="error";
    return error;
  }

int XC::J2Plasticity::getOrder(void) const
  {
    std::cerr << "J2Plasticity::getOrder -- subclass responsibility\n";
    exit(-1);
    return 0;
  }


int XC::J2Plasticity::commitState( ) 
  {
    epsilon_p_n = epsilon_p_nplus1 ;
    xi_n        = xi_nplus1 ;
    return 0;
  }

int XC::J2Plasticity::revertToLastCommit( ) 
  { return 0; }


int XC::J2Plasticity::revertToStart( )
  {
    this->zero( ) ;
    return 0;
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::J2Plasticity::sendData(CommParameters &cp)
  {
    int res= NDMaterial::sendData(cp);
    res+= cp.sendDoubles(bulk,shear,sigma_0,sigma_infty,delta,getDbTagData(),CommMetaData(1));
    res+= cp.sendDoubles(Hard,eta,xi_n,xi_nplus1,getDbTagData(),CommMetaData(2));
    res+= cp.sendMatrix(epsilon_p_n,getDbTagData(),CommMetaData(3));
    res+= cp.sendMatrix(epsilon_p_nplus1,getDbTagData(),CommMetaData(4));
    res+= cp.sendMatrix(stress,getDbTagData(),CommMetaData(5));
    res+= cp.sendMatrix(strain,getDbTagData(),CommMetaData(6));
    size_t conta= 7;
    for(size_t i=0;i<3;i++)
      for(size_t j=0;j<3;j++)
         for(size_t k=0;k<3;k++)
            res+= cp.sendDoubles(tangent[i][j][k][0],tangent[i][j][k][1],tangent[i][j][k][2],getDbTagData(),CommMetaData(conta++));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::J2Plasticity::recvData(const CommParameters &cp)
  {
    int res= NDMaterial::recvData(cp);
    res+= cp.receiveDoubles(bulk,shear,sigma_0,sigma_infty,delta,getDbTagData(),CommMetaData(1));
    res+= cp.receiveDoubles(Hard,eta,xi_n,xi_nplus1,getDbTagData(),CommMetaData(2));
    res+= cp.receiveMatrix(epsilon_p_n,getDbTagData(),CommMetaData(3));
    res+= cp.receiveMatrix(epsilon_p_nplus1,getDbTagData(),CommMetaData(4));
    res+= cp.receiveMatrix(stress,getDbTagData(),CommMetaData(5));
    res+= cp.receiveMatrix(strain,getDbTagData(),CommMetaData(6));
    size_t conta= 7;
    for(size_t i=0;i<3;i++)
      for(size_t j=0;j<3;j++)
         for(size_t k=0;k<3;k++)
            res+= cp.receiveDoubles(tangent[i][j][k][0],tangent[i][j][k][1],tangent[i][j][k][2],getDbTagData(),CommMetaData(conta++));

    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::J2Plasticity::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(88);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::J2Plasticity::recvSelf(const CommParameters &cp)
  {
    inicComm(88);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }
