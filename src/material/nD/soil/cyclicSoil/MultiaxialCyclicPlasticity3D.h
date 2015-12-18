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
 |              MultiaxialCyclicPlasticity  NDMaterial                      |
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


#ifndef MultiaxialCyclicPlasticity3D_h
#define MultiaxialCyclicPlasticity3D_h

#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

#include <material/nD/soil/cyclicSoil/MultiaxialCyclicPlasticity.h>

namespace XC{
//! @ingroup SoilNDMat
//
//! @brief ??.
class MultiaxialCyclicPlasticity3D : public MultiaxialCyclicPlasticity
  {

//-------------------Declarations-------------------------------

  public : 

    MultiaxialCyclicPlasticity3D(int tag) ;
    //null constructor
    MultiaxialCyclicPlasticity3D( ) ;

    //full constructor
    MultiaxialCyclicPlasticity3D(   int    tag,
				  double rho,
				  double K,
				  double G,
				  double Su,
				  double Ho_kin,
				  double Parameter_h,
				  double Parameter_m,
				  double Parameter_beta,
				  double Kcoeff,
				  double viscosity = 0
				  ) ;
  

  //elastic constructor
  MultiaxialCyclicPlasticity3D( int tag, double rho, double K, double G ) ;

  //make a clone of this material
  NDMaterial* getCopy(void) const;

  //send back type of material
  const std::string &getType( ) const ;

  //send back order of strain in vector form
  int getOrder( ) const ;

  //get the strain and integrate plasticity equations
  int setTrialStrain( const Vector &strain_from_element) ;

  //unused trial strain functions
  int setTrialStrain( const Vector &v, const Vector &r ) ;
  int setTrialStrainIncr( const Vector &v ) ;
  int setTrialStrainIncr( const Vector &v, const Vector &r ) ;

  //send back the strain
  const Vector& getStrain(void) const;

  //send back the stress 
  const Vector& getStress(void) const;

  //send back the tangent 
  const Matrix& getTangent(void) const;
  const Matrix& getInitialTangent(void) const;

  //this is mike's problem
  int setTrialStrain(const Tensor &v) ;
  int setTrialStrain(const Tensor &v, const Tensor &r) ;    
  int setTrialStrainIncr(const Tensor &v) ;
  int setTrialStrainIncr(const Tensor &v, const Tensor &r) ;
  const Tensor& getTangentTensor( ) ;
 
  private :

  //static vectors and matrices
  static Vector strain_vec ;     //strain in vector notation
  static Vector stress_vec ;     //stress in vector notation
  static Matrix tangent_matrix ; //material tangent in matrix notation

} ; //end of MultiaxialCyclicPlasticity3D declarations

} //end of XC namespace
#endif
