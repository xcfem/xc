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

//================================================================================
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite XC::Element Program                    #
//# PURPOSE:           Manzari-Dafalias  potential surface                       #
//#                    (Ref. Geotechnique v.47 No.2 255-272, 1997)               #
//# CLASS:             ManzariDafaliasPotentialSurface                                        #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# PROGRAMMER(S):     Boris Jeremic, ZHaohui Yang                               #
//#                                                                              #
//#                                                                              #
//# DATE:              
//# UPDATE HISTORY:    
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//================================================================================


#ifndef ManzariDafaliasPotentialSurface_CPP
#define ManzariDafaliasPotentialSurface_CPP

#include "material/nD/Template3Dep/ManzariDafaliasPotentialSurface.h"
#include <utility/matrix/nDarray/basics.h>
#include <cmath>
#include "material/nD/Template3Dep/EPState.h"
#include <utility/matrix/nDarray/BJtensor.h>

#define sqrt23rd 0.8165

//================================================================================
// Normal constructor
//================================================================================

XC::ManzariDafaliasPotentialSurface::ManzariDafaliasPotentialSurface( ) { } 


//! @brief Virtual constructor.
XC::PotentialSurface * XC::ManzariDafaliasPotentialSurface::getCopy(void)
  { return new ManzariDafaliasPotentialSurface(*this); }

//! @brief tensor dQ/dsigma_ij: the normal to the potential surface
XC::BJtensor XC::ManzariDafaliasPotentialSurface::dQods(const XC::EPState *EPS) const
  { 
    BJtensor dQoverds( 2, def_dim_2, 0.0);
    BJtensor I2("I", 2, def_dim_2);

    BJtensor S = EPS->getStress().deviator();
    //double p = EPS->getStress().p_hydrostatic();

    BJtensor alpha = EPS->getTensorVar( 1 ); //the first XC::BJtensor hardening var is alpha_ij

    //double m = EPS->getScalarVar( 1 );	 //the first scalar hardening var is m
    const double D = EPS->getScalarVar( 2 );	 // D is stored in scalar var array's second cell

    stresstensor n = EPS->getTensorVar( 2 );
    //XC::stresstensor n;

    ////-------------------------------------------------
    //XC::stresstensor r = S *(1.0/p);
    //XC::stresstensor r_bar = r - alpha;
    //XC::stresstensor norm2 = r_bar("ij") * r_bar("ij");
    //double norm = sqrt( norm2.trace() );
    //if ( norm >= d_macheps() ){ 
    //  n = r_bar *(1.0 / norm );
    //}
    //else {
    //  std::cerr->fatal("XC::ManzariDafaliasYieldSurface::dQods |n_ij| = 0, divide by zero! Program exits.");
    //  exit(-1);
    //}
    //
    ////n = r_bar *(1.0 / sqrt23rd / m );
    ////-------------------------------------------------

    dQoverds =  n + stresstensor((D*(1.0/3.0))*I2);

    return dQoverds;
  }


//================================================================================
// BJtensor d2Q/dsigma_ij2: the second derivatives to the potential surface
//================================================================================

