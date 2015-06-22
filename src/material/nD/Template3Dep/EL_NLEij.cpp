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
//  Print vars defined in XC::Linear Evolution Law
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
std::ostream& XC::operator<<(std::ostream& os, const XC::EvolutionLaw_NL_Eij & LEL)
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

