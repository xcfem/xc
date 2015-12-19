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
//# PROJECT:           Object Oriented Finite Element Program                    #
//# PURPOSE:           Von Mises         yield criterion                         #
//# CLASS:             VMYieldSurface                                            #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                               #
//#                                                                              #
//#                                                                              #
//# DATE:              August 03 '93                                             #
//# UPDATE HISTORY:    August 08 '00                                             #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//# SHORT EXPLANATION: Von Mises yield surface                                   #
//#                                                                              #
//#                                                                              #
//################################################################################
//*/

#ifndef VM_YS_H
#define VM_YS_H

#include "material/nD/Template3Dep/YS.h"


namespace XC {
//! @ingroup MatNDYS
//
//! @brief Von Mises yield surface.
class VMYieldSurface : public YieldSurface
  {
  public:
    // Create a colne of itself
    YieldSurface *newObj();  

    // Default constructor
    VMYieldSurface ( ) {}     

    // Copy constructor
    //VMYieldSurface (const VMYieldSurface & );   

    // Evaluation of f
    double f(const EPState *EPS) const;

    //First derivative of F over sigma
    BJtensor dFods(const EPState *EPS) const;

    // Redefine 1st derivative of F over first scalar internal variable
    double xi_s1(const EPState *EPS) const;   

    // Redefine 1st derivative of F over first tensorial internal variable
    BJtensor xi_t1(const EPState *EPS) const;

    void print() { std::cerr << *this; }; 
  
    //================================================================================
    // Overloaded Insertion Operator
    // prints an VM YieldSurface's contents 
    //================================================================================
    friend std::ostream& operator<< (std::ostream& os, const VMYieldSurface & YS);
  };
} // end of XC namespace

#endif

