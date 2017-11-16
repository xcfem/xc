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
// YieldSurface_BC.cpp: implementation of the XC::YieldSurface_BC class.
//
//////////////////////////////////////////////////////////////////////

#include "YieldSurface_BC.h"
#include "material/yieldSurface/evolution/YS_Evolution.h"
#include "utility/matrix/ID.h"
#include "utility/matrix/Matrix.h"
#include <cstdlib>


const int XC::YieldSurface_BC::dFReturn(0);
const int XC::YieldSurface_BC::RadialReturn(1);
const int XC::YieldSurface_BC::ConstantXReturn(2);
const int XC::YieldSurface_BC::ConstantYReturn(3);
const int XC::YieldSurface_BC::NoFP(4);
const int XC::YieldSurface_BC::SurfOnly(5);
const int XC::YieldSurface_BC::StateLoading(6);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::YieldSurface_BC::YieldSurface_BC(int tag, int classtag, YS_Evolution &model,
								double capx)
:TaggedObject(tag), MovableObject(classtag),
 capY_orig(-1.0), capZ_orig(-1.0),
 capX(capx), capY(-1.0), capZ(-1.0), isLoading(true), ele_Tag(-1), ele_Location(-1)
  {
   dimension = 1;
   hModel = model.getCopy();
   //theView= 0;
   T = 0;
   S = 0;
   capX_orig = capx;
   ele_Location  =-1;
   ele_Tag = -1;
  }


// el-tawil unsym .. and probably others
// cause problem by sending 0, 0 to the base class constructor
// => tighly coupled - bad practice.
// Since that is necessary... be aware
XC::YieldSurface_BC::YieldSurface_BC(int tag, int classtag, YS_Evolution &model,
								double capx, double capy)
:TaggedObject(tag), MovableObject(classtag), capZ_orig(-1), capX(capx), capY(capy), capZ(-1.0),
isLoading(true), ele_Tag(-1), ele_Location(-1)

{
   dimension = 2;
   hModel = model.getCopy();
   //theView = 0;
   T = 0;
   S = 0;
   capX_orig = capx;
   capY_orig = capy;
   capXdim = capX_orig;
   capYdim = capY_orig;
   ele_Location  =-1;
   ele_Tag = -1;   
}


XC::YieldSurface_BC::YieldSurface_BC(int tag, int classtag, YS_Evolution &model,
								double capx, double capy, double capz)
:TaggedObject(tag), MovableObject(classtag), capX(capx), capY(capy), capZ(capz),
 isLoading(true), ele_Tag(-1), ele_Location(-1)
{
   dimension = 3;
   hModel = model.getCopy();
   //theView = 0;
   T = 0;
   S = 0;
   capX_orig = capX;
   capY_orig = capY;
   capZ_orig = capZ;
   ele_Location  =-1;
   ele_Tag = -1;
}


XC::YieldSurface_BC::~YieldSurface_BC()
  {
    if(T) delete T;
    if(S) delete S;
    if(hModel)
      delete hModel;
  }

int XC::YieldSurface_BC::commitState(XC::Vector &force)
{
//	return 0;
	
	if(dimension == 1)
	{
	 	capXdim = capX_orig*hModel->getTrialIsotropicFactor(0);
	}
	else if(dimension == 2)
	{
	 	capXdim = capX_orig*hModel->getTrialIsotropicFactor(0);
	 	capYdim = capY_orig*hModel->getTrialIsotropicFactor(1);
	}
	else if(dimension == 3)
	{
	 	capXdim = capX_orig*hModel->getTrialIsotropicFactor(0);
	 	capYdim = capY_orig*hModel->getTrialIsotropicFactor(1);
	 	capZdim = capZ_orig*hModel->getTrialIsotropicFactor(2);
	}
	else
		std::cerr << "WARNING  XC::YieldSurface_BC::commitState - dimension > 3 || < 1\n";
    return 0;
}


double XC::YieldSurface_BC::getCap(int dir)
{
	if(dir == 0)
		return capX;
	else if(dir == 1)
		return capY;
	else if(dir == 2)
		return capZ;
	else
		std::cerr << "XC::YieldSurface_BC::getCap(int dir) - dir not valid\n";

    return 1;
}


