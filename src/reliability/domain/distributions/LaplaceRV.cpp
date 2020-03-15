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
// $Date: 2003/03/04 00:44:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/distributions/LaplaceRV.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu) 
//

#include <reliability/domain/distributions/LaplaceRV.h>
#include <math.h>
#include <string.h>
#include <classTags.h>


XC::LaplaceRV::LaplaceRV(int passedTag, 
		 double passedMean,
		 double passedStdv,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_laplace)
{
	tag = passedTag ;
	alpha = passedMean;
	beta = sqrt(2.0)/passedStdv;
	startValue = passedStartValue;
}
XC::LaplaceRV::LaplaceRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_laplace)
{
	tag = passedTag ;
	alpha = passedParameter1;
	beta = passedParameter2;
	startValue = passedStartValue;
}
XC::LaplaceRV::LaplaceRV(int passedTag, 
		 double passedMean,
		 double passedStdv)
:RandomVariable(passedTag, RANDOM_VARIABLE_laplace)
{
	tag = passedTag ;
	alpha = passedMean;
	beta = sqrt(2.0)/passedStdv;
	startValue = getMean();
}
XC::LaplaceRV::LaplaceRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4)
:RandomVariable(passedTag, RANDOM_VARIABLE_laplace)
{
	tag = passedTag ;
	alpha = passedParameter1;
	beta = passedParameter2;
	startValue = getMean();
}


void
XC::LaplaceRV::Print(std::ostream &s, int flag) const
{
}


double
XC::LaplaceRV::getPDFvalue(double rvValue)
{
	return 0.5*beta*exp(-beta*fabs(rvValue-alpha));
}


double
XC::LaplaceRV::getCDFvalue(double rvValue)
{
	double result;
	if (rvValue < alpha)  {
		result = 0.5*exp(-beta*fabs(rvValue-alpha));
	}
	else  {
		result = 1 - 0.5*exp(-beta*fabs(rvValue-alpha));
	}
	return result;
}


double
XC::LaplaceRV::getInverseCDFvalue(double rvValue)
{
	return 0.0;
}


const char *
XC::LaplaceRV::getType()
{
	return "LAPLACE";
}


double 
XC::LaplaceRV::getMean()
{
	return alpha;
}



double 
XC::LaplaceRV::getStdv()
{
	return sqrt(2.0)/beta;
}


double 
XC::LaplaceRV::getStartValue()
{
	return startValue;
}

double XC::LaplaceRV::getParameter1()  {return alpha;}
double XC::LaplaceRV::getParameter2()  {return beta;}
double XC::LaplaceRV::getParameter3()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}
double XC::LaplaceRV::getParameter4()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}
