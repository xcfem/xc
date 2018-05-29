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
// Kinematic2D02.h
//
//////////////////////////////////////////////////////////////////////

#ifndef KINEMATIC2D02_H
#define KINEMATIC2D02_H

#include "material/yieldSurface/evolution/BkStressLimSurface2D.h"
#include "material/yieldSurface/plasticHardeningMaterial/NullPlasticMaterial.h"

//class YieldSurface_BC;
namespace XC {
//! @ingroup MATYS
//!
//! @brief ??
class Kinematic2D02 : public BkStressLimSurface2D
  {
  public:
    Kinematic2D02(int tag, double min_iso_factor, 
				YieldSurface_BC &lim_surface,
				PlasticHardeningMaterial &kpx,
				PlasticHardeningMaterial &kpy,
				int algo, double resfact, double appfact, double dir);

    virtual void Print(std::ostream &s, int flag =0);
    virtual YS_Evolution *getCopy(void);

    static NullPlasticMaterial nullMat;
  };
} // end of XC namespace

#endif