double XC::YieldSurface_BC::getDrift(double x1)
{
	std::cerr << "XC::YieldSurface_BC::getDrift(.) - This method should not be called\n";
	return 0;
}

double XC::YieldSurface_BC::getDrift(double x1, double y1)
{
	std::cerr << "XC::YieldSurface_BC::getDrift(..) - This method should not be called\n";
	return 0;
}


double XC::YieldSurface_BC::getDrift(double x1, double y1, double z1)
{
	std::cerr << "XC::YieldSurface_BC::getDrift(...) - This method should not be called\n";
	return 0;
}


double XC::YieldSurface_BC::interpolate(double x1, double x2)
{
	std::cerr << "XC::YieldSurface_BC::interpolate(..)  - This method should not be called\n";
	return 0;
}

double XC::YieldSurface_BC::interpolate(double x1, double y1, double x2, double y2)
{
	std::cerr << "XC::YieldSurface_BC::interpolate(....)  - This method should not be called\n";
	return 0;
}
double XC::YieldSurface_BC::interpolate(double x1, double y1, double z1, double x2, double y2, double z2)
{
	std::cerr << "XC::YieldSurface_BC::interpolate(......)  - This method should not be called\n";
	return 0;
}


void XC::YieldSurface_BC::setEleInfo(int tag, int loc)
{
	ele_Tag = tag;
	ele_Location = loc;
}

//////////////////////////////////////////////////////////////////////
// Set Transformation
//////////////////////////////////////////////////////////////////////

void XC::YieldSurface_BC::setTransformation(int xDof, int xFact)
{
    if(T || S)
    {
        std::cerr << "WARNING - XC::YieldSurface_BC::setTransformation(int xDof)\n";
        std::cerr << "Transforation already set\n";
        return;
    }
    T = new ID(1);
    (*T)(0) = xDof;

    S = new ID(1);
    (*S)(0) = xFact;

}

void XC::YieldSurface_BC::setTransformation(int xDof, int yDof, int xFact, int yFact)
{
    if(T || S)
    {
        std::cerr << "WARNING - XC::YieldSurface_BC::setTransformation(int xDof, int yDof)\n";
        std::cerr << "Transforation already set\n";
        return;
    }

    T = new ID(2);
    (*T)(0) = xDof;
    (*T)(1) = yDof;

    S = new ID(2);
    (*S)(0) = xFact;
    (*S)(1) = yFact;

//	std::cerr << " T = " << *T << "\n, S = " << *S;
}

void XC::YieldSurface_BC::setTransformation(int xDof, int yDof, int zDof,
                                       int xFact, int yFact, int zFact)
{
    if(T || S)
    {
        std::cerr << "WARNING - XC::YieldSurface_BC::setTransformation(int xDof, int yDof, int zDof)\n";
        std::cerr << "Transforation already set\n";
        return;
    }

    T = new ID(3);
    (*T)(0) = xDof;
    (*T)(1) = yDof;
    (*T)(2) = zDof;

    S = new ID(3);
    (*S)(0) = xFact;
    (*S)(1) = yFact;
    (*S)(2) = zFact;
}

//////////////////////////////////////////////////////////////////////
// Transform to Local System
//////////////////////////////////////////////////////////////////////


void XC::YieldSurface_BC::toLocalSystem (XC::Vector &eleVector, double &x, bool nonDimensionalize, bool signMult)
{
    if(!T)
    {
        checkT();
        return;
    }

#ifdef _G3DEBUG
    if(T->Size() != 1)
    {
        std::cerr << "WARNING: XC::YieldSurface_BC::toLocalSystem (XC::Vector &eleVector, double &x)\n";
        std::cerr << "T size may not be correct\n";
    }
#endif
	if(signMult==false)
		x = eleVector((*T)(0));
	else
    	x = eleVector((*T)(0))*((*S)(0));
	if (nonDimensionalize)
	x = x/capX;
}

