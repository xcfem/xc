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
//NormalStressStrengthParameters.cc

#include "NormalStressStrengthParameters.h"
#include "PivotsUltimateStrains.h"
#include "xc_basic/src/util/matem.h"


//! @brief Factor de cumplimiento for the max strain.
//! Valid if greater than one.
double XC::NormalStressStrengthParameters::fc_tension(void) const
  { return agot_pivots.getUltimateStrainAPivot()/eps_c_max; }

//! @brief Factor de cumplimiento for the min strain.
//! Valid if greater than one.
double XC::NormalStressStrengthParameters::fc_comp(void) const
  { return agot_pivots.getUltimateStrainBPivot()/eps_c_min; }

//! @brief Factor de cumplimiento for C pivot strain.
//! valid if greater than one.
double XC::NormalStressStrengthParameters::fc_pC(void) const
  { 
    if(eps_c_pC < 0)
      return agot_pivots.getUltimateStrainCPivot()/eps_c_pC;
    else
      return 1.0+eps_c_pC;
  }

//! @brief Minimal factor de cumplimiento.
double XC::NormalStressStrengthParameters::fc_min(void) const
  { return std::min(fabs(fc_tension()),std::min(fabs(fc_comp()),fabs(fc_pC()))); }

XC::NormalStressStrengthParameters::NormalStressStrengthParameters(const PivotsUltimateStrains &ap,const double &emx,const double &emn,const double &ec)
  : agot_pivots(ap), eps_c_max(emx), eps_c_min(emn), eps_c_pC(ec) {}

bool XC::NormalStressStrengthParameters::OK(void) const
  { return (fc_min()>=1.0); }

bool XC::NormalStressStrengthParameters::KO(void) const
  { return !OK(); }
