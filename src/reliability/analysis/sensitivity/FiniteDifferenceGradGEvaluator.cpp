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
** file 'COPYRIGHT' in main directory for information on usage and    **
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
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/sensitivity/FiniteDifferenceGradGEvaluator.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/analysis/sensitivity/FiniteDifferenceGradGEvaluator.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <reliability/domain/components/ReliabilityDomain.h>
#include <reliability/domain/components/LimitStateFunction.h>
#include <reliability/analysis/gFunction/GFunEvaluator.h>
#include <reliability/domain/components/RandomVariable.h>
#include <tcl.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <boost/tokenizer.hpp>

using std::ifstream;
using std::ios;
using std::setw;
using std::setprecision;
using std::setiosflags;


XC::FiniteDifferenceGradGEvaluator::FiniteDifferenceGradGEvaluator(
					GFunEvaluator *passedGFunEvaluator,
					ReliabilityDomain *passedReliabilityDomain,
					Tcl_Interp *passedTclInterp,
					double passedPerturbationFactor,
					bool PdoGradientCheck,
					bool pReComputeG)
  :GradGEvaluator(passedReliabilityDomain, passedTclInterp, PdoGradientCheck)
  {
    theGFunEvaluator = passedGFunEvaluator;
    perturbationFactor= passedPerturbationFactor;
    reComputeG= pReComputeG;

    int nrv= passedReliabilityDomain->getNumberOfRandomVariables();
    grad_g= Vector(nrv);
  }


int XC::FiniteDifferenceGradGEvaluator::computeGradG(double gFunValue, const Vector &passed_x)
  {
	// Call base class method
	computeParameterDerivatives(gFunValue);


	// Possibly re-compute limit-state function value
	int result;
	if (reComputeG) {
		result= theGFunEvaluator->runGFunAnalysis(passed_x);
		if (result < 0) {
			std::cerr << "XC::FiniteDifferenceGradGEvaluator::evaluate_grad_g() - "
				  << std::endl
				<< " could not run analysis to evaluate limit-state function. " << std::endl;
			return -1;
		}
		result= theGFunEvaluator->evaluateG(passed_x);
		if (result < 0) {
			std::cerr << "XC::FiniteDifferenceGradGEvaluator::evaluate_grad_g() - "
				  << std::endl
				<< " could not tokenize limit-state function. " << std::endl;
			return -1;
		}
		gFunValue= theGFunEvaluator->getG();
	}


	// Initial declarations
	int numberOfRandomVariables= passed_x.Size();
	Vector perturbed_x(numberOfRandomVariables);
	RandomVariable *theRandomVariable;
	int i;
	double h;
	double gFunValueAStepAhead;
	double stdv;


	// For each random variable: perturb and run analysis again
	for ( i=0 ; i<numberOfRandomVariables ; i++ )
	{
		// Get random variable from domain
		theRandomVariable= theReliabilityDomain->getRandomVariablePtr(i+1);


		// Get the standard deviation
		stdv= theRandomVariable->getStdv();


		// Compute perturbation
		h= stdv/perturbationFactor;


		// Compute perturbed vector of random variables realization
		perturbed_x= passed_x;
		perturbed_x(i)= perturbed_x(i) + h;


		// Evaluate limit-state function
		result= theGFunEvaluator->runGFunAnalysis(perturbed_x);
		if (result < 0) {
			std::cerr << "XC::FiniteDifferenceGradGEvaluator::evaluate_grad_g() - "
				  << std::endl
				<< " could not run analysis to evaluate limit-state function. " << std::endl;
			return -1;
		}
		result= theGFunEvaluator->evaluateG(perturbed_x);
		if (result < 0) {
			std::cerr << "XC::FiniteDifferenceGradGEvaluator::evaluate_grad_g() - "
				  << std::endl
				<< " could not tokenize limit-state function. " << std::endl;
			return -1;
		}
		gFunValueAStepAhead= theGFunEvaluator->getG();


		// Compute the derivative by finite difference
		grad_g(i)= (gFunValueAStepAhead - gFunValue) / h;
	}


	if (doGradientCheck)
	  {
		char myString[100];
		ofstream outputFile( "FFDgradients.out", ios::out );
		std::cerr << std::endl;
		for (int ffd=0; ffd<grad_g.Size(); ffd++)
		  {
			std::cerr << "FFD("<< (ffd+1) << ")= " << grad_g(ffd) << std::endl;
			sprintf(myString,"%20.16e ",grad_g(ffd));
			outputFile << myString << std::endl;
		}
		outputFile.close();
		std::cerr << "PRESS Ctrl+C TO TERMINATE APPLICATION!" << std::endl;
		while(true) {
		}
	}

	return 0;
}