void XC::YieldSurface_BC::toLocalSystem (XC::Vector &eleVector, double &x, double &y,
                                    bool nonDimensionalize, bool signMult)
{
    if(!T)
    {
        checkT();
        return;
    }

#ifdef _G3DEBUG
    if(T->Size() != 2)
    {
        std::cerr << "WARNING: XC::YieldSurface_BC::toLocalSystem (XC::Vector &eleVector, double &x, double &y)\n";
        std::cerr << "T size may not be correct\n";
    }
#endif

	if(signMult==false)
	{
		x = eleVector((*T)(0));
		y = eleVector((*T)(1));
	}
	else
	{
    	x = eleVector((*T)(0))*((*S)(0));
    	y = eleVector((*T)(1))*((*S)(1));
	}

	if (nonDimensionalize)
	{
		x = x/capX;
		y = y/capY;
	}
}

void XC::YieldSurface_BC::toLocalSystem (XC::Vector &eleVector, double &x, double &y, double &z,
                                    bool nonDimensionalize, bool signMult)
{
    if(!T)
    {
        checkT();
        return;
    }

#ifdef _G3DEBUG
    if(T->Size() != 3)
    {
        std::cerr << "WARNING: XC::YieldSurface_BC::toLocalSystem (XC::Vector &eleVector, double &x, double &y, double &z)\n";
        std::cerr << "T size may not be correct\n";
    }
#endif

	if(signMult==false)
	{
		x = eleVector((*T)(0));
		y = eleVector((*T)(1));
		z = eleVector((*T)(2));
	}
	else
	{
    	x = eleVector((*T)(0))*((*S)(0));
    	y = eleVector((*T)(1))*((*S)(1));
    	z = eleVector((*T)(2))*((*S)(2));
	}

	if (nonDimensionalize)
	{
		x = x/capX;
		y = y/capY;
		z = z/capZ;
	}
}

void XC::YieldSurface_BC::toLocalSystem (XC::Matrix &eleMatrix, double &x, bool nonDimensionalize, bool signMult)
{
    if(!T)
    {
        checkT();
        return;
    }

#ifdef _G3DEBUG
    if(T->Size() != 1)
    {
        std::cerr << "WARNING: XC::YieldSurface_BC::toLocalSystem (XC::Matrix &eleMatrix, double &x)\n";
        std::cerr << "T size may not be correct\n";
    }
    if(eleMatrix.noCols() !=1)
    {
        std::cerr << "WARNING: XC::YieldSurface_BC::toLocalSystem (XC::Matrix &eleMatrix, ..)\n";
        std::cerr << "XC::Matrix columns should be = 1\n";
    }
#endif

	if(signMult==false)
	{
		x = eleMatrix((*T)(0),0);
	}
	else
	{
    	x = eleMatrix((*T)(0), 0)*((*S)(0));
	}
	if (nonDimensionalize)
	{
		x = x/capX;
	}
}

void XC::YieldSurface_BC::toLocalSystem (XC::Matrix &eleMatrix, double &x, double &y,
                                    bool nonDimensionalize, bool signMult)
{
    if(!T)
    {
        checkT();
        return;
    }

#ifdef _G3DEBUG
    if(T->Size() != 2)
    {
        std::cerr << "WARNING: XC::YieldSurface_BC::toLocalSystem (XC::Vector &eleVector, double &x, double &y)\n";
        std::cerr << "T size may not be correct\n";
    }
    if(eleMatrix.noCols() !=1)
    {
        std::cerr << "WARNING: XC::YieldSurface_BC::toLocalSystem (XC::Matrix &eleMatrix, ..)\n";
        std::cerr << "XC::Matrix columns should be = 1\n";
    }
#endif

	if(signMult==false)
	{
		x = eleMatrix((*T)(0),0);
		y = eleMatrix((*T)(1),0);
	}
	else
	{
    	x = eleMatrix((*T)(0), 0)*((*S)(0));
    	y = eleMatrix((*T)(1), 0)*((*S)(1));
	}

	if (nonDimensionalize)
	{
		x = x/capX;
		y = y/capY;
	}
}

