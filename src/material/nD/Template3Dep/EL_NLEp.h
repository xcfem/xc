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
// CLASS:             EvolutionLaw_L_Ep (linear scalar Evolution law)            |
//                                                                               |
//                                                                               |
// VERSION:                                                                      |
// LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )   |
// TARGET OS:         DOS || UNIX || . . .                                       |
// DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                |
// PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                                |
//                                                                               |
//                                                                               |
// DATE:              Mar. 28, 01                                                |
// UPDATE HISTORY:                                                               |
//                                                                               |
//                                                                               |
//                                                                               |
//                                                                               |
// SHORT EXPLANATION: This is a nonlinear evolution law for the evoltion of a    |
//                    scalar variable po which depends on plastic volumetric     |
//                    strain i.e. dpo = (1+eo)po/(lamda-kappa)*de_p              |
//                                                                               |
//================================================================================

#ifndef EL_NLEp_H
#define EL_NLEp_H

#include <cmath>
#include "material/nD/Template3Dep/EL_S.h"

namespace XC {
//! @ingroup MatNDEL
//
//! @brief This is a nonlinear evolution law for the evoltion of a
//! scalar variable po which depends on plastic volumetric
//! strain i.e. dpo = (1+eo)po/(lamda-kappa)*de_p.
class EvolutionLaw_NL_Ep : public EvolutionLaw_S
  {
  // Private vars to define the evolution law

  private:
    double  eo;      //coefficient to define the linear hardening rule of a scalar hardening var
    double  lambda;  //coefficient to define the linear hardening rule of a scalar hardening var
    double  kappa;   //coefficient to define the linear hardening rule of a scalar hardening var

  public:
    //EvolutionLaw_NL_Ep( );    // default constructor---no parameters
    
    EvolutionLaw_NL_Ep( double eod = 0.85, double lambdad = 0.19, double kappad = 0.06);
                         
    EvolutionLaw_NL_Ep(const EvolutionLaw_NL_Ep &LEL );   // Copy constructor
    
    EvolutionLaw_S *newObj();                     //create a clone of itself
    
    //void InitVars(EPState *EPS);    // Initialize all hardening vars called only once 
    //                                // after material point is formed if necessary.
    
    //void setInitD(EPState  *EPS);   // set initial D once current stress hits the y.s.
    //                                // was primarily for Manzari-Dafalias model

    //double h( EPState *EPS,  double norm_dQods);     // Evaluating hardening function h
    double h_s( EPState *EPS, PotentialSurface *PS);    // Evaluating hardening function h
    
    //void UpdateVar( EPState *EPS, double dlamda );  // Evolve corresponding var linearly using de_eq_p
    //Moved to CDriver.cpp

    void print();

  private:
    // some accessor functions
    double geteo() const;      
    //void   seteo( double eod);
    
    double getlambda() const;      
    //void   setlambda( double lambdad);
    
    double getkappa() const;      
    //void   setkappad( double kappad);

    //================================================================================
    // Overloaded Insertion Operator	  Zhaohui Added Aug. 13, 2000
    // prints Linear EvolutionLaw's contents 
    //================================================================================
    friend std::ostream& operator<< (std::ostream& os, const EvolutionLaw_NL_Ep & LEL);

    
};
} // end of XC namespace


#endif




