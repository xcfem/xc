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
                                                                        
/*----+----+----+----+----+----+----+----+----+----+----+----+----+----+----*
 |                                                                          | 
 |              MultiaxialCyclicPlasticity  XC::NDMaterial                      |
 +                                                                          +
 |--------------------------------------------------------------------------|
 |                                                                          |
 +             Authors: Gang Wang  AND  Professor Nicholas Sitar            +
 |                                                                          |
 |                           Department of Civil and Environmental Engineering            |
 +                           Univeristy of California, Berkeley, CA 94720, USA            +
 |                                                                          |
 |             Email: wang@ce.berkeley.edu (G.W.)                           |
 |                                                                          | 
 +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----*/

#include <material/nD/soil/cyclicSoil/MultiaxialCyclicPlasticityPlaneStrain.h>
#include <material/nD/TipoMaterialND.h>

//static vectors and matrices
XC::Vector XC::MultiaxialCyclicPlasticityPlaneStrain::strain_vec(3) ;
XC::Vector XC::MultiaxialCyclicPlasticityPlaneStrain::stress_vec(3) ;
XC::Matrix XC::MultiaxialCyclicPlasticityPlaneStrain::tangent_matrix(3,3) ;

XC::MultiaxialCyclicPlasticityPlaneStrain::MultiaxialCyclicPlasticityPlaneStrain(int tag)
 :XC::MultiaxialCyclicPlasticity(tag, ND_TAG_MultiaxialCyclicPlasticityPlaneStrain) 
  {}

//null constructor
XC::MultiaxialCyclicPlasticityPlaneStrain::MultiaxialCyclicPlasticityPlaneStrain( ) : 
 XC::MultiaxialCyclicPlasticity( ) 
  {}


//full constructor
XC::MultiaxialCyclicPlasticityPlaneStrain::MultiaxialCyclicPlasticityPlaneStrain(int    tag, 
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
 XC::MultiaxialCyclicPlasticity( tag, ND_TAG_MultiaxialCyclicPlasticityPlaneStrain, rho, K, G, 
      Su, Ho_kin, Parameter_h, Parameter_m, Parameter_beta, Kcoeff, viscosity)
{ 

}



//elastic constructor
XC::MultiaxialCyclicPlasticityPlaneStrain::MultiaxialCyclicPlasticityPlaneStrain(   int    tag, double rho,    // add density by Gang Wang
                 double K, 
                 double G ) :
 XC::MultiaxialCyclicPlasticity( tag, ND_TAG_MultiaxialCyclicPlasticityPlaneStrain, rho, K, G )
{ 

}

//make a clone of this material
XC::NDMaterial* XC::MultiaxialCyclicPlasticityPlaneStrain::getCopy(void) const
  { return new MultiaxialCyclicPlasticityPlaneStrain(*this); }



//send back type of material
const std::string &XC::MultiaxialCyclicPlasticityPlaneStrain::getType( ) const 
  { return strTipoPlaneStrain2D; }


//send back order of strain in vector form
int XC::MultiaxialCyclicPlasticityPlaneStrain::getOrder( ) const 
{ 
  return 3 ; 
} 


//get the strain and integrate plasticity equations
int XC::MultiaxialCyclicPlasticityPlaneStrain::setTrialStrain( const XC::Vector &strain_from_element) 
{
  strain.Zero( ) ;

  strain(0,0) =        strain_from_element(0) ;
  strain(1,1) =        strain_from_element(1) ;
  strain(0,1) = 0.50 * strain_from_element(2) ;
  strain(1,0) =        strain(0,1) ;

  if(this->MaterialStageID ==1) { 
      this->elastic_integrator( ) ;
  } else if(this->MaterialStageID ==2) {          
      this->plastic_integrator( ) ;
  }
  return 0 ;
}


//unused trial strain functions
int XC::MultiaxialCyclicPlasticityPlaneStrain::setTrialStrain( const XC::Vector &v, const XC::Vector &r )
{ 
   return this->setTrialStrain( v ) ;
} 

int XC::MultiaxialCyclicPlasticityPlaneStrain::setTrialStrainIncr( const XC::Vector &v ) 
{
  static XC::Vector newStrain(3);
  newStrain(0) = strain(0,0) + v(0);
  newStrain(1) = strain(1,1) + v(1);
  newStrain(2) = 2.0 * strain(0,1) + v(2);

  return this->setTrialStrain(newStrain);  
}

 

int XC::MultiaxialCyclicPlasticityPlaneStrain::setTrialStrainIncr( const XC::Vector &v, const XC::Vector &r ) 
{
 
   // implemented April. 4, 2004, Gang Wang
    static XC::Vector newStrain(3);
    newStrain(0) = strain(0,0) + v(0);
    newStrain(1) = strain(1,1) + v(1);
         newStrain(2) = 2.0*strain(0,1) + v(2);
            
    return this->setTrialStrain(newStrain);

}


//send back the strain
const XC::Vector& XC::MultiaxialCyclicPlasticityPlaneStrain::getStrain(void) const
{
  strain_vec(0) =       strain(0,0) ;
  strain_vec(1) =       strain(1,1) ;
  strain_vec(2) = 2.0 * strain(0,1) ;

  return strain_vec ;
} 


//send back the stress 
const XC::Vector& XC::MultiaxialCyclicPlasticityPlaneStrain::getStress(void) const
{
  stress_vec(0) = stress(0,0) ;
  stress_vec(1) = stress(1,1) ;
  stress_vec(2) = stress(0,1) ;

  return stress_vec ;
}

//send back the tangent 
const XC::Matrix& XC::MultiaxialCyclicPlasticityPlaneStrain::getTangent(void) const
{
  // matrix to XC::BJtensor mapping
  //  XC::Matrix      XC::Tensor
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
const XC::Matrix& XC::MultiaxialCyclicPlasticityPlaneStrain::getInitialTangent(void) const
{
  // matrix to XC::BJtensor mapping
  //  XC::Matrix      XC::Tensor
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

//this is mike's problem
int XC::MultiaxialCyclicPlasticityPlaneStrain::setTrialStrain(const XC::Tensor &v) 
{
  return -1 ;
}

int XC::MultiaxialCyclicPlasticityPlaneStrain::setTrialStrain(const XC::Tensor &v, const XC::Tensor &r)     
{
  return -1 ;
}

int XC::MultiaxialCyclicPlasticityPlaneStrain::setTrialStrainIncr(const XC::Tensor &v) 
{
  return -1 ;
}

int XC::MultiaxialCyclicPlasticityPlaneStrain::setTrialStrainIncr(const XC::Tensor &v, const XC::Tensor &r) 
{
  return -1 ;
}

const XC::Tensor& XC::MultiaxialCyclicPlasticityPlaneStrain::getTangentTensor( ) 
{
  return rank4 ;
}

 
/*
int 
XC::MultiaxialCyclicPlasticityPlaneStrain::commitState( ) 
{
  epsilon_p_n = epsilon_p_nplus1;
  xi_n        = xi_nplus1;

  return 0;
}

int 
XC::MultiaxialCyclicPlasticityPlaneStrain::revertToLastCommit( ) {

  return 0;
}


int 
XC::MultiaxialCyclicPlasticityPlaneStrain::revertToStart( ) 
{
  this->zero( ) ;

  return 0;
}
 

int
XC::MultiaxialCyclicPlasticityPlaneStrain::sendSelf(CommParameters &cp)
{
 
  return -1;
}

int
XC::MultiaxialCyclicPlasticityPlaneStrain::recvSelf(const CommParameters &cp)
{
  
  return -1;
}


*/





