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
// $Date: 2003/10/27 23:45:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/analysis/SORMAnalysis.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/analysis/analysis/SORMAnalysis.h>
#include <reliability/domain/components/ReliabilityDomain.h>
#include <reliability/analysis/analysis/ReliabilityAnalysis.h>
#include <reliability/analysis/curvature/FindCurvatures.h>
#include <reliability/domain/components/LimitStateFunction.h>
#include <reliability/domain/distributions/NormalRV.h>
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


XC::SORMAnalysis::SORMAnalysis(	ReliabilityDomain *passedReliabilityDomain,
							FindCurvatures *passedCurvaturesAlgorithm,
						    const std::string &passedFileName)
:ReliabilityAnalysis()
{
	theReliabilityDomain = passedReliabilityDomain;
	theCurvaturesAlgorithm = passedCurvaturesAlgorithm;
	fileName= passedFileName;
}

int XC::SORMAnalysis::analyze(void)
{
	// Alert the user that the SORM analysis has started
	std::cerr << "SORM XC::Analysis is running ... " << std::endl;


	// Declare variables used in this method
	Vector curvatures;
	int numberOfCurvatures;
	double beta;
	double pf1;
	double psi_beta;
	double product;
	int i;
	double pf2Breitung;
	double betaBreitung;
	LimitStateFunction *theLimitStateFunction;
	NormalRV *aStdNormRV = 0;
	aStdNormRV = new NormalRV(1,0.0,1.0,0.0);


	// Check if computer ran out of memory
	if (aStdNormRV==0) {
		std::cerr << "XC::SORMAnalysis::analyze() - out of memory while instantiating internal objects." << std::endl;
		return -1;
	}


	// Number of limit-state functions
	int numLsf = theReliabilityDomain->getNumberOfLimitStateFunctions();


	// Open output file
	std::ofstream outputFile(fileName.c_str(), ios::out );


	// Loop over number of limit-state functions
	for (int lsf=1; lsf<=numLsf; lsf++ ) {


		// Inform the user which limit-state function is being evaluated
		std::cerr << "Limit-state function number: " << lsf << std::endl;


		// Set tag of "active" limit-state function
		theReliabilityDomain->setTagOfActiveLimitStateFunction(lsf);


		// Get the limit-state function pointer
		theLimitStateFunction = 0;
		lsf = theReliabilityDomain->getTagOfActiveLimitStateFunction();
		theLimitStateFunction = theReliabilityDomain->getLimitStateFunctionPtr(lsf);
		if (theLimitStateFunction == 0) {
			std::cerr << "XC::SORMAnalysis::analyze() - could not find" << std::endl
				<< " limit-state function with tag #" << lsf << "." << std::endl;
			return -1;
		}


		// Compute curvature(s)
		if (theCurvaturesAlgorithm->computeCurvatures(theReliabilityDomain) < 0){
			std::cerr << "XC::SORMAnalysis::analyze() - failed while finding " << std::endl
				<< " curvatures for limit-state function number " << lsf << "." << std::endl;
			return -1;
		}


		// Get results
		curvatures = theCurvaturesAlgorithm->getCurvatures();
		numberOfCurvatures = curvatures.Size();
			

		// Get FORM results from the limit-state function
		beta = theLimitStateFunction->FORMReliabilityIndexBeta;
		pf1 = theLimitStateFunction->FORMProbabilityOfFailure_pf1;


		// Compute failure probability by "Breitung"
		double denominator = aStdNormRV->getCDFvalue(-beta);
		if (denominator == 0.0) {
			std::cerr << "XC::SORMAnalysis::analyze() - denominator zero " << std::endl
				<< " due to too large reliability index value." << std::endl;
			return -1;
		}
		psi_beta = aStdNormRV->getPDFvalue(beta)/denominator;
		product = 1.0;
		for (i=0; i<numberOfCurvatures; i++ ) {
			product = product / sqrt(1.0+psi_beta*curvatures(i));
		}
		pf2Breitung = pf1 * product;


		// Compute corresponding beta's
		betaBreitung = -aStdNormRV->getInverseCDFvalue(pf2Breitung);


		// Put results into reliability domain
		theLimitStateFunction->numberOfCurvatauresUsed = numberOfCurvatures;
		theLimitStateFunction->SORMUsingSearchPf2Breitung = pf2Breitung;
		theLimitStateFunction->SORMUsingSearchBetaBreitung = betaBreitung;


		// Print SORM results to the output file
		outputFile << "#######################################################################" << std::endl;
		outputFile << "#  SORM ANALYSIS RESULTS, LIMIT-STATE FUNCTION NUMBER "
			<<setiosflags(ios::left)<<setprecision(1)<<setw(4)<<lsf <<"            #" << std::endl;
		outputFile << "#  (Curvatures found from search algorithm.)                          #" << std::endl;
		outputFile << "#                                                                     #" << std::endl;
		outputFile << "#  Number of principal curvatures used: ............... " 
			<<setiosflags(ios::left)<<setprecision(5)<<setw(12)<<numberOfCurvatures
			<< "  #" << std::endl;
		outputFile << "#  Reliability index beta (impr. Breitung's formula):.. " 
			<<setiosflags(ios::left)<<setprecision(5)<<setw(12)<<betaBreitung 
			<< "  #" << std::endl;
		outputFile << "#  Corresponding estimated probability of failure pf2:.." 
			<<setiosflags(ios::left)<<setprecision(5)<<setw(12)<<pf2Breitung 
			<< "  #" << std::endl;
		outputFile << "#                                                                     #" << std::endl;
		outputFile << "#######################################################################" << std::endl << std::endl << std::endl;
	}


	// Inform user on screen
	std::cerr << "SORM analysis completed. " << std::endl;

	// Clean up
	outputFile.close();
	delete aStdNormRV;

	return 0;
}

