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
// Orbison2D.cpp: implementation of the YieldSurfaceBC class.
//
//////////////////////////////////////////////////////////////////////

#include "material/yieldSurface/yieldSurfaceBC/Orbison2D.h"
#include <cmath>
#define ORBISON_CLASS_TAG -1
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::Orbison2D::Orbison2D(int tag, double xcap, double ycap,
                     YS_Evolution &model)
:YieldSurface_BC2D(tag, ORBISON_CLASS_TAG, xcap, ycap, model)
  {}

//////////////////////////////////////////////////////////////////////
// YS specific methods
//////////////////////////////////////////////////////////////////////

void XC::Orbison2D::setExtent()
  {
	// Extent along the axis
	xPos =  1;
	xNeg = -1;
	yPos =  1;
	yNeg = -1;
}


void XC::Orbison2D::getGradient(double &gx, double &gy, double x, double y)
{
// check if the point is on the surface
    double drift =  getDrift(x, y);
    double capx = capXdim;
    double capy = capYdim;
    
    if(forceLocation(drift)!=0)
    {
     	std::cerr << "ERROR - XC::Orbison2D::getGradient(double &gx, double &gy, double x, double y)\n";
        std::cerr << "Force point not on the yield surface\n";
		std::cerr << " fx = " << x << ", fy = " << y  << " drift = " << drift << "\n";
        std::cerr << "\a";
    }
    else
    {
    	gx = 2*x/(capx) + 7.34*pow(y, 2)*(x/(capx));
    	gy = 2.3*y/(capy) - 0.9*pow(y, 5)/(capy) + 7.34*pow(x, 2)*(y/(capy));
//  	p1 = 2.3*p - 0.9*pow(p, 5) + 7.34*pow(m, 2)*(p);
//  	m1 = 2*m + 7.34*pow(p, 2)*(m);

//      gx = 2*x + 7.34*pow(y, 2)*x;
//      gy = 2.3*y - 0.9*pow(y, 5) + 7.34*pow(x, 2)*y;

    }

}

double XC::Orbison2D::getSurfaceDrift(double x, double y)
{
double phi = 1.15*y*y - 0.15*pow(y, 6) + x*x + 3.67*y*y*x*x;
double drift = phi - 1;
	return drift;
}

 XC::YieldSurface_BC *XC::Orbison2D::getCopy(void)
{
    Orbison2D *theCopy = new Orbison2D(this->getTag(), capX, capY, *hModel);
    if(theCopy==0)
    {
     	std::cerr << "XC::Orbison2D::getCopy(void) - unable to make copy\n";
     	std::cerr << "\a";
    }
    //later  copy all the state variables
    return theCopy;
}

void XC::Orbison2D::Print(std::ostream &s, int flag) const
  { s << "\nYield Surface No: " << this->getTag() << " type: Attalla2D\n"; }
