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
# PURPOSE:           General platform for elasto-plastic constitutive model      #
#                    implementation                                              #
# CLASS:             PS (the base class for all Potential Surfaces)              #
#                                                                                #
# VERSION:                                                                       #
# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )    #
# TARGET OS:         DOS || UNIX || . . .                                        #
# DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                 #
# PROGRAMMER(S):     Boris Jeremic, Zhao Cheng                                   #
#                                                                                #
# Date:           Jan 2004                                                       #
# UPDATE HISTORY:                                                                #
#                                                                                #
#                                                                                #
#                                                                                #
#                                                                                #
# SHORT EXPLANATION: This is the base class of all Potential surfaces.           #
#                                                                                #
//================================================================================
*/

#ifndef PS_CPP
#define PS_CPP

#include "material/nD/Template3Dep/PS.h"
#include "utility/matrix/nDarray/BJtensor.h"

// At least 4 scalar and/or XC::BJtensor internal variables are allowed at current time

//================================================================================
// The d(dQ/dsigma_ij)/ds1
//================================================================================
XC::BJtensor XC::PotentialSurface::d2Qodsds1( const XC::EPState *EPS ) const
  { 
    BJtensor temp(2,def_dim_2,0.0);
    return temp;
  }

//================================================================================
// The d(dQ/dsigma_ij)/ds2
//================================================================================
XC::BJtensor XC::PotentialSurface::d2Qodsds2( const XC::EPState *EPS ) const
{ 
     BJtensor temp(2,def_dim_2,0.0);
     return temp;
}

//================================================================================
// The d(dQ/dsigma_ij)/ds3
//================================================================================
XC::BJtensor XC::PotentialSurface::d2Qodsds3( const XC::EPState *EPS ) const
{ 
     BJtensor temp(2,def_dim_2,0.0);
     return temp;
}

//================================================================================
// The d(dQ/dsigma_ij)/ds4
//================================================================================
XC::BJtensor XC::PotentialSurface::d2Qodsds4( const XC::EPState *EPS ) const
{ 
     BJtensor temp(2,def_dim_2,0.0);
     return temp;
}
          
//================================================================================
// The d(dQ/dsigma_ij)/dt1_mn
//================================================================================
XC::BJtensor XC::PotentialSurface::d2Qodsdt1( const XC::EPState *EPS ) const
{ 
     BJtensor temp(4,def_dim_4,0.0);
     return temp;
}

//================================================================================
// The d(dQ/dsigma_ij)/dt2_mn
//================================================================================
XC::BJtensor XC::PotentialSurface::d2Qodsdt2( const XC::EPState *EPS ) const
{ 
     BJtensor temp(4,def_dim_4,0.0);
     return temp;
}

//================================================================================
// The d(dQ/dsigma_ij)/dt3_mn
//================================================================================
XC::BJtensor XC::PotentialSurface::d2Qodsdt3( const XC::EPState *EPS ) const
{ 
     BJtensor temp(4,def_dim_4,0.0);
     return temp;
}

//================================================================================
// The d(dQ/dsigma_ij)/dt4_mn
//================================================================================
XC::BJtensor XC::PotentialSurface::d2Qodsdt4( const XC::EPState *EPS ) const
{ 
     BJtensor temp(4,def_dim_4,0.0);
     return temp;
}

#endif
