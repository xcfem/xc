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
// ElTawil2DUnSym.cpp: implementation of the YieldSurfaceBC class.
//
//////////////////////////////////////////////////////////////////////

#include "material/yieldSurface/yieldSurfaceBC/ElTawil2DUnSym.h"
#include "utility/matrix/Vector.h"
#include "material/yieldSurface/evolution/YS_Evolution.h"
#include <cmath>

#define ELTAWIL2DUNSYM_CLASS_TAG -1

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::ElTawil2DUnSym::ElTawil2DUnSym(int tag, double xPosbal, double yPosbal,
    	double xNegbal, double yNegbal,
    	double ypos, double yneg,
    	YS_Evolution &model,
    	double cz_pos, double ty_pos,
    	double cz_neg, double ty_neg)

    :YieldSurface_BC2D(tag, ELTAWIL2DUNSYM_CLASS_TAG, 0, 0, model),
    xPosBal(xPosbal), yPosBal(yPosbal),
    xNegBal(xNegbal), yNegBal(yNegbal),
    yPosCap(ypos), yNegCap(yneg),
    yPosCap_orig(ypos), yNegCap_orig(yneg),
    czPos(cz_pos), tyPos(ty_pos), czNeg(cz_neg), tyNeg(ty_neg), qy(0.005)
{

// This tight coupling with base-class can be a diaster to debug!
    if(yPosBal < 0 || yNegBal < 0)
    	std::cerr << "WARNING - ElTawil2DUnSym() - yBalance < 0" << std::endl;
    	
    //capY = yPosCap; // why did I change this?
    yBal = yPosBal;

    if(yNegBal < yBal)
    	yBal = yNegBal;

//    std::cerr << "yBal= " << yBal << ", yPosBal= " << yPosBal
//	     << ", yNegBal= " << yNegBal << endl << std::endl;

// 	capY    = yPosCap - yBal;//!!
    capY = yPosCap;

    // set to origin
    yPosCap -= yBal;
    yNegCap -= yBal;

    // larger of the two will align with the axis
    // the other one will be above
    yPosBal -= yBal;
    yNegBal -= yBal;
/*	
    std::cerr << "yBal= " << yBal << ", yPosBal= " << yPosBal
         << ", yNegBal= " << yNegBal << std::endl;
*/

    // set translation
    double transY   = yBal/capY;
    XC::Vector t(2);
    t(0) = 0;
    //t(1) = yBal/capY;
    t(1) = transY;
    
    hModel->setInitTranslation(t);

    capX    = xPosBal;
    if( fabs(xNegBal) > capX)
    	capX = fabs(xNegBal);

    // bad:
    capX_orig = capX;
    capY_orig = capY;
    capXdim = capX;
    capYdim = capY;
    	
}

//////////////////////////////////////////////////////////////////////
// YS specific methods
//////////////////////////////////////////////////////////////////////
void XC::ElTawil2DUnSym::setExtent()
{
    // Extent along the axis
//	xPos =  1;
//	xNeg = -1;

    xPos = std::min(xPosBal/capX,
                (xPosBal*(1 - pow( fabs(yPosBal/(yNegCap - yPosBal)) , tyPos)))/capX);
    // xNeg = xNegBal/capX;
    xNeg = std::max(xNegBal/capX,
                (xNegBal*(1 - pow( fabs(yNegBal/(yNegCap - yNegBal)) , tyNeg)))/capX);

    yPos =  yPosCap/capY - qy; //0.02  0.98;
    yNeg =  yNegCap/capY + qy; //-0.98;

    ytPos = yPos - 0.005; // 0.01 // 0.03 // 0.95
    ytNeg = yNeg + 0.005;

double yVal1 = ytPos*capY - yPosBal;
double yVal4 = ytNeg*capY - yPosBal;
double yVal2 = ytPos*capY - yNegBal;
double yVal3 = ytNeg*capY - yNegBal;

/*
char c = ' ';

       std::cerr << "yVals = " << yVal1 << c << yVal2 << c
                          << yVal3 << c << yVal4 << std::endl;

*/
double	xt1 = xPosBal*(1 - pow((yVal1/(yPosCap - yPosBal)) , czPos));
double	xt4 = xPosBal*(1 - pow( fabs(yVal4/(yNegCap - yPosBal)) , tyPos));
double	xt2 = xNegBal*(1 - pow((yVal2/(yPosCap - yNegBal)) , czNeg));
double	xt3 = xNegBal*(1 - pow( fabs(yVal3/(yNegCap - yNegBal)) , tyNeg));


    xt1 = xt1/capX;
    xt2 = xt2/capX;
    xt3 = xt3/capX;
    xt4 = xt4/capX;

/*
    std::cerr << "xPos = " << xPos << ", xNeg = " << xNeg << endl
         << "ytPos = " << ytPos << ", ytNeg = " << ytNeg << endl
         << "xt1   = " << xt1 << ", xt2 = " << xt2  << endl
    	 << "xt3   = " << xt3 << ", xt4 = " << xt4 << std::endl;
*/
}


