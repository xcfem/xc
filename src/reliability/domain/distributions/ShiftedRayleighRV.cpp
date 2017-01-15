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
// $Date: 2003/03/04 00:44:35 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/distributions/ShiftedRayleighRV.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/domain/distributions/ShiftedRayleighRV.h>
#include <math.h>
#include <string.h>
#include <classTags.h>


XC::ShiftedRayleighRV::ShiftedRayleighRV(int passedTag, 
		 double passedMean,
		 double passedStdv,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_shiftedrayleigh)
{
	tag = passedTag;
	double pi = 3.14159265358979;
	u = 2.0 * passedStdv / sqrt(3.0*pi+4.0);
	x0 = passedMean + passedStdv*sqrt(pi) / sqrt(3.0*pi+4.0);
	startValue = passedStartValue;
}
XC::ShiftedRayleighRV::ShiftedRayleighRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_shiftedrayleigh)
{
	tag = passedTag ;
	u = passedParameter1;
	x0 = passedParameter2;
	startValue = passedStartValue;
}
XC::ShiftedRayleighRV::ShiftedRayleighRV(int passedTag, 
		 double passedMean,
		 double passedStdv)
:RandomVariable(passedTag, RANDOM_VARIABLE_shiftedrayleigh)
{
	tag = passedTag;
	double pi = 3.14159265358979;
	u = 2.0 * passedStdv / sqrt(3.0*pi+4.0);
	x0 = passedMean + passedStdv*sqrt(pi) / sqrt(3.0*pi+4.0);
	startValue = getMean();
}
XC::ShiftedRayleighRV::ShiftedRayleighRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4)
:RandomVariable(passedTag, RANDOM_VARIABLE_shiftedrayleigh)
{
	tag = passedTag ;
	u = passedParameter1;
	x0 = passedParameter2;
	startValue = getMean();
}




void
XC::ShiftedRayleighRV::Print(std::ostream &s, int flag)
{
}


double
XC::ShiftedRayleighRV::getPDFvalue(double rvValue)
{
	double result;
	if ( x0 < rvValue ) {
		result = 2.0*(rvValue-x0)/(u*u) * exp(-pow(((rvValue-x0)/u),2));
	}
	else {
		result = 0.0;
	}
	return result;
}


double
XC::ShiftedRayleighRV::getCDFvalue(double rvValue)
{
	double result;
	if ( x0 < rvValue ) {
		result = 1 - exp(-pow(((rvValue-x0)/u),2));
	}
	else {
		result = 0.0;
	}
	return result;
}


double
XC::ShiftedRayleighRV::getInverseCDFvalue(double probValue)
{
	return x0 + u * sqrt(-log(1-probValue));
}


const char *
XC::ShiftedRayleighRV::getType()
{
	return "SHIFTEDRAYLEIGH";
}


double 
XC::ShiftedRayleighRV::getMean()
{
	double pi = 3.14159265358979;
	return x0 + 0.5 * u * sqrt(pi);
}



double 
XC::ShiftedRayleighRV::getStdv()
{
	double pi = 3.14159265358979;
	return 0.5 * u * sqrt(4.0-pi);
}


double 
XC::ShiftedRayleighRV::getStartValue()
{
	return startValue;
}


double XC::ShiftedRayleighRV::getParameter1()  {return u;}
double XC::ShiftedRayleighRV::getParameter2()  {return x0;}
double XC::ShiftedRayleighRV::getParameter3()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}
double XC::ShiftedRayleighRV::getParameter4()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}
