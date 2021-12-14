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
// NullYS2D.cpp: implementation of the YieldSurfaceBC class.
//
//////////////////////////////////////////////////////////////////////

#include "material/yieldSurface/yieldSurfaceBC/NullYS2D.h"
#include <math.h>
#include <material/yieldSurface/evolution/NullEvolution.h>

 XC::NullEvolution XC::NullYS2D::evolModel(0, 1e+20, 1e+20);
 
#define nullptrYS2D_CLASS_TAG -1

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::NullYS2D::NullYS2D(int tag)
:YieldSurface_BC2D(tag, nullptrYS2D_CLASS_TAG, 1, 1, evolModel){}


//////////////////////////////////////////////////////////////////////
// YS specific methods
//////////////////////////////////////////////////////////////////////
void XC::NullYS2D::setExtent()
{
	// Extent along the axis
	xPos =  1;
	xNeg = -1;
	yPos =  1;
	yNeg = -1;
}


void XC::NullYS2D::getGradient(double &gx, double &gy, double x, double y)
{
    	std::cerr << "ERROR - XC::NullYS2D::getGradient function should not be called\n";
		gx = 1;
		gy = 1;
}

double XC::NullYS2D::getSurfaceDrift(double x, double y)
{
	return -1;
}

 XC::YieldSurface_BC *XC::NullYS2D::getCopy(void)
{
    NullYS2D *theCopy = new NullYS2D(this->getTag());
    return theCopy;
}

void XC::NullYS2D::Print(std::ostream &s, int flag) const
{
    s << "\nYield Surface No: " << this->getTag() << " type: NullYS2D\n";
}

