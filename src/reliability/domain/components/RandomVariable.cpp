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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/10/27 23:04:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/components/RandomVariable.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu) 
//

#include <reliability/domain/components/RandomVariable.h>
#include <classTags.h>
#include <cmath>

XC::RandomVariable::RandomVariable(int tag, int classTag)
:ReliabilityDomainComponent(tag,classTag)
 {}


//! @brief Incomplete gamma function.
double XC::RandomVariable::gammaFunction(double x)
  { return tgamma(x); }

//! @brief Incomplete gamma function.
double XC::RandomVariable::incompleteGammaFunction(double a, double x)
  {
	double b = x;
	if (x == 0.0) {
		b = 0.0;
	}
	if (a == 0.0) {
		b = 1.0;
	}
	// Series expansion for x < a+1
	if ( (a != 0.0) && (x != 0.0) && (x < a+1.0) ) {
		double ap = a;
		double del = 1.0;
		double sum = del;
		while (fabs(del) >= 1.0e-12*fabs(sum)) {
			ap = ap + 1.0;
			del = x * del / ap;
			sum = sum + del;
		}
		b = sum * exp(-x + a*log(x) - log(gammaFunction(a+1)) );
		if ( x > 0 && b > 1 )
			b = 1.0;
		
	} else {
		// Continued fraction for x >= a+1
		double a0 = 1.0;
		double a1 = x;
		double b0 = 0.0;
		double b1 = a0;
		double fac = 1.0/a1;
		double n = 1;
		double g = b1*fac;
		double gold = b0;
		while (fabs(g-gold) >= 1.0e-12*fabs(g))  {
			gold = g;
			double ana = n - a;
			a0 = (a1 + a0 *ana) * fac;
			b0 = (b1 + b0 *ana) * fac;
			double anf = n*fac;
			a1 = x * a0 + anf * a1;
			b1 = x * b0 + anf * b1;
			fac = 1.0 / a1;
			g = b1 * fac;
			n = n + 1.0;
		}
		b = 1.0 - exp(-x + a*log(x) - log(gammaFunction(a)) ) * g;
	}
	
	return b;
}


//! @brief Beta function.
double XC::RandomVariable::betaFunction(double q, double r)
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
	double gammaq,gammar,gammaqpr;
	gammaq = gammaFunction(q);
	gammar = gammaFunction(r);
	gammaqpr = gammaFunction(q+r);
	
	double loggammaq,loggammar,loggammaqpr;
	loggammaq = log(gammaq);
	loggammar = log(gammar);
	loggammaqpr = log(gammaqpr);
	return exp(loggammaq+loggammar-loggammaqpr);
  }

//! @brief Regularized Incomplete Beta Function.
double XC::RandomVariable::incompleteBetaFunction(double q, double r, double x)   {
	// Regularized Incomplete Beta Function
	// Original implementation 
	// Lewis Van Winkle
	// http://CodePlea.com

	const double stop = 1e-8;
	const double tiny = 1e-30;

	if (x < 0.0 || x > 1.0) {
		return 0.0;
	}
	if (x > ((q + 1) / (q + r + 2.0))) {
		return 1.0 - incompleteBetaFunction(r, q, 1 - x);
	}

	const double lbeta_qr = log(gammaFunction(q)) + log(gammaFunction(r)) - log(gammaFunction(q + r));

	const double front = exp(log(x) * q + log(1.0 - x) * r - lbeta_qr) / q;

	// Lentz's algorithm to evaluate the continued fraction
	double f = 1.0, c = 1.0, d = 0.0;

	int i, m;
	for (i = 0; i <= 200; ++i) {
		m = i / 2;

		double numerator;
		if (i == 0) {
			numerator = 1.0;
		}
		else if (i % 2 == 0) {
			// Even terms
			numerator = (m * (r - m) * x) / ((q + 2.0 * m - 1.0) * (q + 2.0 * m));
		}
		else {
			// Odd terms
			numerator = -((q+m)*(q+r+m)*x) / ((q+2.0*m)*(q+2.0*m+1.0));
		}

		// Perform an iteration of Lentz algorithm
		d = 1.0 + numerator * d;
		if (fabs(d) < tiny) {
			d = tiny;
		}
		d = 1.0 / d;

		c = 1.0 + numerator / c;
		if (fabs(c) < tiny) {
			c = tiny;
		}

		const double cd = c * d;
		f *= cd;

		// check for stop
		if (fabs(1.0 - cd) < stop) {
			return front * (f - 1.0);
		}
	}

	return 0.0;
  }

int XC::RandomVariable::setNewTag(int newTag)
  {
    this->setTag(newTag);
    return 0;
  }
