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
//# PURPOSE:           Von Mises  yield criterion                                #
//# CLASS:             VMYieldSurface                                            #
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

#ifndef VM_YS_CPP
#define VM_YS_CPP

#include "material/nD/Template3Dep/VM_YS.h"
#include "material/nD/Template3Dep/EPState.h"

//================================================================================
// Copy constructor
//================================================================================
//
//XC::VMYieldSurface::VMYieldSurface(const VMYieldSurface &VMYS ) {
//
//}

//================================================================================
//create a colne of itself
//================================================================================

XC::YieldSurface * XC::VMYieldSurface::newObj()
  {  

     YieldSurface  *new_YS = new VMYieldSurface();
     return new_YS;

  }

//================================================================================
//  Yield criterion evaluation function f = 3/2 *Sij_bar Sij_bar- k^2
//  c.f. pp.274 XC::W.F.Chen Plasticity for Structure Engineers
//================================================================================

double XC::VMYieldSurface::f(const XC::EPState *EPS) const {
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

 XC::BJtensor XC::VMYieldSurface::dFods(const XC::EPState *EPS) const {

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

double XC::VMYieldSurface::xi_s1(const XC::EPState *EPS) const {

    double k = EPS->getScalarVar( 1 );
    
    return 2.0 * k;
}


//================================================================================
// BJtensor xi_k1=dF/d_alpha_ij=-3.0*S_bar_pq  Derivative in terms of 1st XC::BJtensor var
//================================================================================

 XC::BJtensor XC::VMYieldSurface::xi_t1(const XC::EPState *EPS) const {

    XC::stresstensor sigma = EPS->getStress();
    XC::stresstensor alpha = EPS->getTensorVar(1);

    XC::stresstensor sigma_bar = sigma - alpha;   
    XC::stresstensor s_bar = sigma_bar.deviator();
    BJtensor xi = s_bar * (-3.0);
    
    return xi;
}

//================================================================================
std::ostream& XC::operator<< (std::ostream& os, const VMYieldSurface & YS)
  {
    os << "Von Mises Yield Surface Parameters: " << std::endl;
    return os;
  }



#endif

