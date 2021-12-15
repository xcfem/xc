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
/*
//================================================================================
# COPYRIGHT (C):     :-))                                                        #
# PROJECT:           Object Oriented Finite XC::Element Program                      #
# PURPOSE:           General platform for elasto-plastic constitutive model      #
#                    implementation                                              #
# CLASS:             PS (the base class for all Potential Surfaces)              #
#                                                                                #
# VERSION:                                                                       #
# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )    #
# TARGET OS:         DOS || UNIX || . . .                                        #
# DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                 #
# PROGRAMMER(S):     Boris Jeremic, Zhao Cheng                                   #
#                                                                                #
# Date:           Jan 2004                                                       #
# UPDATE HISTORY:                                                                #
#                                                                                #
#                                                                                #
#                                                                                #
#                                                                                #
# SHORT EXPLANATION: This is the base class of all Potential surfaces.           #
#                                                                                #
//================================================================================
*/

#ifndef PS_CPP
#define PS_CPP

#include "material/nD/Template3Dep/PotentialSurface.h"
#include "utility/matrix/nDarray/BJtensor.h"

// At least 4 scalar and/or XC::BJtensor internal variables are allowed at current time

//================================================================================
// The d(dQ/dsigma_ij)/ds1
//================================================================================
XC::BJtensor XC::PotentialSurface::d2Qodsds1( const XC::EPState *EPS ) const
  { 
    BJtensor temp(def_dim_2,0.0);
    return temp;
  }

//================================================================================
// The d(dQ/dsigma_ij)/ds2
//================================================================================
XC::BJtensor XC::PotentialSurface::d2Qodsds2( const XC::EPState *EPS ) const
{ 
     BJtensor temp(def_dim_2,0.0);
     return temp;
}

//================================================================================
// The d(dQ/dsigma_ij)/ds3
//================================================================================
XC::BJtensor XC::PotentialSurface::d2Qodsds3( const XC::EPState *EPS ) const
{ 
     BJtensor temp(def_dim_2,0.0);
     return temp;
}

//================================================================================
// The d(dQ/dsigma_ij)/ds4
//================================================================================
XC::BJtensor XC::PotentialSurface::d2Qodsds4( const XC::EPState *EPS ) const
{ 
     BJtensor temp(def_dim_2,0.0);
     return temp;
}
          
//================================================================================
// The d(dQ/dsigma_ij)/dt1_mn
//================================================================================
XC::BJtensor XC::PotentialSurface::d2Qodsdt1( const XC::EPState *EPS ) const
{ 
     BJtensor temp(def_dim_4,0.0);
     return temp;
}

//================================================================================
// The d(dQ/dsigma_ij)/dt2_mn
//================================================================================
XC::BJtensor XC::PotentialSurface::d2Qodsdt2( const XC::EPState *EPS ) const
{ 
     BJtensor temp(def_dim_4,0.0);
     return temp;
}

//================================================================================
// The d(dQ/dsigma_ij)/dt3_mn
//================================================================================
XC::BJtensor XC::PotentialSurface::d2Qodsdt3( const XC::EPState *EPS ) const
{ 
     BJtensor temp(def_dim_4,0.0);
     return temp;
}

//================================================================================
// The d(dQ/dsigma_ij)/dt4_mn
//================================================================================
XC::BJtensor XC::PotentialSurface::d2Qodsdt4( const XC::EPState *EPS ) const
  { 
     BJtensor temp(def_dim_4,0.0);
     return temp;
  }

//! @brief Print stuff.
std::ostream& XC::operator<<(std::ostream &os, const XC::PotentialSurface & PS)
  {
    os << "Potential Surface Parameters: " << std::endl;
    return os;
  }

#endif
