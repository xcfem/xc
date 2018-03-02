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
# PROJECT:           Object Oriented Finite Element Program                      #
# PURPOSE:           General platform for elaso-plastic constitutive model       #
#                    implementation                                              #
#                                                                                #
# CLASS:             EvolutionLaw_NL_EijMD (on plastic strain)                   #
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
# SHORT EXPLANATION: This is a nonlinear evolution law for the evolution of a    #
#                    tensorial variable alpha which depends on plastic strain    #
#                    i.e. dalpha_ij = 2/3*ha*dE_ij -Cr*de_eq*alpha_ij( Armstrong-#
//                   Frederick Model)                                            #
//================================================================================
*/

#ifndef EL_NLEIJMD_CPP
#define EL_NLEIJMD_CPP

#include <cmath>
#include "material/nD/Template3Dep/EL_NLEijMD.h"
#include "utility/matrix/nDarray/basics.h"
#include "material/nD/Template3Dep/EPState.h"
#include "material/nD/Template3Dep/PS.h"

#define sqrt23rd 0.8165

//================================================================================
// Default constructor
//================================================================================
XC::EvolutionLaw_NL_EijMD::EvolutionLaw_NL_EijMD( 
                           double eod = 0.85,
    			   double ad  = 0.5,    
    			   double Mcd = 1.14,//1.14, 
    			   double Med = 1.14,//1.14, 
    			   double Lambdad = 0.025,
    			   double ec_refd = 0.8, 
    			   double p_refd = 160.0, 
    			   double kc_bd = 3.975, 
    			   double kc_dd = 4.200, 
    			   double ke_bd = 2.000, 
    			   double ke_dd = 0.07,  
    			   double hod = 1200,	 
    			   double Cmd = 0.00,
    			   double Aod = 2.64,    
    			   double Fmaxd = 100,   
    			   double Cfd = 100):
    			   //double ed  = 0.85,    
  eo(eod), a(ad), Mc(Mcd), Me(Med), Lambda(Lambdad), ec_ref(ec_refd), p_ref(p_refd),  
  kc_b(kc_bd), kc_d(kc_dd), ke_b(ke_bd), ke_d(ke_dd), ho(hod), Cm(Cmd),
  D(0.0), Ao(Aod), Fmax(Fmaxd), Cf(Cfd)
  { } 

//================================================================================
// Copy constructor
//================================================================================

XC::EvolutionLaw_NL_EijMD::EvolutionLaw_NL_EijMD(const EvolutionLaw_NL_EijMD &MDE ) {

    this->a      = MDE.geta();
    this->Mc     = MDE.getMc();
    this->Me     = MDE.getMe();
    this->Lambda = MDE.getLambda();
    this->ec_ref = MDE.getec_ref();
    this->p_ref=  MDE.getp_ref();
    this->kc_b =  MDE.getkc_b();
    this->kc_d =  MDE.getkc_d();
    this->ke_b =  MDE.getke_b();
    this->ke_d =  MDE.getke_d();
    this->ho   =  MDE.getho();
    this->eo   =  MDE.geteo();
    //this->e   =  MDE.gete();
    this->Cm   =  MDE.getCm();
    //D is also needed in XC::PS, so it is copied to 2nd cell of scalar var array
    this->D    =  MDE.getD();  
    this->Ao   =  MDE.getAo();
    this->Fmax =  MDE.getFmax(); 
    this->Cf   =  MDE.getCf();
    this->F    =  MDE.getF(); 
}


//================================================================================
//  Create a clone of itself 
//================================================================================
//EvolutionLaw_NL_EijMD * XC::EvolutionLaw_NL_EijMD::newObj() {
XC::EvolutionLaw_T *XC::EvolutionLaw_NL_EijMD::newObj() {
    
    //EvolutionLaw_T *newEL = new EvolutionLaw_NL_EijMD( *this );
    EvolutionLaw_T *newEL = new EvolutionLaw_NL_EijMD( *this );
    
    return newEL;

}
    
//================================================================================
// Evaluating h_s = h * b_ij = h * (alpha_ij_theta_b - alpha_ij) (For the evaluation of Kp)
//================================================================================

