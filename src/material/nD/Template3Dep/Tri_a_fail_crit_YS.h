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

//################################################################################
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           OpenSees                                                  #
//# PURPOSE:           Triaxial Failure Criterion for Concrete - yield criterion #
//# CLASS:             TriFCYieldSurface                                         #
//#                                                                              #
//# VERSION:           1.0                                                       #
//# LANGUAGE:          C++ (ili tako nesto)                                      #
//# TARGET OS:                                                                   #
// DESIGNER(S):       Boris Jeremic and Zhaohui Yang [jeremic,zhyang]@ucdavis.edu|
// PROGRAMMER(S):     Vlado Vukadin                                              |
//#                                                                              #
//#                                                                              #
//# DATE:             June 01, 2002                                              #
//# UPDATE HISTORY:    bice tako dobr da nece biti potreban update :)            #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//# SHORT EXPLANATION:                                                           #
//#                                                                              #
//# Yield surface is based on article by Menetrey, P. and William, K.J.          #
//# published in 1995 in  ACI Structural Journal pp 311-318. Purpose of the     #
//# Yield surface is to model triaxial strenght of concrete                      #
//#                                                                              #
//#                                                                              #
//################################################################################
//*/

#ifndef Tri_a_fail_crit_YS_H
#define Tri_a_fail_crit_YS_H


#include "material/nD/Template3Dep/YS.h"

namespace XC {
//! @ingroup MatNDYS
//
//! @brief Yield surface is based on article by Menetrey, P. and William, K.J.
//! published in 1995 in  ACI Structural Journal pp 311-318. Purpose of the
//! Yield surface is to model triaxial strenght of concrete.
class TriFCYieldSurface : public YieldSurface
  {
  // Private vars to define the TriFCYieldSurface Yield Surface
  private:
     double fcomp;
     double ftens;
     double el; 
     double c;
  public:
    YieldSurface *newObj();  //create a clone of itself
    
    TriFCYieldSurface (double fc, double ft, double e, double coh );   // Default constructor

     

    double f(const EPState *EPS) const;
    BJtensor dFods(const EPState *EPS) const;

    // Redefine 1st derivative of F over scalar internal variables
    //double xi_s1( const EPState *EPS ) const;
    //double xi_s2( const EPState *EPS ) const;

    // Redefine 1st derivative of F over tensorial internal variables
    //tensor xi_t1(const EPState *EPS) const
    //{
    
    //}
    
    double getfcomp() const;
    double getftens() const;
    double getel() const;
    double get_c() const;

    void print() { std::cerr << *this; }; 
  
    //================================================================================
    // Overloaded Insertion Operator
    // prints an XX YieldSurface's contents 
    //================================================================================
    friend std::ostream& operator<< (std::ostream& os, const TriFCYieldSurface & YS);

};
} // end of XC namespace

#endif

