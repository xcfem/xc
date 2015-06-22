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
//# PROJECT:           Object Oriented Finite Element Program                    #
//# PURPOSE:           CAM CLAY yield criterion                                  #
//# CLASS:             CAMYieldSurface01(for monotonic loading)                  #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# PROGRAMMER(S):     Boris Jeremic, ZHaohui Yang                               #
//#                                                                              #
//#                                                                              #
//# DATE:              Mar. 28, 2001                                             #
//# UPDATE HISTORY:                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//# SHORT EXPLANATION:                                                           #
//#                                                                              #
//#                                                                              #
//================================================================================
//*/

#ifndef CAM_YS_H
#define CAM_YS_H

#include "material/nD/Template3Dep/YS.h"


namespace XC {
//! @ingroup MatNDYS
//
//! @brief ??.
class CAMYieldSurface : public YieldSurface
  {
  // Private vars to define the Mazari-Dafalias Yield Surface
  private:
    double M;	// the slope of critical state line
  public:
    YieldSurface *newObj();                  //create a colne of itself
    CAMYieldSurface(double Mp = 1.2);                          // Default constructor

    double f(const EPState *EPS) const;
    BJtensor dFods(const EPState *EPS) const;

    // Redefine 1st derivative of F over scalar internal variables
    double xi_s1( const EPState *EPS ) const;  // df/dm

    // Redefine 1st derivative of F over tensorial internal variables

    double getM() const;
    void print() { std::cerr << *this; };
    
// moved to stresstensor
//   private:
//     BJtensor dpoverds( ) const;
//     BJtensor dqoverds(const EPState *EPS) const;
//     BJtensor dthetaoverds(const EPState *EPS) const;
		         
    //================================================================================

  };

// Overloaded Insertion Operator
std::ostream &operator<<(std::ostream& os, const CAMYieldSurface &YS);

} // fin namespace XC

#endif

