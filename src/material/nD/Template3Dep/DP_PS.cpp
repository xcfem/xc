//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
///*
//################################################################################
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite XC::Element Program                    #
//# PURPOSE:           Drucker - Prager  yield criterion                         #
//# CLASS:             DPPotentialSurface                                        #
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

#ifndef DP_PS_CPP
#define DP_PS_CPP

#include "material/nD/Template3Dep/DP_PS.h"
#include <utility/matrix/nDarray/BJtensor.h>
#include "material/nD/Template3Dep/EPState.h"



//================================================================================
// Normal constructor
//================================================================================

//XC::DPPotentialSurface::DPPotentialSurface( double a2d = 0.3) : alfa2(a2d) 
//{
//
//} 


//================================================================================
// Copy constrstructor  === not necessary for no pointer member
//================================================================================

XC::DPPotentialSurface::DPPotentialSurface(const DPPotentialSurface &DPPS ) {

    alfa2 =  DPPS.getalfa2();

}


//================================================================================
//create a colne of itself
//================================================================================

XC::PotentialSurface * XC::DPPotentialSurface::newObj() {  

     PotentialSurface  *new_PS = new DPPotentialSurface( this->getalfa2() );
     return new_PS;

}



//================================================================================
// BJtensor dQ/dsigma_ij
//================================================================================

XC::BJtensor XC::DPPotentialSurface::dQods(const XC::EPState *EPS) const {
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

XC::BJtensor XC::DPPotentialSurface::d2Qods2(const XC::EPState *EPS) const {
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
XC::BJtensor XC::DPPotentialSurface::d2Qodsds1(const XC::EPState *EPS) const 
{  
  BJtensor I("I", 2, def_dim_2);
  BJtensor d2Qoverdsds1 = I;
  return d2Qoverdsds1;
}

//================================================================================

double XC::DPPotentialSurface::getalfa2() const {

    return alfa2; 

}

//================================================================================
std::ostream& XC::operator<< (std::ostream& os, const XC::DPPotentialSurface &PS)
  {
     os << "Drucker-Prager Potential Surface Parameters: " << std::endl;
     os << "alfa2 = " << PS.getalfa2() << std::endl;
     return os;
  }


#endif
