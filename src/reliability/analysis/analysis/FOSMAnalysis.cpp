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
                                                                        
// $Revision: 1.1 $
// $Date: 2003/10/27 23:45:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/analysis/FOSMAnalysis.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/analysis/analysis/FOSMAnalysis.h>
#include <reliability/analysis/analysis/ReliabilityAnalysis.h>
#include <reliability/domain/components/ReliabilityDomain.h>
#include <reliability/domain/components/RandomVariablePositioner.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <tcl.h>

#include <fstream>
#include <iomanip>
#include <iostream>
using std::ifstream;
using std::ios;
using std::setw;
using std::setprecision;
using std::setiosflags;


XC::FOSMAnalysis::FOSMAnalysis(ReliabilityDomain *passedReliabilityDomain,
							   GFunEvaluator *passedGFunEvaluator,
							   GradGEvaluator *passedGradGEvaluator,
							   Tcl_Interp *passedTclInterp,
			       const std::string &passedFileName)
:ReliabilityAnalysis()
{
	theReliabilityDomain	= passedReliabilityDomain;
	theGFunEvaluator		= passedGFunEvaluator;
	theGradGEvaluator = passedGradGEvaluator;
	theTclInterp			= passedTclInterp;
	fileName= passedFileName;
}


int 
XC::FOSMAnalysis::analyze(void)
{

	// Alert the user that the FORM analysis has started
	std::cerr << "FOSM XC::Analysis is running ... " << std::endl;


	// Initial declarations
	int i,j,k;


	// Open output file
	std::ofstream outputFile(fileName.c_str(), ios::out );


	// Get number of random variables and limit-state function
	int nrv = theReliabilityDomain->getNumberOfRandomVariables();
	int numLsf = theReliabilityDomain->getNumberOfLimitStateFunctions();

	
	// Get mean point
	RandomVariable *aRandomVariable;
	Vector meanVector(nrv);
	for (i=1; i<=nrv; i++)
	{
		aRandomVariable = theReliabilityDomain->getRandomVariablePtr(i);
		if (aRandomVariable == 0) {
			std::cerr << "XC::FOSMAnalysis::analyze() -- Could not find" << std::endl
				<< " random variable with tag #" << i << "." << std::endl;
			return -1;
		}
		meanVector(i-1) = aRandomVariable->getMean();
	}


	// Establish vector of standard deviations
	Vector stdvVector(nrv);
	for (i=1; i<=nrv; i++)
	{
		aRandomVariable = theReliabilityDomain->getRandomVariablePtr(i);
		stdvVector(i-1) = aRandomVariable->getStdv();
	}


	// Evaluate limit-state functions
	Vector meanEstimates(numLsf);
	int result;
	int lsf;
	result = theGFunEvaluator->runGFunAnalysis(meanVector);
	if (result < 0) {
		std::cerr << "XC::FOSMAnalysis::analyze() - " << std::endl
			<< " could not run analysis to evaluate limit-state function. " << std::endl;
		return -1;
	}
	for (lsf=1; lsf<=numLsf; lsf++ ) {
		theReliabilityDomain->setTagOfActiveLimitStateFunction(lsf);
		result = theGFunEvaluator->evaluateG(meanVector);
		if (result < 0) {
			std::cerr << "XC::FOSMAnalysis::analyze() - " << std::endl
				<< " could not tokenize limit-state function. " << std::endl;
			return -1;
		}
		meanEstimates(lsf-1) = theGFunEvaluator->getG();
	}


	// Evaluate the gradients of limit-state functions
	Matrix matrixOfGradientVectors(nrv,numLsf);
	result = theGradGEvaluator->computeAllGradG(meanEstimates,meanVector);
	if (result < 0) {
		std::cerr << "XC::FOSMAnalysis::analyze() -- could not" << std::endl
			<< " compute gradients of the limit-state function. " << std::endl;
		return -1;
	}
	matrixOfGradientVectors = theGradGEvaluator->getAllGradG();


	// Establish covariance matrix
	Matrix covMatrix(nrv,nrv);
	for (i=1; i<=nrv; i++) {
		covMatrix(i-1,i-1) = stdvVector(i-1)*stdvVector(i-1);
	}
	int ncorr = theReliabilityDomain->getNumberOfCorrelationCoefficients();
	CorrelationCoefficient *theCorrelationCoefficient;
	double covariance, correlation;
	int rv1, rv2;
	for (i=1 ; i<=ncorr ; i++) {
		theCorrelationCoefficient = theReliabilityDomain->getCorrelationCoefficientPtr(i);
		correlation = theCorrelationCoefficient->getCorrelation();
		rv1 = theCorrelationCoefficient->getRv1();
		rv2 = theCorrelationCoefficient->getRv2();
		covariance = correlation*stdvVector(rv1-1)*stdvVector(rv2-1);
		covMatrix(rv1-1,rv2-1) = covariance;
		covMatrix(rv2-1,rv1-1) = covariance;
	}


	// Post-processing loop over limit-state functions
	Vector responseStdv(numLsf);
	Vector gradient(nrv);
	double responseVariance;
	for (lsf=1; lsf<=numLsf; lsf++ ) {


		// Set tag of active limit-state function
		theReliabilityDomain->setTagOfActiveLimitStateFunction(lsf);


		// Extract relevant gradient
		for (i=0; i<nrv; i++) {
			gradient(i) = matrixOfGradientVectors(i,lsf-1);
		}


		// Estimate of standard deviation of response
		responseVariance = (covMatrix^gradient)^gradient;
		if (responseVariance <= 0.0) {
			std::cerr << "ERROR: Response variance of limit-state function number "<< lsf << std::endl
				<< " is zero! " << std::endl;
		}
		else {
			responseStdv(lsf-1) = sqrt(responseVariance);
		}

		// Compute importance measure (dgdx*stdv)
		Vector importance(nrv);
		for (i=1 ; i<=nrv ; i++) {
			aRandomVariable = theReliabilityDomain->getRandomVariablePtr(i);
			importance(i-1) = gradient(i-1) * aRandomVariable->getStdv();
		}
		double imptNorm = importance.Norm();
		for (i=1 ; i<=nrv ; i++) {
			importance(i-1) = importance(i-1)/imptNorm;
		}

	
		// Print FOSM results to the output file
		outputFile << "#######################################################################" << std::endl;
		outputFile << "#  FOSM ANALYSIS RESULTS, LIMIT-STATE FUNCTION NUMBER   "
			<<setiosflags(ios::left)<<setprecision(1)<<setw(4)<<lsf <<"          #" << std::endl;
		outputFile << "#                                                                     #" << std::endl;
		
		outputFile << "#  Estimated mean: .................................... " 
			<<setiosflags(ios::left)<<setprecision(5)<<setw(12)<<meanEstimates(lsf-1) 
			<< "  #" << std::endl;
		outputFile << "#  Estimated standard deviation: ...................... " 
			<<setiosflags(ios::left)<<setprecision(5)<<setw(12)<<responseStdv(lsf-1) 
			<< "  #" << std::endl;
		outputFile << "#                                                                     #" << std::endl;
		outputFile << "#      R.v.         Importance measure (dgdx*stdv)                    #" << std::endl;
		outputFile.setf( ios::scientific, ios::floatfield );
		for (int i=0;  i<nrv; i++) {
			outputFile << "#       " <<setw(3)<<(i+1)<<"              ";
			outputFile << "";
			if (importance(i)<0.0) { 
				outputFile << "-"; 
			}
			else { 
				outputFile << " "; 
			}
			outputFile <<setprecision(3)<<setw(11)<<fabs(importance(i));
			outputFile << "                                 #" << std::endl;
		}
		outputFile << "#                                                                     #" << std::endl;
		outputFile << "#######################################################################" << std::endl << std::endl << std::endl;
		outputFile.flush();

	}


	// Estimation of response covariance matrix
	Matrix responseCovMatrix(numLsf,numLsf);
	double responseCovariance;
	Vector gradientVector1(nrv), gradientVector2(nrv);
	for (i=1; i<=numLsf; i++) {
		for (k=0; k<nrv; k++) {
			gradientVector1(k) = matrixOfGradientVectors(k,i-1);
		}
		for (j=i+1; j<=numLsf; j++) {
			for (k=0; k<nrv; k++) {
				gradientVector2(k) = matrixOfGradientVectors(k,j-1);
			}
			responseCovariance = (covMatrix^gradientVector1)^gradientVector2;
			responseCovMatrix(i-1,j-1) = responseCovariance;
		}
	}
	for (i=1; i<=numLsf; i++) {
		for (j=1; j<i; j++) {
			responseCovMatrix(i-1,j-1) = responseCovMatrix(j-1,i-1);
		}
	}


	// Corresponding correlation matrix
	Matrix correlationMatrix(numLsf,numLsf);
	for (i=1; i<=numLsf; i++) {
		for (j=i+1; j<=numLsf; j++) {
			correlationMatrix(i-1,j-1) = responseCovMatrix(i-1,j-1)/(responseStdv(i-1)*responseStdv(j-1));
		}
	}
	for (i=1; i<=numLsf; i++) {
		for (j=1; j<i; j++) {
			correlationMatrix(i-1,j-1) = correlationMatrix(j-1,i-1);
		}
	}

	
	// Print correlation results
	outputFile << "#######################################################################" << std::endl;
	outputFile << "#  RESPONSE CORRELATION COEFFICIENTS                                  #" << std::endl;
	outputFile << "#                                                                     #" << std::endl;
	if (numLsf <=1) {
		outputFile << "#  Only one limit-state function!                                     #" << std::endl;
	}
	else {
		outputFile << "#   gFun   gFun     Correlation                                       #" << std::endl;
		outputFile.setf(ios::fixed, ios::floatfield);
		for (i=0; i<numLsf; i++) {
			for (j=i+1; j<numLsf; j++) {
//				outputFile.setf(ios::fixed, ios::floatfield);
				outputFile << "#    " <<setw(3)<<(i+1)<<"    "<<setw(3)<<(j+1)<<"     ";
				if (correlationMatrix(i,j)<0.0) { outputFile << "-"; }
				else { outputFile << " "; }
//				outputFile.setf(ios::scientific, ios::floatfield);
				outputFile <<setprecision(7)<<setw(11)<<fabs(correlationMatrix(i,j));
				outputFile << "                                      #" << std::endl;
			}
		}
	}
	outputFile << "#                                                                     #" << std::endl;
	outputFile << "#######################################################################" << std::endl << std::endl << std::endl;



	// Print summary of results to screen (more here!!!)
	std::cerr << "FOSMAnalysis completed." << std::endl;


	return 0;
}

