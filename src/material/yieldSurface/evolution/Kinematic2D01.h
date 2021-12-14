// -*-c++-*-
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
// Kinematic2D.h
//
//////////////////////////////////////////////////////////////////////

#ifndef KINEMATIC2D01_H
#define KINEMATIC2D01_H

#include "material/yieldSurface/evolution/PlasticHardening2D.h"

namespace XC {
//! @ingroup YSEvolution
//! 
//! @brief 2D kinematic hardening yield surface evolution law.
class Kinematic2D01: public PlasticHardening2D
  {
  private:
  //	double minIsoFactor;
  public:
    Kinematic2D01(int tag, double min_iso_factor,
				PlasticHardeningMaterial &kpx,
				PlasticHardeningMaterial &kpy, double dir);

    virtual void Print(std::ostream &s, int flag =0) const;
    virtual YS_Evolution *getCopy(void);
    //virtual int evolveSurface(YieldSurfaceBC *ys, double magPlasticDefo);
	
  };
} // end of XC namespace

#endif
