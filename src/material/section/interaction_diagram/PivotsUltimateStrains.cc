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
//PivotsUltimateStrains.cc

#include "PivotsUltimateStrains.h"

//! @brief Constructor.
XC::PivotsUltimateStrains::PivotsUltimateStrains(const double &eA,const double &eB,const double &eC)
 : eps_agot_A(eA), eps_agot_B(eB), eps_agot_C(eC) {}

//! @brief Return ultimate strain for the A pivot.
const double &XC::PivotsUltimateStrains::getUltimateStrainAPivot(void) const
  { return eps_agot_A; }

//! @brief Return ultimate strain for the B pivot.
const double &XC::PivotsUltimateStrains::getUltimateStrainBPivot(void) const
  { return eps_agot_B; }

//! @brief Return ultimate strain for the C pivot.
const double &XC::PivotsUltimateStrains::getUltimateStrainCPivot(void) const
  { return eps_agot_C; }

//! @brief Set ultimate strain for the A pivot.
void XC::PivotsUltimateStrains::setUltimateStrainAPivot(const double &v)
  { eps_agot_A= v; }
//! @brief Set ultimate strain for the B pivot.
void XC::PivotsUltimateStrains::setUltimateStrainBPivot(const double &v)
  { eps_agot_B= v; }
//! @brief Set ultimate strain for the C pivot.
void XC::PivotsUltimateStrains::setUltimateStrainCPivot(const double &v)
  { eps_agot_C= v; }

//! @brief Return the strain increment between A end B pivots.
const double XC::PivotsUltimateStrains::getIncEpsAB(const double &ndiv) const
  { return (eps_agot_A-eps_agot_B)/ndiv; }
