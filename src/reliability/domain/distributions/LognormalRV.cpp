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
                                                                        
// $Revision: 1.7 $
// $Date: 2003/10/27 23:04:39 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/distributions/LognormalRV.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/domain/distributions/LognormalRV.h>
#include <reliability/domain/distributions/NormalRV.h>
#include <math.h>
#include <string.h>
#include <classTags.h>


XC::LognormalRV::LognormalRV(int passedTag, 
		 double passedMean,
		 double passedStdv,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_lognormal)
{
	if (passedMean<0.0) {
		isPositive = false;
		passedMean = -passedMean;
	}
	else {
		isPositive = true;
	}

	tag = passedTag ;
	zeta = sqrt(   log(   1+(passedStdv/passedMean)*(passedStdv/passedMean)   )   );
	lambda = log(passedMean) - 0.5*zeta*zeta;
	startValue = passedStartValue;
}



XC::LognormalRV::LognormalRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_lognormal)
{
	if (passedParameter2<0.0) {
		isPositive = false;
		passedParameter2 = -passedParameter2;
	}
	else {
		isPositive = true;
	}

	tag = passedTag ;
	lambda = passedParameter1;
	zeta = passedParameter2;
	startValue = passedStartValue;
}



XC::LognormalRV::LognormalRV(int passedTag, 
		 double passedMean,
		 double passedStdv)
:RandomVariable(passedTag, RANDOM_VARIABLE_lognormal)
{
	if (passedMean<0.0) {
		isPositive = false;
		passedMean = -passedMean;
	}
	else {
		isPositive = true;
	}

	tag = passedTag ;
	zeta = sqrt(   log(   1+(passedStdv/passedMean)*(passedStdv/passedMean)   )   );
	lambda = log(passedMean) - 0.5*zeta*zeta;
	startValue = getMean();
}
XC::LognormalRV::LognormalRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4)
:RandomVariable(passedTag, RANDOM_VARIABLE_lognormal)
{
	if (passedParameter2<0.0) {
		isPositive = false;
		passedParameter2 = -passedParameter2;
	}
	else {
		isPositive = true;
	}

	tag = passedTag ;
	lambda = passedParameter1;
	zeta = passedParameter2;
	startValue = getMean();
}


void
XC::LognormalRV::Print(std::ostream &s, int flag)
{
}


double
XC::LognormalRV::getPDFvalue(double rvValue)
{
	if (!isPositive) {
		// The formal answer is: f(x) = f_pos(x+2|x|), but let's do it simple here
		rvValue = -rvValue;
	}

	double result;
	if ( 0.0 < rvValue ) {
		double pi = 3.14159265358979;
		result = 1/(sqrt(2*pi)*zeta*rvValue) * exp(-0.5* pow ( (log(rvValue)-lambda) / zeta, 2 )  );
	}
	else {
		result = 0.0;
	}
	return result;
}


double
XC::LognormalRV::getCDFvalue(double rvValue)
{
	double result;

	static XC::NormalRV aStandardNormalRV( 1, 0.0, 1.0, 0.0);
	
	if (isPositive) {
		if ( 0.0 < rvValue ) {
			result = aStandardNormalRV.getCDFvalue((log(rvValue)-lambda)/zeta);
		}
		else {
			result = 0.0;
		}
	}
	else {
		if ( rvValue < 0.0 ) {
			result = aStandardNormalRV.getCDFvalue((log(fabs(rvValue))-lambda)/zeta);
			result = 1.0-result;
		}
		else {
			result = 1.0;
		}
	}

	// Return result depending on type of random variable
	if (isPositive) {
		return result;
	}
	else {
		return 1-result;
	}



/*
	// First, flip it around if it's a negative lognormal
	if (!isPositive) {
		rvValue = -rvValue;
	}

	// Compute the ordinary CDF
	double result;
	if ( 0.0 < rvValue ) {
		RandomVariable *aStandardNormalRV;
		aStandardNormalRV= new NormalRV( 1, 0.0, 1.0, 0.0);
		result = aStandardNormalRV->getCDFvalue((log(rvValue)-lambda)/zeta);
		delete aStandardNormalRV;	
	}
	else {
		result = 0.0;
	}

	// Return result depending on type of random variable
	if (isPositive) {
		return result;
	}
	else {
		return 1-result;
	}
*/
}


double
XC::LognormalRV::getInverseCDFvalue(double probValue)
{
	if ( probValue > 1.0 || probValue < 0.0) {
		std::cerr << "Invalid probability value input to inverse CDF function" << std::endl;
		return 0.0;
	}
	else {
		static XC::NormalRV aStandardNormalRV( 1, 0.0, 1.0, 0.0);

		if (isPositive) {
			double inverseNormal = aStandardNormalRV.getInverseCDFvalue(probValue);
			return exp(inverseNormal*zeta + lambda);
		}
		else {
			double inverseNormal = aStandardNormalRV.getInverseCDFvalue(1.0-probValue);
			return (-exp(inverseNormal*zeta + lambda));
		}
	}
}


const char *
XC::LognormalRV::getType()
{
	return "LOGNORMAL";
}


double 
XC::LognormalRV::getMean()
{
	if (isPositive) {
		return exp(lambda+0.5*zeta*zeta);
	}
	else {
		return -exp(lambda+0.5*zeta*zeta);
	}
}



double 
XC::LognormalRV::getStdv()
{
	return exp(lambda+0.5*zeta*zeta)*sqrt(exp(zeta*zeta)-1);
}


double 
XC::LognormalRV::getStartValue()
{
	return startValue;
}


double XC::LognormalRV::getParameter1()  {return lambda;}

double XC::LognormalRV::getParameter2()  
{
	if (isPositive) {
		return zeta;
	}
	else {
		return -zeta;
	}
}

double XC::LognormalRV::getParameter3()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}

double XC::LognormalRV::getParameter4()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}
