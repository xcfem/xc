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
                                                                        
/*----+----+----+----+----+----+----+----+----+----+----+----+----+----+----*
 |                                                                          | 
 |              MultiaxialCyclicPlasticity  NDMaterial                      |
 +                                                                          +
 |--------------------------------------------------------------------------|
 |                                                                          |
 +             Authors: Gang Wang  AND  Professor Nicholas Sitar            +
 |                                                                          |
 |	       Department of Civil and Environmental Engineering            |
 +	       University of California, Berkeley, CA 94720, USA            +
 |                                                                          |
 |             Email: wang@ce.berkeley.edu (G.W.)                           |
 |                                                                          | 
 +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----*/
#ifndef MultiaxialCyclicPlasticityPlaneStrain_h
#define MultiaxialCyclicPlasticityPlaneStrain_h

#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

#include "material/nD/soil/cyclicSoil/MultiaxialCyclicPlasticity.h"

namespace XC{
//! @ingroup SoilNDMat
//
//! @brief ??.
class MultiaxialCyclicPlasticityPlaneStrain: public MultiaxialCyclicPlasticity
  {

//-------------------Declarations-------------------------------

  private :
    
  //static vectors and matrices
  static Vector strain_vec ;     //strain in vector notation
  static Vector stress_vec ;     //stress in vector notation
  static Matrix tangent_matrix ; //material tangent in matrix notation
  public : 

  MultiaxialCyclicPlasticityPlaneStrain(int tag);
  //null constructor
  MultiaxialCyclicPlasticityPlaneStrain( );

  //full constructor
  MultiaxialCyclicPlasticityPlaneStrain(       int    tag, 
				 double rho,
                 double K,
                 double G,
				 double Su,
 			     double Ho_kin,
                 double Parameter_h,
                 double Parameter_m,
                 double Parameter_beta,
                 double Kcoeff,
                 double viscosity=0 );

  //elastic constructor // add density by Gang Wang
  MultiaxialCyclicPlasticityPlaneStrain( int tag, double rho, double K, double G );

  //make a clone of this material
  NDMaterial* getCopy(void) const;

  //send back type of material
  const std::string &getType( ) const ;

  //send back order of strain in vector form
  int getOrder( ) const ;


  //get the strain and integrate plasticity equations
  int setTrialStrain( const Vector &strain_from_element);

  //unused trial strain functions
  int setTrialStrain( const Vector &v, const Vector &r );
  int setTrialStrainIncr( const Vector &v );
  int setTrialStrainIncr( const Vector &v, const Vector &r );

  //send back the strain
  const Vector& getStrain(void) const;

  //send back the stress 
  const Vector& getStress(void) const;

  //send back the tangent 
  const Matrix& getTangent(void) const;
  const Matrix& getInitialTangent(void) const;

  //this is mike's problem
  int setTrialStrain(const Tensor &v);
  int setTrialStrain(const Tensor &v, const Tensor &r);    
  int setTrialStrainIncr(const Tensor &v);
  int setTrialStrainIncr(const Tensor &v, const Tensor &r);
  const Tensor& getTangentTensor( );
  
  //swap history variables
  //int commitState( ); 
  //int revertToLastCommit( );
  //int revertToStart( );
  }; 

//end of MultiaxialCyclicPlasticityPlaneStrain declarations

} //fin de namespace XC
#endif

