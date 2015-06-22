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
#include "material/nD/TipoMaterialND.h"

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
  { return strTipoAxiSymmetric2D ; }


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

  return tangent_matrix ;
} 

//send back the tangent 
const XC::Matrix& XC::J2AxiSymm::getInitialTangent(void) const
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
int XC::J2AxiSymm::setTrialStrain(const XC::Tensor &v) 
{
  return -1 ;
}

int XC::J2AxiSymm::setTrialStrain(const XC::Tensor &v, const XC::Tensor &r)     
{
  return -1 ;
}

int XC::J2AxiSymm::setTrialStrainIncr(const XC::Tensor &v) 
{
  return -1 ;
}

int XC::J2AxiSymm::setTrialStrainIncr(const XC::Tensor &v, const XC::Tensor &r) 
{
  return -1 ;
}

const XC::Tensor& XC::J2AxiSymm::getTangentTensor(void) const
  { return rank4 ; }

//jeremic@ucdavis.edu 22jan2001const XC::Tensor& XC::J2AxiSymm::getStressTensor( ) 
//jeremic@ucdavis.edu 22jan2001{
//jeremic@ucdavis.edu 22jan2001  return rank2 ;
//jeremic@ucdavis.edu 22jan2001}
//jeremic@ucdavis.edu 22jan2001
//jeremic@ucdavis.edu 22jan2001const XC::Tensor& XC::J2AxiSymm::getStrainTensor( ) 
//jeremic@ucdavis.edu 22jan2001{
//jeremic@ucdavis.edu 22jan2001  return rank2 ;
//jeremic@ucdavis.edu 22jan2001}

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

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::J2AxiSymm::sendData(CommParameters &cp)
  {
    int res= J2Plasticity::sendData(cp);
    res+= cp.sendDoubles(commitEps00,commitEps11,commitEps01,commitEps22,getDbTagData(),CommMetaData(88));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::J2AxiSymm::recvData(const CommParameters &cp)
  {
    int res= J2Plasticity::recvData(cp);
    res+= cp.receiveDoubles(commitEps00,commitEps11,commitEps01,commitEps22,getDbTagData(),CommMetaData(88));
    return res;
  }

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
int XC::J2AxiSymm::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(89);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::J2AxiSymm::recvSelf(const CommParameters &cp)
  {
    inicComm(7);
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












