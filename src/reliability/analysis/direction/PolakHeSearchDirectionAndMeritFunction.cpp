//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/10/27 23:45:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/direction/PolakHeSearchDirectionAndMeritFunction.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu) 
//

#include <reliability/analysis/direction/PolakHeSearchDirectionAndMeritFunction.h>
#include <reliability/analysis/direction/SearchDirection.h>
#include <reliability/analysis/meritFunction/MeritFunctionCheck.h>
#include <utility/matrix/Vector.h>


XC::PolakHeSearchDirectionAndMeritFunction::PolakHeSearchDirectionAndMeritFunction(double pgamma, double pdelta)
:SearchDirection(), MeritFunctionCheck()
{
	gamma = pgamma;
	delta = pdelta;
	alpha = 0.0;
}

XC::Vector XC::PolakHeSearchDirectionAndMeritFunction::getSearchDirection()
{
	return searchDirection;
}


int
XC::PolakHeSearchDirectionAndMeritFunction::setAlpha(double palpha)
{
	alpha = palpha;
	
	return 0;
}



int
XC::PolakHeSearchDirectionAndMeritFunction::computeSearchDirection(
							int stepNumber, 
							Vector u, 
							double gFunctionValue, 
							Vector gradientInStandardNormalSpace)
{
	// Advise the user if the start value of the limit-state function 
	// is out of the 'ideal range' for the Polak-He algorithm
	if (stepNumber == 1) {
		if (gFunctionValue > 15.0 || gFunctionValue < 2.0) {
			std::cerr << "WARNING: The start value of the limit-state function is outside " << std::endl
				<< " the ideal range for fastest convergence of the Polak-He algorithm. " << std::endl;
		}
	}

	// Compute elements of the A matrix
	double oneOverDelta = 1.0/delta;
	double a11 = oneOverDelta * (u ^ u);
	double a22 = oneOverDelta * (gradientInStandardNormalSpace ^ gradientInStandardNormalSpace);
	double a12 = oneOverDelta * (u ^ gradientInStandardNormalSpace); 


	// Compute the elements of the b vector:
	double b1;
	if (gFunctionValue <= 0.0) {
		b1 = 0.0;
	}
	else {
		b1 = gFunctionValue;
	}
	double b2 = b1 - gFunctionValue;
	b1 = gamma*b1;


	// Compute factors of second order equation
	double a = 0.5*a11 + 0.5*a22 - a12;
	double b = b1 - b2 + a12 - a22;
	double c = 0.5*a22 + b2;


	// Find coordinates of minimum point [z1 z2]
	double z1, z2;
	if (a<0.0) {
		std::cerr << "ERROR: XC::PolakHeSearchDirectionAndMeritFunction::computeSearchDirection() " << std::endl
			<< " the quadratic term is negative! " << std::endl;
		return -1;
	}
	else if (a < 1.0e-9) {

		// Here the quadratic term is zero, so we use one of the endpoints
		double Fend1 = c;     // x=0.0
		double Fend2 = a+b+c; // x=1.0

		if (Fend1 < Fend2) {
			z1 = 0.0;
			z2 = 1.0;
			thetaFunction = -Fend1;
		}
		else {
			z1 = 1.0;
			z2 = 0.0;
			thetaFunction = -Fend2;
		}
	}
	else {

		// The 'normal' case
		double x = -b/(2.0*a);
		z1 = x;
		z2 = 1.0 - x;
		thetaFunction = -(a*x*x + b*x + c);



		// Check if the minimum point is outside the 'triangle'
		if (z1 < 0.0 || z1 > 1.0 || z2 < 0.0 || z2 > 1.0 ) {

			// Use one of the endpoints
			double Fend1 = c;     // x=0.0
			double Fend2 = a+b+c; // x=1.0

			if (Fend1 < Fend2) {
				z1 = 0.0;
				z2 = 1.0;
				thetaFunction = -Fend1;
			}
			else {
				z1 = 1.0;
				z2 = 0.0;
				thetaFunction = -Fend2;
			}
		}
	}


	// With the coordinates of the minimum point the direction now reads
	searchDirection = -z1*u - z2*gradientInStandardNormalSpace;
	
	return 0;
}




double 
XC::PolakHeSearchDirectionAndMeritFunction::getMeritFunctionValue(Vector u, double g, Vector grad_G)
{
	std::cerr << "WARNING: XC::PolakHeSearchDirectionAndMeritFunction::getMeritFunctionValue() --" << std::endl
		<< " no explicit merit function value is computed." << std::endl;
	return 0.0;
}



int 
XC::PolakHeSearchDirectionAndMeritFunction::updateMeritParameters(Vector u, double g, Vector grad_G)
{
	std::cerr << "WARNING: XC::PolakHeSearchDirectionAndMeritFunction::updateMeritParameters() --" << std::endl
		<< " no explicit merit function value is computed." << std::endl;

	return 0;
}




int
XC::PolakHeSearchDirectionAndMeritFunction::check(Vector u_old, 
											  double g_old, 
											  Vector grad_G_old, 
											  double stepSize,
											  Vector stepDirection,
											  double g_new)
{
	// New point in standard normal space
	Vector u_new = u_old + stepSize*stepDirection;

	// Check that the factor alpha is set (since this is a dual purpose class...)
	if (alpha == 0.0) {
		std::cerr << "ERROR: XC::PolakHeSearchDirectionAndMeritFunction::check()" << std::endl
			<< " the alpha factor is not set! " << std::endl;
	}


	// Determine the 'g plus' value
	double g_old_plus;
	if (g_old <= 0.0 ) {
		g_old_plus = 0.0;
	}
	else {
		g_old_plus = g_old;
	}
	

	// Evaluate the 'merit' function
	double term1 = 0.5*(u_new^u_new) - 0.5*(u_old^u_old) - gamma * g_old_plus;
	double term2 = g_new - g_old_plus;
	double F;
	if (term1 > term2) {
		F = term1;
	}
	else {
		F = term2;
	}


	// Do the check
	if (  F  <=  (alpha*stepSize*thetaFunction)  ) {
		return 0;  // ok
	}
	else {
		return -1; // not ok
	}
	
}
