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

//================================================================================
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite XC::Element Program                    #
//# PURPOSE:           Manzari-Dafalias  potential surface 01(with Pc)           #
//#                    (Ref. Geotechnique v.47 No.2 255-272, 1997)               #
//# CLASS:             ManzariDafaliasPotentialSurface01                                      #
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


#ifndef ManzariDafaliasPotentialSurface01_CPP
#define ManzariDafaliasPotentialSurface01_CPP

#include "material/nD/Template3Dep/ManzariDafaliasPotentialSurface01.h"
#include <utility/matrix/nDarray/basics.h>
#include <utility/matrix/nDarray/BJtensor.h>
#include "material/nD/Template3Dep/EPState.h"
#include <cmath>

//================================================================================
// Normal constructor
//================================================================================

XC::ManzariDafaliasPotentialSurface01::ManzariDafaliasPotentialSurface01(double pc ) 
{
  Pc = pc;
} 

//! @brief Virtual constructor.
XC::PotentialSurface * XC::ManzariDafaliasPotentialSurface01::getCopy(void)
  { return new ManzariDafaliasPotentialSurface01(*this); }

//================================================================================
//  BJtensor dQ/dsigma_ij: the normal to the potential surface
//================================================================================

XC::BJtensor XC::ManzariDafaliasPotentialSurface01::dQods(const XC::EPState *EPS) const
  { 
    BJtensor dQoverds( def_dim_2, 0.0);
    BJtensor I2("I", def_dim_2);

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

    stresstensor n;
    if ( temp_f3 >= d_macheps() ){ 
      n = temp_f1 * (1.0 / temp_f3 );
    }
    else {
      std::cerr << "XC::ManzariDafaliasPotentialSurface01::dQods  |n_ij| = 0, divide by zero! Program exits.\n";
      exit(-1);
      //::printf(" \n\n n_ij not defined!!!! Program exits\n");
      //exit(1);
    }

    dQoverds =  n +  stresstensor((D *(1.0/3.0))*I2);

    return dQoverds;
  }


//================================================================================
// BJtensor d2Q/dsigma_ij2: the second derivatives to the potential surface
//================================================================================

XC::BJtensor XC::ManzariDafaliasPotentialSurface01::d2Qods2(const XC::EPState *EPS) const 
{

  BJtensor d2Qoverds2;
  BJtensor I2("I", def_dim_2);

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
    std::cerr << "XC::ManzariDafaliasPotentialSurface01::dQods  |n_ij| = 0, divide by zero! Program exits.\n";
    exit(-1);
    //::printf(" \n\n n_ij not defined!!!! Program exits\n");
    //exit(1);
  }
  
  
  //BJtensor d2Qoverds2( def_dim_2, 0.0); // dummy second derivatives. To be redefined.
  BJtensor dnds =  dnods( EPS);
  double A = 2.64;
  double Mc = 1.14;
  double kcd = 4.2;

  if (p < 0.0)
  {
    std::cerr << "XC::ManzariDafaliasPotentialSurface01::d2Qods2  p < 0, Program exits.\n";
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
XC::BJtensor XC::ManzariDafaliasPotentialSurface01::dnods(const XC::EPState *EPS) const
  {
    BJtensor dnods( def_dim_2, 0.0);
    BJtensor I2("I", def_dim_2);

    stresstensor S = EPS->getStress().deviator();
    //S.reportshort("S");
    stresstensor alpha = EPS->getTensorVar( 1 );

    double p = EPS->getStress().p_hydrostatic();
    p = p -  Pc;

    //double m = EPS->getScalarVar( 1 );
    //double fac = 1.0/(sqrt(2.0/3.0)*m);

    BJtensor Ipmnq = I2("pm") * I2("nq");
    BJtensor Imnpq = I2("mn") * I2("pq");
    BJtensor Apqmn = alpha("pq") * I2("mn");
    BJtensor X  = Ipmnq - Imnpq * (1.0/3.0) - Apqmn * (1.0/3.0);

    //Ipmnq.print("in XC::ManzariDafaliasPotentialSurface01", "");

    const BJtensor sa = S("ij") * alpha("ij");
    const double sad =sa.trace();
    const BJtensor aa = alpha("ij") * alpha("ij");
    const double aad =aa.trace();
    BJtensor Y(S - ( p + 0.333333333*(sad-p*aad) )*static_cast<const stresstensor &>(I2));
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
XC::BJtensor XC::ManzariDafaliasPotentialSurface01::apqdnods(const XC::EPState *EPS) const
  {
    BJtensor ddnods( def_dim_2, 0.0);
    BJtensor I2("I", def_dim_2);

    XC::stresstensor S = EPS->getStress().deviator();
    //S.reportshort("S");

    double p = EPS->getStress().p_hydrostatic();
    p = p -  Pc;

    //printf("Here we go!  p %f\n", p);

    stresstensor alpha = EPS->getTensorVar( 1 );
    //stresstensor d = EPS->getTensorVar( 3 );   // getting  d_ij from XC::EPState

    BJtensor akk = alpha("ij") * I2("ij");
    double akkd =akk.trace();

    BJtensor aa = alpha("ij") * alpha("ij");
    double aad =aa.trace();

    BJtensor aX(alpha - static_cast<const stresstensor &>(I2)*(akkd +aad)*0.3333333);

    //Ymn
    BJtensor sa = S("ij") * alpha("ij");
    const double sad =sa.trace();
    BJtensor Y(S - ( p + 0.333333333*(sad-p*aad) )*static_cast<const stresstensor &>(I2));
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
double XC::ManzariDafaliasPotentialSurface01::dgoverdt(double theta, double c) const
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
XC::BJtensor XC::ManzariDafaliasPotentialSurface01::dthetaoverds(const XC::EPState *EPS) const
{
   BJtensor ret(def_dim_2, 0.0);
   stresstensor s( 0.0);
   stresstensor t( 0.0);
   BJtensor I2("I", def_dim_2);

   //double EPS = pow(d_macheps(),(1./3.));
   stresstensor stress = EPS->getStress();
  
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

std::ostream& XC::operator<<(std::ostream &os, const XC::ManzariDafaliasPotentialSurface01 &PS)
  {
    os << "Manzari-Dafalias Potential Surface 01( with Pc) Parameters: " << std::endl;
    os << "Pc = " << PS.Pc << std::endl;
    return os;
  }



#endif

