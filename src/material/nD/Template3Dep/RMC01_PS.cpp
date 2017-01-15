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
//# COPY LEFT and RIGHT:                                                         #
//# Commercial    use    of    this  program without express permission of the   #
//# University  of  California, is strictly encouraged. Copyright and Copyleft   #
//# are covered by the following clause:                                         #
//#                                                                              #
//# Woody's license:                                                             #
//# ``This    source    code is Copyrighted in U.S., by the The Regents of the   #
//# University  of  California,  for  an indefinite period, and anybody caught   #
//# using  it  without  our  permission,  will be mighty good friends of ourn,   #
//# cause  we  don't give a darn. Hack it. Compile it. Debug it. Run it. Yodel   #
//# it. Enjoy it. We wrote it, that's all we wanted to do.'' bj                  #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//# PROJECT:           Object Oriented Finite XC::Element Program                    #
//# PURPOSE:           Rounded Mohr Coulomb Potential Surface                    #
//# CLASS:                                                                       #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++                                                       #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# DESIGNER(S):       Boris Jeremic jeremic@ucdavis.edu                         #
//#                    Zhao Cheng,                                               #
//# PROGRAMMER(S):     Zhao Cheng, Boris Jeremic                                 #
//#                                                                              #
//#                                                                              #
//# DATE:              12 Feb. 2003                                              #
//# UPDATE HISTORY:    Feb 25 2003                                               #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//################################################################################
//*/

#ifndef RMC01_PS_CPP
#define RMC01_PS_CPP

#include "material/nD/Template3Dep/RMC01_PS.h"
#include "material/nD/Template3Dep/RMC01.h"
#include "utility/matrix/nDarray/BJtensor.h"
#include "material/nD/Template3Dep/EPState.h"

//================================================================================
//create a clone of itself
//================================================================================

XC::PotentialSurface * XC::RMC01PotentialSurface::newObj()
  {
    PotentialSurface  *new_YS = new RMC01PotentialSurface();
    return new_YS;
  }



//================================================================================
// BJtensor dQ/dsigma_ij
//================================================================================

XC::BJtensor XC::RMC01PotentialSurface::dQods(const XC::EPState *EPS) const
{
  BJtensor dQoverds( 2, def_dim_2, 0.0);
//  double p = EPS->getStress().p_hydrostatic(); // p
  double q = EPS->getStress().q_deviatoric(); // q
  double theta = EPS->getStress().theta(); // theta
//  double temp_phi = EPS->getScalarVar(1)*3.14159265358979/180.0; // frictional angle
//  double temp_cohesive = EPS->getScalarVar(2); // cohesion
  BJtensor DpoDs = EPS->getStress().dpoverds(); // dp/ds
  BJtensor DqoDs = EPS->getStress().dqoverds(); // dq/ds
  BJtensor DthetaoDs = EPS->getStress().dthetaoverds(); // d(theta)/ds
  double alfa = EPS->getScalarVar(1); // Take alfa & k as internal variables
//  double k = EPS->getScalarVar(2);    // instead of phi & conhesive
  double a1 = (3.0*1.7320508076*alfa) / (2.0+1.7320508076*alfa);  
//  double a1 = -6*sin(temp_phi)/(3.0-sin(temp_phi));
//  double a2 = -6*temp_cohesive*cos(temp_phi)/(3.0-sin(temp_phi));
  double e = (3.0-a1)/(3.0+a1);
  double Frou = g_0(theta, e); // r(theta)
  double Frou_prime = g_prime(theta, e); // r'(theta)
//  double dQoverdp = a1; // dQ/dp
//  double dQoverdq = Frou; // dQ/dq
//  double dQoverdtheta = q*Frou_prime; // dQ/d(theta)
  double dQoverdp = alfa*(-3.0); // dQ/dp
  double dQoverdq = Frou/1.7320508076; // dQ/dq
  double dQoverdtheta = q*Frou_prime/1.7320508076; // dQ/d(theta)
  
  dQoverds = DpoDs  * dQoverdp +
             DqoDs  * dQoverdq +
             DthetaoDs * dQoverdtheta;

  return dQoverds;

}


//================================================================================
// BJtensor d2Q/dsigma_ij_2
//================================================================================

