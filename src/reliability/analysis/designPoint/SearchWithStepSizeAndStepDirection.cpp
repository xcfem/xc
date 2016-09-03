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
                                                                        
// $Revision: 1.7 $
// $Date: 2003/10/27 23:45:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/designPoint/SearchWithStepSizeAndStepDirection.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu) 
//

#include <reliability/analysis/designPoint/SearchWithStepSizeAndStepDirection.h>
#include <reliability/analysis/designPoint/FindDesignPointAlgorithm.h>
#include <reliability/analysis/stepSize/StepSizeRule.h>
#include <reliability/analysis/direction/SearchDirection.h>
#include <reliability/analysis/transformation/ProbabilityTransformation.h>
#include <reliability/analysis/transformation/NatafProbabilityTransformation.h>
#include <reliability/analysis/gFunction/GFunEvaluator.h>
#include <reliability/analysis/sensitivity/GradGEvaluator.h>
#include <reliability/domain/components/RandomVariable.h>
#include <reliability/domain/components/CorrelationCoefficient.h>
#include <reliability/analysis/misc/MatrixOperations.h>
#include <reliability/analysis/hessianApproximation/HessianApproximation.h>
#include <reliability/analysis/convergenceCheck/ReliabilityConvergenceCheck.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <reliability/domain/distributions/GammaRV.h>

#include <fstream>
#include <iomanip>
#include <iostream>

using std::ifstream;
using std::ios;

using std::setw;
using std::setprecision;



XC::SearchWithStepSizeAndStepDirection::SearchWithStepSizeAndStepDirection(
					int passedMaxNumberOfIterations, 
					GFunEvaluator *passedGFunEvaluator,
					GradGEvaluator *passedGradGEvaluator,
					StepSizeRule *passedStepSizeRule,
					SearchDirection *passedSearchDirection,
					ProbabilityTransformation *passedProbabilityTransformation,
					HessianApproximation *passedHessianApproximation,
					ReliabilityConvergenceCheck *passedReliabilityConvergenceCheck,
					int pprintFlag,
					const std::string &pFileNamePrint,
					Vector *pStartPoint)
:FindDesignPointAlgorithm()
{
	maxNumberOfIterations			= passedMaxNumberOfIterations;
	theGFunEvaluator				= passedGFunEvaluator;
	theGradGEvaluator				= passedGradGEvaluator;
	theStepSizeRule					= passedStepSizeRule;
	theSearchDirection				= passedSearchDirection;
	theProbabilityTransformation	= passedProbabilityTransformation;
	theHessianApproximation			= passedHessianApproximation;
	theReliabilityConvergenceCheck  = passedReliabilityConvergenceCheck;
	startPoint						= pStartPoint;
	printFlag						= pprintFlag;
	numberOfEvaluations =0;
	if(printFlag != 0)
          { fileNamePrint= pFileNamePrint; }
	else
          { fileNamePrint= "searchpoints.out"; }
  }



