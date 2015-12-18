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
//
//================================================================================
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite Element Program                    #
//# PURPOSE:           Manzari-Dafalias potential criterion 01 (with Pc)         #
//# CLASS:             MDPotentialSurface01                                      #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                               #
//#                                                                              #
//#                                                                              #
//# DATE:              August 08 '00                                             #
//# UPDATE HISTORY:    December 13 '00                                           #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//# SHORT EXPLANATION:                                                           #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//================================================================================
//

#ifndef MD_PS01_H    
#define MD_PS01_H

#include "material/nD/Template3Dep/PS.h"

namespace XC {
//! @ingroup MatPS
//
//! @brief ??.
class MDPotentialSurface01 : public PotentialSurface
{
  private:
    double Pc;

  public:
    PotentialSurface *newObj();  //create a colne of itself
    MDPotentialSurface01(double pc);        // Default constructor

    BJtensor dQods(const EPState *EPS) const; 
    BJtensor d2Qods2(const EPState *EPS) const ;   

    //aux. functions for d2Qods2
    BJtensor dnods(const EPState *EPS) const;
    BJtensor dthetaoverds(const EPState *EPS) const;
    double dgoverdt(double theta, double c) const;
    BJtensor apqdnods(const EPState *EPS) const;
    
    void print() { std::cerr << *this; };

    //================================================================================
    // Overloaded Insertion Operator
    // prints an PotentialSurface's contents 
    //================================================================================
    friend std::ostream& operator<< (std::ostream& os, const MDPotentialSurface01 &PS);

};
} // end of XC namespace

#endif

