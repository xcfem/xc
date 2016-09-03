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
// $Date: 2003/10/27 23:45:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/analysis/FORMAnalysis.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/analysis/analysis/FORMAnalysis.h>
#include <reliability/analysis/designPoint/FindDesignPointAlgorithm.h>
#include <reliability/domain/components/ReliabilityDomain.h>
#include <reliability/analysis/analysis/ReliabilityAnalysis.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <reliability/analysis/misc/MatrixOperations.h>
#include <reliability/domain/distributions/NormalRV.h>
#include <reliability/domain/components/RandomVariable.h>
#include <cmath>
#include <reliability/analysis/transformation/ProbabilityTransformation.h>

#include <fstream>
#include <iomanip>
#include <iostream>
using std::ifstream;
using std::ios;
using std::setw;
using std::setprecision;
using std::setiosflags;


XC::FORMAnalysis::FORMAnalysis(ReliabilityDomain *passedReliabilityDomain,
						   FindDesignPointAlgorithm *passedFindDesignPointAlgorithm,
						   ProbabilityTransformation *passedProbabilityTransformation,
						   const std::string &passedFileName,
						   int p_relSensTag)
:ReliabilityAnalysis()
{
	theReliabilityDomain = passedReliabilityDomain;
	theFindDesignPointAlgorithm = passedFindDesignPointAlgorithm;
	theProbabilityTransformation = passedProbabilityTransformation;
	fileName= passedFileName;
	relSensTag = p_relSensTag;
}