int
XC::SearchWithStepSizeAndStepDirection::findDesignPoint(ReliabilityDomain *passedReliabilityDomain)
{

	// Set the reliability domain (a data member of this class)
	theReliabilityDomain = passedReliabilityDomain;

	// Declaration of data used in the algorithm
	int numberOfRandomVariables = theReliabilityDomain->getNumberOfRandomVariables();
	int j;
	int zeroFlag;
	Vector dummy(numberOfRandomVariables);
	x = dummy;
	u = dummy;
	Vector u_old(numberOfRandomVariables);
	uSecondLast(numberOfRandomVariables);
	Vector uNew(numberOfRandomVariables);
	alpha (numberOfRandomVariables);
	gamma (numberOfRandomVariables);
	alphaSecondLast(numberOfRandomVariables);
	double gFunctionValue = 1.0;
	double gFunctionValue_old = 1.0;
	Vector gradientOfgFunction(numberOfRandomVariables);
	gradientInStandardNormalSpace(numberOfRandomVariables);
	Vector gradientInStandardNormalSpace_old(numberOfRandomVariables);
	double normOfGradient =0;
	double stepSize= 0;
	Matrix jacobian_x_u(numberOfRandomVariables,numberOfRandomVariables);
	int evaluationInStepSize = 0;
	int result;
	theGFunEvaluator->initializeNumberOfEvaluations();

	
	// Prepare output file to store the search points
	std::ofstream outputFile2(fileNamePrint.c_str(), ios::out );
	if(printFlag == 0)
          {
            outputFile2 << "The user has not specified to store any search points." << std::endl;
	    outputFile2 << "This is just a dummy file. " << std::endl;
	  }


	if (startPoint == 0) {

		// Here we want to start at the origin in the standard normal space. 
		// Hence, just leave 'u' as being initialized to zero. 
		// (Note; this is slightly different from the mean point, as done earlier)
	}
	else {

		// Get starting point
		x = (*startPoint);


		// Transform starting point into standard normal space
		result = theProbabilityTransformation->set_x(x);
		if (result < 0) {
			std::cerr << "XC::SearchWithStepSizeAndStepDirection::doTheActualSearch() - " << std::endl
				<< " could not set x in the xu-transformation." << std::endl;
			return -1;
		}


		result = theProbabilityTransformation->transform_x_to_u();
		if (result < 0) {
			std::cerr << "XC::SearchWithStepSizeAndStepDirection::doTheActualSearch() - " << std::endl
				<< " could not transform from x to u." << std::endl;
			return -1;
		}
		u = theProbabilityTransformation->get_u();
	}

	// Loop to find design point
	i = 1;
	while ( i <= maxNumberOfIterations )
	{

		// Transform from u to x space
		result = theProbabilityTransformation->set_u(u);
		if (result < 0) {
			std::cerr << "XC::SearchWithStepSizeAndStepDirection::doTheActualSearch() - " << std::endl
				<< " could not set u in the xu-transformation." << std::endl;
			return -1;
		}

		result = theProbabilityTransformation->transform_u_to_x_andComputeJacobian();
		if (result < 0) {
			std::cerr << "XC::SearchWithStepSizeAndStepDirection::doTheActualSearch() - " << std::endl
				<< " could not transform from u to x and compute Jacobian." << std::endl;
			return -1;
		}
		x = theProbabilityTransformation->get_x();
		jacobian_x_u = theProbabilityTransformation->getJacobian_x_u();


		// Possibly print the point to output file
		int iii;
		if (printFlag != 0) {

			if (printFlag == 1) {
				outputFile2.setf(ios::scientific, ios::floatfield);
				for (iii=0; iii<x.Size(); iii++) {
					outputFile2<<setprecision(5)<<setw(15)<<x(iii)<<std::endl;
				}
			}
			else if (printFlag == 2) {
				outputFile2.setf(ios::scientific, ios::floatfield);
				for (iii=0; iii<u.Size(); iii++) {
					outputFile2<<setprecision(5)<<setw(15)<<u(iii)<<std::endl;
				}
			}
		}


		// Evaluate limit-state function unless it has been done in 
		// a trial step by the "stepSizeAlgorithm"
		if (evaluationInStepSize == 0) {
			result = theGFunEvaluator->runGFunAnalysis(x);
			if (result < 0) {
				std::cerr << "XC::SearchWithStepSizeAndStepDirection::doTheActualSearch() - " << std::endl
					<< " could not run analysis to evaluate limit-state function. " << std::endl;
				return -1;
			}
			result = theGFunEvaluator->evaluateG(x);
			if (result < 0) {
				std::cerr << "XC::SearchWithStepSizeAndStepDirection::doTheActualSearch() - " << std::endl
					<< " could not tokenize limit-state function. " << std::endl;
				return -1;
			}
			gFunctionValue_old = gFunctionValue;
			gFunctionValue = theGFunEvaluator->getG();
		}


		// Set scale parameter
		if (i == 1)	{
			Gfirst = gFunctionValue;
			std::cerr << " Limit-state function value at start point, g=" << gFunctionValue << std::endl;
			std::cerr << " STEP #0: ";
			theReliabilityConvergenceCheck->setScaleValue(gFunctionValue);
		}


		// Gradient in original space
		result = theGradGEvaluator->computeGradG(gFunctionValue,x);
		if (result < 0) {
			std::cerr << "XC::SearchWithStepSizeAndStepDirection::doTheActualSearch() - " << std::endl
				<< " could not compute gradients of the limit-state function. " << std::endl;
			return -1;
		}
		gradientOfgFunction = theGradGEvaluator->getGradG();


		// Check if all components of the vector is zero
		zeroFlag = 0;
		for (j=0; j<gradientOfgFunction.Size(); j++) {
			if (gradientOfgFunction[j] != 0.0) {
				zeroFlag = 1;
			}
		}
		if (zeroFlag == 0) {
			std::cerr << "XC::SearchWithStepSizeAndStepDirection::doTheActualSearch() - " << std::endl
				<< " all components of the gradient vector is zero. " << std::endl;
			return -1;
		}


		// Gradient in standard normal space
		gradientInStandardNormalSpace_old = gradientInStandardNormalSpace;
		gradientInStandardNormalSpace = jacobian_x_u ^ gradientOfgFunction;


		// Compute the norm of the gradient in standard normal space
		normOfGradient = gradientInStandardNormalSpace.Norm();


		// Check that the norm is not zero
		if (normOfGradient == 0.0) {
			std::cerr << "XC::SearchWithStepSizeAndStepDirection::doTheActualSearch() - " << std::endl
				<< " the norm of the gradient is zero. " << std::endl;
			return -1;
		}

		
		// Compute alpha-vector
		alpha = gradientInStandardNormalSpace *  ( (-1.0) / normOfGradient );


		// Check convergence
		result = theReliabilityConvergenceCheck->check(u,gFunctionValue,gradientInStandardNormalSpace);
		if (result > 0)  {
		

			// Inform the user of the happy news!
			std::cerr << "Design point found!" << std::endl;


			// Print the design point to file, if desired
			int iii;
			if (printFlag != 0) {
				if (printFlag == 3) {

					result = theProbabilityTransformation->set_u(u);
					if (result < 0) {
						std::cerr << "XC::SearchWithStepSizeAndStepDirection::doTheActualSearch() - " << std::endl
							<< " could not set u in the xu-transformation." << std::endl;
						return -1;
					}

					result = theProbabilityTransformation->transform_u_to_x();
					if (result < 0) {
						std::cerr << "XC::SearchWithStepSizeAndStepDirection::doTheActualSearch() - " << std::endl
							<< " could not transform from u to x." << std::endl;
						return -1;
					}
					x = theProbabilityTransformation->get_x();

					outputFile2.setf(ios::scientific, ios::floatfield);
					for (iii=0; iii<x.Size(); iii++) {
						outputFile2<<setprecision(5)<<setw(15)<<x(iii)<<std::endl;
					}
				}
				else if (printFlag == 4) {
				  std::ofstream outputFile2(fileNamePrint.c_str(), ios::out );
					outputFile2.setf(ios::scientific, ios::floatfield);
					for (iii=0; iii<u.Size(); iii++) {
						outputFile2<<setprecision(5)<<setw(15)<<u(iii)<<std::endl;
					}
				}
			}


			// Compute the gamma vector
			MatrixOperations theMatrixOperations(jacobian_x_u);

			result = theMatrixOperations.computeTranspose();
			if (result < 0) {
				std::cerr << "XC::SearchWithStepSizeAndStepDirection::doTheActualSearch() - " << std::endl
					<< " could not compute transpose of jacobian matrix. " << std::endl;
				return -1;
			}
			Matrix transposeOfJacobian_x_u = theMatrixOperations.getTranspose();

			Matrix jacobianProduct = jacobian_x_u * transposeOfJacobian_x_u;

			Matrix D_prime(numberOfRandomVariables,numberOfRandomVariables);
			for (j=0; j<numberOfRandomVariables; j++) {
				D_prime(j,j) = sqrt(jacobianProduct(j,j));
			}


			Matrix jacobian_u_x = theProbabilityTransformation->getJacobian_u_x();

			Vector tempProduct = jacobian_u_x ^ alpha;

			gamma = D_prime ^ tempProduct;
			
			Glast = gFunctionValue;

			numberOfEvaluations = theGFunEvaluator->getNumberOfEvaluations();

			return 1;
		}


		// Store 'u' and 'alpha' at the second last iteration point
		uSecondLast = u;
		alphaSecondLast = alpha;


		// Let user know that we have to take a new step
		std::cerr << " STEP #" << i <<": ";


		// Update Hessian approximation, if any
		if (  (theHessianApproximation!=0) && (i!=1)  ) {
			theHessianApproximation->updateHessianApproximation(u_old,
											    gFunctionValue_old,
											    gradientInStandardNormalSpace_old,
											    stepSize,
											    searchDirection,
											    gFunctionValue,
											    gradientInStandardNormalSpace);
		}


		// Determine search direction
		result = theSearchDirection->computeSearchDirection(i,
			u, gFunctionValue, gradientInStandardNormalSpace );
		if (result < 0) {
			std::cerr << "XC::SearchWithStepSizeAndStepDirection::doTheActualSearch() - " << std::endl
				<< " could not compute search direction. " << std::endl;
			return -1;
		}
		searchDirection = theSearchDirection->getSearchDirection();


		// Determine step size
		result = theStepSizeRule->computeStepSize(
			u, gradientInStandardNormalSpace, gFunctionValue, searchDirection, i);
		if (result < 0) {  // (something went wrong)
			std::cerr << "XC::SearchWithStepSizeAndStepDirection::doTheActualSearch() - " << std::endl
				<< " could not compute step size. " << std::endl;
			return -1;
		}
		else if (result == 0) {  // (nothing was evaluated in step size)
			evaluationInStepSize = 0;
		}
		else if (result == 1) {  // (the gfun was evaluated)
			evaluationInStepSize = 1;
			gFunctionValue_old = gFunctionValue;
			gFunctionValue = theStepSizeRule->getGFunValue();
		}
		stepSize = theStepSizeRule->getStepSize();


		// Determine new iteration point (take the step)
		u_old = u;
		u = u_old + (searchDirection * stepSize);


		// Increment the loop parameter
		i++;

	}


	// Print a message if max number of iterations was reached
	// (Note: in this case the last trial point was never transformed/printed)
	std::cerr << "Maximum number of iterations was reached before convergence." << std::endl;

	
	return -1;
}



 XC::Vector
