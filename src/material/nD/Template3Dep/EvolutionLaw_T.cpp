//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
# CLASS:             EvolutionLaw_T                                                #
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

#ifndef EvolutionLaw_T_CPP
#define EvolutionLaw_T_CPP

#include "material/nD/Template3Dep/EvolutionLaw_T.h"
#include <utility/matrix/nDarray/basics.h>
#include <utility/matrix/nDarray/BJtensor.h>
#include <utility/matrix/nDarray/stresst.h>

// #include <nDarray/straint.h>

// #include "material/nD/Template3Dep/EPState.h"
// #include "material/nD/Template3Dep/PotentialSurface.h"

//================================================================================
//  Create a clone of itself 
//================================================================================
XC::EvolutionLaw_T * XC::EvolutionLaw_T::getCopy(void)
  {
    EvolutionLaw_T *newEL = new EvolutionLaw_T( *this );
    return newEL;
  }

//================================================================================
// Evaluating h_ ( for the evaluation of Kp )
//================================================================================

XC::BJtensor XC::EvolutionLaw_T::h_t( EPState *EPS, PotentialSurface *PS)
{
    // Return zero valued XC::BJtensor
    XC::stresstensor temp;
    return temp;
}

//================================================================================
// updating E, e D and m for Manzari-Dafalias model
//================================================================================

int XC::EvolutionLaw_T::updateEeDm( EPState *EPS, double st_vol, double dLamda)
{
   // do nothing
   return 0;
}  



//================================================================================
//  Print vars defined in Linear Evolution Law_T
//================================================================================
void XC::EvolutionLaw_T::print()
  { std::cerr << (*this); }

//================================================================================
// Overloaded Insertion Operator
// prints base Evolution Law_T's contents 
//================================================================================
std::ostream& XC::operator<<(std::ostream &os, const XC::EvolutionLaw_T & EL)
  {
    os << "Base of Tensorial Evolution Law's Parameters: Nothing" << std::endl;
   return os;
  }




#endif

