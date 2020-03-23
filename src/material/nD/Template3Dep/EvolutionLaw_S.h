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

///================================================================================
// COPYRIGHT (C):     :-))                                                        |
// PROJECT:           Object Oriented Finite Element Program                      |
// PURPOSE:           General platform for elaso-plastic efficient and easy       |
//                    constitutive model implementation                           |
//                                                                                |
// CLASS:             EvolutionLaw_S (the base class for scalar evolution law)    |
//                                                                                |
// VERSION:                                                                       |
// LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )    |
// TARGET OS:         DOS || UNIX || . . .                                        |
// DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                 |
// PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                                 |
//                                                                                |
//                                                                                |
// DATE:              09-02-2000                                                  |
// UPDATE HISTORY:                                                                |
//                                                                                |
//                                                                                |
//                                                                                |
//                                                                                |
// SHORT EXPLANATION: The base class is necessary for we need to use the runtime  |
//                    polymorphism of C++ to achieve the fexibility of the 	  |
//                    platform.                                                   |
//=================================================================================
//

#ifndef EL_S_H
#define EL_S_H

#include <iostream>

namespace XC {
class EPState;
 class PotentialSurface;

//! @ingroup 3DEPMat
//!
//! @defgroup MatNDEL Evolution law.
//!
//! @ingroup MatNDEL
//!
//! @brief Base class for scalar evolution laws.
class EvolutionLaw_S
  {
  public:
    
    EvolutionLaw_S(void){} //Normal Constructor
    virtual ~EvolutionLaw_S(void){}
    virtual EvolutionLaw_S *newObj();  //create a colne of itself

    // Not necessary since the increment of internal var can be evalulated in constitutive driver!
    //virtual void UpdateVar( EPState *EPS, double dlamda ) = 0; // Evolve only one internal variable
    
    virtual void print(); 	//Print the contents of the evolution law
    
    //virtual void InitVars( EPState *EPS) = 0;  // Initializing eo and E for Manzari-Dafalias model, 
    //                                  // other model might not need it!

    //virtual void setInitD( EPState *EPS) = 0;  // Initializing D once current st hits the y.s. for Manzari-Dafalias model , 
    //                                  // other model might not need it
    
    //Why can't i put const before EPState???????????????????????? Aug. 16, 2000
    //virtual double h( EPState *EPS, double d ) = 0;    // Evaluating hardening function h
    virtual double h_s( EPState *EPS, PotentialSurface *PS);    // Evaluating hardening function h

    //================================================================================
    // Overloaded Insertion Operator
    // prints an Evolution Law_S's contents 
    //================================================================================
    friend std::ostream &operator<<(std::ostream &, const EvolutionLaw_S &);
  };

std::ostream &operator<<(std::ostream &, const EvolutionLaw_S &);

} // end of XC namespace


#endif

