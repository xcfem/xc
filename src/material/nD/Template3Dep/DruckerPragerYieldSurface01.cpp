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

//================================================================================
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite XC::Element Program                    #
//# PURPOSE:           Drucker-Prager yield criterion 01 (with Pc)	         #
//#                      (Ref. Geotechnique                    			 #
//#                      V.47 No.2 255-272, 1997)                                #
//# CLASS:             DruckerPragerYieldSurface01                                          #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# PROGRAMMER(S):     Boris Jeremic, ZHaohui Yang                               #
//#                                                                              #
//#                                                                              #
//# DATE:              August 03 '00                                             #
//# UPDATE HISTORY:    December 13, '00                                             #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//================================================================================


#ifndef DruckerPragerYieldSurface01_CPP
#define DruckerPragerYieldSurface01_CPP

#include "material/nD/Template3Dep/DruckerPragerYieldSurface01.h"
#include <utility/matrix/nDarray/basics.h>
#include "material/nD/Template3Dep/EPState.h"

//================================================================================
// Normal constructor
//================================================================================

XC::DruckerPragerYieldSurface01::DruckerPragerYieldSurface01(double pc)  
{ 
     Pc = pc;
} 


//================================================================================
//create a colne of itself
//================================================================================

XC::YieldSurface * XC::DruckerPragerYieldSurface01::newObj() {  

     YieldSurface  *newYieldSurface = new DruckerPragerYieldSurface01( Pc );
     return newYieldSurface;

}

//================================================================================
// Copy constrstructor
//================================================================================
//
//XC::DruckerPragerYieldSurface01::DruckerPragerYieldSurface01(DruckerPragerYieldSurface01 &) { }
//


//================================================================================
//  Yield criterion evaluation function F(EPState)
//================================================================================

double XC::DruckerPragerYieldSurface01::f(const XC::EPState *EPS) const
  {
  XC::stresstensor S = EPS->getStress().deviator();
  //S.print("sigma", "S");

  double p =EPS->getStress().p_hydrostatic();
  p = p - Pc;
  std::cerr << "p " << p;

  XC::stresstensor alpha = EPS->getTensorVar( 1 );
  //alpha.print("alpha", " alpha");
  //std::cerr << "alpha " << alpha;
  
  double m = EPS->getScalarVar(1);

  //XC::stresstensor temp1 = S - p*alpha;
  XC::stresstensor temp1 = S - p*alpha; // S +p * alpha
  temp1.null_indices();
  //temp1.print("temp1 ", " temp1");

  XC::stresstensor temp2 = temp1("ij") * temp1("ij");  

  double temp3 = sqrt( temp2.trace() );

  temp3 = temp3 - sqrt(2.0/3.0) * m * p;        
  //printf("\n========Inside f  temp3 = %.4f x = %.4f\n ", temp3, x);

  return temp3;
}


//================================================================================
// BJtensor dF/dsigma_ij
//================================================================================

XC::BJtensor XC::DruckerPragerYieldSurface01::dFods(const XC::EPState *EPS) const {
  
  BJtensor dFoverds( 2, def_dim_2, 0.0);
  BJtensor I2("I", 2, def_dim_2);

  XC::stresstensor S = EPS->getStress().deviator();
  //S.reportshort("S");

  double p = EPS->getStress().p_hydrostatic();
  p = p - Pc;
  //printf("Here we go!  p %f\n", p);
  	    
  XC::stresstensor alpha = EPS->getTensorVar( 1 ); // getting alpha_ij from XC::EPState
  //alpha.reportshort("alpha");
  
  //XC::stresstensor n = EPS->getTensorVar( 3 );     // getting n_ij from XC::EPState
  //n.reportshort("n");
  
  //-------------------------------------------------
  // might be moved to Evolution Law
    XC::stresstensor r = S * (1.0 / p);
    //r.reportshort("r");
    XC::stresstensor r_bar = r - alpha;
    //r_bar.reportshort("r_bar"); 
    XC::stresstensor norm2 = r_bar("ij") * r_bar("ij");
    double norm = sqrt( norm2.trace() );
    
    //std::cerr << "d_macheps " << d_macheps() << std::endl;

    XC::stresstensor n;
    if ( norm >= d_macheps() ){ 
      n =  r_bar*(1.0 / norm );
    }
    else {
      std::cerr << "XC::DruckerPragerYieldSurface01::dFods  |n_ij| = 0, divide by zero! Program exits.\n";
      exit(-1);
    }
    //EPS->setTensorVar( 3, n); //update n_ij//
  //-------------------------------------------------


  double m = EPS->getScalarVar( 1 );

  
  //tensorial multiplication produces 1st-order XC::BJtensor
  //BJtensor temp = n("ij") * n("ij");
  //double temp1 = temp.trace();
  //printf("==== n_ij*n_ij %e\n", temp1);

  //!!Very important:  N = n_pq * alpha_pq +sqrt(2/3)*m (always) = n_pq*r_pq(not hold when not on the yield surface)
  BJtensor temp = n("ij") * alpha("ij");
  double N = temp.trace() + sqrt(2.0/3.0)*m; 
  //printf("    N =  %e\n", N);

  dFoverds =  n - N *I2 *(1.0/3.0);

  return dFoverds;

}


//================================================================================
// double xi_s1 = dF/dm = -(2/3)^0.5 p  Derivative in terms of first scalar var 
//================================================================================

double XC::DruckerPragerYieldSurface01::xi_s1( const XC::EPState *EPS ) const {

    double p = EPS->getStress().p_hydrostatic();
    //double p = EPS->getStress().Iinvariant1()/3.0;
    p = p - Pc;
    return -1.0*sqrt(2.0/3.0) * p;

}

//================================================================================
// double xi_t1 = dF/dt1 = dF/dalpha= -p*n  Derivative in terms of first tensorial var 
//================================================================================

XC::BJtensor XC::DruckerPragerYieldSurface01::xi_t1( const XC::EPState *EPS) const {
    BJtensor dFoverds( 2, def_dim_2, 0.0);
    BJtensor I2("I", 2, def_dim_2);

    XC::stresstensor S = EPS->getStress().deviator();
    
    double p = EPS->getStress().p_hydrostatic();
    p = p - Pc;
    	    
    XC::stresstensor alpha = EPS->getTensorVar( 1 ); // getting alpha_ij from XC::EPState
    
    XC::stresstensor r = S * (1.0 / p); //for p = sig_kk/3
    XC::stresstensor r_bar = r - alpha;
    XC::stresstensor norm2 = r_bar("ij") * r_bar("ij");
    double norm = sqrt( norm2.trace() );
    
    XC::stresstensor n;
    if ( norm >= d_macheps() ){ 
      n = r_bar *(1.0 / norm );
    }
    else {
      std::cerr << "XC::DruckerPragerYieldSurface01::dFods  |n_ij| = 0, divide by zero! Program exits.\n";
      exit(-1);
    }
      
    return (-1.0) * n * p;
  }

const double &XC::DruckerPragerYieldSurface01::getPc(void) const
  { return Pc; }

std::ostream& operator<<(std::ostream &os,const XC::DruckerPragerYieldSurface01 &YS)
{
   os << "Drucker-Prager Yield Surface 01 Parameters: " << std::endl;
   os << "Pc = " << YS.getPc() << std::endl;
   return os;
}

#endif

