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
//Kinematic2D02
//
//////////////////////////////////////////////////////////////////////

#include "material/yieldSurface/evolution/Kinematic2D02.h"
#include <math.h>
//#include "material/yieldSurface/yieldSurfaceBC/YieldSurface_BC.h"
 
#define evolDebug 0
#define KINEMATIC2D02_CLASSTAG -1

 XC::NullPlasticMaterial XC::Kinematic2D02::nullMat(-1);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::Kinematic2D02::Kinematic2D02(int tag, double min_iso_factor, 
									YieldSurface_BC &lim_surface,
									PlasticHardeningMaterial &kpx,
									PlasticHardeningMaterial &kpy,
									int algo, double resfact, double appfact, double dir)
:BkStressLimSurface2D(tag, KINEMATIC2D02_CLASSTAG, min_iso_factor,0.0, 1.0,
                        lim_surface, kpx, kpy,
                        nullMat, nullMat, nullMat, nullMat, algo, resfact, appfact, dir){}

XC::YS_Evolution *XC::Kinematic2D02::getCopy(void)
  { return new Kinematic2D02(*this); }


void XC::Kinematic2D02::Print(std::ostream &s, int flag) const
  {
    s << "Kinematic2D02 \n";
	s << "iso_Ratio = " << isotropicRatio << "\n";
	s << "isotropicFactor_hist = " << isotropicFactor_hist;
	s << "translateX       = " << translate(0) << ",\ttranslateY = " << translate(1) << "\n";
	s << "\n";

}
	