void XC::ElTawil2DUnSym::getGradient(double &gx, double &gy, double x, double y)
{
// check if the point is on the surface
    double drift =  getDrift(x, y);
    double loc   =  forceLocation(drift);
    double capx = capXdim;
    double capy = capYdim;
    
    // std::cerr << "XC::ElTawil2DUnSym::getGradient drift:" << this->XC::YieldSurface_BC2D::getDrift(x, y) << std::endl;

    if(loc != 0)
    {
     	std::cerr << "ERROR - XC::ElTawil2D::getGradient(double &gx, double &gy, double x, double y)\n";
        std::cerr << "Force point not on yield surface, drift = " << drift << " loc = " << loc <<"\n";
        std::cerr << "\a";
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
    		gy = -1; //*y/capy;
    	}
    	else
    	{
    		double xVal = x*capx;
    		double yVal = y*capy;

    		if(xVal >= 0 && yVal >= yPosBal) // quadrant 1
    		{
    			gx = 1/xPosBal;
    			gy = (1/pow( (yPosCap-yPosBal), czPos))*czPos*(pow(yVal-yPosBal,czPos-1));
    		}
    		else if(xVal >=0 && yVal < yPosBal) // quadrant 1 or 4
    		{
    			gx = 1/xPosBal;
    			gy = -1*(1/pow( fabs(yNegCap-yPosBal), tyPos))*tyPos*(pow(fabs(yVal-yPosBal),tyPos-1));
    		}
    		else if(xVal< 0 && yVal >= yNegBal) // quadrant 2
    		{
    			gx = 1/xNegBal;  // xNegBal should be < 0
    			gy = (1/pow( (yPosCap-yNegBal), czNeg))*czNeg*(pow(yVal-yNegBal,czNeg-1));
    		}
    		else if(xVal<0 && yVal < yNegBal) // quadrant 2 or 3
    		{
    			gx = 1/xNegBal;
    			gy = -1*(1/pow( fabs(yNegCap-yNegBal), tyNeg))*tyNeg*(pow(fabs(yVal-yNegBal),tyNeg-1));
    		}
    		else
    		{
    			std::cerr << "Eltawil2DUnsym - condition not possible" << std::endl;
    			std::cerr << "\a";
    		}

    		/* gx = 1/xBal;
    			if(x < 0)
    				gx = -1*gx;

    			if(y < 0)
    				gy = -1*(1/pow( fabs(yNegCap), ty))*ty*(pow(yVal,ty-1));
    			else
    				gy = (1/pow(yPosCap, cz))*cz*(pow(yVal,cz-1));
    		*/
    	}
    }

//	std::cerr << "gx = " << gx << "gy = " << gy << "\n";
//	std::cerr << "\a";
}

