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
//# DATE:              19AUg2003
//# UPDATE HISTORY:    Sept 2003
//#		       28May2004
//#
//===============================================================================

#ifndef SimoPisterWEnergy_CPP
#define SimoPisterWEnergy_CPP

#include "SimoPisterWEnergy.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/nDarray/Tensor.h>

//================================================================================
// Normal constructor
//================================================================================
XC::SimoPisterWEnergy::SimoPisterWEnergy(const double &K_in)
  :K(K_in) {}


//================================================================================
//! @brief Constructor virtual
//================================================================================
XC::WEnergy *XC::SimoPisterWEnergy::getCopy(void) const
  { return new SimoPisterWEnergy(*this); }


//================================================================================
// w
//================================================================================
const double  XC::SimoPisterWEnergy::wE( const double &J_in, const XC::Vector &lambda_wave_in )
{
    double wEnergy = 0.25 * K * (J_in*J_in - 1.0 - 2.0*log(J_in));
    return wEnergy;
}

//================================================================================
// d(vol)w / dJ
//================================================================================
const double  XC::SimoPisterWEnergy::dvolwOdJ(const double &J_in )
{
   double dcolwOverdJ = K * (-2.0 / J_in + 2.0 * J_in) * 0.25;
   return dcolwOverdJ;
}

//================================================================================
// d2(vol)w / dJ2
//================================================================================
const double  XC::SimoPisterWEnergy::d2volwOdJ2(const double &J_in )
{
   double d2colwOverdJ2 = K * (2.0 / J_in / J_in + 2.0) * 0.25;
   return d2colwOverdJ2;
}


#endif