XC::BJtensor XC::RMC01PotentialSurface::d2Qods2( const XC::EPState *EPS ) const
{
  BJtensor d2Qoverds2( 4, def_dim_4, 0.0); // d2Q/ds2(pqmn)

//  double p = EPS->getStress().p_hydrostatic(); // p
  double q = EPS->getStress().q_deviatoric(); // q
  double theta = EPS->getStress().theta(); // theta
//  double temp_phi = EPS->getScalarVar(1)*3.14159265358979/180.0; // frictional angle
//  double temp_cohesive = EPS->getScalarVar(2); // conhesion
  double alfa = EPS->getScalarVar(1);
//  double k = EPS->getScalarVar(2);
  double a1 = (3.0*1.7320508076*alfa) / (2.0+1.7320508076*alfa);
  BJtensor DpoDs = EPS->getStress().dpoverds(); // dp/ds
  BJtensor DqoDs = EPS->getStress().dqoverds(); // dq/ds
  BJtensor DthetaoDs = EPS->getStress().dthetaoverds(); // d(theta)/ds
  BJtensor D2poDs2 = EPS->getStress().d2poverds2(); // d2p/ds2
  BJtensor D2qoDs2 = EPS->getStress().d2qoverds2(); // d2q/ds2
  BJtensor D2thetaoDs2 = EPS->getStress().d2thetaoverds2(); // d2(theta)/ds2
//  double a1 = -6*sin(temp_phi)/(3.0-sin(temp_phi));
//  double a2 = -6*temp_cohesive*cos(temp_phi)/(3.0-sin(temp_phi));
  double e = (3.0-a1)/(3.0+a1);
  double Frou = g_0(theta, e); // r(theta)
  double Frou_prime = g_prime(theta, e); // r'(theta)
  double Frou_second = g_second(theta, e); // r"(theta)
//  double dQoverdp = a1; // dQ/dp
//  double dQoverdq = Frou; // dQ/dq
//  double dQoverdtheta = q*Frou_prime; // dQ/d(theta)
  double dQoverdp = alfa*(-3.0); // dQ/dp
  double dQoverdq = Frou/1.7320508076; // dQ/dq
  double dQoverdtheta = q*Frou_prime/1.7320508076; // dQ/d(theta)
//  double a23 = Frou_prime; // d2Q/dqd(theta)
//  double a32 = a23; // d2Q/d(theta)dq
//  double a33 = q * Frou_second; // d2Q/d(theta)2
  double a23 = Frou_prime/1.7320508076; // d2Q/dqd(theta)
  double a32 = a23/1.7320508076; // d2Q/d(theta)dq
  double a33 = q * Frou_second/1.7320508076; // d2Q/d(theta)2  
  
//  d2Qoverds2 =  DthetaoDs("mn") * DqoDs("pq")     *  a23 +
//                DqoDs("mn") * DthetaoDs("pq")     *  a32 +
//                DthetaoDs("mn") * DthetaoDs("pq") *  a33 +
//                D2poDs2("pqmn")  * dQoverdp +
//                D2qoDs2("pqmn")  * dQoverdq +
//                D2thetaoDs2("pqmn") * dQoverdtheta;
  d2Qoverds2 =  DqoDs("pq") * DthetaoDs("mn")      *  a23 +
                DthetaoDs("pq") * DqoDs("mn")      *  a32 +
                DthetaoDs("pq") * DthetaoDs("mn")  *  a33 +
                D2poDs2("pqmn")  * dQoverdp +
                D2qoDs2("pqmn")  * dQoverdq +
                D2thetaoDs2("pqmn") * dQoverdtheta;
  return d2Qoverds2;

}

// For Consistent Algorithm, Z Cheng, Jan 2004
XC::BJtensor XC::RMC01PotentialSurface::d2Qodsds1(const XC::EPState *EPS) const 
{  
  BJtensor I("I", 2, def_dim_2);
  BJtensor d2Qoverdsds1 = I;
  return d2Qoverdsds1;
}

//================================================================================
// friend ostream functions for output
//================================================================================

std::ostream & XC::operator<<(std::ostream & os, const XC::RMC01PotentialSurface & PS)
  {
    os << "ROUNDED MC Potential Surface Parameters: " << std::endl;
    return os;
  }


#endif

