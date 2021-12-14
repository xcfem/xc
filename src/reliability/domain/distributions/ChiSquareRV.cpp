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
// $Date: 2003/03/04 00:44:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/distributions/ChiSquareRV.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu) 
//

#include <reliability/domain/distributions/ChiSquareRV.h>
#include "GammaRV.h"
#include <cmath>
#include <cstring>
#include <classTags.h>
#include <utility/matrix/Vector.h>

XC::ChiSquareRV::ChiSquareRV(int passedTag, 
		 double passedMean,
		 double passedStdv,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_chisquare)
{
	tag = passedTag ;
	nu = 0.5*passedMean;
	startValue = passedStartValue;
}
XC::ChiSquareRV::ChiSquareRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_chisquare)
{
	tag = passedTag ;
	nu = passedParameter1;
	startValue = passedStartValue;
}
XC::ChiSquareRV::ChiSquareRV(int passedTag, 
		 double passedMean,
		 double passedStdv)
:RandomVariable(passedTag, RANDOM_VARIABLE_chisquare)
{
	tag = passedTag ;
	nu = 0.5*passedMean;
	startValue = getMean();
}
XC::ChiSquareRV::ChiSquareRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4)
:RandomVariable(passedTag, RANDOM_VARIABLE_chisquare)
{
	tag = passedTag ;
	nu = passedParameter1;
	startValue = getMean();
}


void
XC::ChiSquareRV::Print(std::ostream &s, int flag) const
{
}


double
XC::ChiSquareRV::getPDFvalue(double rvValue)
{
	double result;
	if ( 0.0 < rvValue ) {
		GammaRV *aGammaRV = new GammaRV(1, 0.0, 1.0, 0.0);
		double a = aGammaRV->gammaFunction(0.5*nu);
		result = pow(0.5*rvValue,0.5*nu-1.0)*exp(-0.5*rvValue)/(2.0*a);
		delete aGammaRV;
	}
	else {
		result = 0.0;
	}
	return result;
}

double XC::ChiSquareRV::getCDFvalue(double rvValue)
  {
    double result;
    if ( 0.0 < rvValue )
      { result = incompleteGammaFunction(0.5*nu,0.5*rvValue); }
    else
      { result = 0.0;}
    return result;
  }


double XC::ChiSquareRV::getInverseCDFvalue(double probValue)
  {
    // Here we want to solve the nonlinear equation:
    //         probValue = getCDFvalue(x)
    // with respect to x. 
    // A Newton scheme to find roots - f(x)=0 - looks something like:
    //         x(i+1) = x(i) - f(xi)/f'(xi)
    // In our case the function f(x) is: f(x) = probValue - getCDFvalue(x)

    double x_old = nu;

    double tol = 1.0e-7;
    double x_new = x_old;
    double dx = x_old;
    int nmax = 50;

    for (int i = 1; i <= nmax; i++) {

	    dx = (getCDFvalue(x_new) - probValue) / getPDFvalue(x_new);

	    // Take a Newton step
	    x_new = x_old - dx;

	    if (fabs(getCDFvalue(x_new) - probValue) < tol) {
		    return x_new;
	    }
	    else {
		    if (i == nmax) {
			    std::cerr << "WARNING: ChiSquaredRV did not converge to find inverse CDF!" << std::endl;
			    return 0.0;
		    }
		    else {
			    x_old = x_new;
		    }
	    }
    }

    return x_new;
  }


const std::string XC::ChiSquareRV::getType(void)
{
	return "CHISQUARE";
}


double XC::ChiSquareRV::getMean()
  { return nu; }



double 
XC::ChiSquareRV::getStdv()
{
	return sqrt(2*nu);
}


double 
XC::ChiSquareRV::getStartValue()
{
	return startValue;
}

double XC::ChiSquareRV::getParameter1()  {return nu;}
double XC::ChiSquareRV::getParameter2()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}
double XC::ChiSquareRV::getParameter3()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}
double XC::ChiSquareRV::getParameter4()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}

//! @brief Get parameters.
const XC::Vector &XC::ChiSquareRV::getParameters(void)
  {
    static Vector temp(1);
    temp(0) = nu;
    return temp;
  }

//! @brief Set parameters.
int XC::ChiSquareRV::setParameters(double mean, double stdv)
  {
    nu = mean;
    return 0;
  }
