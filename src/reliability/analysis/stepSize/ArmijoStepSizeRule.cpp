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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/10/27 23:45:44 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/stepSize/ArmijoStepSizeRule.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/analysis/stepSize/ArmijoStepSizeRule.h>
#include <reliability/analysis/gFunction/GFunEvaluator.h>
#include <reliability/analysis/stepSize/StepSizeRule.h>
#include <reliability/analysis/transformation/ProbabilityTransformation.h>
#include <reliability/analysis/meritFunction/MeritFunctionCheck.h>
#include <reliability/analysis/rootFinding/RootFinding.h>
#include <cmath>
#include <utility/matrix/Vector.h>

#include <fstream>
#include <iomanip>
#include <iostream>
using std::ifstream;
using std::ios;
using std::setw;
using std::setprecision;
using std::setiosflags;

XC::ArmijoStepSizeRule::ArmijoStepSizeRule(	GFunEvaluator *passedGFunEvaluator,
						ProbabilityTransformation *passedProbabilityTransformation,
						MeritFunctionCheck *passedMeritFunctionCheck,
						RootFinding *passedRootFindingAlgorithm, 
						double Pbase,
						int    PmaxNumReductions,
						double Pb0,
						int    PnumberOfShortSteps,
						double Pradius,
						double PsurfaceDistance,
						double Pevolution,
						int pprintFlag)
:StepSizeRule()
{
	theGFunEvaluator = passedGFunEvaluator;
	theProbabilityTransformation = passedProbabilityTransformation;
	theMeritFunctionCheck = passedMeritFunctionCheck;
	theRootFindingAlgorithm = passedRootFindingAlgorithm;
	gFunValue = 0;
	base = Pbase; 
	maxNumReductions = PmaxNumReductions; 
	b0 = Pb0; 
	numberOfShortSteps = PnumberOfShortSteps; 
	radius = Pradius; 
	surfaceDistance = PsurfaceDistance; 
	evolution = Pevolution; 
	isCloseToSphere = false;
	printFlag = pprintFlag;
}

double XC::ArmijoStepSizeRule::getStepSize()
{
	return stepSize;
}



double 
XC::ArmijoStepSizeRule::getInitialStepSize()
{
	return b0;
}



double 
XC::ArmijoStepSizeRule::getGFunValue()
{
	return gFunValue;
}


