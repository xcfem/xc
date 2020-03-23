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
//================================================================================
# COPYRIGHT (C):     :-))                                                        #
# PROJECT:           Object Oriented Finite XC::Element Program                      #
# PURPOSE:           General platform for elaso-plastic constitutive model       #
#                    implementation                                              #
# CLASS:             YS (the base class for all Yield Surface)                   #
#                                                                                #
# VERSION:                                                                       #
# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )    #
# TARGET OS:         DOS || UNIX || . . .                                        #
# DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                 #
# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                                 #
#                                                                                #
#                                                                                #
# DATE:              08-03-2000                                                  #
# UPDATE HISTORY:                                                                #
#                                                                                #
#                                                                                #
#                                                                                #
#                                                                                #
# SHORT EXPLANATION: This is the base class of all yield surface. Here defined   #
#                    are initial values of the first derivatives of dF/ds_i 	 #
#                    and dF/ot_i                                                 # 
#                                                                                #
//================================================================================
*/

#ifndef YS_CPP
#define YS_CPP

#include "material/nD/Template3Dep/YieldSurface.h"
#include "utility/matrix/nDarray/BJtensor.h"
#include "utility/matrix/nDarray/stresst.h"

//================================================================================
// First derivative of F over scalar internal variables 
// (at most 4 scalar internal vars allowed currently)
//================================================================================

//================================================================================
//First derivative of F over the 1st scalar var
//================================================================================

double XC::YieldSurface::xi_s1( const XC::EPState *EPS ) const
{ 
     return 0.0;
}

//================================================================================
//First derivative of F over the 2nd scalar var
//================================================================================

double XC::YieldSurface::xi_s2( const XC::EPState *EPS ) const
{ 
     return 0.0;
}

//================================================================================
//First derivative of F over the 3rd scalar var
//================================================================================

double XC::YieldSurface::xi_s3( const XC::EPState *EPS ) const
{ 
     return 0.0;
}

//================================================================================
//First derivative of F over the 4th scalar var
//================================================================================

double XC::YieldSurface::xi_s4( const XC::EPState *EPS ) const
{ 
     return 0.0;
}

//================================================================================
// First derivative of F over scalar internal variables 
// (at most 4 tensorial internal vars allowed currently)
//================================================================================

//================================================================================
//First derivative of F over the 1st scalar var
//================================================================================
XC::BJtensor XC::YieldSurface::xi_t1( const XC::EPState *EPS ) const
  { 
     XC::stresstensor temp;
     return temp;
  }
//================================================================================
//First derivative of F over the 2nd scalar var
//================================================================================
 XC::BJtensor XC::YieldSurface::xi_t2( const XC::EPState *EPS ) const
{ 
     XC::stresstensor temp;
//     clog << " inside XC::YieldSurface::xi_t2 "   <<temp;
     return temp;
}

//================================================================================
//First derivative of F over the 3rd scalar var
//================================================================================
 XC::BJtensor XC::YieldSurface::xi_t3( const XC::EPState *EPS ) const
{ 
     XC::stresstensor temp;
//     clog << "inside  XC::YieldSurface::xi_t3 " <<temp;
     return temp;
}

//================================================================================
//First derivative of F over the 4th scalar var
//================================================================================
 XC::BJtensor XC::YieldSurface::xi_t4( const XC::EPState *EPS ) const
{ 
     XC::stresstensor temp;
    // clog << " inside XC::YieldSurface::xi_t4 "  << temp;
     return temp;
}

//================================================================================
// friend XC::std::ostream functions for output
//================================================================================
std::ostream& XC::operator<<(std::ostream &os, const YieldSurface & YS)
  {
    os << "Yield Surface Parameters: " << std::endl;
    return os;
  }


#endif