void XC::YieldSurface_BC::toLocalSystem (XC::Matrix &eleMatrix, double &x, double &y, double &z,
                                    bool nonDimensionalize, bool signMult)
{
    if(!T)
    {
        checkT();
        return;
    }

#ifdef _G3DEBUG
    if(T->Size() != 3)
    {
        std::cerr << "WARNING: XC::YieldSurface_BC::toLocalSystem (XC::Vector &eleVector, double &x, double &y, double &z)\n";
        std::cerr << "T size may not be correct\n";
    }
    if(eleMatrix.noCols() !=1)
    {
        std::cerr << "WARNING: XC::YieldSurface_BC::toLocalSystem (XC::Matrix &eleMatrix, ..)\n";
        std::cerr << "XC::Matrix columns should be = 1\n";
    }
#endif

	if(signMult==false)
	{
		x = eleMatrix((*T)(0),0);
		y = eleMatrix((*T)(1),0);
		z = eleMatrix((*T)(2),0);
	}
	else
	{
    	x = eleMatrix((*T)(0), 0)*((*S)(0));
    	y = eleMatrix((*T)(1), 0)*((*S)(1));
    	z = eleMatrix((*T)(2), 0)*((*S)(2));
	}

	if (nonDimensionalize)
	{
		x = x/capX;
		y = y/capY;
		z = z/capZ;
	}
}

//////////////////////////////////////////////////////////////////////
// Transform to XC::Element Syatem
//////////////////////////////////////////////////////////////////////

void XC::YieldSurface_BC::toElementSystem(XC::Vector &eleVector, double &x, bool dimensionalize, bool signMult)
{
    if(!T)
    {
        checkT();
        return;
    }

#ifdef _G3DEBUG
    if(T->Size() != 1)
    {
        std::cerr << "WARNING: XC::YieldSurface_BC::toElementSystem (XC::Vector &eleVector, .. \n";
        std::cerr << "T size may not be correct\n";
    }
#endif

double x1 = x;
	if(dimensionalize)
	{
	 	x1 = x*capX;
	}
	if(signMult==false)
	{
		eleVector((*T)(0)) = x1;
	}
	else
    	eleVector((*T)(0)) = x1*((*S)(0));

}

void XC::YieldSurface_BC::toElementSystem(XC::Vector &eleVector, double &x, double &y,
                                     bool dimensionalize, bool signMult)
{
    if(!T)
    {
        checkT();
        return;
    }

#ifdef _G3DEBUG
    if(T->Size() != 2)
    {
        std::cerr << "WARNING: XC::YieldSurface_BC::toElementSystem (XC::Vector &eleVector, .. \n";
        std::cerr << "T size may not be correct\n";
    }
#endif

double x1 = x, y1 = y;
	if(dimensionalize)
	{
		x1 = x*capX;
		y1 = y*capY;
	}
	if(signMult==false)
	{
		eleVector((*T)(0)) = x1;
		eleVector((*T)(1)) = y1;
	}
	else
	{
    	eleVector((*T)(0)) = x1*((*S)(0));
    	eleVector((*T)(1)) = y1*((*S)(1));
	}

}

void XC::YieldSurface_BC::toElementSystem(XC::Vector &eleVector, double &x, double &y, double &z,
                                     bool dimensionalize, bool signMult)
{
    if(!T)
    {
        checkT();
        return;
    }

#ifdef _G3DEBUG
    if(T->Size() != 3)
    {
        std::cerr << "WARNING: XC::YieldSurface_BC::toElementSystem (XC::Vector &eleVector, .. \n";
        std::cerr << "T size may not be correct\n";
    }
#endif

double x1 = x, y1 = y, z1= z;
	if(dimensionalize)
	{
		x1 = x*capX;
		y1 = y*capY;
		z1 = z*capZ;
	}
	if(signMult==false)
	{
		eleVector((*T)(0)) = x1;
		eleVector((*T)(1)) = y1;
		eleVector((*T)(2)) = z1;
	}
	else
	{
    	eleVector((*T)(0)) = x1*((*S)(0));
    	eleVector((*T)(1)) = y1*((*S)(1));
    	eleVector((*T)(2)) = z1*((*S)(2));
	}
}

void XC::YieldSurface_BC::toElementSystem(XC::Matrix &eleMatrix, double &x, bool dimensionalize, bool signMult)
{
    if(!T)
    {
        checkT();
        return;
    }

#ifdef _G3DEBUG
    if(T->Size() != 1)
    {
        std::cerr << "WARNING: XC::YieldSurface_BC::toElementSystem (XC::Matrix &eleMatrix, .. \n";
        std::cerr << "T size may not be correct\n";
    }
    if(eleMatrix.noCols() != 1)
    {
        std::cerr << "WARNING: XC::YieldSurface_BC::toElementSystem (XC::Matrix &eleMatrix, .. \n";
        std::cerr << "eleMatrix columns not equal to 1\n";
    }
#endif

double x1 = x;
	if(dimensionalize)
	{
		x1 = x*capX;
	}

	if(signMult==false)
	{
		eleMatrix((*T)(0),0) = x1;
	}
	else
	{
		eleMatrix((*T)(0),0) = x1*((*S)(0));
	}

}

