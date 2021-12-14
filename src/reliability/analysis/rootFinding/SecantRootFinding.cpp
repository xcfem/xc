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
// $Date: 2003/03/04 00:39:31 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/rootFinding/SecantRootFinding.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu) 
//

#include <reliability/analysis/rootFinding/SecantRootFinding.h>
#include <reliability/analysis/rootFinding/RootFinding.h>
#include <reliability/analysis/gFunction/GFunEvaluator.h>
#include <reliability/analysis/transformation/ProbabilityTransformation.h>
#include <reliability/domain/components/ReliabilityDomain.h>
#include <reliability/domain/components/RandomVariable.h>
#include <cmath>
#include <utility/matrix/Vector.h>


XC::SecantRootFinding::SecantRootFinding(
						ReliabilityDomain *passedReliabilityDomain,
						ProbabilityTransformation *passedProbabilityTransformation,
						GFunEvaluator *passedGFunEvaluator,
						int passedMaxIter,
						double ptol,
						double pmaxStepLength)
:RootFinding()
{
	theReliabilityDomain = passedReliabilityDomain;
	theProbabilityTransformation = passedProbabilityTransformation;
	theGFunEvaluator = passedGFunEvaluator;
	maxIter = passedMaxIter;
	tol = ptol;
	maxStepLength = pmaxStepLength;
}



 XC::Vector
XC::SecantRootFinding::findLimitStateSurface(int space, double g, Vector pDirection, Vector thePoint)
{
	// Set scale factor for 'g' for convergence check
	double scaleG;
	if (fabs(g)>1.0e-4) { scaleG = g;}
	else { 		scaleG = 1.0;}

	// Normalize the direction vector
	Vector Direction = pDirection/pDirection.Norm();

	// Scale 'maxStepLength' by standard deviation
	// (only if the user has specified to "walk" in original space)
	double perturbation;
	double realMaxStepLength = maxStepLength;
	if (space == 1) {

		// Go through direction vector and see which element is biggest
		// compared to its standard deviation
		int nrv = theReliabilityDomain->getNumberOfRandomVariables();
		RandomVariable *theRV;
		double stdv, theStdv= 0.0;
		int theBiggest;
		double maxRatio = 0.0;
		for(int i=0; i<nrv; i++)
                  {
		    theRV = theReliabilityDomain->getRandomVariablePtr(i+1);
		    stdv = theRV->getStdv();
			if (Direction(i)/stdv > maxRatio) {
				maxRatio = Direction(i)/stdv;
				theStdv = stdv;
				theBiggest = i+1;
			}
		}

		// Now scale so that 'maxStepSize' is related to the real stdv
		perturbation = maxStepLength * theStdv;
		realMaxStepLength = perturbation;
	}
	else {
		perturbation = maxStepLength;
	}

	Vector theTempPoint;
	double g_old= 0.0, g_new;
	bool didNotConverge=true;
	double result;
	double tangent;


	int i=0;
	while (i<=maxIter && didNotConverge) {


		// Increment counter right away...
		i++;

		if (i!=1) {

			// Transform the point into x-space if the user has given it in 2-space
			if (space==2) {
				result = theProbabilityTransformation->set_u(thePoint);
				if (result < 0) {
					std::cerr << "XC::GFunVisualizationAnalysis::analyze() - " << std::endl
						<< " could not set u in the xu-transformation." << std::endl;
					//return -1; Comentado LCPT.
				}

				result = theProbabilityTransformation->transform_u_to_x();
				if (result < 0) {
					std::cerr << "XC::GFunVisualizationAnalysis::analyze() - " << std::endl
						<< " could not transform from u to x and compute Jacobian." << std::endl;
					//return -1; Comentado LCPT.
				}
				theTempPoint = theProbabilityTransformation->get_x();
			}
			else {
				theTempPoint = thePoint;
			}


			// Evaluate limit-state function
			result = theGFunEvaluator->runGFunAnalysis(theTempPoint);
			if (result < 0) {
				std::cerr << "XC::GFunVisualizationAnalysis::analyze() - " << std::endl
					<< " could not run analysis to evaluate limit-state function. " << std::endl;
				//return -1; Comentado LCPT.
			}
			result = theGFunEvaluator->evaluateG(theTempPoint);
			if (result < 0) {
				std::cerr << "XC::GFunVisualizationAnalysis::analyze() - " << std::endl
					<< " could not tokenize limit-state function. " << std::endl;
				//return -1; Comentado LCPT.
			}
			g_new = theGFunEvaluator->getG();
		}
		else {
			g_new = g;
		}

		

		// Check convergence
		if (fabs(g_new/scaleG) < tol) {
			didNotConverge = false;
		}
		else {
			if (i==maxIter) {
				std::cerr << "WARNING: Projection scheme failed to find surface..." << std::endl;
			}
			else if (i==1) {
				thePoint = thePoint - perturbation * Direction;
				g_old = g_new;
			}
			else {

				// Take a step
				tangent = (g_new-g_old)/perturbation;
				perturbation = -g_new/tangent;
				if (fabs(perturbation) > realMaxStepLength) {
					perturbation = perturbation/fabs(perturbation)*realMaxStepLength;
				}
				thePoint = thePoint - perturbation * Direction;
				g_old = g_new;
			}
		}
	}

	return thePoint;
}


