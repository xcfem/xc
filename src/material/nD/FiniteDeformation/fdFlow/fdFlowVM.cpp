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

#ifndef fdFlowVM_CPP
#define fdFlowVM_CPP

#include "material/nD/FiniteDeformation/fdFlow/fdFlowVM.h"
#include "material/nD/FiniteDeformation/FDEPState.h"
#include <iostream>


//--------------------------------------------------------------------
XC::fdFlowVM::fdFlowVM(double Y0_in) :Y0(Y0_in)
{

}

//--------------------------------------------------------------------
XC::fdFlow * XC::fdFlowVM::newObj()
{
     fdFlow *newfdyd = new fdFlowVM(Y0);
     return newfdyd;
}

//-------------------------------------------------------------------
// Q = 1.5*(S_ij-a_ij)*(S_ij-a_ij)  - (Y0+q)*(Y0+q) = 0, Note here NumRank = 2
//-------------------------------------------------------------------

//--------------------------------------------------------------------
XC::stresstensor XC::fdFlowVM::dFods(const XC::stresstensor &sts, const XC::FDEPState &fdepstate) const
{    
    return sts.deviator() * 3.0;
}

//--------------------------------------------------------------------
double XC::fdFlowVM::dFodq(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
{  
    double q = fdepstate.getStressLikeInVar();
    return -2.0 * ( Y0+q );
}

//--------------------------------------------------------------------
XC::stresstensor XC::fdFlowVM::dFoda(const XC::stresstensor &sts, const XC::FDEPState &fdepstate) const
{    
    return sts.deviator() * (-3.0);
}

//--------------------------------------------------------------------
 XC::BJtensor XC::fdFlowVM::d2Fodsds(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
{  
    BJtensor I2("I", 2 , def_dim_2);
    BJtensor I4 = I2("ij")*I2("kl"); I4.null_indices();
    //I4 = (I4.transpose0110()+I4.transpose0111())*1.5 - I4; //For symmetric XC::BJtensor
    I4 = I4*3.0 - I4;	//For general XC::BJtensor
    return I4;
}

//--------------------------------------------------------------------
 XC::BJtensor XC::fdFlowVM::d2Fodsda(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
{  
    BJtensor I2("I", 2 , def_dim_2);
    BJtensor I4 = I2("ij")*I2("kl"); I4.null_indices();
    //I4 = (I4.transpose0110()+I4.transpose0111())*1.5 - I4; //For symmetric XC::BJtensor
    I4 = I4*3.0 - I4;	//For general XC::BJtensor
    return I4 *(-1.0);
}

//--------------------------------------------------------------------
double XC::fdFlowVM::d2Fodqdq(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
{  
    return -2.0;
}

//--------------------------------------------------------------------
 XC::BJtensor XC::fdFlowVM::d2Fodada(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
{  
    BJtensor I2("I", 2 , def_dim_2);
    BJtensor I4 = I2("ij")*I2("kl"); I4.null_indices();
    //I4 = (I4.transpose0110()+I4.transpose0111())*1.5; //For symmetric XC::BJtensor
    I4 = I4 *3.0;       //For general XC::BJtensor
    return I4;
}

//--------------------------------------------------------------------
std::ostream& XC::operator<<(std::ostream& os, const XC::fdFlowVM &fdflVM)
  {
    os << "fdFlowVM Parameters: " << "\n";
    os << "Y0: " << fdflVM.Y0 << "\n";
    return os;
  }

void XC::fdFlowVM::print(void)
  { std::cerr << *this; }

#endif

