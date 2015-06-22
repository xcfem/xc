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
//================================================================================
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite XC::Element Program                    #
//# PURPOSE:           Drucker - Prager  yield criterion                         #
//# CLASS:             DPYieldSurface                                            #
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

#ifndef DP_YS_CPP
#define DP_YS_CPP

#include "material/nD/Template3Dep/DP_YS.h"
#include "material/nD/Template3Dep/EPState.h"


//================================================================================
// Normal constructor
//================================================================================

//XC::DPYieldSurface::DPYieldSurface( double a1d = 0.5) : alfa1(a1d) { } 

//================================================================================
// Copy constrstructor
//================================================================================
//
//XC::DPYieldSurface::DPYieldSurface(const DPYieldSurface &DPYS ) {
//
//}

//================================================================================
//create a colne of itself
//================================================================================

//alpha machines complains on this
//DPYieldSurface * XC::DPYieldSurface::newObj() {  

XC::YieldSurface * XC::DPYieldSurface::newObj() {  

     YieldSurface  *new_YS = new DPYieldSurface();
     return new_YS;

}

//================================================================================
//  Yield criterion evaluation function F(EPState)
//  f = a*I1 + 0.5*sqrt(2.0)*||s_ij - p*alpha_ij|| - k =0
//================================================================================

double XC::DPYieldSurface::f(const XC::EPState *EPS) const {
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

XC::BJtensor XC::DPYieldSurface::dFods(const XC::EPState *EPS) const {
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

double XC::DPYieldSurface::xi_s1( const XC::EPState *EPS ) const {

    double temp = EPS->getStress().Iinvariant1();
    return temp;

}

//================================================================================
// double xi_s2 = dF/dS2 = dF/k = -1.0  Derivative in terms of second scalar var 
//================================================================================

double XC::DPYieldSurface::xi_s2( const XC::EPState *EPS ) const {

    return -1.0;

}


//================================================================================
// double xi_t1 = dF/dt1 = dF/dalpha 
//================================================================================

XC::BJtensor XC::DPYieldSurface::xi_t1( const XC::EPState *EPS) const {
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

//================================================================================
std::ostream& XC::operator<<(std::ostream& os, const XC::DPYieldSurface & YS)
  {
   os << "Drucker-Prager Yield Surface Parameters: " << std::endl;
   os.precision(4);
   //os << "alfa1 = " << YS.getalfa1() << std::endl;
   return os;
  }

#endif
