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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** ****************************************************************** */
                                                                        
 
/*----+----+----+----+----+----+----+----+----+----+----+----+----+----+----*
 |                                                                          | 
 |              MultiaxialCyclicPlasticity  XC::NDMaterial                      |
 +                                                                          +
 |--------------------------------------------------------------------------|
 |                                                                          |
 +             Authors: Gang Wang  AND  Professor Nicholas Sitar            +
 |                                                                          |
 |			   Department of Civil and Environmental Engineering            |
 +			   Univeristy of California, Berkeley, CA 94720, USA            +
 |                                                                          |
 |             Email: wang@ce.berkeley.edu (G.W.)                           |
 |                                                                          | 
 +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----*/


#include <material/nD/soil/cyclicSoil/MultiaxialCyclicPlasticity3D.h>
#include "material/nD/TipoMaterialND.h"

//static vectors and matrices
XC::Vector XC::MultiaxialCyclicPlasticity3D::strain_vec(6) ;
XC::Vector XC::MultiaxialCyclicPlasticity3D::stress_vec(6) ;
XC::Matrix XC::MultiaxialCyclicPlasticity3D::tangent_matrix(6,6) ;


XC::MultiaxialCyclicPlasticity3D::MultiaxialCyclicPlasticity3D(int tag)
  : XC::MultiaxialCyclicPlasticity(tag, ND_TAG_MultiaxialCyclicPlasticity3D) 
  {}

//null constructor
 XC::MultiaxialCyclicPlasticity3D::MultiaxialCyclicPlasticity3D( ) : 
 XC::MultiaxialCyclicPlasticity( ) 
{  }


//full constructor
 XC::MultiaxialCyclicPlasticity3D::MultiaxialCyclicPlasticity3D(   int    tag,
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
 XC::MultiaxialCyclicPlasticity( tag, ND_TAG_MultiaxialCyclicPlasticity3D, rho, K, G, 
    	Su, Ho_kin, Parameter_h, Parameter_m, Parameter_beta, Kcoeff, viscosity)
{ 

}


//elastic constructor
 XC::MultiaxialCyclicPlasticity3D::MultiaxialCyclicPlasticity3D(   int    tag, 
    			 double rho,
                 double K, 
                 double G ) :
 XC::MultiaxialCyclicPlasticity( tag, ND_TAG_MultiaxialCyclicPlasticity3D, rho, K, G )
{ 

}



//make a clone of this material
XC::NDMaterial* XC::MultiaxialCyclicPlasticity3D::getCopy(void) const 
  { return new MultiaxialCyclicPlasticity3D(*this); }


//send back type of material
const std::string &XC::MultiaxialCyclicPlasticity3D::getType( ) const 
  { return strTipoThreeDimensional; }


//send back order of strain in vector form
int XC::MultiaxialCyclicPlasticity3D::getOrder( ) const 
{ 
  return 6 ; 
} 


//get the strain and integrate plasticity equations
int XC::MultiaxialCyclicPlasticity3D::setTrialStrain( const XC::Vector &strain_from_element) 
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

  
  //std::cerr<<"DP:setTrialStrain:MaterialStageID= "<<this->MaterialStageID<<std::endl;

  if(this->MaterialStageID ==1) { 
      //std::cerr<<"DP:setTrialStrain:elastic K0"<<std::endl;
      this->elastic_integrator( ) ;
  } else if(this->MaterialStageID ==2) {	  
      //std::cerr<<"DP:setTrialStrain:plastic"<<std::endl;
      // initialize history variables here in first call???
      
      this->plastic_integrator( ) ;
  }
  


  return 0 ;
}


//unused trial strain functions
int XC::MultiaxialCyclicPlasticity3D::setTrialStrain( const XC::Vector &v, const XC::Vector &r )
{ 
   return this->setTrialStrain( v ) ;
} 

int XC::MultiaxialCyclicPlasticity3D::setTrialStrainIncr( const XC::Vector &v ) 
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

int XC::MultiaxialCyclicPlasticity3D::setTrialStrainIncr( const XC::Vector &v, const XC::Vector &r ) 
{
  return this->setTrialStrainIncr(v);
}



//send back the strain
const XC::Vector& XC::MultiaxialCyclicPlasticity3D::getStrain(void) const
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
const XC::Vector& XC::MultiaxialCyclicPlasticity3D::getStress(void) const
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
const XC::Matrix& XC::MultiaxialCyclicPlasticity3D::getTangent(void) const
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
const XC::Matrix& XC::MultiaxialCyclicPlasticity3D::getInitialTangent(void) const
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
int XC::MultiaxialCyclicPlasticity3D::setTrialStrain(const XC::Tensor &v) 
{
  return -1 ;
}

int XC::MultiaxialCyclicPlasticity3D::setTrialStrain(const XC::Tensor &v, const XC::Tensor &r)     
{
  return -1 ;
}

int XC::MultiaxialCyclicPlasticity3D::setTrialStrainIncr(const XC::Tensor &v) 
{
  return -1 ;
}

int XC::MultiaxialCyclicPlasticity3D::setTrialStrainIncr(const XC::Tensor &v, const XC::Tensor &r) 
{
  return -1 ;
}

const XC::Tensor& XC::MultiaxialCyclicPlasticity3D::getTangentTensor( ) 
{
  return rank4 ;
}

//jeremic@ucdavis.edu 22jan2001const XC::Tensor& J2ThreeDimensional::getStressTensor( ) 
//jeremic@ucdavis.edu 22jan2001{
//jeremic@ucdavis.edu 22jan2001  return rank2 ;
//jeremic@ucdavis.edu 22jan2001}
//jeremic@ucdavis.edu 22jan2001
//jeremic@ucdavis.edu 22jan2001const XC::Tensor& J2ThreeDimensional::getStrainTensor( ) 
//jeremic@ucdavis.edu 22jan2001{
//jeremic@ucdavis.edu 22jan2001  return rank2 ;
//jeremic@ucdavis.edu 22jan2001}







