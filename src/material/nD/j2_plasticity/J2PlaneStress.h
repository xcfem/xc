// -*-c++-*-
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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/02/14 23:01:25 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/J2PlaneStress.h,v $

#ifndef J2PlaneStress_h
#define J2PlaneStress_h

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
//                    2 eps_01   }   <--- note the 2
// 
//  set eta := 0 for rate independent case
//

#include "J2Plasticity.h"

namespace XC{
//! @ingroup J2NDMat
//
//! @brief J2 Isotropic hardening material class
//! for plane stress problems.
class J2PlaneStress: public J2Plasticity
  {
  private: 
    //static vectors and matrices
    static Vector strain_vec;     //strain in vector notation
    static Vector stress_vec;     //stress in vector notation
    static Matrix tangent_matrix; //material tangent in matrix notation

    double commitEps22;

    //index mapping special for plane stress because of 
    // condensation on tangent
    void index_map( int matrix_index, int &i, int &j );
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public : 
    J2PlaneStress(int tag= 0);
    //elastic constructor
    J2PlaneStress( int tag, double K, double G);

    //full constructor
    J2PlaneStress(   int    tag, 
                     double K,
                     double G,
                     double yield0,
                     double yield_infty,
                     double d,
                     double H,
                     double viscosity );



    //make a clone of this material
    NDMaterial* getCopy(void) const;

    //send back type of material
    const std::string &getType(void) const;

    //send back order of strain in vector form
    int getOrder( ) const;

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

    //swap history variables
    int commitState( ); 
    int revertToLastCommit( );
    int revertToStart( );

    //sending and receiving
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    


}; //end of J2PlaneStress declarations


} //end of XC namespace
#endif

