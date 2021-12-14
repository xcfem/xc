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
// $Date: 2003/03/04 00:44:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/distributions/Type2LargestValueRV.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/domain/distributions/Type2LargestValueRV.h>
#include "GammaRV.h"
#include <cmath>
#include <cstring>
#include <classTags.h>


XC::Type2LargestValueRV::Type2LargestValueRV(int passedTag, 
		 double passedMean,
		 double passedStdv,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_type2largestvalue)
{
	tag = passedTag ;
	setParameters(passedMean,passedStdv);
	startValue = passedStartValue;
}
XC::Type2LargestValueRV::Type2LargestValueRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_type2largestvalue)
{
	tag = passedTag ;
	u = passedParameter1;
	k = passedParameter2;
	startValue = passedStartValue;
}
XC::Type2LargestValueRV::Type2LargestValueRV(int passedTag, 
		 double passedMean,
		 double passedStdv)
:RandomVariable(passedTag, RANDOM_VARIABLE_type2largestvalue)
{
	tag = passedTag ;
	setParameters(passedMean,passedStdv);
	startValue = getMean();
}
XC::Type2LargestValueRV::Type2LargestValueRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4)
:RandomVariable(passedTag, RANDOM_VARIABLE_type2largestvalue)
{
	tag = passedTag ;
	u = passedParameter1;
	k = passedParameter2;
	startValue = getMean();
}


void
XC::Type2LargestValueRV::Print(std::ostream &s, int flag) const
{
}


double
XC::Type2LargestValueRV::getPDFvalue(double rvValue)
{
	double result;
	if ( 0.0 < rvValue ) {
		result = k/u * pow(u/rvValue,k+1) * exp(-pow(u/rvValue,k));
	}
	else {
		result = 0.0;
	}
	return result;
}


double
XC::Type2LargestValueRV::getCDFvalue(double rvValue)
{
	double result;
	if ( 0.0 < rvValue ) {
		result = exp(-pow(u/rvValue,k));
	}
	else {
		result = 0.0;
	}
	return result;
}


double
XC::Type2LargestValueRV::getInverseCDFvalue(double probValue)
{
	return u * pow((-log(probValue)),(-1.0/k));
}


const std::string XC::Type2LargestValueRV::getType(void)
{
	return "TYPE2LARGESTVALUE";
}


double 
XC::Type2LargestValueRV::getMean()
{
	double result;
	GammaRV *aGammaRV = new GammaRV(1, 0.0, 1.0, 0.0);
	result = u * aGammaRV->gammaFunction(1.0-1.0/k);
	delete aGammaRV;
	return result;
}



double 
XC::Type2LargestValueRV::getStdv()
{
	GammaRV *aGammaRV = new GammaRV(1, 0.0, 1.0, 0.0);
	double a = aGammaRV->gammaFunction(1.0-2.0/k);
	double b = aGammaRV->gammaFunction(1.0-1.0/k);
	delete aGammaRV;
	double result = u*sqrt(a-b*b);
	return result;
}


double 
XC::Type2LargestValueRV::getStartValue()
{
	return startValue;
}

double XC::Type2LargestValueRV::getParameter1()  {return u;}
double XC::Type2LargestValueRV::getParameter2()  {return k;}
double XC::Type2LargestValueRV::getParameter3()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}
double XC::Type2LargestValueRV::getParameter4()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}



void
XC::Type2LargestValueRV::setParameters(double mean, double stdv)
{
	double cov = stdv/mean;
	double xk = 1.5;
	function131(xk, cov,mean);
}


void
XC::Type2LargestValueRV::function131(double xk, double cov, double mean)
{
	GammaRV *aGammaRV = new GammaRV(1, 0.0, 1.0, 0.0);
	xk = xk + 1.0;
	double x1 = 1.0 - 1.0/xk;
	double x2 = 1.0 - 2.0/xk;
	double gm1 = aGammaRV->gammaFunction(x1);
	double gm2 = aGammaRV->gammaFunction(x2);
	delete aGammaRV;
	double vy = sqrt(gm2/gm1/gm1 - 1.0);
	if (cov-vy < 0.0) 
		function131(xk,cov,mean);
	else if (cov-vy == 0.0) 
		function134(xk,cov,mean);
	else 
		function132(xk,cov,mean);
}
void
XC::Type2LargestValueRV::function132(double xk, double cov, double mean)
{
	xk = xk - 0.1;
	double x1 = 1.0 - 1.0/xk;
	double x2 = 1.0 - 2.0/xk;
	GammaRV *aGammaRV = new GammaRV(1, 0.0, 1.0, 0.0);
	double gm1 = aGammaRV->gammaFunction(x1);
	double gm2 = aGammaRV->gammaFunction(x2);
	delete aGammaRV;
	double vy = sqrt(gm2/gm1/gm1 - 1.0);
	if (cov-vy < 0.0) 
		function133(xk,cov,mean);
	else if (cov-vy == 0.0) 
		function134(xk,cov,mean);
	else 
		function132(xk,cov,mean);
}
void
XC::Type2LargestValueRV::function133(double xk, double cov, double mean)
{
	xk = xk + 0.01;
	double x1 = 1.0 - 1.0/xk;
	double x2 = 1.0 - 2.0/xk;
	GammaRV *aGammaRV = new GammaRV(1, 0.0, 1.0, 0.0);
	double gm1 = aGammaRV->gammaFunction(x1);
	double gm2 = aGammaRV->gammaFunction(x2);
	delete aGammaRV;
	double vy = sqrt(gm2/gm1/gm1 - 1.0);
	if (cov-vy < 0.0) 
		function133(xk,cov,mean);
	else if (cov-vy == 0.0) 
		function134(xk,cov,mean);
	else 
		function134(xk,cov,mean);


}
void
XC::Type2LargestValueRV::function134(double xk, double gm1, double mean)
{
	u = mean/gm1;
	k = xk;
}
