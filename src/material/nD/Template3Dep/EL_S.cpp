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
#                                                                                #
# CLASS:             EvolutionLaw_S(base Evolution Law class for scalar var      #
#                                                                                #
# VERSION:                                                                       #
# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )    #
# TARGET OS:         DOS || UNIX || . . .                                        #
# DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                 #
# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                                 #
#                                                                                #
#                                                                                #
# DATE:              09-02-2000                                                  #
# UPDATE HISTORY:    09-12-2000                                                  #
#                                                                                #
#                                                                                #
#                                                                                #
# SHORT EXPLANATION: Here are some initial definitions of some virtual funs,     #
#                    some of which will be redefined in derived classes.         #
#                                                                                #
//================================================================================
*/

#ifndef EL_CPP
#define EL_CPP

#include "material/nD/Template3Dep/EL_S.h"
#include "utility/matrix/nDarray/basics.h"

// #include <nDarray/stresst.h>
// #include <nDarray/straint.h>
// #include <nDarray/BJtensor.h>

// #include "material/nD/Template3Dep/EPState.h"
// #include "material/nD/Template3Dep/PS.h"    

//================================================================================
//  Create a clone of itself 
//================================================================================
XC::EvolutionLaw_S * XC::EvolutionLaw_S::newObj() {
    
    EvolutionLaw_S *newEL = new EvolutionLaw_S( *this );
    
    return newEL;

}


//================================================================================
// Evaluating scalar hardening function h_s ( for the evaluation of Kp )
//================================================================================

double XC::EvolutionLaw_S::h_s( EPState *EPS, PotentialSurface *PS){

    return 0.0;

}


//================================================================================
//  Print content of the base class, might be overwritten! 
//================================================================================
void XC::EvolutionLaw_S::print()
  { std::cerr << *this; }

//================================================================================
// Overloaded Insertion Operator
// prints an Evolution Law_S's contents 
//================================================================================
std::ostream &XC::operator<< (std::ostream &os, const XC::EvolutionLaw_S & EL)
  {
    os << "Scalar Evolution Law's Parameters: " << std::endl;
    return os;
  }



#endif