void XC::YieldSurface_BC::toElementSystem(XC::Matrix &eleMatrix, double &x, double &y,
                                     bool dimensionalize, bool signMult)
{
    if(!T)
    {
        checkT();
        return;
    }

#ifdef _G3DEBUG
    if(T->Size() != 2)
    {
        std::cerr << "WARNING: XC::YieldSurface_BC::toElementSystem (XC::Matrix &eleMatrix, .. \n";
        std::cerr << "T size may not be correct\n";
    }
    if(eleMatrix.noCols() != 1)
    {
        std::cerr << "WARNING: XC::YieldSurface_BC::toElementSystem (XC::Matrix &eleMatrix, .. \n";
        std::cerr << "eleMatrix columns not equal to 1\n";
    }
#endif

double x1 = x, y1 = y;
	if(dimensionalize)
	{
		x1 = x*capX;
		y1 = y*capY;
	}
	if(signMult==false)
	{
		eleMatrix((*T)(0),0) = x1;
		eleMatrix((*T)(1),0) = y1;
	}
	else
	{
		eleMatrix((*T)(0),0) = x1*((*S)(0));
    	eleMatrix((*T)(1),0) = y1*((*S)(1));
	}


}


void XC::YieldSurface_BC::toElementSystem(XC::Matrix &eleMatrix, double &x, double &y, double &z,
                                     bool dimensionalize, bool signMult)
{
    if(!T)
    {
        checkT();
        return;
    }

#ifdef _G3DEBUG
    if(T->Size() != 3)
    {
        std::cerr << "WARNING: XC::YieldSurface_BC::toElementSystem (XC::Matrix &eleMatrix, .. \n";
        std::cerr << "T size may not be correct\n";
    }
    if(eleMatrix.noCols() != 1)
    {
        std::cerr << "WARNING: XC::YieldSurface_BC::toElementSystem (XC::Matrix &eleMatrix, .. \n";
        std::cerr << "eleMatrix columns not equal to 1\n";
    }
#endif

double x1 = x, y1 = y, z1 = z;
	if(dimensionalize)
	{
		x1 = x*capX;
		y1 = y*capY;
		z1 = z*capZ;
	}
	if(signMult==false)
	{
		eleMatrix((*T)(0),0) = x1;
		eleMatrix((*T)(1),0) = y1;
		eleMatrix((*T)(2),0) = z1;
	}
	else
	{
		eleMatrix((*T)(0),0) = x1*((*S)(0));
    	eleMatrix((*T)(1),0) = y1*((*S)(1));
    	eleMatrix((*T)(2),0) = z1*((*S)(2));
	}

}

int XC::YieldSurface_BC::checkT(void)
{
    if(!T)
    {
        std::cerr << "FATAL: XC::YieldSurface_BC::checkT(void)\n";
        std::cerr << "T = null, use setTransformation(..) after the XC::YS object is created\n";
        std::cerr << "\a";
        return 0;
    }
    return 1;
}

//////////////////////////////////////////////////////////////////////
// Other
//////////////////////////////////////////////////////////////////////

int XC::YieldSurface_BC::update(int flag)
  { return 0; }

void XC::YieldSurface_BC::Print(std::ostream &s, int flag)
{
	s << "YieldSurface_BC - tag = " << this->getTag() << std::endl;
	s << "Element Info:\n";
	s << "Element Tag = " << ele_Tag << "\t Location = " << ele_Location << std::endl;
	s << "-----------------------------------------" << std::endl;
}

// friend std::ostream &operator<<(std::ostream &s, const XC::YieldSurface_BC &ys)
//      A function is defined to allow user to print the vectors using std::ostreams.
/* // Already defined in XC::TaggedObject
 std::ostream &operator<<(std::ostream &s, const XC::YieldSurface_BC &ys)
{
	ys.Print(s);
  return s;
}
*/
