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
//DefAgotPivotes.cc

#include "DefAgotPivotes.h"


XC::DefAgotPivotes::DefAgotPivotes(const double &eA,const double &eB,const double &eC)
 : eps_agot_A(eA), eps_agot_B(eB), eps_agot_C(eC) {}

const double &XC::DefAgotPivotes::getDefAgotPivoteA(void) const
  { return eps_agot_A; }

const double &XC::DefAgotPivotes::getDefAgotPivoteB(void) const
  { return eps_agot_B; }

const double &XC::DefAgotPivotes::getDefAgotPivoteC(void) const
  { return eps_agot_C; }

void XC::DefAgotPivotes::setDefAgotPivoteA(const double &v)
  { eps_agot_A= v; }
void XC::DefAgotPivotes::setDefAgotPivoteB(const double &v)
  { eps_agot_B= v; }
void XC::DefAgotPivotes::setDefAgotPivoteC(const double &v)
  { eps_agot_C= v; }

const double XC::DefAgotPivotes::getIncEpsAB(const double &ndiv) const
  { return (eps_agot_A-eps_agot_B)/ndiv; }
