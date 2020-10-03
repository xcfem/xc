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
//################################################################################
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite Element Program                    #
//# PURPOSE:           Drucker - Prager  yield criterion                         #
//# CLASS:             DruckerPragerPotentialSurface                                        #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# PROGRAMMER(S):     Boris Jeremic, ZHaohui Yang                               #
//#                                                                              #
//#                                                                              #
//# DATE:              August 03 '93                                             #
//# UPDATE HISTORY:    August 08 '00                                             #
//# UPDATE HISTORY:    20Aug2004 ZC added kinematic hardening part               #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//################################################################################
//*/

#ifndef DruckerPragerPotentialSurface_CPP
#define DruckerPragerPotentialSurface_CPP

#include "material/nD/Template3Dep/DruckerPragerPotentialSurface.h"
#include <utility/matrix/nDarray/BJtensor.h>
#include "material/nD/Template3Dep/EPState.h"



//================================================================================
// Normal constructor
//================================================================================

//XC::DruckerPragerPotentialSurface::DruckerPragerPotentialSurface( double a2d = 0.3) : alfa2(a2d) 
//{
//
//} 


//================================================================================
// Copy constrstructor  === not necessary for no pointer member
//================================================================================

XC::DruckerPragerPotentialSurface::DruckerPragerPotentialSurface(const DruckerPragerPotentialSurface &DruckerPragerPS ) {

    alfa2 =  DruckerPragerPS.getalfa2();

}


//! @brief Virtual constructor.
XC::PotentialSurface * XC::DruckerPragerPotentialSurface::getCopy(void)
  {  return new DruckerPragerPotentialSurface(*this); }



//================================================================================
// BJtensor dQ/dsigma_ij
//================================================================================

XC::BJtensor XC::DruckerPragerPotentialSurface::dQods(const XC::EPState *EPS) const {
    XC::stresstensor dQods;
    
	BJtensor KroneckerI("I", 2, def_dim_2);
    //double temp1 =  EPS->getScalarVar(1);    
    double temp1 = getalfa2();    
    
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
    	    
    dQods = (KroneckerI * temp1) + Tnsr0;; 
        
    return dQods;
}


//================================================================================
// BJtensor d2Q/dsigma_ij_2
//================================================================================

XC::BJtensor XC::DruckerPragerPotentialSurface::d2Qods2(const XC::EPState *EPS) const {
    BJtensor d2Qods2(4, def_dim_4, 0.0);
    
	BJtensor KroneckerI("I", 2, def_dim_2);
	BJtensor T1 = KroneckerI("ij")*KroneckerI("mn");
	T1.null_indices();
	BJtensor T2 = (T1.transpose0110()+T1.transpose0111())*0.5;
	BJtensor T3 = T2 - T1*(1.0/3.0);
	
    //double temp1 =  EPS->getScalarVar(1);    
    //double temp1 = getalfa2();
    
    BJtensor T4;
    XC::stresstensor alpha;
    XC::stresstensor s_bar;
    BJtensor temp9(4, def_dim_4, 0.0);
    XC::stresstensor sigma = EPS->getStress();
    double p = sigma.p_hydrostatic();
    XC::stresstensor sdev = sigma.deviator();
    double halfRt2 = 0.5 * sqrt(2.0);
    int nod = EPS->getNTensorVar();
    if ( nod >=1 )  { //May not have kinematic hardening
      alpha = EPS->getTensorVar(1);
      temp9 = KroneckerI("ij") * alpha("mn");
      temp9.null_indices();
      T4 = T2 + temp9*(1.0/3.0);
      s_bar = sdev - (alpha*p);
    }
    else {
	  s_bar = sdev;
	  T4 = T2;
    }
    T4 = T2 - temp9;
    BJtensor temp3 = s_bar("ij") * s_bar("ij");
    temp3.null_indices();  
    double temp4 = temp3.trace();
    temp4 = sqrt(temp4);
    BJtensor temp5 = s_bar("ij")*s_bar("mn");
    double eps = pow( d_macheps(), 0.5 );
    if ( fabs(temp4) > eps )  {
      d2Qods2 = T3 * (halfRt2/temp4) - temp5*(halfRt2/(temp4*temp4*temp4));
      d2Qods2 = T4("ijkl") * d2Qods2("klmn");
      d2Qods2.null_indices();
    }   
    
    return d2Qods2;
}

// For Consistent Algorithm, Z Cheng, Jan 2004
XC::BJtensor XC::DruckerPragerPotentialSurface::d2Qodsds1(const XC::EPState *EPS) const 
{  
  BJtensor I("I", 2, def_dim_2);
  BJtensor d2Qoverdsds1 = I;
  return d2Qoverdsds1;
}

//================================================================================

double XC::DruckerPragerPotentialSurface::getalfa2() const {

    return alfa2; 

}

//================================================================================
std::ostream& XC::operator<<(std::ostream &os, const XC::DruckerPragerPotentialSurface &PS)
  {
     os << "Drucker-Prager Potential Surface Parameters: " << std::endl;
     os << "alfa2 = " << PS.getalfa2() << std::endl;
     return os;
  }


#endif
