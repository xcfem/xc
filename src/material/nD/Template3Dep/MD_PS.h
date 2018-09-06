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
//
//================================================================================
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite Element Program                    #
//# PURPOSE:           Mazari - Dafalias  potential criterion                    #
//# CLASS:             MDPotentialSurface                                        #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                               #
//#                                                                              #
//#                                                                              #
//# DATE:              August 08 '00                                             #
//# UPDATE HISTORY:                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//# SHORT EXPLANATION:                                                           #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//================================================================================
//

#ifndef MD_PS_H    
#define MD_PS_H

#include "material/nD/Template3Dep/PS.h"


namespace XC {
//! @ingroup MatPS
//
//! @brief ??.
class MDPotentialSurface : public PotentialSurface
{

  public:
    PotentialSurface *newObj();  //create a colne of itself
    MDPotentialSurface();          // Default constructor

    BJtensor dQods(const EPState *EPS) const; 
    BJtensor d2Qods2(const EPState *EPS) const ;   

    //aux. functions for d2Qods2
    BJtensor dnods(const EPState *EPS) const;
    BJtensor dthetaoverds(const EPState *EPS) const;
    double dgoverdt(double theta, double c) const;
    BJtensor apqdnods(const EPState *EPS) const;
    
    void print() { std::cerr << *this; };

    //================================================================================
    // Overloaded Insertion Operator
    // prints an PotentialSurface's contents 
    //================================================================================
    friend std::ostream& operator<<(std::ostream &, const MDPotentialSurface &);
  };
 
inline std::ostream& operator<<(std::ostream &os, const MDPotentialSurface &PS)
  {
    os << "Manzari-Dafalias Potential Surface Parameters: " << std::endl;
     return os;
  }
} // end of XC namespace

#endif

