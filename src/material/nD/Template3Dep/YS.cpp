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
/*
//================================================================================
# COPYRIGHT (C):     :-))                                                        #
# PROJECT:           Object Oriented Finite XC::Element Program                      #
# PURPOSE:           General platform for elaso-plastic constitutive model       #
#                    implementation                                              #
# CLASS:             YS (the base class for all Yield Surface)                   #
#                                                                                #
# VERSION:                                                                       #
# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )    #
# TARGET OS:         DOS || UNIX || . . .                                        #
# DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                 #
# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                                 #
#                                                                                #
#                                                                                #
# DATE:              08-03-2000                                                  #
# UPDATE HISTORY:                                                                #
#                                                                                #
#                                                                                #
#                                                                                #
#                                                                                #
# SHORT EXPLANATION: This is the base class of all yield surface. Here defined   #
#                    are initial values of the first derivatives of dF/ds_i 	 #
#                    and dF/ot_i                                                 # 
#                                                                                #
//================================================================================
*/

#ifndef YS_CPP
#define YS_CPP

#include "material/nD/Template3Dep/YS.h"
#include "utility/matrix/nDarray/BJtensor.h"
#include "utility/matrix/nDarray/stresst.h"

//================================================================================
// First derivative of F over scalar internal variables 
// (at most 4 scalar internal vars allowed currently)
//================================================================================

//================================================================================
//First derivative of F over the 1st scalar var
//================================================================================

double XC::YieldSurface::xi_s1( const XC::EPState *EPS ) const
{ 
     return 0.0;
}

//================================================================================
//First derivative of F over the 2nd scalar var
//================================================================================

double XC::YieldSurface::xi_s2( const XC::EPState *EPS ) const
{ 
     return 0.0;
}

//================================================================================
//First derivative of F over the 3rd scalar var
//================================================================================

double XC::YieldSurface::xi_s3( const XC::EPState *EPS ) const
{ 
     return 0.0;
}

//================================================================================
//First derivative of F over the 4th scalar var
//================================================================================

double XC::YieldSurface::xi_s4( const XC::EPState *EPS ) const
{ 
     return 0.0;
}

//================================================================================
// First derivative of F over scalar internal variables 
// (at most 4 tensorial internal vars allowed currently)
//================================================================================

//================================================================================
//First derivative of F over the 1st scalar var
//================================================================================
XC::BJtensor XC::YieldSurface::xi_t1( const XC::EPState *EPS ) const
  { 
     XC::stresstensor temp;
     return temp;
  }
//================================================================================
//First derivative of F over the 2nd scalar var
//================================================================================
 XC::BJtensor XC::YieldSurface::xi_t2( const XC::EPState *EPS ) const
{ 
     XC::stresstensor temp;
//     clog << " inside XC::YieldSurface::xi_t2 "   <<temp;
     return temp;
}

//================================================================================
//First derivative of F over the 3rd scalar var
//================================================================================
 XC::BJtensor XC::YieldSurface::xi_t3( const XC::EPState *EPS ) const
{ 
     XC::stresstensor temp;
//     clog << "inside  XC::YieldSurface::xi_t3 " <<temp;
     return temp;
}

//================================================================================
//First derivative of F over the 4th scalar var
//================================================================================
 XC::BJtensor XC::YieldSurface::xi_t4( const XC::EPState *EPS ) const
{ 
     XC::stresstensor temp;
    // clog << " inside XC::YieldSurface::xi_t4 "  << temp;
     return temp;
}

//================================================================================
// friend XC::std::ostream functions for output
//================================================================================
std::ostream& XC::operator<<(std::ostream &os, const YieldSurface & YS)
  {
    os << "Yield Surface Parameters: " << std::endl;
    return os;
  }


#endif

