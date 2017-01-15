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
//================================================================================
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite XC::Element Program                    #
//# PURPOSE:           Von Mises  yield criterion                                #
//# CLASS:             VMPotentialSurface                                        #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# PROGRAMMER(S):     Boris Jeremic, ZHaohui Yang                               #
//#                                                                              #
//#                                                                              #
//# DATE:              August 31 '00                                             #
//# UPDATE HISTORY:                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//================================================================================
//*/

#ifndef VM_PS_CPP
#define VM_PS_CPP

#include "material/nD/Template3Dep/VM_PS.h"
#include "material/nD/Template3Dep/EPState.h"

//================================================================================
// Copy constructor
//================================================================================
//
//XC::VMPotentialSurface::VMPotentialSurface(const VMPotentialSurface &VMYS ) {
//
//}

//================================================================================
//create a colne of itself
//================================================================================

XC::PotentialSurface * XC::VMPotentialSurface::newObj() {  

     PotentialSurface  *new_YS = new VMPotentialSurface();
     return new_YS;

}


//================================================================================
// BJtensor dQ/dsigma_ij = 3*( S_ij )        c.f. pp.274 XC::W.F.Chen
//================================================================================

XC::BJtensor XC::VMPotentialSurface::dQods(const XC::EPState *EPS) const {
    
    // Deviatoric stress XC::BJtensor of sigma - alpha
    XC::stresstensor sigma = EPS->getStress();
    int nod = EPS->getNTensorVar();
    XC::stresstensor alpha;

    if ( nod >=1 ) //May not have kinematic hardening
      alpha = EPS->getTensorVar(1);

    XC::stresstensor sigma_bar = sigma - alpha;   
    XC::stresstensor s_bar = sigma_bar.deviator();
    //s_bar.null_indices();
    
    // XC::Tensor dQ/dsigma_ij 
    BJtensor dQods = s_bar * 3.0;
    dQods.null_indices();
    
    return dQods;
}

//================================================================================
// BJtensor d2Qods2 = d[ 3*(S_ij - alpha_ij) ] /dsigma_ij 
//================================================================================

XC::BJtensor XC::VMPotentialSurface::d2Qods2(const XC::EPState *EPS) const {

    BJtensor I("I", 2, def_dim_2);
    BJtensor temp1 = I("im") * I("jn");
    temp1.null_indices();

    BJtensor I2("I", 2, def_dim_2);
    BJtensor temp2 = I2("mn") * I2("ij") * (1.0/3.0);
    temp2.null_indices();
    
    //// Deviatoric stress XC::BJtensor
    //XC::stresstensor s = EPS->CurrentStress.deviator();
    //s.null_indices();
    
    BJtensor d2Qods2 = (temp1 - temp2) * 3.0;
    d2Qods2.null_indices();
    
    return d2Qods2;
}

// For Consistent Algorithm, Z Cheng, Jan 2004
XC::BJtensor XC::VMPotentialSurface::d2Qodsdt1(const XC::EPState *EPS) const 
{  
  BJtensor d2Qoverdsdt1(4, def_dim_4, 0.0);
  BJtensor I2("I", 2, def_dim_2);
  BJtensor I4 = I2("ij") * I2("mn");
  I2.null_indices();
  d2Qoverdsdt1 = ( I4.transpose0110() + I4.transpose0111() ) * (-1.5);
  return d2Qoverdsdt1;
}

//================================================================================
// friend XC::std::ostream functions for output
//================================================================================
std::ostream& XC::operator<< (std::ostream& os, const XC::VMPotentialSurface & YS)
  {
    os << "Von Mises Potential Surface Parameters: " << std::endl;
    return os;
  }

#endif

