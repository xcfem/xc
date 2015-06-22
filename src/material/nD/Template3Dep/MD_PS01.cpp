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

//================================================================================
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite XC::Element Program                    #
//# PURPOSE:           Manzari-Dafalias  potential surface 01(with Pc)           #
//#                    (Ref. Geotechnique v.47 No.2 255-272, 1997)               #
//# CLASS:             MDPotentialSurface01                                      #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# PROGRAMMER(S):     Boris Jeremic, ZHaohui Yang                               #
//#                                                                              #
//#                                                                              #
//# DATE:              August 03 '93                                             #
//# UPDATE HISTORY:    August 08 '00                                             #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//================================================================================


#ifndef MD_PS01_CPP
#define MD_PS01_CPP

#include "material/nD/Template3Dep/MD_PS01.h"
#include <utility/matrix/nDarray/basics.h>
#include <utility/matrix/nDarray/BJtensor.h>
#include "material/nD/Template3Dep/EPState.h"
#include <cmath>

//================================================================================
// Normal constructor
//================================================================================

XC::MDPotentialSurface01::MDPotentialSurface01(double pc ) 
{
  Pc = pc;
} 


//================================================================================
//create a colne of itself
//================================================================================

XC::PotentialSurface * XC::MDPotentialSurface01::newObj() {  

     PotentialSurface  *new_PS = new MDPotentialSurface01(Pc);
     return new_PS;

}

//================================================================================
//  BJtensor dQ/dsigma_ij: the normal to the potential surface
//================================================================================

XC::BJtensor XC::MDPotentialSurface01::dQods(const XC::EPState *EPS) const { 

  BJtensor dQoverds( 2, def_dim_2, 0.0);
  BJtensor I2("I", 2, def_dim_2);

  BJtensor S = EPS->getStress().deviator();
  double p = EPS->getStress().p_hydrostatic();
  p = p -  Pc;

  BJtensor alpha = EPS->getTensorVar( 1 ); //the first XC::BJtensor hardening var is alpha_ij

  //double m = EPS->getScalarVar( 1 );	 //the first scalar hardening var is m
  double D = EPS->getScalarVar( 2 );	 // D is stored in scalar var array's second cell
  //printf("Here we go!  D= %e\n", D);

  BJtensor r = S *(1.0/p);
  BJtensor temp_f1 = r - alpha;
  BJtensor temp_f2 = temp_f1("ij") * temp_f1("ij");
  double temp_f3 = sqrt( temp_f2.trace() );

  XC::stresstensor n;
  if ( temp_f3 >= d_macheps() ){ 
    n = temp_f1 * (1.0 / temp_f3 );
  }
  else {
    std::cerr << "XC::MDPotentialSurface01::dQods  |n_ij| = 0, divide by zero! Program exits.\n";
    exit(-1);
    //::printf(" \n\n n_ij not defined!!!! Program exits\n");
    //exit(1);
  }
  
  dQoverds =  n + I2 * (D *(1.0/3.0));

  return dQoverds;
}


//================================================================================
// BJtensor d2Q/dsigma_ij2: the second derivatives to the potential surface
//================================================================================

