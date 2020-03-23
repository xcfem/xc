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
# CLASS:             EvolutionLaw_L_Eeq (on plastic equivalent strain)          #
#                                                                                #
# VERSION:                                                                       #
# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )    #
# TARGET OS:         DOS || UNIX || . . .                                        #
# DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                 #
# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                                 #
#                                                                                #
#                                                                                #
# DATE:              09-02-2000                                                  #
# UPDATE HISTORY:                                                                #
#                                                                                #
#                                                                                #
#                                                                                #
# SHORT EXPLANATION: This is a linear evolution law for the evoltion of a        #
#                    scalar variable k which depends on plastic equi. strain     #
#                    i.e. dk = a*de_eq_p                                         #
//================================================================================
*/

#ifndef EvolutionLaw_LEeq_CPP
#define EvolutionLaw_LEeq_CPP

#include "material/nD/Template3Dep/EvolutionLaw_LEeq.h"
#include "utility/matrix/nDarray/basics.h"
#include "material/nD/Template3Dep/EPState.h"
#include "material/nD/Template3Dep/PotentialSurface.h"
    
//================================================================================
// Default constructor
//================================================================================
XC::EvolutionLaw_L_Eeq::EvolutionLaw_L_Eeq( double ad ) 
:a(ad)
{}     

//================================================================================
// Copy constructor
//================================================================================

XC::EvolutionLaw_L_Eeq::EvolutionLaw_L_Eeq(const EvolutionLaw_L_Eeq &LE ) 
{
    this->a = LE.geta();
}


//================================================================================
//  Create a clone of itself 
//================================================================================
// Alpha machine has problem on this
//EvolutionLaw_L_Eeq * XC::EvolutionLaw_L_Eeq::newObj() {
XC::EvolutionLaw_S * XC::EvolutionLaw_L_Eeq::newObj() 
{    
    EvolutionLaw_S *newEL = new EvolutionLaw_L_Eeq( *this );
    
    return newEL;
}

////================================================================================
////  Initialize some  vars in XC::EPState				        
////  nothing						       	        
////================================================================================
//
//void XC::EvolutionLaw_L_Eeq::InitVars(EPState  *EPS) {
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
//  for MD model only    						       	
//================================================================================
//
//void XC::EvolutionLaw_L_Eeq::setInitD(EPState  *EPS) {
//
//}   
	  	  
 
//================================================================================
// Evaluating h_s = a * pow( 2.0*Rij_dev * Rij_dev/3.0, 0.5) (For the evaluation of Kp)
//================================================================================

double XC::EvolutionLaw_L_Eeq::h_s( EPState *EPS, PotentialSurface *PS){

    //=========================================================================
    // Getting de_eq / dLambda
    stresstensor dQods = PS->dQods( EPS );
    //dQods.reportshort("dQods");

    //Evaluate the norm of the deviator of dQods
    //temp1 =  dQods("ij")*dQods("ij");
    BJtensor dQods_dev = dQods.deviator();
    BJtensor temp1 =  dQods_dev("ij")*dQods_dev("ij");
    double norm_dQods_dev =  pow( temp1.trace(), 0.5 );
    
    double de_eqodL = pow( 2.0 / 3.0, 0.5 ) * norm_dQods_dev;

    //Evaluating dSodeeq
    double dSodeeq = geta();

    double h = dSodeeq * de_eqodL;

    return h;

}


//================================================================================
//  Print vars defined in Linear Evolution Law
//================================================================================
void XC::EvolutionLaw_L_Eeq::print()
  { std::cerr << (*this); }


//================================================================================
double XC::EvolutionLaw_L_Eeq::geta() const
  { return a; }

//================================================================================
std::ostream &XC::operator<<(std::ostream &os,const XC::EvolutionLaw_L_Eeq &LEL)
{
  //    os.unsetf( ios::scientific );
    os.precision(5);

    os.width(10);       
    os << std::endl << "Linear Scalar Evolution Law's parameters:" << std::endl;
    os << "a = " << LEL.geta() << "; " << std::endl;
           
    return os;
}

#endif