XC::BJtensor XC::ManzariDafaliasPotentialSurface::d2Qods2(const XC::EPState *EPS) const 
{

  
  BJtensor d2Qoverds2;
  BJtensor I2("I", 2, def_dim_2);

  XC::stresstensor stress = EPS->getStress();
  
  //double J2D = stress.Jinvariant2();
  //double q     = stress.q_deviatoric();
  double theta = stress.theta();

  BJtensor S = stress.deviator();
  double p = stress.p_hydrostatic();
  
  BJtensor alpha = EPS->getTensorVar( 1 ); //the first XC::BJtensor hardening var is alpha_ij
	   
  /*
  BJtensor r = S *(1.0/p);
  XC::stresstensor r_bar = r - alpha;
  XC::stresstensor norm2 = r_bar("ij") * r_bar("ij");
  double norm = sqrt( norm2.trace() );

  XC::stresstensor n;
  if ( norm >= d_macheps() ){ 
    n = r_bar *(1.0 / norm );
  }
  else {
    std::cerr << "XC::ManzariDafaliasPotentialSurface::dQods  |n_ij| = 0, divide by zero! Program exits.\n";
    exit(-1);
  }
  */
  
  //BJtensor d2Qoverds2( 2, def_dim_2, 0.0); // dummy second derivatives. To be redefined.
  BJtensor dnds =  dnods( EPS);
  double A = 2.64;
  double Mc = 1.14;
  double kcd = 4.2;
  //double m = EPS->getScalarVar( 1 );
  if (p < 0.0)
  {
     std::cerr << "XC::ManzariDafaliasPotentialSurface::d2Qods2  p < 0, Program exits.\n";
     exit(-1);
  }

  //double ec = 0.80 - 0.025 * log( p / 160 ); //p_ref = 160; (ec)ref = 0.8
  //double ec = (EPS->getec()) - (EPS->getLam()) * log( p / (EPS->getpo()) );
  //std::cerr << " ************" << log(2.718) << "\n";

  //double e = EPS->gete();
  double xi = EPS->getpsi();

  double c = 1.0;
  double cd = 0.0167; //0.7;

  double dgodthetac  = dgoverdt(theta, c);
  double dgodthetacd = dgoverdt(theta, cd);
  BJtensor dthetaods = dthetaoverds(EPS);
  //XC::stresstensor d = EPS->getTensorVar( 3 );   // getting  d_ij from XC::EPState
  
  //BJtensor dtods = dthetaods("pq"); 

  BJtensor dDds1 = dthetaods*A*(dgodthetac*Mc+dgodthetacd*kcd*xi)*sqrt(2.0/3.0);
  BJtensor dDds2 = apqdnods(EPS)*A;
  BJtensor dDds = dDds1 - dDds2;
  dDds.null_indices(); 
   
  d2Qoverds2 = dnds + dDds("mn")*I2("pq")*(1.0/3.0);
  d2Qoverds2.null_indices();
  

  BJtensor d2Qoverds2x( 4, def_dim_2, 0.0); // dummy second derivatives. To be redefined.
  return d2Qoverds2x;
  //return dnds;
}

//================================================================================
// BJtensor dn_pq/dsigma_mn
//================================================================================
XC::BJtensor XC::ManzariDafaliasPotentialSurface::dnods(const XC::EPState *EPS) const
{
  /*
  BJtensor xdnods;
  BJtensor I2("I", 2, def_dim_2);

  XC::stresstensor S = EPS->getStress().deviator();
  //S.reportshort("S");

  double p = EPS->getStress().p_hydrostatic();
  //printf("Here we go!  p %f\n", p);
  double m = EPS->getScalarVar( 1 );

  double fac = 1.0/(sqrt(2.0/3.0)*m);

  BJtensor Ipmnq = I2("pm") * I2("nq");
  BJtensor Imnpq = I2("mn") * I2("pq");
  BJtensor Spqmn = S("pq") * I2("mn");
  BJtensor temp  = Ipmnq - Imnpq*(1.0/3.0);
  
  xdnods = (temp*p - Spqmn*(1.0/3.0))*(fac/p/p);
  return  xdnods;
  */
  
  
  BJtensor dnods;
  BJtensor I2("I", 2, def_dim_2);

  stresstensor S = EPS->getStress().deviator();
  //S.reportshort("S");
  stresstensor alpha = EPS->getTensorVar( 1 );

  double p = EPS->getStress().p_hydrostatic();
  //p = p -  Pc;

  //double m = EPS->getScalarVar( 1 );
  //double fac = 1.0/(sqrt(2.0/3.0)*m);

  BJtensor Ipmnq = I2("pm") * I2("nq");
  BJtensor Imnpq = I2("mn") * I2("pq");
  BJtensor Apqmn = alpha("pq") * I2("mn");
  BJtensor X  = Ipmnq - Imnpq*(1.0/3.0)  - Apqmn*(1.0/3.0);

  //Ipmnq.print("in ManzariDafaliasPotentialSurface01", "");
  
  BJtensor s_bar = S("ij") - alpha("ij")*p;

  BJtensor ad(alpha - static_cast<const stresstensor &>(I2)); //Vlado found a bug
  BJtensor sa = S("ij") * ad("ij");
  double sad =sa.trace();
  BJtensor aa = alpha("ij") * ad("ij");
  double aad =aa.trace();
  BJtensor Y = s_bar +I2 * (1.0/3.0) * (sad-p*aad);
  Y.null_indices();

  s_bar.null_indices();
  BJtensor t_norm2 = s_bar("ij") * s_bar("ij");
  double norm2 = t_norm2.trace();
  double norm =  sqrt( norm2 );

  double norm21 = 1.0/( norm2 );
  s_bar.null_indices();
  BJtensor tmp = s_bar("pq")*Y("mn");
  dnods = ( X - tmp*norm21)*(1.0/norm);
  
  return  dnods;
  

}