XC::BJtensor XC::EvolutionLaw_NL_EijMD::h_t( EPState *EPS, PotentialSurface *PS)
{
    //=========================================================================
    //calculate  n_ij
    XC::stresstensor S = EPS->getStress().deviator();
    double p = EPS->getStress().p_hydrostatic();
    XC::stresstensor alpha = EPS->getTensorVar( 1 );  // alpha_ij
    double m = EPS->getScalarVar(1);

    XC::stresstensor n;
    //XC::stresstensor n = EPS->getTensorVar( 2 );
   
    XC::stresstensor r = S * (1.0 / p);
    //r.reportshort("r");
    XC::stresstensor r_bar = r - alpha;
    XC::stresstensor norm2 = r_bar("ij") * r_bar("ij");
    double norm = sqrt( norm2.trace() );
    
    if ( norm >= d_macheps()){ 
      n = r_bar *(1.0 / norm );
    }
    else {
      std::cerr << "XC::EvolutionLaw_NL_EijMD::h_t  |n_ij| = 0, divide by zero! Program exits.\n";
      exit(-1);
    }
    // n = r_bar *(1.0 / sqrt23rd / m );
    //std::cerr << "nij = " << n;
    //EPS->setTensorVar(2, n); //n_ij is also stored in XC::Tensor array for 2nd derivative eval in XC::PS  //
    		   
    
    //=========================================================================
    //calculating b_ij for Kp and d_ij for updating D

    //Calculate the state parameters xi 
    //double e = EPS->gete();
    //double e = EPS->getScalarVar(3);
    		   
    //std::cerr << "\n------>h_t(): \n"; 
    //std::cerr << " p: " << p;
    if (p < 0.0)
    {
       //std::cerr << " p: " << p;
      std::cerr << "XC::EvolutionLaw_NL_EijMD::h_t   p < 0, Program exits.\n";
       exit(-1);
    }
    //double ec = (EPS->getec()) - (EPS->getLam()) * log( p / (EPS->getpo()) );
    //double xi = e - ec;
    double xi = EPS->getpsi();
    //std::cerr << " e = " << e << " ec = " << ec << " xi = " << xi << std::endl;

    //  //Calculating the lode angle theta
    //  double J2_bar = r_bar.Jinvariant2();
    //  double J3_bar = r_bar.Jinvariant3();
    //  double tempd = 3.0*pow(3.0, 0.5)/2.0*J3_bar/ pow( J2_bar, 1.5);
    //  //std::cerr << "theta -1 =" << tempd << std::endl;
    //  
    //  if (tempd > 1.0 ) tempd = 1.0; //bug. if tempd = 1.00000000003, acos gives NaN
    //  if (tempd < -1.0 ) tempd = -1.0;    
    //  double theta0 = acos( tempd ) / 3.0;
    double theta = r_bar.theta( );
    //std::cerr << "theta0 = " << theta0/3.1416*180;
    //std::cerr << "  theta = " << theta/3.1416*180 << std::endl;
    
    //calculate the alpha_theta_b and alpha_theta_d
    double c = getMe() / getMc();

    //  double cd = getke_d() / getkc_d();
    //  //XC::stresstensor alpha_theta_d = n("ij") * (g_WW(theta, c) * Mc + g_WW(theta, cd) * kc_d * xi - m) * pow(2.0/3.0, 0.5);
    //  XC::stresstensor alpha_theta_d = n * (g_A(theta, c) * Mc + g_A(theta, cd) * kc_d * xi - m) * sqrt23rd;
    //  //std::cerr << "alpha_theta_d " << alpha_theta_d<<" g_WW(theta, c) "<< g_WW(theta, c) << std::endl;

    double cb = getke_b() / getkc_b();
    if ( xi > 0.0 ) xi = 0.0;  // < -xi > for alpha_theta_b
    //XC::stresstensor alpha_theta_b = n("ij") * (g_WW(theta, c) * Mc - g_WW(theta, cb) * kc_b * xi - m) * pow(2.0/3.0, 0.5);
    //XC::stresstensor alpha_theta_b = n * (g_A(theta, c) * Mc - g_A(theta, cb) * kc_b * xi - m) * sqrt23rd;
    double a_theta_b_scalar = g_A(theta, c) * Mc + g_A(theta, cb) * kc_b *(-1.0*xi) - m; // Mc^b > Mc --> this is right refer to p258 of Mazari's paper
    XC::stresstensor alpha_theta_b = n * a_theta_b_scalar * sqrt23rd;
    alpha_theta_b.null_indices();

    //=========================================================================
    // calculating h
    XC::stresstensor b;
    b =  alpha_theta_b - alpha;
    b.null_indices();
    //  XC::stresstensor d;
    //  d =  alpha_theta_d - alpha;
    //  d.null_indices();

    //double alpha_c_b = g_WW(0.0, c) * Mc + g_WW(0.0, cb) * kc_b * (-xi) - m;
    double alpha_c_b = g_A(60.0, c) * Mc + g_A(60.0, cb) * kc_b * (-1.0*xi) - m;
    double b_ref = 2.0 * sqrt23rd * alpha_c_b;
    //double PI = 3.1416;
    //double a_theta_pi_b = g_A(theta+PI, c) * Mc + g_A(theta+PI, cb) * kc_b *(-1.0*xi) - m;
    //double b_ref = sqrt23rd * ( a_theta_b_scalar + a_theta_pi_b );
    BJtensor temp1 = b("ij") * n("ij");
    double bn = temp1.trace();

    if (bn < 0) bn = 0; // Added Joey 02-19-03
    double abs_bn = fabs( sqrt(bn) );
    //std::cerr << ">>>> alpha_c_b " << alpha_c_b;
    //std::cerr << " alpha_theta_b " << alpha_theta_b;
    //std::cerr << " alpha " << alpha;
    //std::cerr << " \nn " << n;

    //double h = getho() * abs_bn /( b_ref - abs_bn ); 
    double h = getho() * abs_bn /( b_ref - abs_bn );
    //std::cerr << " ||| b_ref =" << b_ref << " bn =" << bn << " std::abs(bn) =" << abs_bn  << " h =" << h << std::endl;
    //cerr << " " << h << std::endl; 

    //Updating D and F---need to fine tune
    //temp1 = d("ij") * n("ij");
    //double dn = temp1.trace();
    //std::cerr << "bn =" << bn << "  dn =" << dn << std::endl; 
    
    //// Calculating A
    //XC::stresstensor F = EPS->getTensorVar( 2 );   // getting  F_ij from XC::EPState
    //temp1 = F("ij") * n("ij");
    //double temp = temp1.trace();
    //if (temp < 0)   temp = 0;   
    //double A = getAo()*(1.0 + temp);

        
    BJtensor ht = b * h;

    return ht;

}


