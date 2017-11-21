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

#ifndef fdEvolution_TL_CPP
#define fdEvolution_TL_CPP

#include "material/nD/FiniteDeformation/fdEvolution/fdEvolution_TL.h"

using namespace XC;

//------------------------------------------------------------------------
XC::fdEvolution_TL::fdEvolution_TL(double H_linear_in)
:H_linear(H_linear_in)
{
    
}

//------------------------------------------------------------------------
 XC::fdEvolution_T * XC::fdEvolution_TL::newObj() 
{   
    fdEvolution_T *newEL = new fdEvolution_TL( *this );    
    return newEL;
}

//------------------------------------------------------------------------
 XC::BJtensor XC::fdEvolution_TL::HModulus(const XC::stresstensor &sts, const XC::FDEPState &fdepstate) const 
{
   BJtensor eta = fdepstate.getStrainLikeKiVar();
   BJtensor I2("I", 2 , def_dim_2);
   BJtensor I4 = I2("ij")*I2("kl"); I4.null_indices();
    //I4 = (I4.transpose0110()+I4.transpose0111())*0.5; //For symmetric XC::BJtensor
    I4 = I4.transpose0110();  //For general XC::BJtensor
    return I4*H_linear;
}

//------------------------------------------------------------------------
void XC::fdEvolution_TL::print()
{
    std::cerr << (*this);
}

//------------------------------------------------------------------------
std::ostream& XC::operator<< (std::ostream& os, const XC::fdEvolution_TL & fdetl)
{
   os.precision(5);
   os.width(10);
   os << "XC::Tensor XC::Linear Evolution Law's Modulus: " << "\n";
   
   return os;
}

#endif
