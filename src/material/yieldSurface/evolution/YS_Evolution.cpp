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
// YS_Evolution.cpp: implementation of the YS_Evolution class.
//
//////////////////////////////////////////////////////////////////////

#include "material/yieldSurface/evolution/YS_Evolution.h"

XC::Vector XC::YS_Evolution::crd1(1);
XC::Vector XC::YS_Evolution::crd2(2);
XC::Vector XC::YS_Evolution::crd3(3);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::YS_Evolution::YS_Evolution(int tag, int classtag,
                                     double iso_ratio, double kin_ratio,
                                     int _dimension,
                                     double shr_iso, double shr_kin)
  :TaggedObject(tag), MovableObject(classtag),freezeEvolution(false), deformable(false),
   isotropicFactor_hist(_dimension), isotropicFactor(_dimension), 
   translate_hist(_dimension), translate(_dimension), translate_init(_dimension),
   isotropicRatio_orig(iso_ratio), isotropicRatio(iso_ratio),isotropicRatio_shrink(shr_iso),
   kinematicRatio_orig(kin_ratio), kinematicRatio(kin_ratio), kinematicRatio_shrink(shr_kin),
   dimension(_dimension)
{
	/*if( (isotropicRatio + kinematicRatio) != 1.0 || (isotropicRatio + kinematicRatio) != 0.0)
	{
	 	std::cerr << "WARNING XC::YS_Evolution(..) -  isotropicRatio + kinematicRatio != 1 or 0 -> "
		  << isotropicRatio + kinematicRatio << std::endl;
		// this always gives warning message
	}*/
	translate.Zero();
	translate_hist.Zero();
	translate_init.Zero();

//	std::cerr << "DIM = " << dimension << std::endl;
	for(int i=0; i<dimension;i++)
	{
		isotropicFactor(i)=1;
		isotropicFactor_hist(i)=1;
	}
}

void XC::YS_Evolution::setInitTranslation(XC::Vector &initTranslate)
  {
    if(initTranslate.Size() > dimension)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING -  newTranslate" << initTranslate
		  << " outside the dimensions\n";
      }
    translate = initTranslate;
    translate_hist = initTranslate;
    translate_init = initTranslate;
  }

const XC::Vector &XC::YS_Evolution::getInitTranslation(void)
{
	return translate_init;
}


int XC::YS_Evolution::update(int flag)
{
	// does nothing
	return 0;
}

void XC::YS_Evolution::setResidual(double res)
{
	// does nothing
}

	
int XC::YS_Evolution::commitState()
{
	isotropicFactor_hist   = isotropicFactor;
	translate_hist         = translate;
	return 0;
}

int XC::YS_Evolution::revertToLastCommit(void)
{
	isotropicFactor   =  isotropicFactor_hist;
	translate         =  translate_hist;
	return 0;
}

void  XC::YS_Evolution::toDeformedCoord(XC::Vector &coord)
{
//	if(getTag() == 10)
//	{
//		std::cerr << "DIM: " << dimension << std::endl;
//		std::cerr << "ISO: " << isotropicFactor;
//		std::cerr << "\a";
//		}
	
    // no vector multipication in opensees
    for(int i=0; i< coord.Size(); i++)
    {
		coord(i) = coord(i)*isotropicFactor(i) + translate(i);
	}

}
	
void  XC::YS_Evolution::toOriginalCoord(XC::Vector &coord)
{
    for(int i=0; i< coord.Size(); i++)
    {
		double notrans = coord(i) - translate(i);
		coord(i) = notrans/isotropicFactor(i);
	}

}

void XC::YS_Evolution::toDeformedCoord(double &x)
{
//	std::cerr << "WARNING XC::YS_Evolution::toDeformedCoord(.) - This method should not be called\n";
	crd1(0) = x;
	this->toDeformedCoord(crd1);
	x = crd1(0);
}

void XC::YS_Evolution::toDeformedCoord(double &x, double &y)
{
//  std::cerr << "WARNING XC::YS_Evolution::toDeformedCoord(..) - This method should not be called\n";
	crd2(0) = x;
	crd2(1) = y;
	this->toDeformedCoord(crd2);
	x = crd2(0);
	y = crd2(1);
}

void XC::YS_Evolution::toDeformedCoord(double &x, double &y, double &z)
{
//	std::cerr << "WARNING XC::YS_Evolution::toDeformedCoord(...) - This method should not be called\n";
	crd3(0) = x;
	crd3(1) = y;
	crd3(2) = z;
	this->toDeformedCoord(crd3);
	x = crd3(0);
	y = crd3(1);
	z = crd3(2);

}

void XC::YS_Evolution::toOriginalCoord(double &x)
{
//	std::cerr << "WARNING XC::YS_Evolution::toOriginalCoord(.) - This method should not be called\n";
	crd1(0) = x;
	this->toOriginalCoord(crd1);
	x = crd1(0);

}

void XC::YS_Evolution::toOriginalCoord(double &x, double &y)
{
//	std::cerr << "WARNING XC::YS_Evolution::toOriginalCoord(..) - This method should not be called\n";
	crd2(0) = x;
	crd2(1) = y;
	this->toOriginalCoord(crd2);
	x = crd2(0);
	y = crd2(1);
}

void XC::YS_Evolution::toOriginalCoord(double &x, double &y, double &z)
{
//	std::cerr << "WARNING XC::YS_Evolution::toOriginalCoord(...) - This method should not be called\n";
	crd3(0) = x;
	crd3(1) = y;
	crd3(2) = z;
	this->toOriginalCoord(crd3);
	x = crd3(0);
	y = crd3(1);
	z = crd3(2);

}

double XC::YS_Evolution::getCommitTranslation(int dir)
{
#ifdef _G3DEBUG
	checkDimension(dir);
#endif

	return translate_hist(dir);
}

double XC::YS_Evolution::getTrialTranslation(int dir)
{
#ifdef _G3DEBUG
	checkDimension(dir);
#endif

	return translate(dir);
}

double XC::YS_Evolution::getCommitIsotropicFactor(int dir)
{
	return isotropicFactor_hist(dir);
}

double XC::YS_Evolution::getTrialIsotropicFactor(int dir)
{
	return isotropicFactor(dir);
}

void XC::YS_Evolution::checkDimension(int dir)
  {
    if(dir < 0 || dir >= dimension)
      {
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; WARNING - Direction " << dir
		    << " outside the dimensions\n";
      }

  }

void  XC::YS_Evolution::setDeformable(bool defo)
{
	this->deformable = defo;
}

void XC::YS_Evolution::Print(std::ostream &s, int flag) const
  { s << " YS_Evolution - tag = " << getTag() << std::endl; }

//! @brief Return generalized stress.
const XC::Vector &XC::YS_Evolution::getGeneralizedStress(void) const
  {
    static Vector retval(1);
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; Not implemented." << std::endl;
    return retval;
  }

//! @brief Return generalized strain.
const XC::Vector &XC::YS_Evolution::getGeneralizedStrain(void) const
  {
    static Vector retval(1);
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; Not implemented." << std::endl;
    return retval;
  }

/*std::ostream &operator<<(std::ostream &s, const XC::YS_Evolution &hModel)
{
	hModel.Print(s);
  return s;
}*/

	
	
	
