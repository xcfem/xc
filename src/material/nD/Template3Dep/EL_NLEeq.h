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
// CLASS:             NonLinear EvolutionLaw (on plastic equivalent strain)      |
//                                                                               |
// VERSION:                                                                      |
// LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )   |
// TARGET OS:         DOS || UNIX || . . .                                       |
// DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                |
// PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                                |
//                                                                               |
//                                                                               |
// DATE:              09-02-2000                                                 |
// UPDATE HISTORY:                                                               |
//                                                                               |
//                                                                               |
//                                                                               |
//                                                                               |
// SHORT EXPLANATION: This is a nonlinear evolution law for the evoltion of a    |
//                    scalar variable k which depends on plastic equi. strain    |
//                    i.e. dk = f( de_eq_p )                                     |
//                                                                               |
//================================================================================

#ifndef EL_NLEeq_H
#define EL_NLEeq_H

#include <cmath>
#include "material/nD/Template3Dep/EL_S.h"

namespace XC {
//! @ingroup MatNDEL
//
//! @brief ??.
class EvolutionLaw_NL_Eeq : public EvolutionLaw_S
  {
  // Private vars to define the evolution law

    //Coefficients to define the nonlinear hardening rule of a scalar var eta
    double eeqEtaPeak, etaResidual, etaStart, etaPeak, e, d;

  public:
    // default constructor
    EvolutionLaw_NL_Eeq( double eeqEtaPeakd = 0.003,
                         double etaResiduald = .2,
                         double etaStartd = 0.3,
                         double etaPeakd = 0.5,
                         double ed = 0.5,
                         double dd = 500.0) :
                       eeqEtaPeak(eeqEtaPeakd), etaResidual(etaResiduald),
                       etaStart(etaStartd), etaPeak(etaPeakd), e(ed), d(dd) {}
                         
    EvolutionLaw_NL_Eeq(const EvolutionLaw_NL_Eeq &NLEL );   // Copy constructor
    
    EvolutionLaw_S *newObj();                     //create a colne of itself
    
    //void InitVars(EPState *EPS);    // Initialize all hardening vars called only once 
    //                                // after material point is formed if necessary.
    
    //void setInitD(EPState  *EPS);   // set initial D once current stress hits the y.s.
    //                                // was primarily for Manzari-Dafalias model

    //double h( EPState *EPS, double d );     // Evaluating hardening function h
    double h_s( EPState *EPS, PotentialSurface *PS);    // Evaluating hardening function h
    
    //void UpdateVar( EPState *EPS, double dlamda );  // Evolve corresponding var linearly using de_eq_p

    void print();

    // some accessor functions
    // some accessor functions
    double geteeqEtaPeak() const;
    double getetaResidual() const;
    double getetaStart() const;
    double getetaPeak() const;
    double gete() const;
    double getd() const;

    //================================================================================
    // Overloaded Insertion Operator	 
    // prints nonlinear EvolutionLaw's contents 
    //================================================================================
    friend std::ostream& operator<< (std::ostream& os, const EvolutionLaw_NL_Eeq & NLEL);

    
};
} // end of XC namespace


#endif




