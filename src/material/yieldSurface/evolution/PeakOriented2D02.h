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
/***************************************************************************
                          PeakOriented2D02.h  -  description
                             -------------------
    begin                : Fri Jul 12 2002
    email                : rkaul@ce-blume215-pent-2.stanford.edu
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef PEAKORIENTED2D02_H
#define PEAKORIENTED2D02_H

#include <material/yieldSurface/evolution/BkStressLimSurface2D.h>
#include <material/yieldSurface/plasticHardeningMaterial/NullPlasticMaterial.h>

/** Grows/ shrinks one side while keeping the diametrically opposite end 
the same.  Kp_isoXPos = Kp_isoXNeg = (should be) Kp_kinX
Since kinematic is function of back-stress and
isotropic is a function of plastic-strains - calibration
will be difficult.

Therefore, this is a pseudo PeakOriented model, for true
PeakOriented, use PeakOriented01

isotropic:kinematic ratio = 0.5:0.5
  *@author rkaul
  */

namespace XC {
//! @ingroup MATYS
//
//! @brief Grows/shrinks one side while keeping the diametrically opposite end
//! the same.  Kp_isoXPos = Kp_isoXNeg = (should be) Kp_kinX
//! Since kinematic is function of back-stress and
//! isotropic is a function of plastic-strains - calibration
//! will be difficult.

//! Therefore, this is a pseudo PeakOriented model, for true
//! PeakOriented, use PeakOriented01

//! isotropic:kinematic ratio = 0.5:0.5
class PeakOriented2D02 : public BkStressLimSurface2D
  {
  private:
    static NullPlasticMaterial nullMat;
  public: 
    PeakOriented2D02(int tag, double min_iso_factor,
                     YieldSurface_BC &lim_surface,
		PlasticHardeningMaterial &kinX,
		PlasticHardeningMaterial &kinY,
		PlasticHardeningMaterial &isoX,
					PlasticHardeningMaterial &isoY,
					int algo
					);
	YS_Evolution * getCopy();
	void Print(std::ostream & s, int flag);
  };
} // end of XC namespace

#endif