XC::BJtensor XC::MDPotentialSurface01::d2Qods2(const XC::EPState *EPS) const 
{

  BJtensor d2Qoverds2;
  BJtensor I2("I", 2, def_dim_2);

  XC::stresstensor stress = EPS->getStress();
  
  //double J2D = stress.Jinvariant2();
  //double q     = stress.q_deviatoric();
  double theta = stress.theta();

  BJtensor S = stress.deviator();
  double p = stress.p_hydrostatic();
  p = p -  Pc;
  std::cerr << " p = " << p << std::endl;
  
  BJtensor alpha = EPS->getTensorVar( 1 ); //the first XC::BJtensor hardening var is alpha_ij
	   
  BJtensor r = S *(1.0/p);
  BJtensor temp_f1 = r - alpha;
  BJtensor temp_f2 = temp_f1("ij") * temp_f1("ij");
  double temp_f3 = sqrt( temp_f2.trace() );

  XC::stresstensor n;
  if ( temp_f3 >= d_macheps() ){ 
    n = temp_f1 * (1.0 / temp_f3 );
  }
  else {
    std::cerr << "XC::MDPotentialSurface01::dQods  |n_ij| = 0, divide by zero! Program exits.\n";
    exit(-1);
    //::printf(" \n\n n_ij not defined!!!! Program exits\n");
    //exit(1);
  }
  
  
  //BJtensor d2Qoverds2( 2, def_dim_2, 0.0); // dummy second derivatives. To be redefined.
  BJtensor dnds =  dnods( EPS);
  double A = 2.64;
  double Mc = 1.14;
  double kcd = 4.2;

  if (p < 0.0)
  {
    std::cerr << "XC::MDPotentialSurface01::d2Qods2  p < 0, Program exits.\n";
     exit(-1);
  }
  double ec = 0.80 - 0.025 * log( p / 160 );

  double e = EPS->getScalarVar(3);
  double xi = e - ec;

  double c = 1.0;
  double cd = 0.0167;

  double dgodthetac  = dgoverdt(theta, c);
  double dgodthetacd = dgoverdt(theta, cd);
  BJtensor dthetaods = dthetaoverds(EPS);
  //XC::stresstensor d = EPS->getTensorVar( 3 );   // getting  d_ij from XC::EPState
  
  //BJtensor dtods = dthetaods("pq"); 

  BJtensor dDds1 = A*dthetaods*(dgodthetac*Mc+dgodthetacd*kcd*xi)*sqrt(2.0/3.0); 
  BJtensor dDds2 = A*apqdnods(EPS); 
  BJtensor dDds = dDds1 - dDds2;
  dDds.null_indices(); 
   
  d2Qoverds2 = dnds + I2("pq")*dDds("mn")*(0.33333333);
  d2Qoverds2.null_indices();

  return d2Qoverds2;
}

//================================================================================
// BJtensor dn_pq/dsigma_mn
//================================================================================
XC::BJtensor XC::MDPotentialSurface01::dnods(const XC::EPState *EPS) const
{
  BJtensor dnods( 2, def_dim_2, 0.0);
  BJtensor I2("I", 2, def_dim_2);

  XC::stresstensor S = EPS->getStress().deviator();
  //S.reportshort("S");
  XC::stresstensor alpha = EPS->getTensorVar( 1 );

  double p = EPS->getStress().p_hydrostatic();
  p = p -  Pc;

  //double m = EPS->getScalarVar( 1 );
  //double fac = 1.0/(sqrt(2.0/3.0)*m);

  BJtensor Ipmnq = I2("pm") * I2("nq");
  BJtensor Imnpq = I2("mn") * I2("pq");
  BJtensor Apqmn = alpha("pq") * I2("mn");
  BJtensor X  = Ipmnq - Imnpq * (1.0/3.0) - Apqmn * (1.0/3.0);

  //Ipmnq.print("in XC::MD_PS01", "");
  
  BJtensor sa = S("ij") * alpha("ij");
  double sad =sa.trace();
  BJtensor aa = alpha("ij") * alpha("ij");
  double aad =aa.trace();
  BJtensor Y = S - ( p + 0.333333333*(sad-p*aad) )*I2;
  Y.null_indices();

  BJtensor s_bar = S("ij") - p*alpha("ij");
  s_bar.null_indices();
  BJtensor norm2 = s_bar("ij") * s_bar("ij");
  double norm =  norm2.trace();

  s_bar.null_indices();
  BJtensor tmp = s_bar("pq")*Y("mn");
  dnods = ( X - 2.0 * tmp)*(1.0/norm);
  
  return  dnods;
}

