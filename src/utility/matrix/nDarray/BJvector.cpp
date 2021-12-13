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
                                                                        
// $Revision: 1.1 $                                                              
// $Date: 2001/08/23 16:45:47 $                                                                  
// $Source: /usr/local/cvs/OpenSees/SRC/nDarray/BJvector.cpp,v $                                                                
                                                                        
                                                                        
//############################################################################
//#                                                                          #
//#             /~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/~~\              #
//#            |                                          |____|             #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |                 B A S E                  |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |              C L A S S E S               |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |          C + +     S O U R C E           |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#         /~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/   |                  #
//#        |                                         |    |                  #
//#         \_________________________________________\__/                   #
//#                                                                          #
//#                                                                          #
//############################################################################
//
//   "C makes it easy to shoot yourself in the foot, C++ makes it harder,
//   but when you do, it blows away your whole leg" -- Bjarne Stroustrup
//
///*
//################################################################################
//# COPY-YES  (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite XC::Element Program                    #
//# PURPOSE:                                                                     #
//# CLASS:             Vector class                                              #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# DESIGNER(S):       Boris Jeremic ( with help from ARKoenig in JOOP )         #
//# PROGRAMMER(S):     Boris Jeremic ( with help from ARKoenig in JOOP )         #
//#                                                                              #
//#                                                                              #
//# DATE:              Nov. 14. 1992.                                            #
//# UPDATE HISTORY:    05 - __ avgust '93.  derived class from XC::BJmatrix class      #
//#                                         which is derived from                #
//#                                         nDarray class                        #
//#                    August 22-29 '94 choped to separate files and worked on   #
//#                                   const and & issues                         #
//#                    August 30-31 '94 added use_def_dim to full the CC         #
//#                                   resolved problem with temoraries for       #
//#                                   operators + and - ( +=, -= )               #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//################################################################################
//*/

// All of this inheritance idioms are after
// Jim Coplien : "Advanced C++ programming styles and idioms".

#ifndef VECTOR_CC
#define VECTOR_CC

#include "BJvector.h"

//##############################################################################
XC::BJvector::BJvector(int order_n, double initvalue):
  BJmatrix( 2, order_n, 1, initvalue)  {  }  // default constructor
// rank 2 ^ just to be consistent with rank of XC::BJmatrix
//##############################################################################
XC::BJvector::BJvector(int order_n, double *initval)
  : BJmatrix(2, order_n, 1, initval)  {  }
//rank 2 ^ just to be consistent with rank of XC::BJmatrix

//##############################################################################
XC::BJvector::BJvector(const nDarray & x)
  : BJmatrix( x )   {  } // copy-initializer



//! @brief Assignment operator.
XC::BJvector &XC::BJvector::operator=(const XC::BJvector & rval)
  {
    if(&rval == this) // if assign an BJvector to itself
      return *this;
    
    assign(rval);
    return *this;
  }

//##############################################################################
//
// CODE BOUND checking routine ( slower but safer )
//
double XC::BJvector::cval(int subscript, ... )  const
  { return (this->XC::BJmatrix::cval(subscript,1)); }

double &XC::BJvector::val(int subscript, ... )
  { return (this->XC::BJmatrix::val(subscript,1)); }

const double &XC::BJvector::val(int subscript, ... ) const
  { return (this->XC::BJmatrix::val(subscript,1)); }

#endif
