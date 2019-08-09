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

//################################################################################
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           OpenSees                                                  #
//# PURPOSE:           Triaxial Failure Criterion for Concrete - yield criterion #
//# CLASS:             TriFCYieldSurface                                         #
//#                                                                              #
//# VERSION:           1.0                                                       #
//# LANGUAGE:          C++ (ili tako nesto)                                      #
//# TARGET OS:                                                                   #
// DESIGNER(S):       Boris Jeremic and Zhaohui Yang [jeremic,zhyang]@ucdavis.edu|
// PROGRAMMER(S):     Vlado Vukadin                                              |
//#                                                                              #
//#                                                                              #
//# DATE:             June 01, 2002                                              #
//# UPDATE HISTORY:    bice tako dobr da nece biti potreban update :)            #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//# SHORT EXPLANATION:                                                           #
//#                                                                              #
//# Yield surface is based on article by Menetrey, P. and William, K.J.          #
//# published in 1995 in  ACI Structural Journal pp 311-318. Purpose of the     #
//# Yield surface is to model triaxial strength of concrete                      #
//#                                                                              #
//#                                                                              #
//################################################################################
//*/

#ifndef Tri_a_fail_crit_YS_H
#define Tri_a_fail_crit_YS_H


#include "material/nD/Template3Dep/YS.h"

namespace XC {
//! @ingroup MatNDYS
//
//! @brief Yield surface is based on article by Menetrey, P. and William, K.J.
//! published in 1995 in  ACI Structural Journal pp 311-318. Purpose of the
//! Yield surface is to model triaxial strength of concrete.
class TriFCYieldSurface : public YieldSurface
  {
  // Private vars to define the TriFCYieldSurface Yield Surface
  private:
     double fcomp;
     double ftens;
     double el; 
     double c;
  public:
    YieldSurface *newObj();  //create a clone of itself
    
    TriFCYieldSurface (double fc, double ft, double e, double coh );   // Default constructor

     

    double f(const EPState *EPS) const;
    BJtensor dFods(const EPState *EPS) const;

    // Redefine 1st derivative of F over scalar internal variables
    //double xi_s1( const EPState *EPS ) const;
    //double xi_s2( const EPState *EPS ) const;

    // Redefine 1st derivative of F over tensorial internal variables
    //tensor xi_t1(const EPState *EPS) const
    //{
    
    //}
    
    double getfcomp() const;
    double getftens() const;
    double getel() const;
    double get_c() const;

    void print() { std::cerr << *this; }; 
  
    //================================================================================
    // Overloaded Insertion Operator
    // prints an XX YieldSurface's contents 
    //================================================================================
    friend std::ostream& operator<<(std::ostream &, const TriFCYieldSurface &);
  };
std::ostream& operator<<(std::ostream &, const TriFCYieldSurface &);
} // end of XC namespace

#endif

