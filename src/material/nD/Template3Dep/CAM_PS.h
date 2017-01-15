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
//================================================================================
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite Element Program                    #
//# PURPOSE:           CAM CLAY potential criterion                              #
//# CLASS:             CAMPotentialSurface                                           #
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

#ifndef CAM_PS_H
#define CAM_PS_H

#include "material/nD/Template3Dep/PS.h"

namespace XC {
  class BJtensor;
//! @ingroup MatPS
//
//! @brief ??.
class CAMPotentialSurface : public PotentialSurface
  {
  // Private vars to define the Mazari-Dafalias Potential Surface
  private:
    double M;	// the slope of critical state line

  public:
    PotentialSurface *newObj();                  //create a colne of itself
    CAMPotentialSurface(double Mp = 1.2);        // Default constructor

    BJtensor dQods(const EPState *EPS) const;
    BJtensor d2Qods2(const EPState *EPS) const;
    
    BJtensor d2Qodsds1(const EPState *EPS) const; // For Consistent Algorithm, Z Cheng, Jan 2004

    void print() { std::cerr << *this; };
    double getM() const;
    

// moved to stresstensor Boris Jeremic@ucdavis.edu 21Aug2001
//  private:
//    BJtensor dpoverds( ) const;
//    BJtensor dqoverds(const EPState *EPS) const;
//    BJtensor dthetaoverds(const EPState *EPS) const;
//    BJtensor d2poverds2( void ) const;
//    BJtensor d2qoverds2(const EPState *EPS) const;
//    BJtensor d2thetaoverds2(const EPState *EPS) const;
//	     	          
    //================================================================================
};

// Overloaded Insertion Operator
std::ostream &operator<<(std::ostream &os,const CAMPotentialSurface &YS);

} // end of XC namespace

#endif