double XC::ElTawil2DUnSym::getSurfaceDrift(double x, double y)
  {
    double phi= 0.0;
    double a = 5;//10.277; //3.043;//4.29293; --> effects convergence
    // why isnt a = 10.77 here or 5 in getGradient
    double capx = capX;
    double capy = capY;
     
    // ignore the small difference between xt1, xt2
    // and xt4, xt3 for determining whether the
    // quadratic should be used
    if(y > ytPos && fabs(x) < fabs(y*xt1/ytPos) )
      {
    	phi = a*x*x + y + qy;
    }
    else if(y < ytNeg && fabs(x) < fabs(y*xt4/ytNeg))
    {
    	phi = a*x*x - y + qy;
    }
    else
    {
    	double xVal = x*capx;
    	double yVal = y*capy;

    	if(xVal >=0 && yVal >= yPosBal) 	// quad 1
    	{
    	 	phi = fabs(xVal/xPosBal) + pow((yVal - yPosBal)/(yPosCap - yPosBal), czPos);
    	}
    	else if(xVal >=0 && yVal < yPosBal)	// quad 1 or 4
    	{
    	 	phi = fabs(xVal/xPosBal) + pow(fabs((yVal - yPosBal)/(yNegCap - yPosBal)), tyPos);
    	}
    	else if(xVal < 0 && yVal >= yNegBal)// quad 2
    	{
    	 	phi = fabs(xVal/xNegBal) + pow((yVal - yNegBal)/(yPosCap - yNegBal), czNeg);
    	}
    	else if(xVal < 0 && yVal < yNegBal)	// quad 2 or 3
    	{
    	
    	 	phi = fabs(xVal/xNegBal) + pow(fabs((yVal - yNegBal)/(yNegCap - yNegBal)), tyNeg);
    	}
    	else
    	{
    		std::cerr << "XC::ElTawil2DUnSym::getSurfaceDrift(..) - cond not possible\n";
    		std::cerr << "x=" << x << ", y=" << y << ", capx=" << capx << ", capy=" << capy << std::endl;
    		std::cerr << "xVal = " << xVal << ", yVal = " << yVal << std::endl;
    		std::cerr << "\a";
    	}
    	/*	
    	if(y < 0)
    		phi = fabs(xVal/xBal) + pow(fabs(yVal/yNegCap), ty);
    	else
    		phi = fabs(xVal/xBal) + pow(yVal/yPosCap, cz);
    	*/
    }

    double drift = phi - 1;
    return drift;
}

// Always call the base class method first
void XC::ElTawil2DUnSym::customizeInterpolate(double &xi, double &yi, double &xj, double &yj)
  {
    XC::YieldSurface_BC2D::customizeInterpolate(xi, yi, xj, yj);
    
    // again, ignore the small difference between xt1, xt2
    // and xt4, xt3

    if(yj >= ytPos && fabs(xj) <= fabs(yj*xt1/ytPos) )
    {
    	xi = 0;
    	yi = 0;
    }
    else if(yj <= ytNeg && fabs(xj) <= fabs(yj*xt4/ytNeg))
    {
    	xi = 0;
    	yi = 0;
    }
    else
      ; // values are okay
  }


 XC::YieldSurface_BC *XC::ElTawil2DUnSym::getCopy(void)
{
    // ElTawil2D *theCopy = new ElTawil2D(this->getTag(), xBal, yBal, yPosCap_orig, yNegCap_orig, *hModel,
    //                                   cz, ty);
    //later  copy all the state variables


    ElTawil2DUnSym *theCopy = new ElTawil2DUnSym(this->getTag(), xPosBal, yPosBal+yBal,
    							  xNegBal, yNegBal+yBal, yPosCap+yBal, yNegCap+yBal,
    							  *hModel, czPos, tyPos, czNeg, tyNeg);

    return theCopy;
}


void XC::ElTawil2DUnSym::Print(std::ostream &s, int flag)
  {
    s << "\nYield Surface No: " << this->getTag() << " type: ElTawil2DUnSym\n";
  }




