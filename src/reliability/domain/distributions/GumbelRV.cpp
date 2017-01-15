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
// $Date: 2004/08/27 17:51:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/distributions/GumbelRV.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include "GumbelRV.h"
#include <cmath>
#include <cstring>
#include <classTags.h>


XC::GumbelRV::GumbelRV(int passedTag, 
		 double passedMean,
		 double passedStdv,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_gumbel)
{
	tag = passedTag ;
	double pi = 3.14159265358979;
	double gamma = 0.5772156649;
	u = passedMean - (gamma*sqrt(6.0)*passedStdv)/pi;
	alpha = pi / ( sqrt(6.0) * passedStdv );
	startValue = passedStartValue;
}
XC::GumbelRV::GumbelRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_gumbel)
{
	tag = passedTag ;
	u = passedParameter1;
	alpha = passedParameter2;
	startValue = passedStartValue;
}
XC::GumbelRV::GumbelRV(int passedTag, 
		 double passedMean,
		 double passedStdv)
:RandomVariable(passedTag, RANDOM_VARIABLE_gumbel)
{
	tag = passedTag ;
	double pi = 3.14159265358979;
	double gamma = 0.5772156649;
	u = passedMean - (gamma*sqrt(6.0)*passedStdv)/pi;
	alpha = pi / ( sqrt(6.0) * passedStdv );
	startValue = getMean();
}
XC::GumbelRV::GumbelRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4)
:RandomVariable(passedTag, RANDOM_VARIABLE_gumbel)
{
	tag = passedTag ;
	u = passedParameter1;
	alpha = passedParameter2;
	startValue = getMean();
}


void
XC::GumbelRV::Print(std::ostream &s, int flag)
{
}


double
XC::GumbelRV::getPDFvalue(double rvValue)
{
	return alpha * exp(-alpha*(rvValue-u)-exp(-alpha*(rvValue-u)));
}


double
XC::GumbelRV::getCDFvalue(double rvValue)
{
	return exp(-exp(-alpha*(rvValue-u)));
}


double
XC::GumbelRV::getInverseCDFvalue(double probValue)
{
	return (alpha*u - log(-log(probValue))) / alpha;
}


const char *
XC::GumbelRV::getType()
{
	return "GUMBEL";
}


double 
XC::GumbelRV::getMean()
{
	double gamma = 0.5772156649;
	return u+gamma/alpha;
}



double 
XC::GumbelRV::getStdv()
{
	double pi = 3.14159265358979;
	return pi/(sqrt(6.0)*alpha);
}


double 
XC::GumbelRV::getStartValue()
{
	return startValue;
}

double XC::GumbelRV::getParameter1()  {return u;}
double XC::GumbelRV::getParameter2()  {return alpha;}
double XC::GumbelRV::getParameter3()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}
double XC::GumbelRV::getParameter4()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}
