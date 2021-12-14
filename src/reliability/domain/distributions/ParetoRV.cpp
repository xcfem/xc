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
// $Date: 2003/03/04 00:44:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/distributions/ParetoRV.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu) 
//

#include <reliability/domain/distributions/ParetoRV.h>
#include <math.h>
#include <string.h>
#include <classTags.h>


XC::ParetoRV::ParetoRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_pareto)
{
	tag = passedTag ;
	k = passedParameter1;
	u = passedParameter2;
	startValue = passedStartValue;
}
XC::ParetoRV::ParetoRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4)
:RandomVariable(passedTag, RANDOM_VARIABLE_pareto)
{
	tag = passedTag ;
	k = passedParameter1;
	u = passedParameter2;
	startValue = getMean();
}


void
XC::ParetoRV::Print(std::ostream &s, int flag) const
{
}


double
XC::ParetoRV::getPDFvalue(double rvValue)
{
	double result;
	if ( u <= rvValue ) {
		result = k/u * pow(u/rvValue,k+1);
	}
	else {
		result = 0.0;
	}
	return result;
}


double XC::ParetoRV::getCDFvalue(double rvValue)
{
	double result;
	if ( u <= rvValue ) {
		result = 1-pow(u/rvValue,k);
	}
	else {
		result = 0.0;
	}
	return result;
}


double XC::ParetoRV::getInverseCDFvalue(double probValue)
  {
    if (k <= 0) // shape should be greater than 0
      { return 0.0; }
    else
      { return pow((1 - probValue) / pow(u, k), -1 / k); }
  }


const std::string XC::ParetoRV::getType(void)
{
	return "PARETO";
}


double 
XC::ParetoRV::getMean()
{
	return k*u/(k-1);
}



double 
XC::ParetoRV::getStdv()
{
	return sqrt(k/(k-2))*(u/(k-1));
}


double 
XC::ParetoRV::getStartValue()
{
	return startValue;
}

double XC::ParetoRV::getParameter1()  {return k;}
double XC::ParetoRV::getParameter2()  {return u;}
double XC::ParetoRV::getParameter3()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}
double XC::ParetoRV::getParameter4()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}

