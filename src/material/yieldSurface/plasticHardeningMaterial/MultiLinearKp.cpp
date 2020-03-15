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
// MultiLinearKp.cpp: implementation of the XC::MultiLinearKp class.
//
//////////////////////////////////////////////////////////////////////

#include "material/yieldSurface/plasticHardeningMaterial/MultiLinearKp.h"
#include <cstdlib>

#define MAT_TAG_MULTILINEAR -1
#define DEBG 0
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::MultiLinearKp::MultiLinearKp(int tag)
  :PlasticHardeningMaterial(tag,MAT_TAG_MULTILINEAR)
  {}

XC::MultiLinearKp::MultiLinearKp(int tag, XC::Vector &sum_plas_defo, XC::Vector &kp)
:PlasticHardeningMaterial(tag,MAT_TAG_MULTILINEAR),
sumPlasDefo(sum_plas_defo.Size()+1), Kp(kp.Size()+1)
{
 	if(sumPlasDefo.Size() != Kp.Size())
 	{
 		std::cerr << "ERROR MultiLinear() - incompatible vector sizes\n";
 	}

 	numPoints = sum_plas_defo.Size();

 	for(int i=0; i < numPoints; i++)
 	{
 		sumPlasDefo(i) =  sum_plas_defo(i);
 		if(sumPlasDefo(i) < 0)
 			std::cerr << "ERROR MultiLinear() -  sumPlasDefo < 0\n";

 		Kp(i) = kp(i);
 	}
 	if(sumPlasDefo(0) != 0)
 		std::cerr << "WARNING MultiLinear() -  sumPlasDefo(0) != 0\n";

 	Kp(numPoints)          = Kp(numPoints -1);
 	sumPlasDefo(numPoints) = sumPlasDefo(numPoints -1)*1000;

  }

//! @brief Return trial value of plastic stiffness.
double XC::MultiLinearKp::getTrialPlasticStiffness(void) const
  {
    double K = 0;
    double sumDisp = val_trial;

	if( sumDisp > sumPlasDefo(numPoints-1))
	{
		K = residual*Kp(numPoints-1);
		
		if(sFactor != 1.0)
			K = Kp(0)*sFactor;
		return K;
	}
		
	for(int i=0; i< numPoints; i++)
	{
		double x1 = sumPlasDefo(i);
		double y1 = Kp(i);
		double x2 = sumPlasDefo(i+1);
		double y2 = Kp(i+1);
		
		if(sumDisp < x2 && sumDisp >= x1)
		{
			if (sumDisp == x1)
				return y1;
				
			if(x2 == x1)
			{
				std::cerr << "WARNING - XC::MultiLinear::getTangent() x2 -x1 = 0 \n";
				return 0;
			}
			
			double m = (y2 - y1)/(x2 -x1);
		    double b = y1 - m*x1;
		    K = m*sumDisp + b;
		 	break;
		}
	}

	if(sFactor != 1.0)
		K = Kp(0)*sFactor;
	else
		K = residual*K;

	// std::cerr << "K = " << K << ", sFactor = " << sFactor << std::endl; // std::cerr << "\a";
	return K;
}


void XC::MultiLinearKp::Print(std::ostream &s, int flag) const
  {
	this->XC::PlasticHardeningMaterial::Print(s, flag);
	
	s << "+-MultiLinear" << std::endl;
	s << "    Kp = " << this->getTrialPlasticStiffness();
	s << "    SumPlasDefo XC::Vector = " <<  sumPlasDefo;
	s << "    Kp XC::Vector          = " <<  Kp << std::endl;
  }

XC::PlasticHardeningMaterial *XC::MultiLinearKp::getCopy(void)
  {
    Vector spd(numPoints);
    Vector kp(numPoints);

    for(int i =0; i < numPoints; i++)
      {
    	spd(i) =  sumPlasDefo(i);
    	kp(i)  =  Kp(i);
      }

    // Don't want to pass the actual vectors or else the size will
    // keep on increasing by 1
    PlasticHardeningMaterial *theMat = new MultiLinearKp(getTag(), spd, kp);
    return theMat;
  }

