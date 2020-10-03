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
//# PURPOSE:           Cam Clay yield criterion                          	 #
//#                      (Ref. Wood p113)                    			 #
//# CLASS:             CamClayYieldSurface(for monotonic loading)                    #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# PROGRAMMER(S):     Boris Jeremic, ZHaohui Yang                               #
//#                                                                              #
//#                                                                              #
//# DATE:              Mar. 28, 2001                                             #
//# UPDATE HISTORY:                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//================================================================================


#ifndef CamClayYieldSurface_CPP
#define CamClayYieldSurface_CPP

#include "material/nD/Template3Dep/CamClayYieldSurface.h"
#include "utility/matrix/nDarray/basics.h"
#include "material/nD/Template3Dep/EPState.h"


//================================================================================
// Normal constructor
//================================================================================

XC::CamClayYieldSurface::CamClayYieldSurface(double Mp )  
{
   M = Mp;
} 


//! @brief Virtual constructor.
XC::YieldSurface *XC::CamClayYieldSurface::getCopy(void)
  { return new CamClayYieldSurface(*this); }

//================================================================================
// Copy constrstructor
//================================================================================
//
//XC::CamClayYieldSurface::CamClayYieldSurface(CamClayYieldSurface &) { }
//


//================================================================================
//  Yield criterion evaluation function F(EPState)
//================================================================================

double XC::CamClayYieldSurface::f(const XC::EPState *EPS) const
{
  double p =EPS->getStress().p_hydrostatic();
  double q =EPS->getStress().q_deviatoric();
	   
  double po = EPS->getScalarVar(1);
  double temp3 = q*q - M*M*p*(po - p);

  //printf("\n========Inside f = %.4f \n ", temp3);

  return temp3;
}


//================================================================================
// BJtensor dF/dsigma_ij
//================================================================================

XC::BJtensor XC::CamClayYieldSurface::dFods(const XC::EPState *EPS) const {
  
   BJtensor dFoverds( 2, def_dim_2, 0.0);
   BJtensor I2("I", 2, def_dim_2);

  double p = EPS->getStress().p_hydrostatic();
  double q = EPS->getStress().q_deviatoric();
  double po = EPS->getScalarVar( 1 );
	 BJtensor DpoDs = EPS->getStress().dpoverds();
	 BJtensor DqoDs = EPS->getStress().dqoverds();

  double dFoverdp = -1.0*M*M*( po - 2.0*p );
  double dFoverdq = 2.0*q;

  dFoverds = DpoDs  * dFoverdp +
             DqoDs  * dFoverdq;

  return dFoverds;

}


//================================================================================
// double xi_s1 = dF/dpo = -M*M*p  Derivative in terms of first scalar var 
//================================================================================

double XC::CamClayYieldSurface::xi_s1( const XC::EPState *EPS ) const {

    //double p = EPS->getStress().Iinvariant1()/3.0;
    double p = EPS->getStress().p_hydrostatic();
    return -1.0 * M * M * p;

}


// moved to XC::stresstensor Boris Jeremic@ucdavis.edu 21Aug2001
// // I took these functions from mmodel.cpp, programmed by Dr. Jeremic
// //#############################################################################
// BJtensor XC::CamClayYieldSurface::dpoverds( ) const
//   {
//     BJtensor ret(2, def_dim_2, 0.0);
//     BJtensor I2("I", 2, def_dim_2);
//     ret = I2*(-1.0/3.0);
//     ret.null_indices();
//     return ret;
//   }
// 
// //#############################################################################
// BJtensor XC::CamClayYieldSurface::dqoverds(const XC::EPState *EPS) const
//   {
//     
//     XC::stresstensor stress = EPS->getStress();
//     
//     BJtensor ret(2, def_dim_2, 0.0);
//     double q = stress.q_deviatoric();
//     XC::stresstensor s( 0.0);
// //...    double J2D = stress.Jinvariant2();
// //...    double temp1 = sqrt(J2D);
// //...    double temp2 = sqrt(3.0)/(2.0*temp1);
//     double temp2 = (3.0/2.0)*(1/q);
//     s = stress.deviator();
//     ret = s*temp2;
//     ret.null_indices();
//     return ret;
//   }
// 
// //#############################################################################
// BJtensor XC::CamClayYieldSurface::dthetaoverds(const XC::EPState *EPS) const
//   {
//     XC::stresstensor stress = EPS->getStress();
// 
//     BJtensor ret(2, def_dim_2, 0.0);
//     XC::stresstensor s( 0.0);
//     XC::stresstensor t( 0.0);
//     BJtensor I2("I", 2, def_dim_2);
// 
// //    double EPS = pow(d_macheps(),(1./3.));
//     double J2D = stress.Jinvariant2();
//     double q     = stress.q_deviatoric();
//     double theta = stress.theta();
// 
// //out    while ( theta >= 2.0*PI )
// //out      theta = theta - 2.0*PI; // if bigger than full cycle
// //out    while ( theta >= 4.0*PI/3.0 )
// //out      theta = theta - 4.0*PI/3.0; // if bigger than four thirds of half cycle
// //out    while ( theta >= 2.0*PI/3.0 )
// //out      theta = theta - 2.0*PI/3.0; // if bigger than two third of half cycle
// //out    while ( theta >= PI/3.0 )
// //out      theta = 2.0*PI/3.0 - theta; // if bigger than one third of half cycle
// //out
// //out    if ( theta < 0.0001 )
// //out      {
// //out        ::printf("theta = %.10e in XC::CamClayYieldSurface::dthetaoverds(stress)\n",
// //out                           theta);
// //out//>><<>><<>><<        return ret;
// //out      }
// 
//     double c3t = cos(3.0*theta);
//     double s3t = sin(3.0*theta);
// 
//     double tempS = (3.0/2.0)*(c3t/(q*q*s3t));
//     double tempT = (9.0/2.0)*(1.0/(q*q*q*s3t));
// 
//     s = stress.deviator();
//     t = s("qk")*s("kp") - I2*(J2D*(2.0/3.0));
// 
//     s.null_indices();
//     t.null_indices();
// 
//     ret = s*tempS - t*tempT;
//     ret.null_indices();
//     return ret;
// }

//================================================================================
double XC::CamClayYieldSurface::getM() const
{       
    return M;
}


std::ostream& XC::operator<<(std::ostream &os, const XC::CamClayYieldSurface &YS)
  {
    os << "Cam Clay Yield Surface Parameters: " << std::endl;
    os << "M = " << YS.getM() << std::endl;
    return os;
  }

#endif

