// -*-c++-*-
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
///*
//================================================================================
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite Element Program                    #
//# PURPOSE:           Cam Clay yield criterion                                  #
//# CLASS:             CamClayYieldSurface01(for monotonic loading)                  #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# PROGRAMMER(S):     Boris Jeremic, ZHaohui Yang                               #
//#                                                                              #
//#                                                                              #
//# DATE:              Mar. 28, 2001                                             #
//# UPDATE HISTORY:                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//# SHORT EXPLANATION:                                                           #
//#                                                                              #
//#                                                                              #
//================================================================================
//*/

#ifndef CamClayYieldSurface_H
#define CamClayYieldSurface_H

#include "material/nD/Template3Dep/YieldSurface.h"


namespace XC {
//! @ingroup MatNDYS
//!
//! @brief Cam-Clay yield surface.
class CamClayYieldSurface: public YieldSurface
  {
  // Private vars to define the Manzari-Dafalias Yield Surface
  private:
    double M; //!< the slope of critical state line
  public:
    CamClayYieldSurface(const double &Mp= 1.2); // Default constructor
    YieldSurface *getCopy(void); // create a clone of itself

    double f(const EPState *EPS) const;
    BJtensor dFods(const EPState *EPS) const;

    // Redefine 1st derivative of F over scalar internal variables
    double xi_s1( const EPState *EPS ) const;  // df/dm

    // Redefine 1st derivative of F over tensorial internal variables

    double getM() const;
    void setM(const double &);
    void print() { std::cerr << *this; };    
  };

// Overloaded Insertion Operator
std::ostream &operator<<(std::ostream &, const CamClayYieldSurface &);

} // end of XC namespace

#endif

