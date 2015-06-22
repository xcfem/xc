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

///================================================================================
// COPYRIGHT (C):     :-))                                                        |
// PROJECT:           Object Oriented Finite Element Program                      |
// PURPOSE:           General platform for elaso-plastic efficient and easy       |
//                    constitutive model implementation                           |
//                                                                                |
// CLASS:             EvolutionLaw_T (the base class for tensorial evolution law) |
//                                                                                |
// VERSION:                                                                       |
// LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )    |
// TARGET OS:         DOS || UNIX || . . .                                        |
// DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                 |
// PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                                 |
//                                                                                |
//                                                                                |
// DATE:              09-02-2000                                                  |
// UPDATE HISTORY:                                                                |
//                                                                                |
//                                                                                |
//                                                                                |
//                                                                                |
// SHORT EXPLANATION: The base class is necessary for we need to use to runtime   |
//                    polymorphism of C++ to achieve the fexibility of the 	  |
//                    platform.                                                   |
//                                                                                |
//=================================================================================
//

#ifndef EL_T_H
#define EL_T_H

#include <iostream>

namespace XC {
class EPState;
class PotentialSurface;
 class BJtensor;

//! @ingroup MatNDEL
//
//! @brief ??.
class EvolutionLaw_T
  {
  public:
    // Constructor
    EvolutionLaw_T(void) {}	 
    virtual ~EvolutionLaw_T(void) {}
    //create a colne of itself
    virtual EvolutionLaw_T *newObj();  

    // Not necessary since the increment of internal var can be evalulated in constitutive driver!
    //virtual void UpdateVar( EPState *EPS, double dlamda ) = 0; // Evolve only one internal variable
    
    //Print the contents of the evolution law
    virtual void print(); 	
    
    // updating E, e, D and m for Manzari-Dafalias model, 
    virtual int updateEeDm( EPState *EPS, double st_vol, double dLamda);  

    //virtual void setInitD( EPState *EPS) = 0;  // Initializing D once current st hits the y.s. for Manzari-Dafalias model , 
    //                                  // other model might not need it
    
    //Why can't i put const before EPState???????????????????????? Aug. 16, 2000
    //virtual double h( EPState *EPS, double d ) = 0;    // Evaluating hardening function h
    // Evaluating hardening function h_t
    virtual BJtensor h_t(EPState *EPS, PotentialSurface *PS);    

    //================================================================================
    // Overloaded Insertion Operator
    // prints an Evolution Law_T's contents 
    //================================================================================
    friend std::ostream& operator<<(std::ostream& os, const EvolutionLaw_T & EL);

};
} // fin namespace XC


#endif

