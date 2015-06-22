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
# CLASS:             EvolutionLaw_NL_Eeq (on plastic equivalent strain)          #
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
# SHORT EXPLANATION: This is a nonlinear evolution law for the evoltion of a     #
#                    scalar variable k which depends on plastic equi. strain     #
#                    i.e. dk = f(e_eq)*de_eq                                     #
//================================================================================
*/

#ifndef EL_NLEeq_CPP
#define EL_NLEeq_CPP

#include "material/nD/Template3Dep/EL_NLEeq.h"
#include "utility/matrix/nDarray/basics.h"
#include "material/nD/Template3Dep/EPState.h"
#include "material/nD/Template3Dep/PS.h"
    
//================================================================================
// Copy constructor
//================================================================================

XC::EvolutionLaw_NL_Eeq::EvolutionLaw_NL_Eeq(const EvolutionLaw_NL_Eeq &NLE )
  {
    this->eeqEtaPeak  = NLE.geteeqEtaPeak();
    this->etaResidual = NLE.getetaResidual();
    this->etaStart    = NLE.getetaStart();
    this->etaPeak     = NLE.getetaPeak();
    this->e           = NLE.gete();
    this->d           = NLE.getd();
  }


//================================================================================
//  Create a clone of itself 
//================================================================================
XC::EvolutionLaw_S* XC::EvolutionLaw_NL_Eeq::newObj(void)
  { return new EvolutionLaw_NL_Eeq(*this); }


////================================================================================
////  Initialize some  vars in XC::EPState				        
////  nothing						       	        
////================================================================================
//
//void XC::EvolutionLaw_NL_Eeq::InitVars(EPState  *EPS) {
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
//  for NL model only    						       	
//							       		        
//							       	                
//================================================================================
//
//void XC::EvolutionLaw_NL_Eeq::setInitD(EPState  *EPS) {
//
//}   

////================================================================================
////  Updating corresponding internal variable       done in CDriver                
////================================================================================
//
//void XC::EvolutionLaw_NL_Eeq::UpdateVar( EPState *EPS, int WhichOne) {
//   
//    //=========================================================================
//    // Updating corresponding internal var by a nonliear function f
//    
//    // Getting e_eq
//    XC::straintensor pstrain =  EPS->getPlasticStrain();
//    double e_eq  = pstrain.equivalent();
//    double e_eq2  = e_eq * e_eq;
//
//    double upper1 = getd()*getetaResidual()*e_eq2;
//    double upper2 = (gete()*getetaPeak()+2.0*getd()*geteeqEtaPeak()*(-getetaResidual()+getetaPeak()))*e_eq;
//    double upper3 = getetaStart()*getd()*pow(geteeqEtaPeak(),2.0)*(-getetaResidual()+getetaPeak())/(getetaPeak()-getetaStart());
//    double upper = upper1 + upper2 + upper3;
//    
//    double lower = getd()*e_eq2 + gete()*e_eq +getd()*pow(geteeqEtaPeak(), 2.0)*(-getetaResidual()+getetaPeak())/(getetaPeak()-getetaStart());
//    
//    double new_S =  upper/lower;
//    
//    EPS->setScalarVar(WhichOne, new_S);
//
//}


 
//================================================================================
// Evaluating h_s ( for the evaluation of Kp )
//================================================================================
//double XC::EvolutionLaw_NL_Eeq::h( EPState *EPS, double norm_dQods_dev ) {

