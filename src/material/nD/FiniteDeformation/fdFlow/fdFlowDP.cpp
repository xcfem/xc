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

#include "material/nD/FiniteDeformation/fdFlow/fdFlowDP.h"
#include "material/nD/FiniteDeformation/FDEPState.h"
#include <cmath>
#include <iostream>

const double pipi= 3.14159265358979323846;

XC::fdFlowDP::fdFlowDP(double DilatedAngle_in, double Cohesion_in, int ConeIndex_in)
:DilatedAngle(DilatedAngle_in), Cohesion(Cohesion_in), ConeIndex(ConeIndex_in)
  {
   const double Root3 = sqrt(3.0);
   const double Root1o3 = 1.0/Root3;
   const double archAngle = DilatedAngle*pipi/180.0; 

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

//! @brief Virtual constructor.
XC::fdFlow *XC::fdFlowDP::getCopy(void)
  { return new fdFlowDP(*this); }

//-------------------------------------------------------------------
// Q = k1*I1 + sqrt(0.5*Sij*Sji) - k2*(c+q) = 0, 
// Note here NumRank = 1, No Kinematic Hardening
// Yd = (3/2)(sij-p_bar*alpha_ij)(sij-p_bar*alpha_ij) - M^2*(p_bar)^2 = 0, p_bar = p + c0/tan() + c
// Note here NumRank = 2: with Kinematic hardening
//-------------------------------------------------------------------

//--------------------------------------------------------------------
XC::stresstensor XC::fdFlowDP::dFods(const XC::stresstensor &sts, const XC::FDEPState &fdepstate) const
  {    
    // NumRank=1, No Ki Hardeing
    //BJtensor tI2("I", 2 , def_dim_2);
    //XC::stresstensor dev = sts.deviator();
    //BJtensor st = dev("ij")*dev("ij");  
    //  st.null_indices();
    //double x = st.trace();
    //return tI2*k1 + dev *(1.0/sqrt(2.0*x));

    // NumRank=2, with Ki Hardeing
    double archAngle = DilatedAngle*pipi/180.0;
    BJtensor tI2("I", 2, def_dim_2);
    double root3 = sqrt(3.0);
    double M = 3.0*root3*k1;
    double cv = fdepstate.getStressLikeInVar();
    if( tan(archAngle) < 1.0e-4 )
      {
        std::cerr << "fdFlowDP::" << __FUNCTION__
	          << "; (close) zero or invalid dilation angle, try VM flow rule\n";
        exit (-1);      
      }
    double pa = Cohesion/tan(archAngle) + cv;
    stresstensor a = fdepstate.getStressLikeKiVar();
    stresstensor dev = sts.deviator();
    double p = sts.p_hydrostatic();
    double pb = p + pa;
    BJtensor aijaij = a("ij")*a("ij");
      aijaij.null_indices();
    double aa = aijaij.trace();
    stresstensor w(dev - a *pb);
    BJtensor wijaij = w("ij")*a("ij");
      wijaij.null_indices();
    const double wa = wijaij.trace();
    stresstensor retval(w*3.0 + stresstensor(tI2*(pb*aa +wa + (2.0/3.0)*M*M*pb)));
    return retval;
  }

//--------------------------------------------------------------------
double XC::fdFlowDP::dFodq(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
{  
    // NumRank=1, No Ki Hardeing
    //return -k2;

    // NumRank=2, with Ki Hardeing
    double archAngle = DilatedAngle*pipi/180.0;
    double root3 = sqrt(3.0);
    double M = 3.0*root3*k1;
    double cv = fdepstate.getStressLikeInVar();
    if ( tan(archAngle) < 1.0e-4 ) {
      std::cerr << "fdFlowDP-- (close) zero or invalid dilation angle, try VM flow rule\n";
      exit (-1);      
    }
    double pa = Cohesion/tan(archAngle) + cv;
    double p = sts.p_hydrostatic();
    double pb = p + pa;
    stresstensor a = fdepstate.getStressLikeKiVar();
    stresstensor dev = sts.deviator();
    stresstensor w(dev - a *pb);
    BJtensor wijaij = w("ij")*a("ij");
      wijaij.null_indices();
    double wa = wijaij.trace();

    return wa*(-3.0) + pb*(-2.0*M*M);
}

//--------------------------------------------------------------------
XC::stresstensor XC::fdFlowDP::dFoda(const XC::stresstensor &sts, const XC::FDEPState &fdepstate) const
{    
    // NumRank=2, with Ki Hardeing
    double archAngle = DilatedAngle*pipi/180.0;
    double cv = fdepstate.getStressLikeInVar();
    if( tan(archAngle) < 1.0e-4 )
      {
        std::cerr << "fdFlowDP::" << __FUNCTION__
	          << "; (close) zero or invalid dilation angle, try VM flow rule\n";
        exit (-1);      
      }
    double pa = Cohesion/tan(archAngle) + cv;
    stresstensor a = fdepstate.getStressLikeKiVar();
    stresstensor dev = sts.deviator();
    double p = sts.p_hydrostatic();
    double pb = p + pa;
    stresstensor w(dev - a*pb);
    stresstensor retval(w*(-3.0*pb));
    return retval;
  }

//--------------------------------------------------------------------
XC::BJtensor XC::fdFlowDP::d2Fodsds(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
{  
    // NumRank=1, No Ki Hardeing
    //BJtensor tI2("I", 2, def_dim_2);
    //XC::stresstensor dev = sts.deviator();
    //BJtensor st = dev("ij")*dev("ij")*2.0;  
    //  st.null_indices();
    //double x = st.trace();
    //BJtensor I4 = tI2("ij")*tI2("kl"); 
    //  I4.null_indices();
    //I4 = I4 - I4*(1.0/3.0);  
    //BJtensor st4 = dev("ij")*dev("kl");
    //  st4.null_indices();
    //
    //return I4 *(1.0/sqrt(x)) - st4 *(2.0/x/sqrt(x));

    // NumRank=2, with Ki Hardeing
    double archAngle = DilatedAngle*pipi/180.0;
    BJtensor tI2("I", 2, def_dim_2);
    double root3 = sqrt(3.0);
    double M = 3.0*root3*k1;
    double cv = fdepstate.getStressLikeInVar();
    if ( tan(archAngle) < 1.0e-4 ) {
      std::cerr << "fdFlowDP-- (close) zero or invalid dilation angle, try VM flow rule\n";
      exit (-1);      
    }
    double pa = Cohesion/tan(archAngle) + cv;
    stresstensor a = fdepstate.getStressLikeKiVar();
    stresstensor dev = sts.deviator();
    double p = sts.p_hydrostatic();
    double pb = p + pa;
    stresstensor w(dev - a *pb);
    double akk = a.Iinvariant1();
    BJtensor aijaij = a("ij")*a("ij");
      aijaij.null_indices();
    double aa = aijaij.trace();
    BJtensor tI40 = tI2("ij")*tI2("kl");
      tI40.null_indices();	
    BJtensor tI41 = tI40.transpose0110(); // for non-symmetric
    BJtensor atI = a("ij")*tI2("kl");
      atI.null_indices();
    BJtensor tIa = tI2("ij")*a("kl");
      tIa.null_indices();

    return tI41*3.0 + tI40*((aa- akk)/3.0 - M*M*2.0/9.0 -1.0) + atI + tIa;    
}

//--------------------------------------------------------------------
XC::stresstensor XC::fdFlowDP::d2Fodsdq(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
  {
    // NumRank=2, with Ki Hardeing
    BJtensor tI2("I", 2, def_dim_2);
    double root3 = sqrt(3.0);
    double M = 3.0*root3*k1;
    stresstensor a = fdepstate.getStressLikeKiVar();
    double akk = a.Iinvariant1();
    BJtensor aijaij = a("ij")*a("ij");
      aijaij.null_indices();
    const double aa = aijaij.trace();
    stresstensor retval(a*(-3.0) + stresstensor(tI2*(akk - aa + M*M*2.0/3.0)));
    return retval; 
  }

//--------------------------------------------------------------------
XC::BJtensor XC::fdFlowDP::d2Fodsda(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
{  
    // NumRank=2, with Ki Hardeing
    double archAngle = DilatedAngle*pipi/180.0;
    BJtensor tI2("I", 2, def_dim_2);
    double cv = fdepstate.getStressLikeInVar();
    if ( tan(archAngle) < 1.0e-4 ) {
      std::cerr << "fdFlowDP-- (close) zero or invalid dilation angle, try VM flow rule\n";
      exit (-1);      
    }
    double pa = Cohesion/tan(archAngle) + cv;
    stresstensor a = fdepstate.getStressLikeKiVar();
    stresstensor dev = sts.deviator();
    double p = sts.p_hydrostatic();
    double pb = p + pa;
    stresstensor w(dev - a *pb);
    BJtensor tI40 = tI2("ij")*tI2("kl");
      tI40.null_indices();	
    BJtensor tI41 = tI40.transpose0110(); // for non-symmetric
    BJtensor wtI = w("ij")*tI2("kl");
      wtI.null_indices();
    BJtensor atI = a("ij")*tI2("kl");
      atI.null_indices();

    return tI41*(-3.0*pb) + tI40*pb + atI*(-pb) + wtI;    
}
    
//--------------------------------------------------------------------
double XC::fdFlowDP::d2Fodqdq(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
{
    // NumRank=2, with Ki Hardeing
    double root3 = sqrt(3.0);
    double M = 3.0*root3*k1;
    stresstensor a = fdepstate.getStressLikeKiVar();
    BJtensor aijaij = a("ij")*a("ij");
      aijaij.null_indices();
    double aa = aijaij.trace();

    return aa*3.0 - M*M*2.0; 
}

//--------------------------------------------------------------------
XC::stresstensor XC::fdFlowDP::d2Fodqda(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
{
    // NumRank=2, with Ki Hardeing
    double archAngle = DilatedAngle*pipi/180.0;
    BJtensor tI2("I", 2, def_dim_2);
    double cv = fdepstate.getStressLikeInVar();
    if ( tan(archAngle) < 1.0e-4 )
      {
        std::cerr << "fdFlowDP::" << __FUNCTION__
		  << "; (close) zero or invalid dilation angle, try VM flow rule\n";
        exit (-1);      
      }
    double pa = Cohesion/tan(archAngle) + cv;
    stresstensor a = fdepstate.getStressLikeKiVar();
    stresstensor dev = sts.deviator();
    double p = sts.p_hydrostatic();
    double pb = p + pa;
    stresstensor w(dev - a*pb);
    stresstensor retval(a*(3.0*pb) - w*3.0);
    return retval; 
  }

XC::BJtensor XC::fdFlowDP::d2Fodada(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
{  
    // NumRank=2, with Ki Hardeing
    double archAngle = DilatedAngle*pipi/180.0;
    BJtensor tI2("I", 2, def_dim_2);
    double cv = fdepstate.getStressLikeInVar();
    if ( tan(archAngle) < 1.0e-4 ) {
      std::cerr << "fdFlowDP-- (close) zero or invalid dilation angle, try VM flow rule\n";
      exit (-1);      
    }
    double pa = Cohesion/tan(archAngle) + cv;
    stresstensor a = fdepstate.getStressLikeKiVar();
    stresstensor dev = sts.deviator();
    double p = sts.p_hydrostatic();
    double pb = p + pa;
    BJtensor tI40 = tI2("ij")*tI2("kl");
      tI40.null_indices();	
    BJtensor tI41 = tI40.transpose0110(); // for non-symmetric

    return tI41*(3.0*pb*pb);
}

//--------------------------------------------------------------------
std::ostream& XC::operator<<(std::ostream &os, const XC::fdFlowDP &fdfdDP)
  {
    os << "fdFlowDP Parameters: " << "\n";
    os << "DilatedAngle: " << fdfdDP.DilatedAngle << "\n";
    os << "Cohesion: " << fdfdDP.Cohesion << "\n";
    os << "ConeIndex: " << fdfdDP.ConeIndex << "\n";
    return os;
  }

void XC::fdFlowDP::print(void)
  { std::cerr << *this; }



