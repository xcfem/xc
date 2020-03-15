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
// ElTawil2D.cpp: implementation of the YieldSurfaceBC class.
//
//////////////////////////////////////////////////////////////////////

#include "material/yieldSurface/yieldSurfaceBC/ElTawil2D.h"
#include "utility/matrix/Vector.h"
#include "material/yieldSurface/evolution/YS_Evolution.h"
#include <cmath>

#define ELTAWIL2D_CLASS_TAG -1

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::ElTawil2D::ElTawil2D(int tag, double xbal, double ybal, double ypos, double yneg,
					YS_Evolution &model, double cz_, double ty_)
		 :YieldSurface_BC2D(tag, ELTAWIL2D_CLASS_TAG, 0, 0, model),
		 xBal(xbal), yBal(ybal), yPosCap(ypos), yNegCap(yneg),
		 yPosCap_orig(ypos), yNegCap_orig(yneg), cz(cz_), ty(ty_), qy(0.005) //0.01
  {
    capY = yPosCap; //!! why did I change this ?
	// capY    = yPosCap - yBal;

	// set to origin
	yPosCap -= yBal;
	yNegCap -= yBal;

	// set translation
	double transY   = yBal/capY;
	XC::Vector t(2);
	t(0) = 0;
	//t(1) = yBal/capY;
	t(1) = transY;
	hModel->setInitTranslation(t);

//	std::cerr << "Translation Y = " << transY << std::endl;

	//capX = xBal*(1 - pow( fabs((transY*capY)/yNegCap) , ty));
	capX    = xBal;

	// bad:
	capX_orig = capX;
	capY_orig = capY;
	capXdim = capX;
	capYdim = capY;
	
}

//////////////////////////////////////////////////////////////////////
// YS specific methods
//////////////////////////////////////////////////////////////////////
void XC::ElTawil2D::setExtent()
  {
	// Extent along the axis
//	xPos =  1;
//	xNeg = -1;

	xPos = xBal/capX;
	xNeg = -xPos;

	yPos =  yPosCap/capY - qy; //0.02  0.98;
	yNeg =  yNegCap/capY + qy; //-0.98;

	ytPos = yPos - 0.005; // 0.01 // 0.03 // 0.95
	ytNeg = yNeg + 0.005;

double yValPos = ytPos*capY;
double yValNeg = ytNeg*capY;

	xtPos = xBal*(1 - pow((yValPos/yPosCap) , cz));
	xtNeg = xBal*(1 - pow( fabs(yValNeg/yNegCap) , ty));

	xtPos = xtPos/capX;
	xtNeg = xtNeg/capX;
/*
	std::cerr << "ytPos = " << ytPos << ", ytNeg = " << ytNeg << ", xtPos = " << xtPos
	                            << ", xtNeg = " << xtNeg << std::endl;
*/
}


void XC::ElTawil2D::getGradient(double &gx, double &gy, double x, double y)
{
// check if the point is on the surface
    double drift =  getDrift(x, y);
    double loc   =  forceLocation(drift);
    double capx = capXdim;
    double capy = capYdim;

    if(loc != 0)
    {
     	std::cerr << "ERROR - XC::ElTawil2D::getGradient(double &gx, double &gy, double x, double y)\n";
        std::cerr << "Force point not on yield surface, drift = " << drift << " loc = " << loc <<"\n";
        // std::cerr << "\a";
        gx = 1.0;
        gy = 1.0;
    }
    else
    {
		double a = 10.277;//3.043;//4.29293; //by compatibility, check err in gradient
		// double yt = 0.95;

		if(y > ytPos)
		{
		 	gx = 2*a*x/capx;
			gy = 1;
		}
		else if(y < ytNeg)
		{
		 	gx = 2*a*x/capx;
			gy = -1; //-1*y/capy; <-- why did i write this?// -1 ?
		}
		else
		{
			// double xVal = x*capx;
			double yVal = fabs(y*capy);  //!!
			// double yVal = y*capy;           //!!

			gx = 1/xBal;
			if(x < 0)
				gx = -1*gx;

			if(y < 0)
				gy = -1*(1/pow( fabs(yNegCap), ty))*ty*(pow(yVal,ty-1));
			else
				gy = (1/pow(yPosCap, cz))*cz*(pow(yVal,cz-1));
		}
	}

	// std::cerr << "gx = " << gx << ", gy = " << gy << ", capy = " << capy << "\n";
	// std::cerr << "\a";
}

double XC::ElTawil2D::getSurfaceDrift(double x, double y)
{
double phi;
double a = 5;//10.277; //3.043;//4.29293; --> effects convergence


	if(y > ytPos && fabs(x) < fabs(y*xtPos/ytPos) )
	{
		phi = a*x*x + y + qy;
	}
	else if(y < ytNeg && fabs(x) < fabs(y*xtNeg/ytNeg))
	{
		phi = a*x*x - y + qy;
	}
	else
	{
		double xVal = x*capX;
		double yVal = y*capY;

		if(y < 0)
			phi = fabs(xVal/xBal) + pow(fabs(yVal/yNegCap), ty);
		else
			phi = fabs(xVal/xBal) + pow(yVal/yPosCap, cz);
	}

	double drift = phi - 1;

//	std::cerr << "Eltawil2D - x = " << x << ", y = " << y << ", drift = " << drift << std::endl;

	return drift;
}

// Always call the base class method first
void XC::ElTawil2D::customizeInterpolate(double &xi, double &yi, double &xj, double &yj)
{
	this->XC::YieldSurface_BC2D::customizeInterpolate(xi, yi, xj, yj);

	if(yj >= ytPos && fabs(xj) <= fabs(yj*xtPos/ytPos) )
	{
		xi = 0;
		yi = 0;
	}
	else if(yj <= ytNeg && fabs(xj) <= fabs(yj*xtNeg/ytNeg))
	{
		xi = 0;
		yi = 0;
	}
	else
		; // values are okay


}


 XC::YieldSurface_BC *XC::ElTawil2D::getCopy(void)
{
    ElTawil2D *theCopy = new ElTawil2D(this->getTag(), xBal, yBal, yPosCap_orig, yNegCap_orig, *hModel,
                                       cz, ty);
    //later  copy all the state variables
    return theCopy;
}

void XC::ElTawil2D::Print(std::ostream &s, int flag) const
{
    s << "\nYield Surface No: " << this->getTag() << " type: ElTawil2D\n";
	this->XC::YieldSurface_BC::Print(s, flag);
}


