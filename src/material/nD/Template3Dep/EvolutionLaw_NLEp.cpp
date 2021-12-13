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
# PURPOSE:           Cam clay model evolution law                    		 #
#                                                                                #
#                                                                                #
# CLASS:             EvolutionLaw_NL_Ep (on plastic volumetric strain)           #
#                                                                                #
# VERSION:                                                                       #
# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )    #
# TARGET OS:         DOS || UNIX || . . .                                        #
# DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                 #
# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                                 #
#                                                                                #
#                                                                                #
# DATE:              Mar. 28, 2001                                               #
# UPDATE HISTORY:                                                                #
#                                                                                #
#                                                                                #
#                                                                                #
# SHORT EXPLANATION: This is a nonlinear evolution law for the evoltion of a     #
#                    scalar variable po which depends on plastic vol. strain     #
#                    i.e. dpo = (1+eo)po/(lamda-kappa)*de_p                      #
//================================================================================
*/

#ifndef EvolutionLaw_NLEp_CPP
#define EvolutionLaw_NLEp_CPP

#include "material/nD/Template3Dep/EvolutionLaw_NLEp.h"
#include "utility/matrix/nDarray/basics.h"
#include "material/nD/Template3Dep/EPState.h"
#include "material/nD/Template3Dep/PotentialSurface.h"
   
//================================================================================
// Default constructor
//================================================================================
XC::EvolutionLaw_NL_Ep::EvolutionLaw_NL_Ep( double eod, double lambdad, double kappad ) 
:eo(eod), lambda(lambdad), kappa(kappad)
{}     

//================================================================================
// Copy constructor
//================================================================================

XC::EvolutionLaw_NL_Ep::EvolutionLaw_NL_Ep(const EvolutionLaw_NL_Ep &LE ) 
{
    this->eo     = LE.geteo();
    this->lambda = LE.getlambda();
    this->kappa  = LE.getkappa();
}

//! @brief Virtual constructor.
XC::EvolutionLaw_S * XC::EvolutionLaw_NL_Ep::getCopy(void) 
  { return new EvolutionLaw_NL_Ep(*this); }

////================================================================================
////  Initialize some  vars in XC::EPState				        
////  nothing						       	        
////================================================================================
//
//void XC::EvolutionLaw_NL_Ep::InitVars(EPState  *EPS) {
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
//void XC::EvolutionLaw_NL_Ep::setInitD(EPState  *EPS) {
//
//}   
	  	  
 
//================================================================================
// Evaluating h_s = a * pow( 2.0*Rij_dev * Rij_dev/3.0, 0.5) (For the evaluation of Kp)
//================================================================================

double XC::EvolutionLaw_NL_Ep::h_s( EPState *EPS, PotentialSurface *PS)
  {

    //=========================================================================
    // Getting de_eq / dLambda
    stresstensor dQods(PS->dQods( EPS ));
    //dQods.reportshort("dQods");

    //Evaluate the norm of the deviator of dQods
    //temp1 =  dQods("ij")*dQods("ij");
    double dQods_p = dQods.p_hydrostatic();
    
    double de_podL = dQods_p;

    //Evaluating dSodeeq

    double po = EPS->getScalarVar( 1 );
    double dSodep = (1.0+geteo())*po/( getlambda()-getkappa() );

    double h = dSodep * de_podL;

    return h;

}


//================================================================================
//  Print vars defined in Linear Evolution Law
//================================================================================
void XC::EvolutionLaw_NL_Ep::print()
  { std::cerr << (*this); }


//================================================================================
double XC::EvolutionLaw_NL_Ep::geteo() const
{       
    return eo;
}

//================================================================================
double XC::EvolutionLaw_NL_Ep::getlambda() const
{       
    return lambda;
}

//================================================================================
double XC::EvolutionLaw_NL_Ep::getkappa() const
{       
    return kappa;
}


//================================================================================
std::ostream& XC::operator<<(std::ostream &os, const XC::EvolutionLaw_NL_Ep & LEL)
{
  //    os.unsetf( ios::scientific );
    os.precision(5);

    os.width(10);       
    os << std::endl << "Nonlinear Scalar Evolution Law(Cam Clay model)'s parameters:" << std::endl;
    os << "eo = " << LEL.geteo() << "; " << std::endl;
    os << "lambda = " << LEL.getlambda() << "; " << std::endl;
    os << "kappa = " << LEL.getkappa() << "; " << std::endl;
           
    return os;
}  

#endif

