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
///*
//##############################################################################
//# COPYRIGHT (C):   :-))                                                      #
//# PROJECT:         Object Oriented Finite Element Program                    #
//# PURPOSE:         Von Mises         yield criterion                         #
//# CLASS:           VonMisesYieldSurface                                      #
//#                                                                            #
//# VERSION:                                                                   #
//# LANGUAGE:        C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:       DOS || UNIX || . . .                                      #
//# PROGRAMMER(S):   Boris Jeremic, Zhaohui Yang                               #
//#                                                                            #
//#                                                                            #
//# DATE:            August 03 '93                                             #
//# UPDATE HISTORY:  August 08 '00                                             #
//#                                                                            #
//#                                                                            #
//#                                                                            #
//#                                                                            #
//# SHORT EXPLANATION: Von Mises yield surface                                 #
//#                                                                            #
//#                                                                            #
//##############################################################################
//*/

#ifndef VonMisesYieldSurface_H
#define VonMisesYieldSurface_H

#include "material/nD/Template3Dep/YieldSurface.h"


namespace XC {
//! @ingroup MatNDYS
//!
//! @brief Von Mises yield surface.
class VonMisesYieldSurface: public YieldSurface
  {
  public:
    // Default constructor
    VonMisesYieldSurface ( ) {}     
    // Create a clone of itself
    YieldSurface *getCopy(void);  

    // Evaluation of f
    double f(const EPState *EPS) const;

    //First derivative of F over sigma
    BJtensor dFods(const EPState *EPS) const;

    double xi_s1(const EPState *EPS) const;   

    BJtensor xi_t1(const EPState *EPS) const;

    void print() { std::cerr << *this; }; 
  
    friend std::ostream& operator<<(std::ostream &, const VonMisesYieldSurface &);
  };
 
std::ostream& operator<<(std::ostream &, const VonMisesYieldSurface &);
} // end of XC namespace

#endif