//================================================================================
//  Print vars defined in XC::Linear Evolution Law
//================================================================================
void XC::EvolutionLaw_NL_EijMD::print()
{
    std::cerr << (*this);
}
    
//================================================================================
// prints Manzari-Dafalia EvolutionLaw's contents 
//================================================================================
std::ostream& XC::operator<<(std::ostream& os, const XC::EvolutionLaw_NL_EijMD & MDEL)
//ostream& operator<< (ostream& os, const EvolutionLaw_NL_EijMD & MDEL)
{
  //    os.unsetf( ios::scientific );
    os.precision(5);

    //os.width(10);       
    os << std::endl << "Manzari-Dafalias Evolution Law's parameters:" << std::endl;
    //os << "a = " << MDEL.geta() << "; ";
    os << "Mc = " << MDEL.getMc() << "; ";
    //os.width(10);       
    os << "Me = "  << MDEL.getMe() << "; ";
    //os.width(10);       
    os << "Lambda = " << MDEL.getLambda() << "; ";
    //os.width(10);       
    os << "ec_ref = " << MDEL.getec_ref() << "; ";
    //os.width(10);       
    os << "p_ref = " << MDEL.getp_ref() << "kPa"  << "; " << std::endl;

    //os.width(10);       
    os << "kc_b = " << MDEL.getkc_b() << "; ";
    //os.width(10);       
    os << "kc_d = " << MDEL.getkc_d() << "; ";
    //os.width(10);       
    os << "ke_b = " << MDEL.getke_b() << "; ";
    //os.width(10);       
    os << "ke_d = " << MDEL.getke_d() << "; " << std::endl;

    //os.width(10);       
    //os << "h = " << MDEL.h << "; ";
    //os.width(10);       
    os << "ho = " << MDEL.getho() << "; ";
    //os.width(10);       
    os << "Cm = " << MDEL.getCm() << "; " << std::endl;

    //os.width(10);       
    os << "D = " << MDEL.getD() << "; ";
    //os.width(10);       
    os << "Ao = " << MDEL.getAo() << "; ";
    //os.width(10);       
    //os << "Fmax = " << MDEL.getFmax() << "; ";
    ////os.width(10);       
    //os << "Cf = " << MDEL.getCf() << "; " << std::endl; 
    //os << "F = " << MDEL.getF() << std::endl; 
           
    return os;
}  


