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
// $Date: 2003/03/04 00:44:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/distributions/BetaRV.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu) 
//

#include <reliability/domain/distributions/BetaRV.h>
#include "GammaRV.h"
#include <cmath>
#include <cstring>
#include <classTags.h>


XC::BetaRV::BetaRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_beta)
{
	tag = passedTag ;
	a = passedParameter1;
	b = passedParameter2;
	q = passedParameter3;
	r = passedParameter4;
	startValue = passedStartValue;
}
XC::BetaRV::BetaRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4)
:RandomVariable(passedTag, RANDOM_VARIABLE_beta)
{
	tag = passedTag ;
	a = passedParameter1;
	b = passedParameter2;
	q = passedParameter3;
	r = passedParameter4;
	startValue = getMean();
}


void XC::BetaRV::Print(std::ostream &s, int flag) const
  {}


double XC::BetaRV::getPDFvalue(double rvValue)
  {
	double result;
	if ( ((q<1.0 && rvValue > a) || (q >= 1.0 && rvValue >= a)) && ((r < 1.0 && rvValue < b) || (r >= 1.0 && rvValue <= b)) )
	  {
		double par1 = pow(rvValue-a,q-1.0);
		double par2 = pow(b-rvValue,r-1.0);
		double par3 = betaFunction(q,r);
		double par4 = pow(b-a,q+r-1.0);
		result = par1*par2/(par3*par4);
	}
	else {
		result = 0.0;
	}
	return result;
}


double XC::BetaRV::getCDFvalue(double rvValue)
  {
    if(rvValue <= a)
      { return 0.0; }
    if(rvValue >= b)
      { return 1.0; }

    const double z = (rvValue - a) / (b - a);
    const double Ix = incompleteBetaFunction(q, r, z);
    //std::cerr << "x = " << rvValue << " z = " << z << " Ix = " << Ix;
    return Ix;
  }

double XC::BetaRV::getInverseCDFvalue(double probValue)
  {
    double result = 0.0;
    // Here we want to solve the nonlinear equation:
    //         probValue = getCDFvalue(x)
    // with respect to x. 
    // A Newton scheme to find roots - f(x)=0 - looks something like:
    //         x(i+1) = x(i) - f(xi)/f'(xi)
    // In our case the function f(x) is: f(x) = probValue - getCDFvalue(x)
    // The derivative of the function can be found approximately by a
    // finite difference scheme where e.g. stdv/200 is used as perturbation.

    // Newton method does not converge in general starting from mean particularly when q and/or r are < 1.0
    double tol = 1e-6;
    double x_old = getMean();   // Start at the mean of the random variable
    double x_new;
    double f;
    double df;
    double h;
    double perturbed_f;
    for (int i=1;  i<=100;  i++ )  {

	    // Evaluate function
	    f = probValue - getCDFvalue(x_old);
	    // Evaluate perturbed function
	    h = getStdv()/200.0;
	    perturbed_f = probValue - getCDFvalue(x_old+h);

	    // Evaluate derivative of function
	    df = ( perturbed_f - f ) / h;

	    if ( fabs(df) < 1.0e-15) {
		    /*std::cerr << "WARNING: BetaRV::getInverseCDFvalue() -- zero derivative " << " in Newton algorithm. " << std::endl;
		    std::cerr << "Switch bisection method" << std::endl;*/
		    // In case of zero derivative in Newton MEthod (This often happen because we went outside the boundaries)
		    // We use slower but robust bisection method
		    double x_A, x_B;
		    double f_A, f_B, f_M;
		    x_A = a;
		    x_B = b;
		    f_A = probValue - getCDFvalue(x_A);
		    f_B = probValue - getCDFvalue(x_B);
		    for (int j = 1; j <= 200; j++) {
			    x_new = (x_A + x_B) / 2.0;
			    f_M = probValue - getCDFvalue(x_new);
			    if (fabs(f_M) < tol) {
				    return x_new;
			    }
			    else {
				    if (i == 200) {
					    std::cerr << "WARNING: Did not converge to find inverse CDF!" << std::endl;
					    return 0.0;
				    }
				    if (f_M * f_A < 0) {
					    // The zero is between A and M
					    x_B = x_new;
					    f_B = f_M;
				    }
				    else {
					    // The zero is between M and B
					    x_A = x_new;
					    f_A = f_M;
				    }
			    }
		    }

	    }
	    else {

		    // Take a Newton step
		    x_new = x_old - f/df;

		    // Check convergence; quit or continue
		    // Changed the tolerance check because if x_new is zero it doesn't work
		    if (fabs(probValue - getCDFvalue(x_new)) < tol) {
			    return x_new;
		    }
		    else {
			    if (i==100)
			      {
				std::cerr << "WARNING: Did not converge to find inverse CDF!" << std::endl;
				    return 0.0;
			    }
			    else {
				    x_old = x_new;
			    }

		    }
	    }
    }

    return result;
  }



const std::string XC::BetaRV::getType(void)
{
	return "BETA";
}


double 
XC::BetaRV::getMean()
{
	return (a*r+b*q)/(q+r);
}



double 
XC::BetaRV::getStdv()
{
	return ((b-a)/(q+r)) * sqrt(q*r/(q+r+1));
}


double 
XC::BetaRV::getStartValue()
{
	return startValue;
}

double XC::BetaRV::getParameter1()  {return a;}
double XC::BetaRV::getParameter2()  {return b;}
double XC::BetaRV::getParameter3()  {return q;}
double XC::BetaRV::getParameter4()  {return r;}



double 
XC::BetaRV::betaFunction(double q, double r)
{
/*	OLD CODE: 
	GammaRV *aGammaRV = new GammaRV(1, 0.0, 1.0, 0.0);
	double par1,par2,par3;
	par1 = aGammaRV->gammaFunction(q);
	par2 = aGammaRV->gammaFunction(q);
	par3 = aGammaRV->gammaFunction(q+r);
	delete aGammaRV;
	return par1*par2/par3;
*/

	// Matlab definition of the beta function:
	//    y = exp(gammaln(q)+gammaln(r)-gammaln(q+r));
	//    ... where gammaln(.) = ln(gamma(.))
	// So, try this instead:
	GammaRV *aGammaRV = new GammaRV(1, 0.0, 1.0, 0.0);
	double gammaq,gammar,gammaqpr;
	gammaq = aGammaRV->gammaFunction(q);
	gammar = aGammaRV->gammaFunction(r);
	gammaqpr = aGammaRV->gammaFunction(q+r);
	delete aGammaRV;
	double loggammaq,loggammar,loggammaqpr;
	loggammaq = log(gammaq);
	loggammar = log(gammar);
	loggammaqpr = log(gammaqpr);
	return exp(loggammaq+loggammar-loggammaqpr);
}
