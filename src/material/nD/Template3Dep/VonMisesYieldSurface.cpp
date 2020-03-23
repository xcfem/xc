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
//# CLASS:             VonMisesYieldSurface                                            #
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

#ifndef VonMisesYieldSurface_CPP
#define VonMisesYieldSurface_CPP

#include "material/nD/Template3Dep/VonMisesYieldSurface.h"
#include "material/nD/Template3Dep/EPState.h"

//================================================================================
// Copy constructor
//================================================================================
//
//XC::VonMisesYieldSurface::VonMisesYieldSurface(const VonMisesYieldSurface &) {
//
//}

//================================================================================
//create a colne of itself
//================================================================================

XC::YieldSurface * XC::VonMisesYieldSurface::newObj()
  {  

     YieldSurface  *newYieldSurface = new VonMisesYieldSurface();
     return newYieldSurface;

  }

//================================================================================
//  Yield criterion evaluation function f = 3/2 *Sij_bar Sij_bar- k^2
//  c.f. pp.274 XC::W.F.Chen Plasticity for Structure Engineers
//================================================================================

double XC::VonMisesYieldSurface::f(const XC::EPState *EPS) const {
    //deviatoric stress XC::BJtensor
    int nod = EPS->getNTensorVar();
    XC::stresstensor alpha;

    XC::stresstensor sigma = EPS->getStress();
    if ( nod >=1 ) //May not have kinematic hardening
      alpha = EPS->getTensorVar(1);

    XC::stresstensor sigma_bar = sigma - alpha;   
    XC::stresstensor s_bar = sigma_bar.deviator();


    double k = EPS->getScalarVar(1);
    double k2 = k * k;
    
    XC::stresstensor temp1 = s_bar("ij") * s_bar("ij");
    double temp = temp1.trace();
    temp = temp * 3.0 / 2.0;

    double f   = temp - k2;

    return f;
}


//================================================================================
// BJtensor dF/dsigma_ij = 3*( S_ij - alpha_ij )
//================================================================================

 XC::BJtensor XC::VonMisesYieldSurface::dFods(const XC::EPState *EPS) const {

    int nod = EPS->getNTensorVar();

    XC::stresstensor sigma = EPS->getStress();
    XC::stresstensor alpha;
    if ( nod >=1 ) //May not have kinematic hardening
      alpha = EPS->getTensorVar(1);

    XC::stresstensor sigma_bar = sigma - alpha;   
    XC::stresstensor s_bar = sigma_bar.deviator();
    BJtensor dFods = s_bar * 3.0;
    
    return dFods;
}

//================================================================================
// double xi1 = dF/dk = 2.0*k  Derivative in terms of first scalar var
//================================================================================

double XC::VonMisesYieldSurface::xi_s1(const XC::EPState *EPS) const {

    double k = EPS->getScalarVar( 1 );
    
    return 2.0 * k;
}


//================================================================================
// BJtensor xi_k1=dF/d_alpha_ij=-3.0*S_bar_pq  Derivative in terms of 1st XC::BJtensor var
//================================================================================

 XC::BJtensor XC::VonMisesYieldSurface::xi_t1(const XC::EPState *EPS) const {

    XC::stresstensor sigma = EPS->getStress();
    XC::stresstensor alpha = EPS->getTensorVar(1);

    XC::stresstensor sigma_bar = sigma - alpha;   
    XC::stresstensor s_bar = sigma_bar.deviator();
    BJtensor xi = s_bar * (-3.0);
    
    return xi;
}

//================================================================================
std::ostream& XC::operator<<(std::ostream &os, const VonMisesYieldSurface & YS)
  {
    os << "Von Mises Yield Surface Parameters: " << std::endl;
    return os;
  }



#endif

