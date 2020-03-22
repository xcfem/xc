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
//BoundingSurface2D
//
//////////////////////////////////////////////////////////////////////

#include "material/yieldSurface/evolution/BoundingSurface2D.h"
#include "material/yieldSurface/plasticHardeningMaterial/PlasticHardeningMaterial.h"
#include <cmath>
#include <material/yieldSurface/yieldSurfaceBC/YieldSurface_BC.h>

#define evolDebug 0
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::BoundingSurface2D::BoundingSurface2D(int tag, int classTag, double min_iso_factor,
						double iso_ratio, double kin_ratio,
                        PlasticHardeningMaterial &kpx,
						PlasticHardeningMaterial &kpy,
						YieldSurface_BC &bound_surface)
:YS_Evolution2D(tag, classTag, min_iso_factor, iso_ratio, kin_ratio)
{
	kpMatX = kpx.getCopy();
	kpMatY = kpy.getCopy();
	boundSurface = bound_surface.getCopy();
}

XC::BoundingSurface2D::~BoundingSurface2D()
{
  if (kpMatX != 0)
    delete kpMatX;

  if (kpMatY != 0)
    delete kpMatY;

  if (boundSurface != 0)
    delete boundSurface;
}

int XC::BoundingSurface2D::commitState()
{
	this->XC::YS_Evolution2D::commitState();

    int res  = kpMatX->commitState();
	    res += kpMatY->commitState();

	return res;
}

int XC::BoundingSurface2D::revertToLastCommit(void)
{
	this->XC::YS_Evolution2D::revertToLastCommit();

	kpMatX->revertToLastCommit();
	kpMatY->revertToLastCommit();

	return 0;
}

const XC::Vector &XC::BoundingSurface2D::getEquiPlasticStiffness(void)
{
// find the conjugate point
// find "dR"
// find Kp
	  {
		v2(0) = 0;
		v2(1) = 0;
	  }
	return v2;
}

void    XC::BoundingSurface2D::setTrialPlasticStrains(double ep, const XC::Vector &f, const XC::Vector &g)
{
// set for isotropic materials	
	
}

double  XC::BoundingSurface2D::getIsoPlasticStiffness(int dir)
{
	return 0;
}

double  XC::BoundingSurface2D::getKinPlasticStiffness(int dir)
{
	return 0;	
}

XC::Vector& XC::BoundingSurface2D::getEvolDirection(XC::Vector &f_new)
{
// find the conjugate point
// return that as dir	
	return v2;
}


void XC::BoundingSurface2D::Print(std::ostream &s, int flag) const
{
	s << "BoundingSurface2D \n";
	s << "iso_Ratio = " << isotropicRatio << "\n";
	s << "isotropicFactor_hist = " << isotropicFactor_hist;
	s << "translateX       = " << translate(0) << ",\ttranslateY = " << translate(1) << "\n";
	s << "\n";

}
	