int
XC::ArmijoStepSizeRule::computeStepSize(Vector u_old, 
									Vector grad_G_old, 
									double g_old, 
									Vector dir_old,
									int stepNumber)
{

	// Initial declarations
	bool isOutsideSphere;
	bool isSecondTime;
	bool FEconvergence;
	double result;
	Vector x_new;
	Matrix jacobian_x_u;


	// Inform user through log file
	static ofstream logfile( "ArmijoRuleLog.txt", ios::out );
	logfile << "Entering the Armijo rule to compute step size ..." << std::endl;
	logfile.flush();
	
	
	// Check that the norm of the gradient is not zero
	double gradNorm = grad_G_old.Norm();
	if (gradNorm == 0.0) {
		std::cerr << "XC::ArmijoStepSizeRule::computeStepSize() - the norm " << std::endl
			<< " of the gradient is zero. " << std::endl;
		return -1;
	}


	// Check if this potentially could be the second time close to the surface
	if (isCloseToSphere) {
		isSecondTime = true;
	}
	else {
		isSecondTime = false;
	}


	// Set the first trial step size
	double lambda_new;
	if (stepNumber <= numberOfShortSteps) {
		lambda_new = b0;
	}
	else {
		lambda_new = 1.0;
	}
	

	// Take a trial step in standard normal space
	Vector u_new = u_old + dir_old * lambda_new;


	// Inform the user
	if (printFlag != 0) {
		std::cerr << "Armijo starting gFun evaluation at distance " << u_new.Norm() << "..." << std::endl
			<< " .......: ";
	}
	logfile << "Armijo starting gFun evaluation at distance " << u_new.Norm() << "..." << std::endl;
	logfile.flush();


	double g_new;
	Vector grad_G_new;
	if (u_new.Norm()>radius) {

		isOutsideSphere = true;

		// Set some dummy values to 'g' and 'gradG'; it doesn't matter
		// that the merit functions then will be 'wrong'; the step will
		// fail in any case because it is outside the sphere. 
		g_new = g_old;
		grad_G_new = grad_G_old;


		// Inform the user
		if (printFlag != 0) {
			std::cerr << "Armijo skipping gFun evaluation because of hyper sphere requirement..." << std::endl
				<< " .......: ";
		}
		logfile << "Armijo skipping gFun evaluation because of hyper sphere requirement..." << std::endl;
		logfile.flush();


	}
	else {

		isOutsideSphere = false;

		// Register where this u was close to the sphere surface
		if (u_new.Norm() > radius-surfaceDistance  && 
			u_new.Norm() < radius+surfaceDistance) {
			isCloseToSphere = true;
			if (isSecondTime) {
				radius = radius + evolution;
			}
		}
		else {
			isCloseToSphere = false;
		}


		// Transform the trial point into original space
		result = theProbabilityTransformation->set_u(u_new);
		if (result < 0) {
			std::cerr << "XC::ArmijoStepSizeRule::computeStepSize() - could not set " << std::endl
				<< " vector u in the xu-transformation. " << std::endl;
			return -1;
		}
		result = theProbabilityTransformation->transform_u_to_x_andComputeJacobian();
		if (result < 0) {
			std::cerr << "XC::ArmijoStepSizeRule::computeStepSize() - could not  " << std::endl
				<< " transform u to x. " << std::endl;
			return -1;
		}
		x_new = theProbabilityTransformation->get_x();
		jacobian_x_u = theProbabilityTransformation->getJacobian_x_u();


		// Evaluate the limit-state function
		FEconvergence = true;
		result = theGFunEvaluator->runGFunAnalysis(x_new);
/*		if (result < 0) {
			// In this case the FE analysis did not converge
			// In this case; do not accept the new point!
			FEconvergence = false;
			std::cerr << "step size rejected due to non-converged FE analysis ..." << std::endl
				<< " .......: ";
		}
*/		result = theGFunEvaluator->evaluateG(x_new);
		if (result < 0) {
			std::cerr << "XC::ArmijoStepSizeRule::computeStepSize() - could not  " << std::endl
				<< " tokenize the limit-state function. " << std::endl;
			return -1;
		}
		g_new = theGFunEvaluator->getG();

	}


	///////////////////////////////////////////
	// POSSIBLY START REDUCING THE STEP SIZE //
	///////////////////////////////////////////
	int i = 1;
	bool mustGoOn = false;

	if (theMeritFunctionCheck->check(u_old, g_old, grad_G_old, lambda_new, dir_old, g_new)<0) {
		mustGoOn = true;
	}
	if (!FEconvergence) {
		mustGoOn = true;
	}
	if (isOutsideSphere) {
		mustGoOn = true;
	}
	if (i>maxNumReductions) {
		mustGoOn = false;
	}


	while ( mustGoOn ) {

		
		// Notify user that step sizes are being reduced
		std::cerr << "Armijo trial point rejected; reducing step size..." << std::endl
			<< " .......: ";
		logfile << "Armijo trial point rejected; reducing step size..." << std::endl;
		logfile.flush();
		

		// Cut the step size in half (or whichever base the user has set) and try that instead
		if (stepNumber <= numberOfShortSteps) {
			lambda_new = b0 * pow(base,i);
		}
		else {
			lambda_new = pow(base,i);
		}
		u_new = u_old + dir_old * lambda_new;


		// Check if we are beyond the bounding sphere
		if (u_new.Norm()>radius) {

			isOutsideSphere = true;

			// Inform the user
			if (printFlag != 0) {
				std::cerr << "Armijo skipping gFun evaluation because of hyper sphere requirement..." << std::endl
					<< " .......: ";
			}
			logfile << "Armijo skipping gFun evaluation because of hyper sphere requirement..." << std::endl;
			logfile.flush();
		}
		else {

			isOutsideSphere = false;

			// Register where this u was close to the sphere surface
			if (u_new.Norm() > radius-surfaceDistance  && 
				u_new.Norm() < radius+surfaceDistance) {
				isCloseToSphere = true;
				if (isSecondTime) {
					radius = radius + evolution;
				}
			}
			else {
				isCloseToSphere = false;
			}
		
			if (printFlag != 0) {
				std::cerr << "Armijo starting gFun evaluation at distance " << u_new.Norm() << "..." << std::endl
					<< " .......: ";
			}
			logfile << "Armijo starting gFun evaluation at distance " << u_new.Norm() << "..." << std::endl;

			// Transform the trial point into original space
			double result = theProbabilityTransformation->set_u(u_new);
			if (result < 0) {
				std::cerr << "XC::ArmijoStepSizeRule::computeStepSize() - could not set " << std::endl
					<< " vector u in the xu-transformation. " << std::endl;
				return -1;
			}
			result = theProbabilityTransformation->transform_u_to_x_andComputeJacobian();
			if (result < 0) {
				std::cerr << "XC::ArmijoStepSizeRule::computeStepSize() - could not  " << std::endl
					<< " transform u to x. " << std::endl;
				return -1;
			}
			x_new = theProbabilityTransformation->get_x();
			jacobian_x_u = theProbabilityTransformation->getJacobian_x_u();


			// Evaluate the limit-state function
			FEconvergence = true;
			result = theGFunEvaluator->runGFunAnalysis(x_new);
/*			if (result < 0) {
				// In this case the FE analysis did not converge
				// May still accept the new point!
				FEconvergence = false;
				std::cerr << "step size rejected due to non-converged FE analysis ..." << std::endl
					<< " .......: ";
				logfile << "step size rejected due to non-converged FE analysis ..." << std::endl;
			}
*/			result = theGFunEvaluator->evaluateG(x_new);
			if (result < 0) {
				std::cerr << "XC::ArmijoStepSizeRule::computeStepSize() - could not  " << std::endl
					<< " tokenize the limit-state function. " << std::endl;
				return -1;
			}
			g_new = theGFunEvaluator->getG();


			// Possibly project the point onto the limit-state surface
			// (it is assumed that the user has provided a projection object 
			// if this is to be done)
			// Note that this has effect on the previously computed search direction
			// because the new values of the performance function and its gradient
			// is being used...
			// Q: Is the u_new point also being kept in the orchestrating algorithm?
			if (theRootFindingAlgorithm != 0) {
				theRootFindingAlgorithm->findLimitStateSurface(2,g_new, grad_G_old, u_new);
			}
		}

		// Increment counter
		i++;

		// Check if we need to go on
		mustGoOn = false;

		if (theMeritFunctionCheck->check(u_old, g_old, grad_G_old, lambda_new, dir_old, g_new)<0) {
			mustGoOn = true;
		}
		if (!FEconvergence) {
			mustGoOn = true;
		}
		if (isOutsideSphere) {
			mustGoOn = true;
		}
		if (i>maxNumReductions) {
			mustGoOn = false;
		}

 	
	}

	stepSize = lambda_new;
	gFunValue = g_new;

	return 1;

}
