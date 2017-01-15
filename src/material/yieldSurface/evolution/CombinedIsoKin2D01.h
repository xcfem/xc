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
// CombinedIsoKin2D01.h
//
//////////////////////////////////////////////////////////////////////

#if !defined USERDEFINED2D_H
#define USERDEFINED2D_H

#include "PlasticHardening2D.h"

namespace XC {
//! \ingroup MATYS
//! @brief ??.
class CombinedIsoKin2D01: public PlasticHardening2D
  {
  public:
    CombinedIsoKin2D01(int tag, double iso_ratio, double kin_ratio, double shr_iso_ratio, double shr_kin_ratio,double min_iso_factor,PlasticHardeningMaterial &kpx_pos, PlasticHardeningMaterial &kpx_neg, PlasticHardeningMaterial &kpy_pos, PlasticHardeningMaterial &kpy_neg, bool isDeformable, double dir);
    virtual void Print(std::ostream &s, int flag =0);
    virtual YS_Evolution *getCopy(void);
  };
} // end of XC namespace

#endif
