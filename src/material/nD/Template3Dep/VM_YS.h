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
///*
//################################################################################
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite Element Program                    #
//# PURPOSE:           Von Mises         yield criterion                         #
//# CLASS:             VMYieldSurface                                            #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                               #
//#                                                                              #
//#                                                                              #
//# DATE:              August 03 '93                                             #
//# UPDATE HISTORY:    August 08 '00                                             #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//# SHORT EXPLANATION: Von Mises yield surface                                   #
//#                                                                              #
//#                                                                              #
//################################################################################
//*/

#ifndef VM_YS_H
#define VM_YS_H

#include "material/nD/Template3Dep/YS.h"


namespace XC {
//! @ingroup MatNDYS
//
//! @brief Von Mises yield surface.
class VMYieldSurface : public YieldSurface
  {
  public:
    // Create a colne of itself
    YieldSurface *newObj();  

    // Default constructor
    VMYieldSurface ( ) {}     

    // Copy constructor
    //VMYieldSurface (const VMYieldSurface & );   

    // Evaluation of f
    double f(const EPState *EPS) const;

    //First derivative of F over sigma
    BJtensor dFods(const EPState *EPS) const;

    // Redefine 1st derivative of F over first scalar internal variable
    double xi_s1(const EPState *EPS) const;   

    // Redefine 1st derivative of F over first tensorial internal variable
    BJtensor xi_t1(const EPState *EPS) const;

    void print() { std::cerr << *this; }; 
  
    //================================================================================
    // Overloaded Insertion Operator
    // prints an VM YieldSurface's contents 
    //================================================================================
    friend std::ostream& operator<<(std::ostream &, const VMYieldSurface &);
  };
 
std::ostream& operator<<(std::ostream &, const VMYieldSurface &);
} // end of XC namespace

#endif

