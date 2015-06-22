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

#ifndef EL_NLEp_CPP
#define EL_NLEp_CPP

#include "material/nD/Template3Dep/EL_NLEp.h"
#include "utility/matrix/nDarray/basics.h"
#include "material/nD/Template3Dep/EPState.h"
#include "material/nD/Template3Dep/PS.h"
   
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


//================================================================================
//  Create a clone of itself 
//================================================================================
// Alpha machine has problem on this
//EvolutionLaw_NL_Ep * XC::EvolutionLaw_NL_Ep::newObj() {
XC::EvolutionLaw_S * XC::EvolutionLaw_NL_Ep::newObj() 
{    
    EvolutionLaw_S *newEL = new EvolutionLaw_NL_Ep( *this );
    
    return newEL;
}

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

double XC::EvolutionLaw_NL_Ep::h_s( EPState *EPS, PotentialSurface *PS){

    //=========================================================================
    // Getting de_eq / dLambda
    XC::stresstensor dQods = PS->dQods( EPS );
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
//  Print vars defined in XC::Linear Evolution Law
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
std::ostream& XC::operator<<(std::ostream& os, const XC::EvolutionLaw_NL_Ep & LEL)
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

