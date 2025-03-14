// -*-c++-*-
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
//
//================================================================================
//# COPYRIGHT (C):     :-))                                                   #
//# PROJECT:           Object Oriented Finite Element Program                 #
//# PURPOSE:           Manzari - Dafalias  potential criterion                 #
//# CLASS:             ManzariDafaliasPotentialSurface                                     #
//#                                                                           #
//# VERSION:                                                                  #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                   #
//# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                            #
//#                                                                           #
//#                                                                           #
//# DATE:              August 08 '00                                          #
//# UPDATE HISTORY:                                                           #
//#                                                                           #
//#                                                                           #
//#                                                                           #
//#                                                                           #
//# SHORT EXPLANATION:                                                        #
//#                                                                           #
//#                                                                           #
//#                                                                           #
//#                                                                           #
//#                                                                           #
//#                                                                           #
//#                                                                           #
//#                                                                           #
//#                                                                           #
//#                                                                           #
//=============================================================================
//

#ifndef ManzariDafaliasPotentialSurface_H    
#define ManzariDafaliasPotentialSurface_H

#include "material/nD/template_3d_ep/PotentialSurface.h"

namespace XC {
//! @ingroup MatPS
//!
//! @brief Manzari-Dafalias model potential surface.
class ManzariDafaliasPotentialSurface: public PotentialSurface
{

  public:
    PotentialSurface *getCopy(void) const; // create a clone of itself
    ManzariDafaliasPotentialSurface(); // Default constructor

    BJtensor dQods(const EPState *EPS) const; 
    BJtensor d2Qods2(const EPState *EPS) const ;   

    //aux. functions for d2Qods2
    BJtensor dnods(const EPState *EPS) const;
    BJtensor dthetaoverds(const EPState *EPS) const;
    double dgoverdt(double theta, double c) const;
    BJtensor apqdnods(const EPState *EPS) const;
    
    virtual void print(std::ostream &os) const;
  };
 
} // end of XC namespace

#endif

