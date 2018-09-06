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

//================================================================================
// COPYRIGHT (C):     :-))                                                       |
// PROJECT:           Object Oriented Finite Element Program                     |
// PURPOSE:           General platform for elaso-plastic constitutive model      |
//                    implementation                                             |
//                                                                               |
// CLASS:             EvolutionLaw_L_Ep (linear scalar Evolution law)            |
//                                                                               |
//                                                                               |
// VERSION:                                                                      |
// LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )   |
// TARGET OS:         DOS || UNIX || . . .                                       |
// DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                |
// PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                                |
//                                                                               |
//                                                                               |
// DATE:              Mar. 28, 01                                                |
// UPDATE HISTORY:                                                               |
//                                                                               |
//                                                                               |
//                                                                               |
//                                                                               |
// SHORT EXPLANATION: This is a nonlinear evolution law for the evoltion of a    |
//                    scalar variable po which depends on plastic volumetric     |
//                    strain i.e. dpo = (1+eo)po/(lamda-kappa)*de_p              |
//                                                                               |
//================================================================================

#ifndef EL_NLEp_H
#define EL_NLEp_H

#include <cmath>
#include "material/nD/Template3Dep/EL_S.h"

namespace XC {
//! @ingroup MatNDEL
//
//! @brief This is a nonlinear evolution law for the evoltion of a
//! scalar variable po which depends on plastic volumetric
//! strain i.e. dpo = (1+eo)po/(lamda-kappa)*de_p.
class EvolutionLaw_NL_Ep : public EvolutionLaw_S
  {
  // Private vars to define the evolution law

  private:
    double  eo;      //coefficient to define the linear hardening rule of a scalar hardening var
    double  lambda;  //coefficient to define the linear hardening rule of a scalar hardening var
    double  kappa;   //coefficient to define the linear hardening rule of a scalar hardening var

  public:
    //EvolutionLaw_NL_Ep( );    // default constructor---no parameters
    
    EvolutionLaw_NL_Ep( double eod = 0.85, double lambdad = 0.19, double kappad = 0.06);
                         
    EvolutionLaw_NL_Ep(const EvolutionLaw_NL_Ep &LEL );   // Copy constructor
    
    EvolutionLaw_S *newObj();                     //create a clone of itself
    
    //void InitVars(EPState *EPS);    // Initialize all hardening vars called only once 
    //                                // after material point is formed if necessary.
    
    //void setInitD(EPState  *EPS);   // set initial D once current stress hits the y.s.
    //                                // was primarily for Manzari-Dafalias model

    //double h( EPState *EPS,  double norm_dQods);     // Evaluating hardening function h
    double h_s( EPState *EPS, PotentialSurface *PS);    // Evaluating hardening function h
    
    //void UpdateVar( EPState *EPS, double dlamda );  // Evolve corresponding var linearly using de_eq_p
    //Moved to CDriver.cpp

    void print();

  private:
    // some accessor functions
    double geteo() const;      
    //void   seteo( double eod);
    
    double getlambda() const;      
    //void   setlambda( double lambdad);
    
    double getkappa() const;      
    //void   setkappad( double kappad);

    //================================================================================
    // Overloaded Insertion Operator	  Zhaohui Added Aug. 13, 2000
    // prints Linear EvolutionLaw's contents 
    //================================================================================
    friend std::ostream& operator<<(std::ostream &, const EvolutionLaw_NL_Ep &);
  };
std::ostream& operator<<(std::ostream &, const EvolutionLaw_NL_Ep &);
} // end of XC namespace


#endif




