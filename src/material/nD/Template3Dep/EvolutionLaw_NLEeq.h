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

//================================================================================
// COPYRIGHT (C):     :-))                                                       |
// PROJECT:           Object Oriented Finite Element Program                     |
// PURPOSE:           General platform for elaso-plastic constitutive model      |
//                    implementation                                             |
//                                                                               |
// CLASS:             NonLinear EvolutionLaw (on plastic equivalent strain)      |
//                                                                               |
// VERSION:                                                                      |
// LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )   |
// TARGET OS:         DOS || UNIX || . . .                                       |
// DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                |
// PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                                |
//                                                                               |
//                                                                               |
// DATE:              09-02-2000                                                 |
// UPDATE HISTORY:                                                               |
//                                                                               |
//                                                                               |
//                                                                               |
//                                                                               |
// SHORT EXPLANATION: This is a nonlinear evolution law for the evoltion of a    |
//                    scalar variable k which depends on plastic equi. strain    |
//                    i.e. dk = f( de_eq_p )                                     |
//                                                                               |
//================================================================================

#ifndef EvolutionLaw_NLEeq_H
#define EvolutionLaw_NLEeq_H

#include <cmath>
#include "material/nD/Template3Dep/EvolutionLaw_S.h"

namespace XC {
//! @ingroup MatNDEL
//!
//! @brief Non linear evolution law (on plastic equivalent strain).
//!
//! This is a nonlinear evolution law for the evoltion of a scalar
//! variable k which depends on plastic equi. strain i.e.
//! \f$dk = f( de_eq_p )\f$.                                    
class EvolutionLaw_NL_Eeq: public EvolutionLaw_S
  {
    // Private vars to define the evolution law
    //Coefficients to define the nonlinear hardening rule of a scalar var eta
    double eeqEtaPeak, etaResidual, etaStart, etaPeak, e, d;

  public:
    // default constructor
    EvolutionLaw_NL_Eeq( double eeqEtaPeakd = 0.003,
                         double etaResiduald = .2,
                         double etaStartd = 0.3,
                         double etaPeakd = 0.5,
                         double ed = 0.5,
                         double dd = 500.0) :
                       eeqEtaPeak(eeqEtaPeakd), etaResidual(etaResiduald),
                       etaStart(etaStartd), etaPeak(etaPeakd), e(ed), d(dd) {}
                         
    EvolutionLaw_NL_Eeq(const EvolutionLaw_NL_Eeq &NLEL );   // Copy constructor
    
    EvolutionLaw_S *getCopy(void); // create a clone of itself
    
    //void InitVars(EPState *EPS);    // Initialize all hardening vars called only once 
    //                                // after material point is formed if necessary.
    
    //void setInitD(EPState  *EPS);   // set initial D once current stress hits the y.s.
    //                                // was primarily for Manzari-Dafalias model

    //double h( EPState *EPS, double d );     // Evaluating hardening function h
    double h_s( EPState *EPS, PotentialSurface *PS);    // Evaluating hardening function h
    
    //void UpdateVar( EPState *EPS, double dlamda );  // Evolve corresponding var linearly using de_eq_p

    void print();

    // some accessor functions
    // some accessor functions
    double geteeqEtaPeak() const;
    double getetaResidual() const;
    double getetaStart() const;
    double getetaPeak() const;
    double gete() const;
    double getd() const;

    //================================================================================
    // Overloaded Insertion Operator	 
    // prints nonlinear EvolutionLaw's contents 
    //================================================================================
    friend std::ostream& operator<<(std::ostream &, const EvolutionLaw_NL_Eeq &);
  };
std::ostream& operator<<(std::ostream &, const EvolutionLaw_NL_Eeq &);
} // end of XC namespace


#endif




