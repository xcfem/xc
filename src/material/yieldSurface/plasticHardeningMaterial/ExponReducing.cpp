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
// ExponReducing.cpp: implementation of the ExponReducing class.
//
//////////////////////////////////////////////////////////////////////

#include "ExponReducing.h"
#include <cstdlib>
#include <cmath>

#define MAT_TAG_EXPON -1
#define DEBG 0

//! Constructor.
XC::ExponReducing::ExponReducing(int tag)
  :PlasticHardeningMaterial(tag,MAT_TAG_EXPON), Kp0(0.0), alpha(0.0), resFactor(0.0)
  {}

//! Constructor.
XC::ExponReducing::ExponReducing(int tag, double kp0, double alfa)
:PlasticHardeningMaterial(tag,MAT_TAG_EXPON),
  Kp0(kp0), alpha(alfa), resFactor(0.0)
{
}

//! Constructor.
XC::ExponReducing::ExponReducing(int tag, double kp0, double alfa, double min_fact)
:PlasticHardeningMaterial(tag,MAT_TAG_EXPON),
  Kp0(kp0), alpha(alfa), resFactor(min_fact)
{
//	std::cerr << "ResFact = " <<  res_fact << std::endl; std::cerr << "\a";
}


//! @brief Return trial value of plastic stiffness.
double XC::ExponReducing::getTrialPlasticStiffness(void) const
  {
    //K = Kp0*exp(-1*val_trial*alpha);

    // if x0 and Kp0 is a const:
    // K = Kp0(1.0  - exp(-alpha*x0 + alpha*val_trial));	
    // K = Kp0*(1.0 - exp(-alpha + alpha*val_trial));

    // for pinching type stuff
    double K= residual*Kp0*(1 - exp(-1*alpha*val_trial));

    if(sFactor != 1.0)
      K= Kp0*sFactor;

    if(K<(Kp0*resFactor))
      K = Kp0*resFactor;

//  std::cerr << "K = " << K << ", sFactor = " << sFactor << std::endl;

    if(K <0.0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; Ri = " << val_trial << ", Factor = "
		  << K/Kp0 << ", res_fact = " << resFactor << std::endl
	          << "\a";
      }
    return K;
  }

//! @brief Print stuff.
void XC::ExponReducing::Print(std::ostream &s, int flag) const
  {
    s << "MultiLinear, Tag = " << getTag() << std::endl;
    s << "Kp0 = " << Kp0 << std::endl;
    s << "Alpha = " <<  alpha << std::endl;
  }

//! @brief Virtual constructor.
XC::PlasticHardeningMaterial *XC::ExponReducing::getCopy(void)
  {  return new ExponReducing(*this); }