//================================================================================
// BJtensor alpha_pq*dnods
//================================================================================
XC::BJtensor XC::ManzariDafaliasPotentialSurface::apqdnods(const XC::EPState *EPS) const
{
  /* old fomulation, not correct
  BJtensor ddnods( 2, def_dim_2, 0.0);
  BJtensor I2("I", 2, def_dim_2);

  XC::stresstensor S = EPS->getStress().deviator();
  //S.reportshort("S");

  double p = EPS->getStress().p_hydrostatic();
  //printf("Here we go!  p %f\n", p);
  double m = EPS->getScalarVar( 1 );

  double fac = 1.0/(sqrt(2.0/3.0)*m);

  XC::stresstensor d = EPS->getTensorVar( 3 );   // getting  d_ij from XC::EPState
  BJtensor dkk = d("ij") * I2("ij");
  double dkkd =dkk.trace();
  BJtensor dkkImn = dkkd * I2;

  BJtensor Spqdpq = S("pq") * d("pq");
  double Sqpdpqd =Spqdpq.trace();
  BJtensor dSI = Sqpdpqd*I2;
     
  BJtensor temp  = d  - (1.0/3.0)*dkkImn;
  
  ddnods = fac/p*(temp - 1.0/(p*3.0)*dSI);
  return  ddnods;
  */

  /*
  BJtensor ddnods( 2, def_dim_2, 0.0);
  BJtensor I2("I", 2, def_dim_2);

  XC::stresstensor S = EPS->getStress().deviator();
  //S.reportshort("S");

  double p = EPS->getStress().p_hydrostatic();
  //p = p -  Pc;

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
  BJtensor Y = S - I2*( p + 0.333333333*(sad-p*aad) );
  Y.null_indices();
  
  BJtensor s_bar = S - alpha*p;
  s_bar.null_indices();

  BJtensor as_bar = alpha("pq") * s_bar("pq");
  double as_bard = as_bar.trace();
  s_bar.null_indices();
  
  //Norm
  BJtensor norm2 = s_bar("ij") * s_bar("ij");
  double norm = norm2.trace();
  
	   	     
  ddnods = (aX - Y*2.0*as_bard)*(1.0/norm);
  return  ddnods;
  */

  BJtensor xdnods = dnods(EPS);
  XC::stresstensor alpha = EPS->getTensorVar( 1 );

  BJtensor adnods = alpha("pq") * xdnods("pqmn");

  return adnods;

}


//================================================================================
// BJtensor dg(theta, c)/dsigma_mn
//================================================================================
double XC::ManzariDafaliasPotentialSurface::dgoverdt(double theta, double c) const
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
XC::BJtensor XC::ManzariDafaliasPotentialSurface::dthetaoverds(const XC::EPState *EPS) const
  {
    BJtensor ret(2, def_dim_2, 0.0);
    stresstensor s( 0.0);
    stresstensor t( 0.0);
    BJtensor I2("I", 2, def_dim_2);

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




#endif

