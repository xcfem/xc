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

//##################################################################################
//# COPYRIGHT (C):     :-))                                                        #
//# PROJECT:           Object Oriented Finite Element Program                      #
//# PURPOSE:           General platform for elaso-plastic constitutive model       #
//#                    implementation                                              #
//# CLASS:             PotentialSurface(the base class for all potential surfaces) #
//#                                                                                #
//# VERSION:                                                                       #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )    #
//# TARGET OS:         DOS || UNIX || . . .                                        #
//# DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                 #
//# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                                 #
//#                                                                                #
//#                                                                                #
//# DATE:              08-03-2000                                                  #
//# UPDATE HISTORY:                                                                #
//#                                                                                #
//#                                                                                #
//#                                                                                #
//#                                                                                #
//# SHORT EXPLANATION: The goal is to create a platform for efficient and easy     #
//#                    implementation of any elasto-plastic constitutive model!     #
//#                                                                                #
//##################################################################################
//

#ifndef PS_H
#define PS_H

#include <iostream>

namespace XC {
  class BJtensor;
  class EPState;

//! @ingroup 3DEPMat
//!
//! @defgroup MatPS Potential surface.
//
//! @ingroup MatPS
//
//! @brief The goal is to create a platform for efficient and easy
//! implementation of any elasto-plastic constitutive model.
class PotentialSurface
  {
  public:
    virtual PotentialSurface *newObj()  = 0;  //create a colne of itself
    virtual ~PotentialSurface() {};	      // Codewizard requires virtual destructor
    double  q() const { return 0.0; }; // Codewizard does not like function defined in class definition
    virtual BJtensor dQods(const EPState *EPS ) const =  0;  //pure virtual func
    virtual BJtensor d2Qods2(const EPState *EPS ) const = 0; //pure virtual func   
    virtual void print() = 0; //pure virtual func
    
    // Added for Consistent Algorithm, Z. Cheng, Jan 2004
    // Ref. Jeremic & Sture, Mechanics of Cohesive-Frictional Materials, Vol.2 165-183 (1997) 
    virtual BJtensor d2Qodsds1(const EPState *EPS ) const;
    virtual BJtensor d2Qodsds2(const EPState *EPS ) const;
    virtual BJtensor d2Qodsds3(const EPState *EPS ) const;
    virtual BJtensor d2Qodsds4(const EPState *EPS ) const;
    virtual BJtensor d2Qodsdt1(const EPState *EPS ) const;
    virtual BJtensor d2Qodsdt2(const EPState *EPS ) const;
    virtual BJtensor d2Qodsdt3(const EPState *EPS ) const;
    virtual BJtensor d2Qodsdt4(const EPState *EPS ) const;
                                 
    //================================================================================
    // Overloaded Insertion Operator
    // prints an PotentialSurface's contents 
    //================================================================================
    friend std::ostream& operator<<(std::ostream &, const PotentialSurface &);
  };

std::ostream& operator<<(std::ostream &os, const PotentialSurface & PS);

} // end of XC namespace


#endif

