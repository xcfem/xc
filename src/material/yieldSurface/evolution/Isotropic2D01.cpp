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
//Isotropic2D01.cpp: implementation of the YS_HardeningModel class.
//
//////////////////////////////////////////////////////////////////////

#include "material/yieldSurface/evolution/Isotropic2D01.h"
#include <math.h>

#define evolDebug 0
#define ISOTROPIC2D01_CLASSTAG 1
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::Isotropic2D01::Isotropic2D01(int tag, double min_iso_factor,
                        PlasticHardeningMaterial &kpx,
						PlasticHardeningMaterial &kpy)
:PlasticHardening2D(tag, ISOTROPIC2D01_CLASSTAG, min_iso_factor,1, 0,
                    kpx, kpx, kpy, kpy, 0.0)
{

}

XC::YS_Evolution *XC::Isotropic2D01::getCopy(void)
{
	Isotropic2D01 *theCopy = new  Isotropic2D01(this->getTag(), minIsoFactor, *kpMatXPos, *kpMatYPos);
	if(theCopy==0)
	{
		std::cerr << "WARNING - Isotropic2D01, unable to get copy\n";
	}
	
	return theCopy;
}

void XC::Isotropic2D01::Print(std::ostream &s, int flag) const
{
	s << "Isotropic2D01 \n";
	s << "iso_Ratio = " << isotropicRatio << "\n";
	s << "isotropicFactor_hist = " << isotropicFactor_hist;
	s << "translateX       = " << translate(0) << ",\ttranslateY = " << translate(1) << "\n";
	s << "\n";

}
	
