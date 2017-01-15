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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 2001, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** Reliability module developed by:                                   **
**   Terje Haukaas (haukaas@ce.berkeley.edu)                          **
**   Armen Der Kiureghian (adk@ce.berkeley.edu)                       **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.6 $
// $Date: 2003/03/04 00:44:37 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/distributions/Type3SmallestValueRV.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/domain/distributions/Type3SmallestValueRV.h>
#include "GammaRV.h"
#include <cmath>
#include <cstring>
#include <classTags.h>


XC::Type3SmallestValueRV::Type3SmallestValueRV(int passedTag, 
		 double passedMean,
		 double passedStdv,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_type3smallestvalue)
{
	// Note: this constructor is void.
	std::cerr << "WARNING: This type of random variable is not uniquely defined by mean and stdv." << std::endl;
}
XC::Type3SmallestValueRV::Type3SmallestValueRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_type3smallestvalue)
{
	tag = passedTag ;
	epsilon = passedParameter1;
	u = passedParameter2;
	k = passedParameter3;
	startValue = passedStartValue;
}
XC::Type3SmallestValueRV::Type3SmallestValueRV(int passedTag, 
		 double passedMean,
		 double passedStdv)
:RandomVariable(passedTag, RANDOM_VARIABLE_type3smallestvalue)
{
	// Note: this constructor is void.
	std::cerr << "WARNING: This type of random variable is not uniquely defined by mean and stdv." << std::endl;
}
XC::Type3SmallestValueRV::Type3SmallestValueRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4)
:RandomVariable(passedTag, RANDOM_VARIABLE_type3smallestvalue)
{
	tag = passedTag ;
	epsilon = passedParameter1;
	u = passedParameter2;
	k = passedParameter3;
	startValue = getMean();
}


void
XC::Type3SmallestValueRV::Print(std::ostream &s, int flag)
{
}


double
XC::Type3SmallestValueRV::getPDFvalue(double rvValue)
{
	double result;
	if ( epsilon < rvValue ) {
		result = k/(u-epsilon) * pow(((rvValue-epsilon)/(u-epsilon)),k-1) * exp(-pow(((rvValue-epsilon)/(u-epsilon)),k));
	}
	else {
		result = 0.0;
	}
	return result;
}


double
XC::Type3SmallestValueRV::getCDFvalue(double rvValue)
{
	double result;
	if ( epsilon < rvValue ) {
		result = 1 - exp(-pow(((rvValue-epsilon)/(u-epsilon)),k));
	}
	else {
		result = 0.0;
	}
	return result;
}


double
XC::Type3SmallestValueRV::getInverseCDFvalue(double probValue)
{
	return (u-epsilon) * ( epsilon/(u-epsilon) + pow((-log(1-probValue)),(1/k)) );
}


const char *
XC::Type3SmallestValueRV::getType()
{
	return "TYPE3SMALLESTVALUE";
}


double 
XC::Type3SmallestValueRV::getMean()
{
	double result;
	GammaRV *aGammaRV = new XC::GammaRV(1, 0.0, 1.0, 0.0);
	result = epsilon + (u-epsilon) * aGammaRV->gammaFunction(1.0+1.0/k);
	delete aGammaRV;
	return result;
}



double 
XC::Type3SmallestValueRV::getStdv()
{
	GammaRV *aGammaRV = new XC::GammaRV(1, 0.0, 1.0, 0.0);
	double a = aGammaRV->gammaFunction(1.0+2.0/k);
	double b = aGammaRV->gammaFunction(1.0+1.0/k);
	delete aGammaRV;
	return (u-epsilon) * sqrt(a-b*b);
}


double 
XC::Type3SmallestValueRV::getStartValue()
{
	return startValue;
}


double XC::Type3SmallestValueRV::getParameter1()  {return epsilon;}
double XC::Type3SmallestValueRV::getParameter2()  {return u;}
double XC::Type3SmallestValueRV::getParameter3()  {return k;}
double XC::Type3SmallestValueRV::getParameter4()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}


