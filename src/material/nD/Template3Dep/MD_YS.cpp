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

//================================================================================
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite XC::Element Program                    #
//# PURPOSE:           Mazari-Dafalias  yield criterion                          #
//#                      (Ref. Geotechnique                    			 #
//#                      V.47 No.2 255-272, 1997)                                #
//# CLASS:             MDYieldSurface01                                          #
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


#ifndef MD_YS_CPP
#define MD_YS_CPP

#include "material/nD/Template3Dep/MD_YS.h"
#include <utility/matrix/nDarray/basics.h>
#include "material/nD/Template3Dep/EPState.h"

#define sqrt23rd 0.8165

//================================================================================
// Normal constructor
//================================================================================

XC::MDYieldSurface::MDYieldSurface( )  { } 


//================================================================================
//create a colne of itself
//================================================================================

XC::YieldSurface * XC::MDYieldSurface::newObj() {  

     YieldSurface  *new_YS = new MDYieldSurface();
     return new_YS;

}

//================================================================================
// Copy constrstructor
//================================================================================
//
//XC::MDYieldSurface::MDYieldSurface(MDYieldSurface &MDYS ) { }
//


//================================================================================
//  Yield criterion evaluation function F(EPState)
//================================================================================

double XC::MDYieldSurface::f(const XC::EPState *EPS) const
{
  XC::stresstensor S = EPS->getStress().deviator();

  double p =EPS->getStress().p_hydrostatic();

  // Get a_ij
  XC::stresstensor alpha = EPS->getTensorVar( 1 );
  
  double m = EPS->getScalarVar(1);

  XC::stresstensor r = S - alpha*p;
  r.null_indices();
  XC::stresstensor r2 = r("ij") * r("ij");  
  
  XC::stresstensor n;

  double norm = sqrt( r2.trace() );

  ////Added on Feb. 10, 2003 by Joey
  //if ( norm >= d_macheps() ){ 
  //  n = r *(1.0 / norm );
  //}
  //else {
  //  g3ErrorHandler->fatal("XC::MDYieldSurface::f  |n_ij| = 0, divide by zero! Program exits.");
  //  exit(-1);
  //}
  //EPS->setTensorVar( 2, n); //update n_ij//
  

  double ff = norm - sqrt23rd * m * p;        
  //printf("\n========Inside f  temp3 = %.4f x = %.4f\n ", temp3, x);

  return ff;
}


//================================================================================
// BJtensor dF/dsigma_ij
//================================================================================

XC::BJtensor XC::MDYieldSurface::dFods(const XC::EPState *EPS) const {
  
  BJtensor dFoverds( 2, def_dim_2, 0.0);
  BJtensor I2("I", 2, def_dim_2);

  XC::stresstensor S = EPS->getStress().deviator();
  //S.reportshort("S");

  double m = EPS->getScalarVar( 1 );

  //double p = EPS->getStress().p_hydrostatic();
  //printf("Here we go!  p %f\n", p);
  	    
  XC::stresstensor alpha = EPS->getTensorVar( 1 ); // getting alpha_ij from XC::EPState
  //alpha.reportshort("alpha");
  
  //XC::stresstensor n = EPS->getTensorVar( 3 );     // getting n_ij from XC::EPState
  //n.reportshort("n");
  
  // might be moved to Evolution Law
    XC::stresstensor n = EPS->getTensorVar( 2 );
  //-------------------------------------------------
  //  //XC::stresstensor n;
  //  XC::stresstensor r = S * (1.0 / p);
  //  //r.reportshort("r");
  //  XC::stresstensor r_bar = r - alpha;
  //  //r_bar.reportshort("r_bar"); 
  //
  //  XC::stresstensor norm2 = r_bar("ij") * r_bar("ij");
  //  double norm = sqrt( norm2.trace() );    
  //  if ( norm >= d_macheps() ){ 
  //    n = r_bar *(1.0 / norm );
  //  }
  //  else {
  //    g3ErrorHandler->fatal("XC::MDYieldSurface::dFods  |n_ij| = 0, divide by zero! Program exits.");
  //    exit(-1);
  //  }
  //EPS->setTensorVar( 3, n); //update n_ij//
  ////
  ////  //n = r_bar *(1.0 / sqrt23rd / m );
  ////
  //////-------------------------------------------------
  


  
  //tensorial multiplication produces 1st-order XC::BJtensor
  //BJtensor temp = n("ij") * n("ij");
  //double temp1 = temp.trace();
  //printf("==== n_ij*n_ij %e\n", temp1);

  //!!Very important:  N = n_pq * alpha_pq +sqrt(2/3)*m (always) = n_pq*r_pq(not hold when not on the yield surface)
  BJtensor temp = alpha("ij") * n("ij");
  double N = temp.trace() + sqrt23rd*m; 
  //double N = temp.trace() + 2.0/3.0*m; 
  //printf("    N =  %e\n", N);

  dFoverds =  n - I2 * N*(1.0/3.0);

  return dFoverds;

}


//================================================================================
// double xi_s1 = dF/dm = -(2/3)^0.5 p  Derivative in terms of first scalar var 
//================================================================================

double XC::MDYieldSurface::xi_s1( const XC::EPState *EPS ) const {

    double p = EPS->getStress().p_hydrostatic();
    return -1.0*sqrt23rd * p;

}

//================================================================================
// double xi_t1 = dF/dt1 = dF/dalpha= -p*n  Derivative in terms of first tensorial var 
//================================================================================

XC::BJtensor XC::MDYieldSurface::xi_t1( const XC::EPState *EPS) const {
  BJtensor dFoverds( 2, def_dim_2, 0.0);
  BJtensor I2("I", 2, def_dim_2);

  XC::stresstensor S = EPS->getStress().deviator();

  double p = EPS->getStress().p_hydrostatic();
 
  XC::stresstensor n = EPS->getTensorVar( 2 );
  //XC::stresstensor n;
  	    
  ////-------------------------------------------------
  //double m = EPS->getScalarVar( 1 );
  //XC::stresstensor alpha = EPS->getTensorVar( 1 ); // getting alpha_ij from XC::EPState  
  //XC::stresstensor r = S * (1.0 / p);
  //XC::stresstensor r_bar = r - alpha;
  //XC::stresstensor norm2 = r_bar("ij") * r_bar("ij");
  //double norm = sqrt( norm2.trace() );
  //
  //if ( norm >= d_macheps() ){ 
  //  n = r_bar *(1.0 / norm );
  //}
  //else {
  //  std::cerr->fatal("XC::MDYieldSurface::dFods  |n_ij| = 0, divide by zero! Program exits.");
  //  exit(-1);
  //}
  //n = r_bar *(1.0 / sqrt23rd / m );
  ////-------------------------------------------------
    
  return  n  * (-1.0)* p;
}


std::ostream& XC::operator<<(std::ostream& os, const MDYieldSurface & YS)
  {
    os << "Manzari-Dafalias Yield Surface Parameters: " << std::endl;
    return os;
  }

#endif

