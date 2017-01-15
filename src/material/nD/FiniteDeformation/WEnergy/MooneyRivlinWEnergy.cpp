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
//# UPDATE HISTORY:
//#
//#
//===============================================================================


#ifndef MooneyRivlinWEnergy_CPP
#define MooneyRivlinWEnergy_CPP

#include "MooneyRivlinWEnergy.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/nDarray/Tensor.h>

//================================================================================
// Normal constructor
//================================================================================
XC::MooneyRivlinWEnergy::MooneyRivlinWEnergy( double c1_in,  double c2_in)
 : c1(c1_in), c2(c2_in)
{

}

XC::MooneyRivlinWEnergy::MooneyRivlinWEnergy( )
 : c1(0.0), c2(0.0)
{

}

//================================================================================
//create a clone of itself
//================================================================================
XC::WEnergy * XC::MooneyRivlinWEnergy::getCopy(void) const
  { return new MooneyRivlinWEnergy(*this); }


//================================================================================
// w
//================================================================================
const double XC::MooneyRivlinWEnergy::wE(const double &J_in, const XC::Vector &lambda_wave_in)
  {
    double temp1 = lambda_wave_in(0) * lambda_wave_in(0)
                           + lambda_wave_in(1) * lambda_wave_in(1)
                           + lambda_wave_in(2) * lambda_wave_in(2) - 3.0;
    double temp2 = 1.0 / lambda_wave_in(0) / lambda_wave_in(0)
                           + 1.0 / lambda_wave_in(1) / lambda_wave_in(1)
                           + 1.0 / lambda_wave_in(2) / lambda_wave_in(2) - 3.0;
    double w_iso = c1 * temp1 + c2 * temp2;
    return w_iso;
  }

//================================================================================
// d(iso)w / d(lambda)
//================================================================================
const XC::Vector XC::MooneyRivlinWEnergy::disowOdlambda(const XC::Vector &lambda_wave_in)
  {
        XC::Vector disowOverdlambda(3);
        disowOverdlambda(0) = 2.0 * c1 *lambda_wave_in(0) - 2.0 * c2 * pow(lambda_wave_in(0), -3.0);
        disowOverdlambda(1) = 2.0 * c1 *lambda_wave_in(1) - 2.0 * c2 * pow(lambda_wave_in(1), -3.0);
        disowOverdlambda(2) = 2.0 * c1 *lambda_wave_in(2) - 2.0 * c2 * pow(lambda_wave_in(2), -3.0);
    return disowOverdlambda;
  }

//================================================================================
// d2(iso)w / d(lambda)2
//================================================================================
const XC::Vector XC::MooneyRivlinWEnergy::d2isowOdlambda2(const XC::Vector &lambda_wave_in)
  {
        XC::Vector d2isowOverdlambda2(3);
        d2isowOverdlambda2(0) = 2.0 * c1  + 6.0 * c2 * pow(lambda_wave_in(0), -4.0);
        d2isowOverdlambda2(1) = 2.0 * c1  + 6.0 * c2 * pow(lambda_wave_in(1), -4.0);
        d2isowOverdlambda2(2) = 2.0 * c1  + 6.0 * c2 * pow(lambda_wave_in(2), -4.0);
    return d2isowOverdlambda2;
  }


#endif

