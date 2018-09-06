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
# CLASS:             EvolutionLaw_NL_Eij (on plastic strain)                     #
#                                                                                #
# VERSION:                                                                       #
# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )    #
# TARGET OS:         DOS || UNIX || . . .                                        #
# DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                 #
# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                                 #
#                                                                                #
#                                                                                #
# DATE:              09-13-2000                                                  #
# UPDATE HISTORY:    18May2004 Zhao fixed equation                               #
#                    BJtensor h = dQods * (2.0/3.0) * ha - alpha * Cr * temp2;     #
#                    so it is consistent with XC::BJtensor multiplication (revised)    #
#                                                                                #
#                                                                                #
# SHORT EXPLANATION: This is a nonlinear evolution law for the evoltion of a     #
#                    tensorial variable alpha which depends on plastic strain    #
#                    i.e. dalpha_ij = 2/3*ha*dE_ij -Cr*de_eq*alpha_ij( Amstrong- #
//                   Frederick Model)                                            #
#                                                                                #
#                                                                                #
#                                                                                #
//================================================================================
*/

#ifndef EL_NLEij_CPP
#define EL_NLEij_CPP

#include "material/nD/Template3Dep/EL_NLEij.h"
#include <utility/matrix/nDarray/basics.h>
#include "material/nD/Template3Dep/EPState.h"
#include "material/nD/Template3Dep/PS.h"
#include <utility/matrix/nDarray/BJtensor.h>

//================================================================================
// Copy constructor
//================================================================================

XC::EvolutionLaw_NL_Eij::EvolutionLaw_NL_Eij(const EvolutionLaw_NL_Eij &LE ) {

    this->ha = LE.getha();
    this->Cr = LE.getCr();
}


//================================================================================
//  Create a clone of itself 
//================================================================================
XC::EvolutionLaw_T * XC::EvolutionLaw_NL_Eij::newObj() {
    
    EvolutionLaw_T *newEL = new EvolutionLaw_NL_Eij( *this );
    
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

////================================================================================
////  Updating corresponding internal variable           MOVED to CDriver.cpp       
////================================================================================
//
//void XC::EvolutionLaw_L_Eij::UpdateVar( EPState *EPS, int WhichOne) {
//   
//    //=========================================================================
//    // Updating alfa1 by dalfa1 = a* de_eq
//    
//    // Calculate  e_eq = sqrt( 2.0 * epsilon_ij * epsilon_ij / 3.0)
//    XC::straintensor pstrain =  EPS->getdPlasticStrain(); 
//    double e_eq  = pstrain.equivalent();
//
//    double dS =  e_eq * geta();
//    double S  = EPS->getScalarVar( WhichOne );
//
//    EPS->setScalarVar(WhichOne, S + dS);
//
//}


 
//================================================================================
// Evaluating h_s = 2*ha*Rij/3 - Cr*pow( 2.0*Rij_dev * Rij_dev/3.0, 0.5) (For the evaluation of Kp)
//================================================================================

XC::BJtensor XC::EvolutionLaw_NL_Eij::h_t( EPState *EPS, PotentialSurface *PS){

    //=========================================================================
    // Getting de_ij / dLambda
    
    XC::stresstensor dQods = PS->dQods( EPS );
    //dQods.reportshort("dQods");

    BJtensor dQods_dev = dQods.deviator();
    BJtensor temp1 =  dQods_dev("ij")*dQods_dev("ij");
    double norm_dQods_dev =  pow( temp1.trace(), 0.5 );
    double temp2 = pow( 2.0 / 3.0, 0.5 ) * norm_dQods_dev;
    
    double ha = getha();
    double Cr = getCr();
    BJtensor alpha = EPS->getTensorVar(1);
             
    BJtensor h = dQods * (2.0/3.0) * ha - alpha * Cr * temp2;

    return h;

}


//================================================================================
//  Print vars defined in Linear Evolution Law
//================================================================================
void XC::EvolutionLaw_NL_Eij::print()
  { std::cerr << (*this); }


//================================================================================
double XC::EvolutionLaw_NL_Eij::getha() const
{       
    return ha;
}

//================================================================================
double XC::EvolutionLaw_NL_Eij::getCr() const
{       
    return Cr;
}

//================================================================================
std::ostream& XC::operator<<(std::ostream &os, const XC::EvolutionLaw_NL_Eij & LEL)
  {
  //    os.unsetf( ios::scientific );
    os.precision(5);

    os.width(10);       
    os << std::endl << "NonLinear Tensorial Evolution Law(A-F model)'s parameters:" << std::endl;
    os << "ha = " << LEL.getha() << "; ";
    os << "Cr = " << LEL.getCr() << std::endl;
           
    return os;
  }  

#endif