//================================================================================
//  Initialize some  vars in XC::EPState				        
//================================================================================
//void XC::EvolutionLaw_L_EijMD::InitVars(EPState  *EPS) 

int XC::EvolutionLaw_NL_EijMD::updateEeDm(EPState *EPS, double st_vol, double dLamda)
{
    int err = 0;
    
    //std::cerr << "------>updateEeDm(): \n"; 
    
    double p = EPS->getStress().p_hydrostatic();//Pc???

    //// Updating E_Young corresponding to current stress state
    //double p_atm = 100.0; //Kpa atmospheric pressure
    //double E = EPS->getEo()* pow( (p/p_atm), geta());
    //EPS->setE( E );
    //std::cerr << " EL_NLEijMD -->Ec = " << E << std::endl;
     
    //if ( dLamda < 0)   dLamda = 0;   //One more bug found by Joey Yang 07-16-02 Forgot <Macauley bracket>

    // Updating e
    //double st_vol = st.p_hydrostatic();
    double e = EPS->gete();
    //Should focus on plastic volumetric strain  Joey 02-18-03
    //double de = -(1.0 + geteo())*st_vol; //bug found should be (-st_vol) 07-16-02 Compressive strain is negative
    //double de = -(1.0 + (EPS->geteo()) )*st_vol; //ZC
    double de = -(1.0 + eo )*st_vol;    
    //e = e + de;
    EPS->sete( e + de );
    
    //double ec = (EPS->getec()) - (EPS->getLam()) * log( p / (EPS->getpo()) );
    double ec = getec_ref() - getLambda() * log( p / getp_ref() );    
    double xi = EPS->gete() + de - ec;
    EPS->setpsi( xi );
    //EPS->setScalarVar(3, e); // e also stored in scalar array's 3nd cell for XC::PS
    //std::cerr << " ++st_vol = " << st_vol << " de = " << de << " e = " << e << " p = " << p << " ec = " << ec << " xi " << xi;
    
    //double Do = EPS->getScalarVar(2);
    //std::cerr << " !!!!! Do = " << Do;
    //if ( dLamda < 0)
    //std::cerr << " dLamda = " << dLamda << std::endl;

    //Just moved here!!!!!
       double m = EPS->getScalarVar(1);
       XC::stresstensor n;
       XC::stresstensor S = (EPS->getStress()).deviator();
       XC::stresstensor alpha = EPS->getTensorVar( 1 );  // alpha_ij

       XC::stresstensor r = S * (1.0 / p);
       //r.reportshort("r");
       XC::stresstensor r_bar = r - alpha;
       XC::stresstensor norm2 = r_bar("ij") * r_bar("ij");
       double norm = sqrt( norm2.trace() );
       
       if ( norm >= d_macheps()){ 
         n = r_bar *(1.0 / norm );
       }
       else {
         std::cerr << "XC::EvolutionLaw_L_EijMD::dFods  |n_ij| = 0, divide by zero! Program exits.\n";
         err += 1;
       	 exit(-1);
       }       	         
       //wrong Joey 02-10-03
       //n = r_bar *(1.0 / sqrt23rd / m );
       
       //if ( dLamda > 0 ) 
          EPS->setTensorVar(2, n); //n_ij is also stored in XC::Tensor array for 2nd derivative eval in XC::PS  //
       //std::cerr << " alpha = " << alpha;
       //std::cerr << " n = " << n;
       //std::cerr << "\n **** n =     " << n;

      //Updating D and m if dLamda != 0.0
      // Should also update D no matter dLamda is 0 or not.
      //if (dLamda != 0.0)
      //{

       //=========================================================================
       //calculate  n_ij



       //Calculate the state parameters xi 

       // if (p < 0.0)
       // {
       //    //p = 0.1;
       // 	  std::clog << "XC::EvolutionLaw_NL_EijMD::updateEeDm   p < 0, Program exits.";
       //    exit(-1);
       // }
       
       //double ec = ( EPS->getec() ) - (EPS->getLam()) * log( p/( EPS->getpo() ) );
       //double xi = ( EPS->gete() ) - ec;
       
       //  //=========================================================================
       //  // Update m
       //  double m = EPS->getScalarVar(1);
       //  double Cm = getCm(); 
       //  double eo = geteo(); 
       //  double dm = dLamda * Cm * ( 1.0 + eo ) * D;
       //  m = m - dm;
       //  EPS->setScalarVar(1, m);
       //  //std::cerr  << std::endl << "dm = " << dm << "m = " << m << std::endl;

       //=========================================================================
       // Update F
       XC::stresstensor dF;
       XC::stresstensor F = EPS->getTensorVar( 3 );   // getting  F_ij from XC::EPState
       double D = getD();
       //if (D != EPS->getScalarVar(2)) {
       //  g3ErrorHandler->warning("XC::EvolutionLaw_L_EijMD::updateEeDm  D values contradict:%f %f ", D, EPS->getScalarVar(2));
       //	 err += 1;
       //  //exit(-1);
       //}
       
       if ( D > 0.0 ) D = 0.0;
       if (dLamda < 0) dLamda = 0;
       dF =  ( n("ij") * getFmax()  + F("ij") ) * dLamda * getCf() * (-1.0*D);
       //std::cerr << "dF" << dF;
       F = F - dF;
       EPS->setTensorVar(3, F);
       this->F = F;
       //std::cerr << " \n F =          " << F << std::endl;       

       BJtensor temp_tensor = F("ij") * n("ij");
       double temp = temp_tensor.trace();
       if (temp < 0)   temp = 0;
       double A = getAo()*(1.0 + temp);
       //double A = getAo();
       
       //  //Calculating the lode angle theta
       //  double J2_bar = r_bar.Jinvariant2();
       //  double J3_bar = r_bar.Jinvariant3();
       //  double tempd = 3.0*pow(3.0, 0.5)/2.0*J3_bar/ pow( J2_bar, 1.5);
       //  
       //  if (tempd > 1.0 ) tempd = 1.0; //bug. if tempd = 1.00000000003, acos gives nan
       //  if (tempd < -1.0 ) tempd = -1.0;
       //  
       double theta = r_bar.theta( );
       //std::cerr << "Update... theta = " << theta/3.1416*180 << std::endl;
       
       //Calculate alpha_theta_d
       double c = getMe() / getMc();
       
       double cd = getke_d() / getkc_d();
       
       //Testing g_WW
       //std::cerr << "g_A(0, c) " << g_A(0.0, 0.0167) << std::endl;
       //std::cerr << "g_A(60, c) " << g_A(1.0472, 0.0167) << std::endl;
       double alpha_theta_dd = g_A(theta, c) * Mc + g_A(theta, cd) * getkc_d() * xi - m;
       XC::stresstensor alpha_theta_d = n("ij") * alpha_theta_dd * sqrt23rd;
       ////std::cerr << " cd "<< cd << " c " << c << "alpha_theta_d_norm " << alpha_theta_dd << std::endl;
       //std::cerr << " alpha_theta_d " << alpha_theta_d; //<<" g_A(theta, cd) "<< g_A(theta, cd) << " Mc " << Mc << std::endl;
       //std::cerr << " alpha = " << alpha << "\n";
       		        
       XC::stresstensor d;
       d =  alpha_theta_d - alpha;
       d.null_indices();
       //EPS->setTensorVar(2, d); //d is also stored in XC::Tensor array for 2nd derivative eval in XC::PS  //no use since F is gone.
       //std::cerr << " d = "  << d << " n = "  << n << std::endl;

       //=========================================================================
       // Updating D --does not depend on dLamda
       BJtensor temp1 = d("ij") * n("ij");
       temp1.null_indices();
       double dn =  temp1.trace();
       
       // Restrictions 
       if ( xi > 0.0 && dn < 0.0) {
         std::cerr << "Restriction !";
         dn = 0.0;
       }
       
       double newD = dn * A;
       EPS->setScalarVar(2, newD); // D also stored in scalar array's 2nd cell for XC::PS
       this->D = newD;
       //std::cerr << " dn = "  << dn << " ** D = " << newD << std::endl << std::endl;
       ////std::cerr << "alpha_theta_d " << alpha_theta_d<<" g_WW(theta, c) "<< g_WW(theta, c);
    
    //}
    
    return err;
}   


