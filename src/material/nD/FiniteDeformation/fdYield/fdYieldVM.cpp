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

#include "material/nD/FiniteDeformation/fdYield/fdYieldVM.h"
#include "material/nD/FiniteDeformation/FDEPState.h"


//--------------------------------------------------------------------
XC::fdYieldVM::fdYieldVM(double Y0_in) :Y0(Y0_in)
  {}

//! @brief Virtual constructor.
XC::fdYield * XC::fdYieldVM::getCopy(void)
  { return new fdYieldVM(*this); }

int XC::fdYieldVM::getNumRank()
  {
    //return 1;
    return 2;
  }

double XC::fdYieldVM::getTolerance()
{
    double tol = (Y0*(1.0e-8)) * (Y0*(1.0e-8));
    return tol > 1.0e-8?  tol : 1.0e-8;
    //return Y0*1.0e-8 > 1.0e-8? Y0*1.0e-8 : 1.0e-8;
}

//--------------------------------------------------------------------------------------
// Yd =  3.0*(J2) - (Y0+q)*(Y0+q) = 0, Note here NumRank = 2: No Kinematic hardening
// Yd =  |S_ij| - sqrt(2/3)*(Y0+q) = 0, Note here NumRank = 1: No Kinematic hardening
// Yd =  1.5 * (S_ij - a_ij)*(S_ij-a_ij) - (Y0+q)*(Y0+q) = 0, Note here NumRank = 2
//--------------------------------------------------------------------------------------

double XC::fdYieldVM::Yd(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
{
    //// NumRank=2, No Ki Hardeing
    //double J2 = sts.Jinvariant2();
    //double q = fdepstate.getStressLikeInVar();
    //return 3.0*J2 - (Y0+q)*(Y0+q);
    
    //// NumRank=1, No Ki Hardeing
    //return sqrt(2.0*J2) - sqrt(2.0/3.0);
    
    // NumRank=2, With Ki Hardeing
    stresstensor a = fdepstate.getStressLikeKiVar();
    double q = fdepstate.getStressLikeInVar();
    stresstensor dev = sts.deviator() - a;
    BJtensor st = dev("ij")*dev("ij");   
      st.null_indices();
    double x = st.trace();
    return 1.5*x - (Y0+q)*(Y0+q);
}

//--------------------------------------------------------------------
XC::stresstensor XC::fdYieldVM::dYods(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
{   
    //// NumRank=2, No Ki Hardeing
    //return sts.deviator() * 3.0;

    //// NumRank=1, No Ki Hardeing
    //double J2 = sts.Jinvariant2();
    //return sts.deviator()/(sqrt(8.0*J2);

    // NumRank=2, With Ki Hardeing
    stresstensor a = fdepstate.getStressLikeKiVar();
    stresstensor dev = sts.deviator() - a;
    stresstensor retval(dev *3.0);
    return retval;
  }					      

//--------------------------------------------------------------------
double XC::fdYieldVM::dYodq(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
  {  
    //// NumRank=2, No Ki Hardeing
    //double q = fdepstate.getStressLikeInVar();
    //return -2.0 * (Y0+q);
    
    //// NumRank=1, No Ki Hardeing
    //return sqrt(2.0/3.0);

    // NumRank=2, With Ki Hardeing
    double q = fdepstate.getStressLikeInVar();
    return -2.0 * (Y0+q);
  }

//--------------------------------------------------------------------
XC::stresstensor XC::fdYieldVM::dYoda(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
{   
    //// NumRank=2, No Ki Hardeing
    //return sts.deviator() * 3.0;

    //// NumRank=1, No Ki Hardeing
    //double J2 = sts.Jinvariant2();
    //return sts.deviator()/(sqrt(8.0*J2);

    // NumRank=2, With Ki Hardeing
    stresstensor a = fdepstate.getStressLikeKiVar();
    stresstensor dev = sts.deviator() - a;
    stresstensor retval(dev *(-3.0));
    return retval;
  }	

//--------------------------------------------------------------------
std::ostream& XC::operator<<(std::ostream &os, const XC::fdYieldVM &fdydVM)
  {
    os << "fdYieldVM Parameters: " << "\n";
    os << "Y0: " << fdydVM.Y0 << "\n";
    return os;
  }

void XC::fdYieldVM::print(void)
  { std::cerr << *this; }