int XC::FiniteDifferenceGradGEvaluator::computeAllGradG(Vector gFunValues, const Vector &passed_x)
  {

	// Get number of random variables and performance functions
	int nrv= theReliabilityDomain->getNumberOfRandomVariables();
	int lsf= theReliabilityDomain->getNumberOfLimitStateFunctions();


	// Allocate result matrix
	if(grad_g_matrix.isEmpty())
	  { grad_g_matrix= Matrix(nrv,lsf); }
	else
	  { grad_g_matrix.Zero(); }

	// Initial declarations
	Vector perturbed_x(nrv);
	RandomVariable *theRandomVariable;
	double h;
	double gFunValueAStepAhead;
	double stdv;
	int result;


	// For each random variable: perturb and run analysis again

	for (int i=1; i<=nrv; i++) {

		// Get random variable from domain
		theRandomVariable= theReliabilityDomain->getRandomVariablePtr(i);


		// Get the standard deviation
		stdv= theRandomVariable->getStdv();


		// Compute perturbation
		h= stdv/perturbationFactor;


		// Compute perturbed vector of random variables realization
		perturbed_x= passed_x;
		perturbed_x(i-1)= perturbed_x(i-1) + h;


		// Evaluate limit-state function
		result= theGFunEvaluator->runGFunAnalysis(perturbed_x);
		if (result < 0) {
			std::cerr << "XC::FiniteDifferenceGradGEvaluator::evaluate_grad_g() - " << std::endl
				<< " could not run analysis to evaluate limit-state function. " << std::endl;
			return -1;
		}

		
		// Now loop over limit-state functions
		for (int j=1; j<=lsf; j++) {

			// Set tag of active limit-state function
			theReliabilityDomain->setTagOfActiveLimitStateFunction(j);

			// Limit-state function value
			result= theGFunEvaluator->evaluateG(perturbed_x);
			if (result < 0) {
				std::cerr << "XC::FiniteDifferenceGradGEvaluator::evaluate_grad_g() - "
					  << std::endl
					<< " could not tokenize limit-state function. " << std::endl;
				return -1;
			}
			gFunValueAStepAhead= theGFunEvaluator->getG();

			// Compute the derivative by finite difference
			grad_g_matrix(i-1,j-1)= (gFunValueAStepAhead - gFunValues(j-1)) / h;

		}
	}

	return 0;
}





const XC::Matrix &XC::FiniteDifferenceGradGEvaluator::getDgDdispl(void) const
  {
    // This method is implemented solely for the purpose of mean 
    // out-crossing rate analysis using "two searches". Then the 
    // derivative of the limit-state function wrt. the displacement
    // is needed to expand the limit-state function expression

    // Result matrix
    DgDdispl= Matrix();


    // Initial declaractions
    double perturbationFactor= 0.001; // (is multiplied by stdv and added to others...)
    char tclAssignment[500];
    const std::string dollarSign= "$";
    const std::string underscore= "_";
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> separators("}{");
    std::string tempchar("");
    double g, g_perturbed;
    int i;

    // "Download" limit-state function from reliability domain
    int lsf= theReliabilityDomain->getTagOfActiveLimitStateFunction();
    LimitStateFunction *theLimitStateFunction=  theReliabilityDomain->getLimitStateFunctionPtr(lsf);
    std::string theExpression= theLimitStateFunction->getExpression();
    // Tokenize the limit-state function and COMPUTE GRADIENTS
    tokenizer lsf_copy(theExpression, separators);
    std::string token;
    for(tokenizer::const_iterator tok_iter= lsf_copy.begin();
       tok_iter != lsf_copy.end(); ++tok_iter)
      {
	token= *tok_iter;
	const char *tokenPtr= token.c_str();
	tempchar= token;

	// If a nodal displacement is detected
	if(strncmp(tokenPtr, "u", 1) == 0)
	  {
	    // Get node number and dof number
	    int nodeNumber, direction;
	    sscanf(tempchar.c_str(),"u_%i_%i", &nodeNumber, &direction);

	    // Evaluate the limit-state function again
	    std::string theTokenizedExpression= theLimitStateFunction->getTokenizedExpression();
	    Tcl_ExprDouble( theTclInterp, theTokenizedExpression.c_str(), &g );

	    // Keep the original displacement value
	    double originalValue;
	    sprintf(tclAssignment,"$u_%d_%d", nodeNumber, direction);
	    Tcl_ExprDouble( theTclInterp, tclAssignment, &originalValue);

	    // Set perturbed value in the Tcl workspace
	    double newValue= originalValue*(1.0+perturbationFactor);
	    sprintf(tclAssignment,"set u_%d_%d %35.20f", nodeNumber, direction, newValue);
	    Tcl_Eval( theTclInterp, tclAssignment);

	    // Evaluate the limit-state function again
	    Tcl_ExprDouble( theTclInterp, theTokenizedExpression.c_str(), &g_perturbed );

	    // Compute gradient
	    double onedgdu= (g_perturbed-g)/(originalValue*perturbationFactor);

	    // Store the DgDdispl in a matrix
	    if(DgDdispl.isEmpty())
	      {
		DgDdispl= Matrix(1, 3);
		DgDdispl(0,0)= static_cast<double>(nodeNumber);
		DgDdispl(0,1)= static_cast<double>(direction);
		DgDdispl(0,2)= onedgdu;
	      }
	    else
	      {
		int oldSize= DgDdispl.noRows();
		Matrix tempMatrix= DgDdispl;
		DgDdispl= Matrix(oldSize+1, 3);
		for(i=0; i<oldSize; i++)
		  {
		    DgDdispl(i,0)= tempMatrix(i,0);
		    DgDdispl(i,1)= tempMatrix(i,1);
		    DgDdispl(i,2)= tempMatrix(i,2);
		  }
		DgDdispl(oldSize,0)= static_cast<double>(nodeNumber);
		DgDdispl(oldSize,1)= static_cast<double>(direction);
		DgDdispl(oldSize,2)= onedgdu;
	      }


	    // Make assignment back to its original value
	    sprintf(tclAssignment,"set u_%d_%d %35.20f", nodeNumber, direction, originalValue);
	    Tcl_Eval( theTclInterp, tclAssignment);
	}

      } 

    return DgDdispl;
  }

