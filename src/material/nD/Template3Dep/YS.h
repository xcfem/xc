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
################################################################################
# COPYRIGHT (C):     :-))                                                      #
# PROJECT:           Object Oriented Finite Element Program                    #
# PURPOSE:           General platform for elaso-plastic constitutive model     #
#                    implementation                                            #
# CLASS:             YieldSurface (the base class for all yield surfaces)      #
#                                                                              #
# VERSION:                                                                     #
# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
# TARGET OS:         DOS || UNIX || . . .                                      #
# DESIGNER(S):       Boris Jeremic, Zhaohui Yang                               #
# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                               #
#                                                                              #
#                                                                              #
# DATE:              08-03-2000                                                #
# UPDATE HISTORY:                                                              #
#                                                                              #
#                                                                              #
#                                                                              #
#                                                                              #
# SHORT EXPLANATION: The goal is to create a platform for efficient and easy   #
#                    implemetation of any elasto-plastic constitutive model!   #
#                                                                              #
################################################################################*/

#ifndef YS_H
#define YS_H

#include <iostream>

namespace XC {
  class EPState;
  class BJtensor;

//! @ingroup 3DEPMat
//!
//! @defgroup MatNDYS Yield surface.
//
//! @ingroup MatNDYS
//
//! @brief The goal is to create a platform for efficient and easy
//! implemetation of any elasto-plastic constitutive model.
class YieldSurface
  {
  public:
    YieldSurface(void){} //Normal Constructor
    inline virtual ~YieldSurface(void){} 
    virtual YieldSurface *newObj(void)= 0;  //create a clone of itself
  
    virtual double f( const EPState *EPS ) const = 0;	 //pure virtual func
    virtual BJtensor dFods( const EPState *EPS ) const = 0;  //pure virtual func
    virtual void print() = 0; //pure virtual func

    // 1st derivative of F over scalar internal variables (at most 4 scalar internal vars allowed currently)
    virtual double xi_s1( const EPState *EPS ) const;	 
    virtual double xi_s2( const EPState *EPS ) const;
    virtual double xi_s3( const EPState *EPS ) const;
    virtual double xi_s4( const EPState *EPS ) const;

    // 1st derivative of F over scalar internal variables (at most 4 tensor internal vars allowed currently)
    virtual BJtensor xi_t1( const EPState *EPS ) const;	 
    virtual BJtensor xi_t2( const EPState *EPS ) const;
    virtual BJtensor xi_t3( const EPState *EPS ) const;
    virtual BJtensor xi_t4( const EPState *EPS ) const;


    //================================================================================
    // Overloaded Insertion Operator
    // prints an YieldSurface's contents 
    //================================================================================
    friend std::ostream& operator<<(std::ostream &, const YieldSurface &);
  };
std::ostream& operator<<(std::ostream &, const YieldSurface &);
} // end of XC namespace


#endif

