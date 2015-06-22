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
################################################################################
# COPYRIGHT (C):     :-))                                                      #
# PROJECT:           Object Oriented Finite Element Program                    #
# PURPOSE:           General platform for elaso-plastic constitutive model     #
#                    implementation                                            #
# CLASS:             YieldSurface (the base class for all yield surfaces)      #
#                                                                              #
# VERSION:                                                                     #
# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
# TARGET OS:         DOS || UNIX || . . .                                      #
# DESIGNER(S):       Boris Jeremic, Zhaohui Yang                               #
# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                               #
#                                                                              #
#                                                                              #
# DATE:              08-03-2000                                                #
# UPDATE HISTORY:                                                              #
#                                                                              #
#                                                                              #
#                                                                              #
#                                                                              #
# SHORT EXPLANATION: The goal is to create a platform for efficient and easy   #
#                    implemetation of any elasto-plastic constitutive model!   #
#                                                                              #
################################################################################*/

#ifndef YS_H
#define YS_H

#include <iostream>

namespace XC {
  class EPState;
  class BJtensor;

//! @ingroup 3DEPMat
//
//! @defgroup MatNDYS Yield surface.
//
//! @ingroup MatNDYS
//
//! @brief The goal is to create a platform for efficient and easy
//! implemetation of any elasto-plastic constitutive model.
class YieldSurface
  {
  public:
    YieldSurface(void){} //Normal Constructor
    inline virtual ~YieldSurface(void){} 
    virtual YieldSurface *newObj(void)= 0;  //create a clone of itself
  
    virtual double f( const EPState *EPS ) const = 0;	 //pure virtual func
    virtual BJtensor dFods( const EPState *EPS ) const = 0;  //pure virtual func
    virtual void print() = 0; //pure virtual func

    // 1st derivative of F over scalar internal variables (at most 4 scalar internal vars allowed currently)
    virtual double xi_s1( const EPState *EPS ) const;	 
    virtual double xi_s2( const EPState *EPS ) const;
    virtual double xi_s3( const EPState *EPS ) const;
    virtual double xi_s4( const EPState *EPS ) const;

    // 1st derivative of F over scalar internal variables (at most 4 tensor internal vars allowed currently)
    virtual BJtensor xi_t1( const EPState *EPS ) const;	 
    virtual BJtensor xi_t2( const EPState *EPS ) const;
    virtual BJtensor xi_t3( const EPState *EPS ) const;
    virtual BJtensor xi_t4( const EPState *EPS ) const;


    //================================================================================
    // Overloaded Insertion Operator
    // prints an YieldSurface's contents 
    //================================================================================
    friend std::ostream& operator<< (std::ostream& os, const YieldSurface & YS);
};
} // fin namespace XC


#endif

