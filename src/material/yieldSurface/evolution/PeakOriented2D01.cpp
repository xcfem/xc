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
//PeakOriented2D01.cpp: implementation of the YS_HardeningModel class.
//
//////////////////////////////////////////////////////////////////////

#include "material/yieldSurface/evolution/PeakOriented2D01.h"
#include <cmath>

#define evolDebug 0
#define PEAK_ORIENTED2D01_CLASSTAG -1
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::PeakOriented2D01::PeakOriented2D01(int tag, double min_iso_factor,
                                   PlasticHardeningMaterial &kpx,
                                   PlasticHardeningMaterial &kpy)
:PlasticHardening2D(tag, PEAK_ORIENTED2D01_CLASSTAG, min_iso_factor, 0.5, 0.5,
                    kpx, kpx, kpy, kpy, 0.0)
{

}

XC::YS_Evolution *XC::PeakOriented2D01::getCopy(void)
  { return new  PeakOriented2D01(*this); }

void XC::PeakOriented2D01::Print(std::ostream &s, int flag) const
{
	s << "PeakOriented2D \n";
	s << "iso_Ratio = " << isotropicRatio << "\n";
	s << "isotropicFactor_hist = " << isotropicFactor_hist;
	s << "translateX       = " << translate(0) << ",\ttranslateY = " << translate(1) << "\n";
	s << "\n";

}
	
