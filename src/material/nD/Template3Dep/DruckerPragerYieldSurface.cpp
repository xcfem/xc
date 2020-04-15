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
//# PURPOSE:           Drucker - Prager  yield criterion                         #
//# CLASS:             DruckerPragerYieldSurface                                            #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# PROGRAMMER(S):     Boris Jeremic, ZHaohui Yang                               #
//#                                                                              #
//#                                                                              #
//# DATE:              August 08 '00                                             #
//# UPDATE HISTORY:    20Aug2004 ZC added kinematic hardening part               #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//================================================================================
//*/

#ifndef DruckerPragerYieldSurface_CPP
#define DruckerPragerYieldSurface_CPP

#include "material/nD/Template3Dep/DruckerPragerYieldSurface.h"
#include "material/nD/Template3Dep/EPState.h"


//================================================================================
// Normal constructor
//================================================================================

//XC::DruckerPragerYieldSurface::DruckerPragerYieldSurface( double a1d = 0.5) : alfa1(a1d) { } 

//================================================================================
// Copy constrstructor
//================================================================================
//
//XC::DruckerPragerYieldSurface::DruckerPragerYieldSurface(const DruckerPragerYieldSurface &) {
//
//}

//! @brief Virtual constructor.
XC::YieldSurface * XC::DruckerPragerYieldSurface::getCopy(void)
  {
     YieldSurface  *newYieldSurface = new DruckerPragerYieldSurface();
     return newYieldSurface;
  }

//================================================================================
//  Yield criterion evaluation function F(EPState)
//  f = a*I1 + 0.5*sqrt(2.0)*||s_ij - p*alpha_ij|| - k =0
//================================================================================

double XC::DruckerPragerYieldSurface::f(const XC::EPState *EPS) const {
    double temp1 = EPS->getStress().Iinvariant1();
    double temp2 = temp1 * EPS->getScalarVar(1);

    XC::stresstensor alpha;
    XC::stresstensor s_bar;
    XC::stresstensor sigma = EPS->getStress();
    double p = sigma.p_hydrostatic();
    XC::stresstensor sdev = sigma.deviator();
    double halfRt2 = 0.5 * sqrt(2.0);
    int nod = EPS->getNTensorVar();
    if ( nod >=1 )  { //May not have kinematic hardening
      alpha = EPS->getTensorVar(1);   
      s_bar = sdev - (alpha*p);
    }
    else {
	  s_bar = sdev;
    }  
    XC::stresstensor temp3 = s_bar("ij") * s_bar("ij");
    temp3.null_indices();
    double temp4 = temp3.trace();
    temp4 = sqrt(temp4);
    double temp5 = halfRt2 * temp4;  
    
    double k = EPS->getScalarVar(2);
    
    double f   = temp2 + temp5 - k;

    return f;
}

//================================================================================
// BJtensor dF/dsigma_ij
//================================================================================

XC::BJtensor XC::DruckerPragerYieldSurface::dFods(const XC::EPState *EPS) const {
    XC::stresstensor dFods;
    
	BJtensor KroneckerI("I", 2, def_dim_2);
    double temp1 =  EPS->getScalarVar(1);    
    
    XC::stresstensor alpha;
    XC::stresstensor s_bar;
    XC::stresstensor Tnsr0;
    double temp6 = 0.0;
    XC::stresstensor temp5;
    XC::stresstensor sigma = EPS->getStress();   
    double p = sigma.p_hydrostatic();
    XC::stresstensor sdev = sigma.deviator();
    double halfRt2 = 0.5 * sqrt(2.0);
    int nod = EPS->getNTensorVar();
    if ( nod >=1 )  { //May not have kinematic hardening
      alpha = EPS->getTensorVar(1);  
      s_bar = sdev - (alpha*p);
      temp5 = alpha("ij") * s_bar("ij");
      temp5.null_indices();
      temp6 = temp5.trace();
      Tnsr0 = KroneckerI*(temp6/3.0);
    }
    else {
	  s_bar = sdev;
    }
    XC::stresstensor temp3 = s_bar("ij") * s_bar("ij");
    temp3.null_indices();
    double temp4 = temp3.trace();
    temp4 = sqrt(temp4);
    Tnsr0 += s_bar;
    double eps = pow( d_macheps(), 0.5 );
    if ( fabs(temp4) > eps )  {
      Tnsr0 = Tnsr0 * (halfRt2/temp4);
    }
        	    
    dFods = (KroneckerI*temp1) + Tnsr0; 
        
    return dFods;
}

//================================================================================
// double xi_s1 = dF/dS1 = dF/dalfa1 = I1  Derivative in terms of first scalar var 
//================================================================================

double XC::DruckerPragerYieldSurface::xi_s1( const XC::EPState *EPS ) const {

    double temp = EPS->getStress().Iinvariant1();
    return temp;

}

//================================================================================
// double xi_s2 = dF/dS2 = dF/k = -1.0  Derivative in terms of second scalar var 
//================================================================================

double XC::DruckerPragerYieldSurface::xi_s2( const XC::EPState *EPS ) const {

    return -1.0;

}


//================================================================================
// double xi_t1 = dF/dt1 = dF/dalpha 
//================================================================================

XC::BJtensor XC::DruckerPragerYieldSurface::xi_t1( const XC::EPState *EPS) const {
    XC::stresstensor xi;
    
    XC::stresstensor alpha;
    XC::stresstensor sigma = EPS->getStress();
    double p = sigma.p_hydrostatic();
    double halfRt2 = 0.5 * sqrt(2.0);
    int nod = EPS->getNTensorVar();
    if ( nod >=1 )  { //May not have kinematic hardening
      alpha = EPS->getTensorVar(1); 
    }
    XC::stresstensor sigma_bar = sigma - alpha*p;   
    XC::stresstensor s_bar = sigma_bar.deviator();
    XC::stresstensor temp3 = s_bar("ij") * s_bar("ij");
    temp3.null_indices();
    double temp4 = temp3.trace();
    temp4 = sqrt(temp4);
    double eps = pow( d_macheps(), 0.5 );
    if ( fabs(temp4) > eps )  {
      xi = s_bar * (-halfRt2*p/temp4); //Note the Negative 1 here
    }
        
    return xi;
}

//===========================================================================
std::ostream& XC::operator<<(std::ostream &os, const XC::DruckerPragerYieldSurface &YS)
  {
   os << "Drucker-Prager Yield Surface Parameters: " << std::endl;
   os.precision(4);
   //os << "alfa1 = " << YS.getalfa1() << std::endl;
   return os;
  }

#endif