double XC::EvolutionLaw_NL_Eeq::h_s( EPState *EPS, PotentialSurface *PS)
{
    double h;

    //=========================================================================
    // Getting de_eq/dLambda
    //double  de_eq = EPS->getdPlasticStrain().equivalent();
    XC::stresstensor dQods = PS->dQods( EPS );
    BJtensor dQods_dev = dQods.deviator();

    //Evaluate the norm of the deviator of dQods
    //temp1 =  dQods("ij")*dQods("ij");
    BJtensor temp1 =  dQods_dev("ij")*dQods_dev("ij");
    double norm_dQods_dev =  pow( temp1.trace(), 0.5 );
    double de_eqodL = pow( 2.0 / 3.0, 0.5 ) * norm_dQods_dev;
    
    //Evaluating dSodeeq
    double dSodeeq;
    XC::straintensor pstrain =  EPS->getPlasticStrain(); //bug! should be total plastic strain's deviatoric part????
    XC::straintensor pstrain_dev =  pstrain.deviator(); 

    //Checking equivalent()
    //Using deviatoric plastic strain to evolve the scalar internal var!
    //BJtensor temptx  = pstrain("ij") * pstrain("ij");
    //double tempdx = temptx.trace();
    //double e_eq  = pow( 2.0 * tempdx / 3.0, 0.5 );

    BJtensor temptx  = pstrain_dev("ij") * pstrain_dev("ij");
    double tempdx = temptx.trace();
    double e_eq  = pow( 2.0 * tempdx / 3.0, 0.5 );
    
    
    //double e_eq  = pstrain.equivalent(); ????????????why this wouldn't work?
    double e_eq2  = e_eq * e_eq;

    double upper1 = -getd()*(getetaPeak() - getetaResidual())*(getetaPeak() - getetaStart())*( e_eq- geteeqEtaPeak());
    double upper21 = gete()*(getetaPeak() - getetaStart())* (e_eq + geteeqEtaPeak()); 
    double upper22 = 2.0*getd()* geteeqEtaPeak()* (-(getetaStart()*e_eq)-getetaResidual()* geteeqEtaPeak() + getetaPeak()* (e_eq+geteeqEtaPeak()));
    double upper = upper1 * (upper21 + upper22);

    double lower1 = gete()*(getetaPeak()-getetaStart())*e_eq;
    double lower2 = getd()*(getetaPeak()*(e_eq2+pow(geteeqEtaPeak(), 2.0) )-getetaStart()*e_eq2-getetaResidual()*pow(geteeqEtaPeak(), 2.0));
    double lower = pow(lower1 + lower2, 2.0);
    
    dSodeeq = upper / lower;
    
    h = dSodeeq * de_eqodL;

    // Drucker-Prager's evolution law
    // Get the current stress's I1
    //double I1 = EPS->getStress().Iinvariant1();

    //Von Mises 
    //double k = EPS->getScalarVar(1);
    //double Kp = -2.0 * k * geta() * temp;

    return h;

}


//================================================================================
//  Print vars defined in NLinear Evolution Law
//================================================================================
void XC::EvolutionLaw_NL_Eeq::print()
  { std::cerr << (*this); }


//================================================================================
double XC::EvolutionLaw_NL_Eeq::geteeqEtaPeak() const
{
    return eeqEtaPeak;
}

//================================================================================
double XC::EvolutionLaw_NL_Eeq::getetaResidual() const
{
    return etaResidual;
}

//================================================================================
double XC::EvolutionLaw_NL_Eeq::getetaStart() const
{
    return etaStart;
}

//================================================================================
double XC::EvolutionLaw_NL_Eeq::getetaPeak() const
{
    return etaPeak;
}

//================================================================================
double XC::EvolutionLaw_NL_Eeq::gete() const
{
    return e;
}

//================================================================================
double XC::EvolutionLaw_NL_Eeq::getd() const
{
    return d;
}

//================================================================================
std::ostream& XC::operator<< (std::ostream& os, const XC::EvolutionLaw_NL_Eeq & NLEL)
{
  //    os.unsetf( ios::scientific );
    os.precision(5);

    //os.width(10);       
    os << std::endl << "Nonlinear Evolution(plastic equivalent strain ) Law's parameters:" << std::endl;
    os << "eeqEtaPeak = " << NLEL.geteeqEtaPeak() << "; ";
    os << "etaResidual = " << NLEL.getetaResidual() << "; " << std::endl;
    os << "etaStart = " << NLEL.getetaStart() << "; ";
    os << "etaPeak = " << NLEL.getetaPeak() << "; ";
    os << "e = " << NLEL.gete() << "; ";
    os << "d = " << NLEL.getd() << "; " << std::endl;
    //os.width(10);       
           
    return os;
}  

#endif

