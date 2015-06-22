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
//# PURPOSE:           Von Mises         potential criterion                     #
//# CLASS:             VMpotentialSurface                                        #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                               #
//#                                                                              #
//#                                                                              #
//# DATE:              August 03 '93                                             #
//# UPDATE HISTORY:    September 01 '00                                          #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//# SHORT EXPLANATION: Von Mises potential surface                               #
//#                                                                              #
//#                                                                              #
//################################################################################
//*/

#ifndef VM_PS_H
#define VM_PS_H


#include "material/nD/Template3Dep/PS.h"


namespace XC {
//! @ingroup MatPS
//
//! @brief Von Mises potential surface.
class VMPotentialSurface : public PotentialSurface
{
  // Private vars to define the Von Mises Potential Surface
  private:		  
  
  public:
    PotentialSurface *newObj();  //create a colne of itself
    VMPotentialSurface ( ) {}      // Default constructor
    //VMPotentialSurface (const VMPotentialSurface & );   // copy constructor

    BJtensor dQods(const EPState *EPS) const;
    BJtensor d2Qods2(const EPState *EPS) const;
       
    BJtensor d2Qodsdt1(const EPState *EPS) const; // For Consistent Algorithm, Z Cheng, Jan 2004
    
    void print() { std::cerr << *this; }; 
  
    //================================================================================
    // Overloaded Insertion Operator
    // prints an VM PotentialSurface's contents 
    //================================================================================
    friend std::ostream& operator<< (std::ostream& os, const VMPotentialSurface & YS);

};
} // fin namespace XC

#endif