//================================================================================
//  Set initial value of D once the current stress hit the yield surface     	
//  for MD Hardening model only    						
//================================================================================
//void XC::EvolutionLaw_L_EijMD::setInitD(EPState  *EPS) 
//
//void XC::EvolutionLaw_NL_EijMD::updateDF(EPState  *EPS) 
//{
//
//}

//================================================================================
// Private accessory functions

//================================================================================
double XC::EvolutionLaw_NL_EijMD::geta() const 
{
    return a;
}

//================================================================================
double XC::EvolutionLaw_NL_EijMD::getMc() const 
{
    return Mc;
}

//================================================================================
double XC::EvolutionLaw_NL_EijMD::getMe() const 
{
    return Me;
}


//================================================================================
double XC::EvolutionLaw_NL_EijMD::getLambda() const
{
    return Lambda;
}

//================================================================================
double XC::EvolutionLaw_NL_EijMD::getec_ref() const
{
    return ec_ref;
}

//================================================================================
double XC::EvolutionLaw_NL_EijMD::getp_ref() const 
{
    return p_ref;
}

//================================================================================
double XC::EvolutionLaw_NL_EijMD::getkc_b() const
{
    return kc_b;
}

//================================================================================
double XC::EvolutionLaw_NL_EijMD::getkc_d() const  
{
    return kc_d;
}

