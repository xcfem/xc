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
                          PeakOriented2D02.cpp  -  description
                             -------------------
    begin                : Fri Jul 12 2002
    email                : rkaul@ce-blume215-pent-2.stanford.edu
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

#include "material/yieldSurface/evolution/PeakOriented2D02.h"
 XC::NullPlasticMaterial XC::PeakOriented2D02::nullMat(-1);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::PeakOriented2D02::PeakOriented2D02(int tag, double min_iso_factor,
					YieldSurface_BC &lim_surface,
					PlasticHardeningMaterial &kinX,
					PlasticHardeningMaterial &kinY,
					PlasticHardeningMaterial &isoX,
					PlasticHardeningMaterial &isoY,
					int algo)
:BkStressLimSurface2D(tag, -1, min_iso_factor, 0.5, 0.5,
                      lim_surface,
                      kinX, kinY,
                      isoX, isoX, isoY, isoY, algo, 0.0, 0.0, 0.0)
{
                                                  
}

XC::YS_Evolution * XC::PeakOriented2D02::getCopy()
  { return new PeakOriented2D02(*this); }

void XC::PeakOriented2D02::Print(std::ostream & s, int flag) const
{
	s << "PeakOriented2D02 \n";
	s << "iso_Ratio = " << isotropicRatio << "\n";
	s << "isotropicFactor_hist = " << isotropicFactor_hist;
	s << "translateX       = " << translate(0) << ",\ttranslateY = " << translate(1) << "\n";
	s << "\n";

}
