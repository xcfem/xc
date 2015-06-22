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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
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
	aStdNormRV = new XC::NormalRV(1,0.0,1.0,0.0);


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