XC::SearchWithStepSizeAndStepDirection::get_x()
{
	return x;
}

 XC::Vector
XC::SearchWithStepSizeAndStepDirection::get_u()
{
	return u;
}

 XC::Vector
XC::SearchWithStepSizeAndStepDirection::get_alpha()
{
	return alpha;
}

 XC::Vector
XC::SearchWithStepSizeAndStepDirection::get_gamma()
{
	return gamma*(1.0/gamma.Norm());
}

int
XC::SearchWithStepSizeAndStepDirection::getNumberOfSteps()
{
	return (i-1);
}

 XC::Vector
XC::SearchWithStepSizeAndStepDirection::getSecondLast_u()
{
	return uSecondLast;
}

 XC::Vector
XC::SearchWithStepSizeAndStepDirection::getSecondLast_alpha()
{
	return alphaSecondLast;
}

 XC::Vector
XC::SearchWithStepSizeAndStepDirection::getLastSearchDirection()
{
	return searchDirection;
}

double
XC::SearchWithStepSizeAndStepDirection::getFirstGFunValue()
{
	return Gfirst;
}

double
XC::SearchWithStepSizeAndStepDirection::getLastGFunValue()
{
	return Glast;
}


 XC::Vector
XC::SearchWithStepSizeAndStepDirection::getGradientInStandardNormalSpace()
{
	return gradientInStandardNormalSpace;
}



int
XC::SearchWithStepSizeAndStepDirection::getNumberOfEvaluations()
{
	return numberOfEvaluations;
}






