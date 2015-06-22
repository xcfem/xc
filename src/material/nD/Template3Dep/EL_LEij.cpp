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
//================================================================================
# COPYRIGHT (C):     :-))                                                        #
# PROJECT:           Object Oriented Finite XC::Element Program                      #
# PURPOSE:           General platform for elaso-plastic constitutive model       #
#                    implementation                                              #
#                                                                                #
# CLASS:             EvolutionLaw_L_Eij (on plastic equivalent strain)           #
#                                                                                #
# VERSION:                                                                       #
# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )    #
# TARGET OS:         DOS || UNIX || . . .                                        #
# DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                 #
# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                                 #
#                                                                                #
#                                                                                #
# DATE:              09-13-2000                                                  #
# UPDATE HISTORY:                                                                #
#                                                                                #
#                                                                                #
#                                                                                #
# SHORT EXPLANATION: This is a linear evolution law for the evoltion of a        #
#                    tensorial variable k which depends on plastic strain        #
#                    i.e. dalpha = a*de_ij_p                                     #
//================================================================================
*/

#ifndef EL_LEij_CPP
#define EL_LEij_CPP

#include "material/nD/Template3Dep/EL_LEij.h"
#include "utility/matrix/nDarray/basics.h"
#include "material/nD/Template3Dep/EPState.h"
#include "material/nD/Template3Dep/PS.h"
    
//================================================================================
// Default constructor
//================================================================================
XC::EvolutionLaw_L_Eij::EvolutionLaw_L_Eij( double ad) 
: a(ad)
{}     

//================================================================================
// Copy constructor
//================================================================================

XC::EvolutionLaw_L_Eij::EvolutionLaw_L_Eij(const EvolutionLaw_L_Eij &LE ) 
{
    this->a = LE.geta();
}


//================================================================================
//  Create a clone of itself 
//================================================================================
XC::EvolutionLaw_T * XC::EvolutionLaw_L_Eij::newObj() {
    
    EvolutionLaw_T *newEL = new EvolutionLaw_L_Eij( *this );
    
    return newEL;

}

////================================================================================
////  Initialize some  vars in XC::EPState				        
////  nothing						       	        
////================================================================================
//
//void XC::EvolutionLaw_L_Eij::InitVars(EPState  *EPS) {
//
//    // set initial E_Young corresponding to current stress state
//    //double p_atm = 100.0; //Kpa atmospheric pressure
//    //double p = EPS->getStress().p_hydrostatic();
//    //double E = EPS->getEo() * pow( (p/p_atm), geta());
//    EPS->setE( EPS->getEo() );
//      
//}   


//================================================================================
//  Set initial value of D once the current stress hit the yield surface     	
//  for L model only    						       	
//							       		        
//							       	                
//================================================================================
//
//void XC::EvolutionLaw_L_Eij::setInitD(EPState  *EPS) {
//
//}   

 
//================================================================================
// Evaluating h_s = a * dSodeij = a*Rij (For the evaluation of Kp)
//================================================================================

XC::BJtensor XC::EvolutionLaw_L_Eij::h_t( EPState *EPS, PotentialSurface *PS){

    //=========================================================================
    // Getting de_ij / dLambda
    XC::stresstensor dQods = PS->dQods( EPS );
    //dQods.reportshort("dQods");
    
    BJtensor de_ijodLam = dQods;

    //Evaluating dSodeeq
    double dSodeij = geta();

    BJtensor h = de_ijodLam*dSodeij;

    return h;

}


//================================================================================
//  Print vars defined in XC::Linear Evolution Law
//================================================================================
void XC::EvolutionLaw_L_Eij::print()
  { std::cerr << (*this); }


//================================================================================
double XC::EvolutionLaw_L_Eij::geta() const
  { return a; }

//================================================================================
std::ostream& XC::operator<< (std::ostream& os, const XC::EvolutionLaw_L_Eij & LEL)
  {
    //    os.unsetf(ios::scientific );
    os.precision(5);

    os.width(10);       
    os << std::endl << "Linear Tensorial Evolution Law's parameters:" << std::endl;
    os << "a = " << LEL.geta() << "; " << std::endl;
    return os;
  }  

#endif