//================================================================================
double XC::EvolutionLaw_NL_EijMD::getke_b() const
{
    return ke_b;
}

//================================================================================
double XC::EvolutionLaw_NL_EijMD::getke_d() const
{
    return ke_d;
}

//================================================================================
//double XC::EvolutionLaw_NL_EijMD::geth() const
//{       
//    return h;
//}

//================================================================================
double XC::EvolutionLaw_NL_EijMD::getho() const
{      
    return ho;
}

//================================================================================
double XC::EvolutionLaw_NL_EijMD::getCm() const
{       
    return Cm;
}

//================================================================================
double XC::EvolutionLaw_NL_EijMD::geteo() const
{       
    return eo;
}

////================================================================================
//double XC::EvolutionLaw_NL_EijMD::gete() const
//{       
//    return e;
//}


//================================================================================
double XC::EvolutionLaw_NL_EijMD::getAo() const
{       
    return Ao;
}

//================================================================================
double XC::EvolutionLaw_NL_EijMD::getD() const
{       
    return D;
}


//================================================================================
const XC::stresstensor &XC::EvolutionLaw_NL_EijMD::getF() const
  { return F; }

//================================================================================
double XC::EvolutionLaw_NL_EijMD::getFmax() const
{       
    return Fmax;
}

//================================================================================
double XC::EvolutionLaw_NL_EijMD::getCf() const
{       
    return Cf;
}


//================================================================================
//  Interpolation function No.1  -- Agyris: g_A(theta, e) 
//================================================================================

double XC::EvolutionLaw_NL_EijMD::g_A(double theta, double e) 
{
    double temp = 2.0 * e;
    temp  = temp / ( (1.0 + e) - (1.0 - e) * cos(3.0*theta) ); 

    return temp;
}


//================================================================================
//  Interpolation function No.2  -- Willan-Warkne: g_WW(theta, e)
//================================================================================
//g_WW(60, 0.0167) =  +3.3818e-02?????

double XC::EvolutionLaw_NL_EijMD::g_WW(double theta, double e) 
{
    //Rotate 60 degree to produce 1 at 0.0, c at 60 degrees
    theta = theta - 3.1415926/3;
    double g1 = 4.0*( 1.0 - e*e ) * cos(theta) * cos(theta) + pow(2.0*e-1.0, 2.0);
    double d1 = 2.0*( 1.0 - e*e ) * cos( theta );
    double d2 = ( 2.0*e-1.0 ) * pow( (4.0*(1.0-e*e)*cos(theta)*cos(theta) + 5.0*e*e - 4.0*e), 0.5);
    double temp =( d1 + d2 ) / g1; 

    return temp;
}

#endif

