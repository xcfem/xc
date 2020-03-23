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
//################################################################################
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite Element Program                    #
//# PURPOSE:           Drucker - Prager  potential criterion                     #
//# CLASS:             DruckerPragerPotentialSurface                                        #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# PROGRAMMER(S):     Boris Jeremic, ZHaohui Yang                               #
//#                                                                              #
//#                                                                              #
//# DATE:              August 03 '93                                             #
//# UPDATE HISTORY:    August 08 '00                                             #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//# SHORT EXPLANATION:                                                           #
//#                                                                              #
//# if alfa1#=0 && alfa2#=0 && alfa1#=alfa2 =>                                   #
//#              Drucker-Prager with non-associated flow rule                    #
//#                                                                              #
//# if alfa1#=0 && alfa2#=0 && alfa1==alfa2 =>                                   #
//#              Drucker-Prager with associated flow rule                        #
//#                                                                              #
//# if alfa1==0 && alfa2#=0 =>                                                   #
//#              Von Mises with non-associated Drucker-Prager flow rule          #
//#                                                                              #
//# if alfa1#=0 && alfa2==0 =>                                                   #
//#              Drucker-Prager with non-associated Von Mises flow rule          #
//#                                                                              #
//# if alfa1==0 && alfa2==0 =>                                                   #
//#              Von Mises with associated flow rule                             #
//#                                                                              #
//################################################################################
//*/

#ifndef DruckerPragerPotentialSurface_H
#define DruckerPragerPotentialSurface_H

#include "material/nD/Template3Dep/PotentialSurface.h"

namespace XC {
//! @ingroup MatPS
//!
//! @brief Drucker-Prager model potential surface.
class DruckerPragerPotentialSurface: public PotentialSurface
{
  private:		  // Private vars to define the Drucker-Prager Potential Surface
    double alfa2;	  // potential surface orientation angle  
  
  public:
    DruckerPragerPotentialSurface( double a2d = 0.0 ) : alfa2(a2d) {}   // Default constructor
    virtual ~DruckerPragerPotentialSurface() { }; //Virtual functions not all redefined
    DruckerPragerPotentialSurface( const DruckerPragerPotentialSurface &); //Copy constructor
    PotentialSurface *newObj();              //create a colne of itself
    double getalfa2() const;

    BJtensor dQods(const EPState *EPS) const;
    BJtensor d2Qods2(const EPState *EPS) const;
    
    BJtensor d2Qodsds1(const EPState *EPS) const; // For Consistent Algorithm, Z Cheng, Jan 2004        
    
    void print() { std::cerr << *this; };


};

//================================================================================
// Overloaded Insertion Operator
// prints an DruckerPrager-PotentialSurface's contents 
//================================================================================
std::ostream& operator<<(std::ostream &, const DruckerPragerPotentialSurface &);

} // end of XC namespace

#endif