int 
XC::FORMAnalysis::analyze(void)
{

	// Alert the user that the FORM analysis has started
	std::cerr << "FORM XC::Analysis is running ... " << std::endl;


	// Declare variables used in this method
	Vector xStar;
	Vector uStar;
	Vector alpha;
	Vector gamma;
	double stdv,mean;
	int i;
	double Go, Glast;
	Vector uSecondLast;
	Vector alphaSecondLast;
	Vector lastSearchDirection;
	double beta;
	double pf1;
	int numberOfEvaluations;
	int lsf;
	int numRV = theReliabilityDomain->getNumberOfRandomVariables();
	int numLsf = theReliabilityDomain->getNumberOfLimitStateFunctions();
	RandomVariable *aRandomVariable;
	LimitStateFunction *theLimitStateFunction;
	NormalRV *aStdNormRV=0;
	aStdNormRV = new XC::NormalRV(1,0.0,1.0,0.0);
	Vector delta(numRV); 
	Vector eta(numRV);
	Vector kappa(numRV);


	// Check if computer ran out of memory
	if (aStdNormRV==0) {
		std::cerr << "XC::FORMAnalysis::analyze() - out of memory while instantiating internal objects." << std::endl;
		return -1;
	}


	// Open output file
	std::ofstream outputFile( fileName.c_str(), ios::out );


	// Loop over number of limit-state functions and perform FORM analysis
	for (lsf=1; lsf<=numLsf; lsf++ ) {


		// Inform the user which limit-state function is being evaluated
		std::cerr << "Limit-state function number: " << lsf << std::endl;


		// Set tag of "active" limit-state function
		theReliabilityDomain->setTagOfActiveLimitStateFunction(lsf);


		// Get the limit-state function pointer
		theLimitStateFunction = 0;
		lsf = theReliabilityDomain->getTagOfActiveLimitStateFunction();
		theLimitStateFunction = theReliabilityDomain->getLimitStateFunctionPtr(lsf);
		if (theLimitStateFunction == 0) {
			std::cerr << "XC::FORMAnalysis::analyze() - could not find" << std::endl
				<< " limit-state function with tag #" << lsf << "." << std::endl;
			return -1;
		}


		// Find the design point
		if (theFindDesignPointAlgorithm->findDesignPoint(theReliabilityDomain) < 0){
			std::cerr << "XC::FORMAnalysis::analyze() - failed while finding the" << std::endl
				<< " design point for limit-state function number " << lsf << "." << std::endl;
			outputFile << "#######################################################################" << std::endl;
			outputFile << "#  FORM ANALYSIS RESULTS, LIMIT-STATE FUNCTION NUMBER "
				<<setiosflags(ios::left)<<setprecision(1)<<setw(4)<<lsf <<"            #" << std::endl;
			outputFile << "#                                                                     #" << std::endl;
			outputFile << "#  No convergence!                                                    #" << std::endl;
			outputFile << "#                                                                     #" << std::endl;
			outputFile << "#######################################################################" << std::endl << std::endl << std::endl;
		}
		else {
		
			// Get results from the "find desingn point algorithm"
			xStar				= theFindDesignPointAlgorithm->get_x();
			uStar				= theFindDesignPointAlgorithm->get_u();
			alpha				= theFindDesignPointAlgorithm->get_alpha();
			gamma				= theFindDesignPointAlgorithm->get_gamma();
			i					= theFindDesignPointAlgorithm->getNumberOfSteps();
			Go					= theFindDesignPointAlgorithm->getFirstGFunValue();
			Glast				= theFindDesignPointAlgorithm->getLastGFunValue();
			uSecondLast			= theFindDesignPointAlgorithm->getSecondLast_u();
			alphaSecondLast		= theFindDesignPointAlgorithm->getSecondLast_alpha();
			lastSearchDirection	= theFindDesignPointAlgorithm->getLastSearchDirection();
			numberOfEvaluations	= theFindDesignPointAlgorithm->getNumberOfEvaluations();


			// Postprocessing
			beta = alpha ^ uStar;
			pf1 = 1.0 - aStdNormRV->getCDFvalue(beta);


			// Reliability sensitivity analysis wrt. mean/stdv
			if (relSensTag == 1) {
				Vector DuStarDmean;
				Vector DuStarDstdv; 
				double dBetaDmean;
				double dBetaDstdv;

				for ( int j=1; j<=numRV; j++ )
				{
					DuStarDmean = theProbabilityTransformation->meanSensitivityOf_x_to_u(xStar,j);
					DuStarDstdv = theProbabilityTransformation->stdvSensitivityOf_x_to_u(xStar,j);
					dBetaDmean = alpha^DuStarDmean;
					dBetaDstdv = alpha^DuStarDstdv;
					aRandomVariable = theReliabilityDomain->getRandomVariablePtr(j);
					stdv = aRandomVariable->getStdv();
					mean = aRandomVariable->getMean();
					delta(j-1) = stdv * dBetaDmean;
					eta(j-1) = stdv * dBetaDstdv;
					// (Kappa is the sensitivity wrt. the coefficient of variation)
					if (mean != 0.0) {
						kappa(j-1) = -dBetaDmean*stdv/((stdv/mean)*(stdv/mean))+dBetaDstdv*mean;
					}
					else {
						kappa(j-1) = 0.0;
					}
				}
				// Don't scale them:
//				delta = delta * (1.0/delta.Norm());
//				eta = eta * (1.0/eta.Norm());
//				kappa = kappa * (1.0/kappa.Norm());
			}


			// Store key results in the limit-state functions
			theLimitStateFunction->FORMReliabilityIndexBeta				= beta;
			theLimitStateFunction->FORMProbabilityOfFailure_pf1			= pf1;
			theLimitStateFunction->designPoint_x_inOriginalSpace		= xStar;
			theLimitStateFunction->designPoint_u_inStdNormalSpace		= uStar;
			theLimitStateFunction->normalizedNegativeGradientVectorAlpha= alpha;
			theLimitStateFunction->importanceVectorGamma				= gamma;
			theLimitStateFunction->numberOfStepsToFindDesignPointAlgorithm		= i;
			theLimitStateFunction->GFunValueAtStartPt					= Go;
			theLimitStateFunction->GFunValueAtEndPt						= Glast;
			theLimitStateFunction->secondLast_u							= uSecondLast;
			theLimitStateFunction->secondLastAlpha						= alphaSecondLast;
			theLimitStateFunction->lastSearchDirection					= lastSearchDirection;


			// Print FORM results to the output file
			outputFile << "#######################################################################" << std::endl;
			outputFile << "#  FORM ANALYSIS RESULTS, LIMIT-STATE FUNCTION NUMBER "
				<<setiosflags(ios::left)<<setprecision(1)<<setw(4)<<lsf <<"            #" << std::endl;
			outputFile << "#                                                                     #" << std::endl;
			outputFile << "#  Limit-state function value at start point: ......... " 
				<<setiosflags(ios::left)<<setprecision(5)<<setw(12)<<Go 
				<< "  #" << std::endl;
			outputFile << "#  Limit-state function value at end point: ........... " 
				<<setiosflags(ios::left)<<setprecision(5)<<setw(12)<<Glast 
				<< "  #" << std::endl;
			outputFile << "#  Number of steps: ................................... " 
				<<setiosflags(ios::left)<<setw(12)<<i 
				<< "  #" << std::endl;
			outputFile << "#  Number of g-function evaluations: .................. " 
				<<setiosflags(ios::left)<<setw(12)<<numberOfEvaluations 
				<< "  #" << std::endl;
			outputFile << "#  Reliability index beta: ............................ " 
				<<setiosflags(ios::left)<<setprecision(5)<<setw(12)<<beta 
				<< "  #" << std::endl;
			outputFile << "#  FO approx. probability of failure, pf1: ............ " 
				<<setiosflags(ios::left)<<setprecision(5)<<setw(12)<<pf1 
				<< "  #" << std::endl;
			outputFile << "#                                                                     #" << std::endl;
			outputFile << "# rv#     x*          u*         alpha    gamma    delta    eta       #" << std::endl;
			outputFile.setf( ios::scientific, ios::floatfield );
			for (int i=0;  i<xStar.Size(); i++) {
			outputFile << "#  " <<setw(3)<<(i+1)<<" ";
			outputFile.setf(ios::scientific, ios::floatfield);
			if (xStar(i)<0.0) { outputFile << "-"; }
			else { outputFile << " "; }
			outputFile <<setprecision(3)<<setw(11)<<fabs(xStar(i));
			if (uStar(i)<0.0) { outputFile << "-"; }
			else { outputFile << " "; }
			outputFile <<setprecision(3)<<setw(11)<<fabs(uStar(i));
			outputFile.unsetf( ios::scientific );
			outputFile.setf(ios::fixed, ios::floatfield);

			if (alpha(i)<0.0) { outputFile << "-"; }
			else { outputFile << " "; }
			outputFile<<setprecision(5)<<setw(8)<<fabs(alpha(i));

			if (gamma(i)<0.0) { outputFile << "-"; }
			else { outputFile << " "; }
			outputFile<<setprecision(5)<<setw(8)<<fabs(gamma(i));		
			
			if (relSensTag == 1) {
				if (delta(i)<0.0) { outputFile << "-"; }
				else { outputFile << " "; }
				outputFile<<setprecision(5)<<setw(8)<<fabs(delta(i));		
				
				if (eta(i)<0.0) { outputFile << "-"; }
				else { outputFile << " "; }
				outputFile<<setprecision(5)<<setw(8)<<fabs(eta(i));		

//              Printing reliability sensitivity wrt. coefficient of variation
//				aRandomVariable = theReliabilityDomain->getRandomVariablePtr(i+1);
//				mean = aRandomVariable->getMean();
//				if (mean==0.0) { outputFile << "    -    "; }
//				else {
//					if (kappa(i)<0.0) { outputFile << "-"; }
//					else { outputFile << " "; }
//					outputFile<<setprecision(5)<<setw(8)<<fabs(kappa(i));		
//				}
			}
			else {
				outputFile << "    -        -    ";
			}
			
			outputFile<<"   #" << std::endl;
			}
			outputFile << "#                                                                     #" << std::endl;
			outputFile << "#######################################################################" << std::endl << std::endl << std::endl;


			// Inform the user that we're done with this limit-state function
			std::cerr << "Done analyzing limit-state function " << lsf << ", beta=" << beta << std::endl;
		}

	}


	// Clean up
	outputFile.close();
	delete aStdNormRV;

	// Print summary of results to screen (more here!!!)
	std::cerr << "FORMAnalysis completed." << std::endl;

	return 0;
}

