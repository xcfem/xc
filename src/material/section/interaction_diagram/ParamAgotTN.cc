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
//ParamAgotTN.cc

#include "ParamAgotTN.h"
#include "DefAgotPivotes.h"
#include "xc_basic/src/util/matem.h"


//Factor de cumplimiento para la deformación máxima.
//válido si es mayor que uno.
double XC::ParamAgotTN::fc_tracc(void) const
  { return agot_pivotes.getDefAgotPivoteA()/eps_c_max; }

//Factor de cumplimiento para la deformación mínima.
//válido si es mayor que uno.
double XC::ParamAgotTN::fc_comp(void) const
  { return agot_pivotes.getDefAgotPivoteB()/eps_c_min; }

//Factor de cumplimiento para la deformación en el pivote C.
//válido si es mayor que uno.
double XC::ParamAgotTN::fc_pC(void) const
  { 
    if(eps_c_pC < 0)
      return agot_pivotes.getDefAgotPivoteC()/eps_c_pC;
    else
      return 1.0+eps_c_pC;
  }

//Factor de cumplimiento mínimo.
double XC::ParamAgotTN::fc_min(void) const
  { return std::min(fabs(fc_tracc()),std::min(fabs(fc_comp()),fabs(fc_pC()))); }

XC::ParamAgotTN::ParamAgotTN(const DefAgotPivotes &ap,const double &emx,const double &emn,const double &ec)
  : agot_pivotes(ap), eps_c_max(emx), eps_c_min(emn), eps_c_pC(ec) {}

bool XC::ParamAgotTN::Cumple(void) const
  { return (fc_min()>=1.0); }

bool XC::ParamAgotTN::Agotada(void) const
  { return !Cumple(); }
