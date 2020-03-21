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
// 
//
//////////////////////////////////////////////////////////////////////

#ifndef ISOTROPIC2D01_H
#define ISOTROPIC2D01_H

#include "PlasticHardening2D.h"

namespace XC {
//! @ingroup MATYS
//! @brief ??.
class Isotropic2D01: public PlasticHardening2D
  {
  public:
    Isotropic2D01(int tag, double min_iso_factor,PlasticHardeningMaterial &kpx, PlasticHardeningMaterial  &kpy);

    virtual void Print(std::ostream &s, int flag =0) const;
    virtual YS_Evolution *getCopy(void);
    //virtual int evolveSurface(YieldSurfaceBC *ys, double magPlasticDefo);
  };
} // end of XC namespace

#endif
