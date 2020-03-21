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
// $Date: 2001/08/23 16:45:51 $                                                                  
// $Source: /usr/local/cvs/OpenSees/SRC/nDarray/BJvector.h,v $                                                                
                                                                        
//#############################################################################
//                                                                            #
//                                                                            #
//             /~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/~~\                #
//            |                                          |____|               #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |        B A S E   C L A S S E S           |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |          C + +     H E A D E R           |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//         /~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/   |                    #
//         \_________________________________________\__/                     #
//                                                                            #
//                                                                            #
//#############################################################################
//#############################################################################
///*
//################################################################################
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite Element Program                    #
//# PURPOSE:                                                                     #
//# CLASS:             BJvector                                                    #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.10, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# DESIGNER(S):       Boris Jeremic                                             #
//# PROGRAMMER(S):     Boris Jeremic                                             #
//#                                                                              #
//#                                                                              #
//# DATE:              November '92                                              #
//# UPDATE HISTORY:    05 - __ avgust '93.  redefined as derived class from      #
//#                                 nDarray class                                #
//#                    August 22-29 '94 choped to separate files and worked on   #
//#                                   const and & issues                         #
//#                    August 30-31 '94 added use_def_dim to full the CC         #
//#                                   resolved problem with temoraries for       #
//#                                   operators + and - ( +=, -= )               #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//################################################################################
//*/


#ifndef VECTOR_HH
#define VECTOR_HH

#include "utility/matrix/nDarray/BJmatrix.h"

// All of this inheritance idioms are after
// Jim Coplien : "Advanced C++ programming styles and idioms".
// I tried to understand idioms and I think I succeeded.

namespace XC {
//! @ingroup Matrix
//!
//! @brief Boris Jeremic vector class.
class BJvector: virtual public BJmatrix
  {
    public:
      BJvector(int order_n = 1, double initvalue = 0.0);  // default constructor

      BJvector(int order_n, double *initval);

      explicit BJvector(const nDarray &x); // copy-initializer

//....       ~BJvector( );

      BJvector& operator=( const BJvector & x ); // BJvector assignment
//..      BJvector& operator=( const BJmatrix & x ); // BJvector assignment
//..      BJvector& operator=( const nDarray & x ); // BJvector assignment

//#######        BJmatrix operator*( BJvector &); // BJvector multiplication

//....     BJvector operator*( double arg); // scalar multiplication 
// this  ellipsis at the end are just to prevent the compiler
// from issuing a warning on hiding function from base class nDarray . . . 
     double &val(int subscript, ... );
     const double &val(int subscript, ... ) const;
     double cval(int subscript, ... ) const; // const
// THE ROW COUNTER STARTS FROM 1 ( NOT FROM 0 )
  };
} // end of XC namespace
#endif
