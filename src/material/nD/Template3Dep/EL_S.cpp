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
/*
//================================================================================
# COPYRIGHT (C):     :-))                                                        #
# PROJECT:           Object Oriented Finite XC::Element Program                      #
# PURPOSE:           General platform for elaso-plastic constitutive model       #
#                    implementation                                              #
#                                                                                #
# CLASS:             EvolutionLaw_S(base Evolution Law class for scalar var      #
#                                                                                #
# VERSION:                                                                       #
# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )    #
# TARGET OS:         DOS || UNIX || . . .                                        #
# DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                 #
# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                                 #
#                                                                                #
#                                                                                #
# DATE:              09-02-2000                                                  #
# UPDATE HISTORY:    09-12-2000                                                  #
#                                                                                #
#                                                                                #
#                                                                                #
# SHORT EXPLANATION: Here are some initial definitions of some virtual funs,     #
#                    some of which will be redefined in derived classes.         #
#                                                                                #
//================================================================================
*/

#ifndef EL_CPP
#define EL_CPP

#include "material/nD/Template3Dep/EL_S.h"
#include "utility/matrix/nDarray/basics.h"

// #include <nDarray/stresst.h>
// #include <nDarray/straint.h>
// #include <nDarray/BJtensor.h>

// #include "material/nD/Template3Dep/EPState.h"
// #include "material/nD/Template3Dep/PS.h"    

//================================================================================
//  Create a clone of itself 
//================================================================================
XC::EvolutionLaw_S * XC::EvolutionLaw_S::newObj() {
    
    EvolutionLaw_S *newEL = new EvolutionLaw_S( *this );
    
    return newEL;

}


//================================================================================
// Evaluating scalar hardening function h_s ( for the evaluation of Kp )
//================================================================================

double XC::EvolutionLaw_S::h_s( EPState *EPS, PotentialSurface *PS){

    return 0.0;

}


//================================================================================
//  Print content of the base class, might be overwritten! 
//================================================================================
void XC::EvolutionLaw_S::print()
  { std::cerr << *this; }

//================================================================================
// Overloaded Insertion Operator
// prints an Evolution Law_S's contents 
//================================================================================
std::ostream &XC::operator<< (std::ostream &os, const XC::EvolutionLaw_S & EL)
  {
    os << "Scalar Evolution Law's Parameters: " << std::endl;
    return os;
  }



#endif

