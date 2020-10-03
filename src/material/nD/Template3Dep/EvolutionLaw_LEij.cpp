//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
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

#ifndef EvolutionLaw_LEij_CPP
#define EvolutionLaw_LEij_CPP

#include "material/nD/Template3Dep/EvolutionLaw_LEij.h"
#include "utility/matrix/nDarray/basics.h"
#include "material/nD/Template3Dep/EPState.h"
#include "material/nD/Template3Dep/PotentialSurface.h"
    
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


//! @brief Virtual constructor.
XC::EvolutionLaw_T * XC::EvolutionLaw_L_Eij::getCopy(void)
  { return new EvolutionLaw_L_Eij(*this); }

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
//  Print vars defined in Linear Evolution Law
//================================================================================
void XC::EvolutionLaw_L_Eij::print()
  { std::cerr << (*this); }


//================================================================================
double XC::EvolutionLaw_L_Eij::geta() const
  { return a; }

//================================================================================
std::ostream& XC::operator<<(std::ostream &os, const XC::EvolutionLaw_L_Eij & LEL)
  {
    //    os.unsetf(ios::scientific );
    os.precision(5);

    os.width(10);       
    os << std::endl << "Linear Tensorial Evolution Law's parameters:" << std::endl;
    os << "a = " << LEL.geta() << "; " << std::endl;
    return os;
  }  

#endif