//================================================================================
// BJtensor alpha_pq*dnods
//================================================================================
XC::BJtensor XC::MDPotentialSurface01::apqdnods(const XC::EPState *EPS) const
{
  BJtensor ddnods( 2, def_dim_2, 0.0);
  BJtensor I2("I", 2, def_dim_2);

  XC::stresstensor S = EPS->getStress().deviator();
  //S.reportshort("S");

  double p = EPS->getStress().p_hydrostatic();
  p = p -  Pc;

  //printf("Here we go!  p %f\n", p);
	      
  XC::stresstensor alpha = EPS->getTensorVar( 1 );
  //XC::stresstensor d = EPS->getTensorVar( 3 );   // getting  d_ij from XC::EPState

  BJtensor akk = alpha("ij") * I2("ij");
  double akkd =akk.trace();

  BJtensor aa = alpha("ij") * alpha("ij");
  double aad =aa.trace();

  BJtensor aX = alpha - I2*(akkd +aad)*0.3333333;
	         
  //Ymn
  BJtensor sa = S("ij") * alpha("ij");
  double sad =sa.trace();
  BJtensor Y = S - ( p + 0.333333333*(sad-p*aad) )*I2;
  Y.null_indices();
  
  BJtensor s_bar = S - p*alpha;
  s_bar.null_indices();

  BJtensor as_bar = alpha("pq") * s_bar("pq");
  double as_bard = as_bar.trace();
  s_bar.null_indices();
  
  //Norm
  BJtensor norm2 = s_bar("ij") * s_bar("ij");
  double norm = norm2.trace();
  
	   	     
  ddnods = (aX - Y*2.0*as_bard)*(1.0/norm);
  return  ddnods;
}


//================================================================================
// BJtensor dg(theta, c)/dsigma_mn
//================================================================================
double XC::MDPotentialSurface01::dgoverdt(double theta, double c) const
{
   //XC::stresstensor stress = EPS->getStress();
  
   //double J2D = stress.Jinvariant2();
   //double q     = stress.q_deviatoric();
   //double theta = stress.theta();
  
   double temp = (-6*(1 - c)*c*sin(3*theta))/pow(1 + c - (1 - c)*cos(3*theta),2);  
   return temp;
}

//================================================================================
// BJtensor dtheta/dsigma_pq
//================================================================================
XC::BJtensor XC::MDPotentialSurface01::dthetaoverds(const XC::EPState *EPS) const
{
   BJtensor ret(2, def_dim_2, 0.0);
   XC::stresstensor s( 0.0);
   XC::stresstensor t( 0.0);
   BJtensor I2("I", 2, def_dim_2);

   //double EPS = pow(d_macheps(),(1./3.));
   XC::stresstensor stress = EPS->getStress();
  
   double J2D = stress.Jinvariant2();
   double q     = stress.q_deviatoric();
   double theta = stress.theta();

   //out    while ( theta >= 2.0*PI )
   //out      theta = theta - 2.0*PI; // if bigger than full cycle
   //out    while ( theta >= 4.0*PI/3.0 )
   //out      theta = theta - 4.0*PI/3.0; // if bigger than four thirds of half cycle
   //out    while ( theta >= 2.0*PI/3.0 )
   //out      theta = theta - 2.0*PI/3.0; // if bigger than two third of half cycle
   //out    while ( theta >= PI/3.0 )
   //out      theta = 2.0*PI/3.0 - theta; // if bigger than one third of half cycle
   //out
   //out    if ( theta < 0.0001 )
   //out      {
   //out        ::printf("theta = %.10e in XC::Material_Model::dthetaoverds(stress)\n",
   //out                           theta);
   //out//>><<>><<>><<        return ret;
   //out      }
   
   double c3t = cos(3.0*theta);
   double s3t = sin(3.0*theta);

   double tempS = (3.0/2.0)*(c3t/(q*q*s3t));
   double tempT = (9.0/2.0)*(1.0/(q*q*q*s3t));

   s = stress.deviator();
   t = s("qk")*s("kp") - I2*(J2D*(2.0/3.0));

   s.null_indices();
   t.null_indices();

   ret = s*tempS - t*tempT;
   ret.null_indices();
   return ret;
}

std::ostream& XC::operator<< (std::ostream& os, const XC::MDPotentialSurface01 &PS)
  {
    os << "Manzari-Dafalias Potential Surface 01( with Pc) Parameters: " << std::endl;
    os << "Pc = " << PS.Pc << std::endl;
    return os;
  }



#endif

