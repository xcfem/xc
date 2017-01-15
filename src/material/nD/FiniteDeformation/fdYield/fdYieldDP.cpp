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
//===============================================================================
//# COPYRIGHT (C): Woody's license (by BJ):
//                 ``This    source  code is Copyrighted in
//                 U.S.,  for  an  indefinite  period,  and anybody
//                 caught  using it without our permission, will be
//                 mighty good friends of ourn, cause we don't give
//                 a  darn.  Hack it. Compile it. Debug it. Run it.
//                 Yodel  it.  Enjoy it. We wrote it, that's all we
//                 wanted to do.''
//
//# PROJECT:           Object Oriented Finite XC::Element Program
//# PURPOSE:           Finite Deformation Hyper-Elastic classes
//# CLASS:
//#
//# VERSION:           0.6_(1803398874989) (golden section)
//# LANGUAGE:          C++
//# TARGET OS:         all...
//# DESIGN:            Zhao Cheng, Boris Jeremic (jeremic@ucdavis.edu)
//# PROGRAMMER(S):     Zhao Cheng, Boris Jeremic
//#
//#
//# DATE:              July 2004
//# UPDATE HISTORY:
//#
//===============================================================================

#ifndef fdYieldDP_CPP
#define fdYieldDP_CPP

#include "material/nD/FiniteDeformation/fdYield/fdYieldDP.h"
#include "material/nD/FiniteDeformation/FDEPState.h"


//--------------------------------------------------------------------
XC::fdYieldDP::fdYieldDP(double FricAngle_in, double Cohesion_in, int ConeIndex_in)
 : FricAngle(FricAngle_in), Cohesion(Cohesion_in), ConeIndex(ConeIndex_in)
{   
   double pipi = 3.14159265358979323846;
   double Root3 = sqrt(3.0);
   double Root1o3 = 1.0/Root3;
   double archAngle = FricAngle*pipi/180.0;
   
   switch (ConeIndex) {

     case 0: { // Compressive (Outer) Cone
       k1 = 2.0*Root1o3*sin(archAngle)/(3.0-sin(archAngle));
       k2 = 6.0*Root1o3*cos(archAngle)/(3.0-sin(archAngle));
       break;
     }

     case 1: { // Tensile (Inner) Cone
       k1 = 2.0*Root1o3*sin(archAngle)/(3.0+sin(archAngle));
       k2 = 6.0*Root1o3*cos(archAngle)/(3.0+sin(archAngle));
       break;
     }

     case 2: { // Mean  Cone
       k1 = Root3*sin(archAngle)/(9.0-sin(archAngle)*sin(archAngle));
       k2 = 2.0*Root3*cos(archAngle)/(9.0-sin(archAngle)*sin(archAngle));
       break;
     }

     case 3: { // Inner-tangent  Cone
       k1 = tan(archAngle)/sqrt(9.0+12.0*tan(archAngle)*tan(archAngle));
       k2 = 3.0/sqrt(9.0+12.0*tan(archAngle)*tan(archAngle));
       break;
     }   

     default: { // Compressive (Outer) Cone
       k1 = 2.0*Root1o3*sin(archAngle)/(3.0-sin(archAngle));
       k2 = 6.0*Root1o3*cos(archAngle)/(3.0-sin(archAngle));
     }
     
   }
}

//--------------------------------------------------------------------
 XC::fdYield * XC::fdYieldDP::newObj()
{
    fdYield *newfdyd = new XC::fdYieldDP(FricAngle, Cohesion, ConeIndex);
    return newfdyd;
}

int XC::fdYieldDP::getNumRank()
{
    //return 1;
    return 2;
}

double XC::fdYieldDP::getTolerance()
{
    //return 1.0e-8*Cohesion > 1.0e-8?  1.0e-8*Cohesion : 1.0e-8;
    double tol = 1.0e-8*Cohesion;
    return (tol*tol) > 1.0e-8?  (tol*tol) : 1.0e-8;
}

//--------------------------------------------------------------------------------------------
// Yd =  k1*I1 + sqrt(0.5*Sij*Sij) - k2*(c+q) = 0, 
// Note here NumRank = 1: No Kinematic hardening
// Yd = (3/2)(sij-p_bar*alpha_ij)(sij-p_bar*alpha_ij) - M^2*(p_bar)^2 = 0, p_bar = p + c0/tan() + c
// Note here NumRank = 2: with Kinematic hardening
//--------------------------------------------------------------------------------------------

