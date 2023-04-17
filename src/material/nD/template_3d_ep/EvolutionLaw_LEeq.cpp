//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//============================================================================
# COPYRIGHT (C):     :-))                                                     
# PROJECT:           Object Oriented Finite XC::Element Program               
# PURPOSE:           General platform for elaso-plastic constitutive model
#                    implementation                                       
#                                                                         
# CLASS:             EvolutionLaw_L_Eeq (on plastic equivalent strain)   
#                                                                         
# VERSION:                                                                
# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )   #
# TARGET OS:         DOS || UNIX || . . .                                 
# DESIGNER(S):       Boris Jeremic, Zhaohui Yang                          
# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                          
#                                                                         
#                                                                         
# DATE:              09-02-2000                                           
# UPDATE HISTORY:                                                         
#                                                                         
#                                                                         
#                                                                         
# SHORT EXPLANATION: This is a linear evolution law for the evoltion of a 
#                    scalar variable k which depends on plastic equi. strain
#                    i.e. dk = a*de_eq_p                                  
//=============================================================================
*/

#ifndef EvolutionLaw_LEeq_CPP
#define EvolutionLaw_LEeq_CPP

#include "material/nD/template_3d_ep/EvolutionLaw_LEeq.h"
#include "utility/matrix/nDarray/basics.h"
#include "material/nD/template_3d_ep/EPState.h"
#include "material/nD/template_3d_ep/PotentialSurface.h"
    
//! @brief  Default constructor
XC::EvolutionLaw_L_Eeq::EvolutionLaw_L_Eeq(const double &ad) 
 : a(ad) {}

//! @brief Virtual constructor.
XC::EvolutionLaw_S * XC::EvolutionLaw_L_Eeq::getCopy(void) const 
  { return new EvolutionLaw_L_Eeq(*this); }

//! @brief  Evaluating h_s = a * pow( 2.0*Rij_dev * Rij_dev/3.0, 0.5) (For the evaluation of Kp)
double XC::EvolutionLaw_L_Eeq::h_s( EPState *EPS, PotentialSurface *PS)
  {
    //=========================================================================
    // Getting de_eq / dLambda
    const stresstensor dQods(PS->dQods( EPS ));
    //dQods.reportshort("dQods");

    //Evaluate the norm of the deviator of dQods
    //temp1 =  dQods("ij")*dQods("ij");
    const BJtensor dQods_dev = dQods.deviator();
    const BJtensor temp1 =  dQods_dev("ij")*dQods_dev("ij");
    const double norm_dQods_dev =  pow( temp1.trace(), 0.5 );
    
    const double de_eqodL = pow( 2.0 / 3.0, 0.5 ) * norm_dQods_dev;

    //Evaluating dSodeeq
    const double dSodeeq = geta();
    const double h = dSodeeq * de_eqodL;
    return h;

  }


//================================================================================
//! @brief Print vars defined in Linear Evolution Law
//================================================================================
void XC::EvolutionLaw_L_Eeq::print()
  { std::cerr << (*this); }


//================================================================================
double XC::EvolutionLaw_L_Eeq::geta(void) const
  { return a; }

void XC::EvolutionLaw_L_Eeq::seta(const double &d)
  { a= d; }

//================================================================================
std::ostream &XC::operator<<(std::ostream &os,const XC::EvolutionLaw_L_Eeq &LEL)
  {
  //    os.unsetf( ios::scientific );
    os.precision(5);

    os.width(10);       
    os << std::endl << "Linear Scalar Evolution Law's parameters:" << std::endl;
    os << "a= " << LEL.geta() << "; " << std::endl;
           
    return os;
  }

#endif

