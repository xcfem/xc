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
# CLASS:             EvolutionLaw_T                                                #
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

#ifndef EL_T_CPP
#define EL_T_CPP

#include "material/nD/Template3Dep/EL_T.h"
#include <utility/matrix/nDarray/basics.h>
#include <utility/matrix/nDarray/BJtensor.h>
#include <utility/matrix/nDarray/stresst.h>

// #include <nDarray/straint.h>

// #include "material/nD/Template3Dep/EPState.h"
// #include "material/nD/Template3Dep/PS.h"

//================================================================================
//  Create a clone of itself 
//================================================================================
XC::EvolutionLaw_T * XC::EvolutionLaw_T::newObj() {
    
    EvolutionLaw_T *newEL = new EvolutionLaw_T( *this );
    
    return newEL;

}

//================================================================================
// Evaluating h_ ( for the evaluation of Kp )
//================================================================================

XC::BJtensor XC::EvolutionLaw_T::h_t( EPState *EPS, PotentialSurface *PS)
{
    // Return zero valued XC::BJtensor
    XC::stresstensor temp;
    return temp;
}

//================================================================================
// updating E, e D and m for Manzari-Dafalias model
//================================================================================

int XC::EvolutionLaw_T::updateEeDm( EPState *EPS, double st_vol, double dLamda)
{
   // do nothing
   return 0;
}  



//================================================================================
//  Print vars defined in XC::Linear Evolution Law_T
//================================================================================
void XC::EvolutionLaw_T::print()
  { std::cerr << (*this); }

//================================================================================
// Overloaded Insertion Operator
// prints base Evolution Law_T's contents 
//================================================================================
std::ostream& XC::operator<<(std::ostream& os, const XC::EvolutionLaw_T & EL)
  {
    os << "Base of Tensorial Evolution Law's Parameters: Nothing" << std::endl;
   return os;
  }




#endif