double XC::fdYieldDP::Yd(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
{    
    //// NumRank=1, No Ki Hardeing
    //double q = fdepstate.getStressLikeInVar();
    //XC::stresstensor dev = sts.deviator();
    //double I1 = sts.Iinvariant1();
    //BJtensor st = dev("ij")*dev("ij");
    //  st.null_indices();
    //double x = st.trace();
    //return k1*I1 + sqrt(0.5*x) - k2*(Cohesion+q);

    // NumRank=2, with Ki Hardeing
    double pipi = 3.14159265358979323846;
    double archAngle = FricAngle*pipi/180.0;
    double root3 = sqrt(3.0);
    double M = 3.0*root3*k1;
    double cv = fdepstate.getStressLikeInVar();
    if ( tan(archAngle) < 1.0e-4 ) {
      std::cerr << "fdYieldDP-- (close) zero or invalid friction angle\n";
      exit (-1);      
    }
    double pa = Cohesion/tan(archAngle) + cv;
    XC::stresstensor a = fdepstate.getStressLikeKiVar();
    XC::stresstensor dev = sts.deviator();
    double p = sts.p_hydrostatic();
    double pb = p + pa;
    XC::stresstensor w  = dev - a *pb;
    BJtensor st = w("ij")*w("ij");
      st.null_indices();
    double x = st.trace();

    return 1.5*x - M*M*pb*pb;
}

//--------------------------------------------------------------------
XC::stresstensor XC::fdYieldDP::dYods(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
{   
    //// NumRank=1, No Ki Hardeing
    //BJtensor tI2("I", 2, def_dim_2);
    //XC::stresstensor dev = sts.deviator();
    //BJtensor st = dev("ij")*dev("ij");  
    //  st.null_indices();
    //double x = st.trace();
    //return tI2*k1 + dev*(1.0/sqrt(2.0*x));

    // NumRank=2, with Ki Hardeing
    double pipi = 3.14159265358979323846;
    double archAngle = FricAngle*pipi/180.0;
    BJtensor tI2("I", 2, def_dim_2);
    double root3 = sqrt(3.0);
    double M = 3.0*root3*k1;
    double cv = fdepstate.getStressLikeInVar();
    if ( tan(archAngle) < 1.0e-4 ) {
      std::cerr << "fdYieldDP-- (close) zero or invalid friction angle\n";
      exit (-1);      
    }
    double pa = Cohesion/tan(archAngle) + cv;
    XC::stresstensor a = fdepstate.getStressLikeKiVar();
    XC::stresstensor dev = sts.deviator();
    double p = sts.p_hydrostatic();
    double pb = p + pa;
    BJtensor aijaij = a("ij")*a("ij");
      aijaij.null_indices();
    double aa = aijaij.trace();
    XC::stresstensor w  = dev - a *pb;
    BJtensor wijaij = w("ij")*a("ij");
      wijaij.null_indices();
    double wa = wijaij.trace();
    return w*3.0 + tI2*(pb*aa +wa + (2.0/3.0)*M*M*pb);
}					      

//--------------------------------------------------------------------
double XC::fdYieldDP::dYodq(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
{      
    //// NumRank=1, No Ki Hardeing
    //return -k2;

    // NumRank=2, with Ki Hardeing
    double pipi = 3.14159265358979323846;
    double archAngle = FricAngle*pipi/180.0;
    double root3 = sqrt(3.0);
    double M = 3.0*root3*k1;
    double cv = fdepstate.getStressLikeInVar();
    if ( tan(archAngle) < 1.0e-4 ) {
      std::cerr << "fdYieldDP-- (close) zero or invalid friction angle\n";
      exit (-1);      
    }
    double pa = Cohesion/tan(archAngle) + cv;
    double p = sts.p_hydrostatic();
    double pb = p + pa;
    XC::stresstensor a = fdepstate.getStressLikeKiVar();
    XC::stresstensor dev = sts.deviator();
    XC::stresstensor w  = dev - a *pb;
    BJtensor wijaij = w("ij")*a("ij");
      wijaij.null_indices();
    double wa = wijaij.trace();

    return wa*(-3.0) + pb*(-2.0*M*M);
}

//--------------------------------------------------------------------
XC::stresstensor XC::fdYieldDP::dYoda(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
{   
    // NumRank=2, with Ki Hardeing
    double pipi = 3.14159265358979323846;
    double archAngle = FricAngle*pipi/180.0;
    double cv = fdepstate.getStressLikeInVar();
    if ( tan(archAngle) < 1.0e-4 ) {
      std::cerr << "fdYieldDP-- (close) zero or invalid friction angle\n";
      exit (-1);      
    }
    double pa = Cohesion/tan(archAngle) + cv;
    double p = sts.p_hydrostatic();
    double pb = p + pa;
    XC::stresstensor a = fdepstate.getStressLikeKiVar();
    XC::stresstensor dev = sts.deviator();
    XC::stresstensor w  = dev - a *pb;

    return w*(-3.0*pb);
}
	
//--------------------------------------------------------------------
std::ostream& XC::operator<<(std::ostream& os, const XC::fdYieldDP &fdydDP)
{
    os << "fdYieldDP Parameters: " << "\n";
    os << "FricAngle: " << fdydDP.FricAngle << "\n";
    os << "Cohesion: " << fdydDP.Cohesion << "\n";
    os << "ConeIndex: " << fdydDP.ConeIndex << "\n";
    return os;
}

void XC::fdYieldDP::print(void)
  { std::cerr << *this; }

#endif

