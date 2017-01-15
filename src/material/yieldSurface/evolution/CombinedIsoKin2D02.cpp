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
                          CombinedIsoKin2D02.cpp  -  description
                             -------------------
    begin                : Fri Jul 12 2002
    email                : rkaul@ce-blume215-pent-2.stanford.edu
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

#include "material/yieldSurface/evolution/CombinedIsoKin2D02.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::CombinedIsoKin2D02::CombinedIsoKin2D02(int tag,  double min_iso_factor,
				double iso_ratio, double kin_ratio,
				YieldSurface_BC  &lim_surface,
				PlasticHardeningMaterial &kinX,
				PlasticHardeningMaterial &kinY,
				PlasticHardeningMaterial &isoXPos,
				PlasticHardeningMaterial &isoXNeg,
				PlasticHardeningMaterial &isoYPos,
				PlasticHardeningMaterial &isoYNeg,
				bool isDeformable,
				int  algo, double resfact, double appfact, double dir)
:BkStressLimSurface2D(tag, -1, min_iso_factor,
				iso_ratio, kin_ratio,
				lim_surface, kinX, kinY,
				isoXPos, isoXNeg, isoYPos, isoYNeg, algo, resfact, appfact, dir)
{
	deformable = isDeformable;
}

XC::YS_Evolution * XC::CombinedIsoKin2D02::getCopy()
  { return new CombinedIsoKin2D02(*this); }


void XC::CombinedIsoKin2D02::Print(std::ostream & s, int flag)
{
	s << "CombinedIsoKin2D02 \n";
	s << "iso_Ratio = " << isotropicRatio << "\n";
	s << "isotropicFactor_hist = " << isotropicFactor_hist;
	s << "translateX       = " << translate(0) << ",\ttranslateY = " << translate(1) << "\n";
	s << "\n";

}
