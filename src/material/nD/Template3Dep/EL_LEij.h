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

//================================================================================
// COPYRIGHT (C):     :-))                                                       |
// PROJECT:           Object Oriented Finite Element Program                     |
// PURPOSE:           General platform for elaso-plastic constitutive model      |
//                    implementation                                             |
//                                                                               |
// CLASS:             EvolutionLaw_L_Eij (linear tensorial Evolution law)        |
//                                                                               |
//                                                                               |
// VERSION:                                                                      |
// LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )   |
// TARGET OS:         DOS || UNIX || . . .                                       |
// DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                |
// PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                                |
//                                                                               |
//                                                                               |
// DATE:              09-13-2000                                                 |
// UPDATE HISTORY:                                                               |
//                                                                               |
//                                                                               |
//                                                                               |
//                                                                               |
// SHORT EXPLANATION: This is a linear evolution law for the evoltion of a       |
//                    tensorial variable alpha which depends on plastic strain   |
//                    i.e. dalpha = a * de_ij                                    |
//                                                                               |
//================================================================================

#ifndef EL_LEij_H
#define EL_LEij_H

#include <cmath>
#include "material/nD/Template3Dep/EL_T.h"

namespace XC {
//! @ingroup MatNDEL
//
//! @brief ??.
class EvolutionLaw_L_Eij : public EvolutionLaw_T
  {
  // Private vars to define the evolution law
  private:
    double  a;  //coefficient to define the linear hardening rule of a scalar hardening var

  public:
    //EvolutionLaw_L_Eij( );    // default constructor---no parameters
    
    EvolutionLaw_L_Eij( double ad = 10.0);
                         
    EvolutionLaw_L_Eij(const EvolutionLaw_L_Eij &LEL );   // Copy constructor
    
    EvolutionLaw_T *newObj();                     //create a colne of itself
    
    //void InitVars(EPState *EPS);    // Initialize all hardening vars called only once 
    //                                // after material point is formed if necessary.
    
    //void setInitD(EPState  *EPS);   // set initial D once current stress hits the y.s.
    //                                // was primarily for Manzari-Dafalias model

    //double h( EPState *EPS,  double norm_dQods);     // Evaluating hardening function h
    BJtensor h_t( EPState *EPS, PotentialSurface *PS);    // Evaluating hardening function h
    
    //void UpdateVar( EPState *EPS, double dlamda );  // Evolve corresponding var linearly using de_eq_p
    //Moved to CDriver.cpp

    void print();

  private:
    // some accessor functions
    double geta() const;      // Linear coefficient used to evolve internal var
    void   seta(double ad);

    //================================================================================
    // Overloaded Insertion Operator	  Zhaohui Added Aug. 13, 2000
    // prints Linear EvolutionLaw's contents 
    //================================================================================
    friend std::ostream& operator<< (std::ostream& os, const EvolutionLaw_L_Eij & LEL);

    
};
} // fin namespace XC


#endif




