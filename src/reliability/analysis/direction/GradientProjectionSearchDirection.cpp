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
                                                                        
// $Revision: 1.1 $
// $Date: 2003/03/04 00:39:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/direction/GradientProjectionSearchDirection.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include "GradientProjectionSearchDirection.h"
#include <reliability/analysis/direction/SearchDirection.h>
#include <reliability/analysis/stepSize/StepSizeRule.h>
#include <reliability/analysis/transformation/ProbabilityTransformation.h>
#include <reliability/analysis/gFunction/GFunEvaluator.h>
#include <reliability/analysis/rootFinding/RootFinding.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>


XC::GradientProjectionSearchDirection::GradientProjectionSearchDirection(
										StepSizeRule *passedStepSizeRule,
										ProbabilityTransformation *passedProbabilityTransformation,
										GFunEvaluator *passedGFunEvaluator,
										RootFinding *passedRootFindingAlgorithm)
:SearchDirection()
{
	theStepSizeRule = passedStepSizeRule;
	theProbabilityTransformation = passedProbabilityTransformation;
	theGFunEvaluator = passedGFunEvaluator;
	theRootFindingAlgorithm = passedRootFindingAlgorithm;
}


XC::Vector XC::GradientProjectionSearchDirection::getSearchDirection()
{
	return searchDirection;
}



int XC::GradientProjectionSearchDirection::computeSearchDirection(int stepNumber,const Vector &u,const double &passed_g, const Vector &gradG)
  {

    // Initial declarations
	int i,j;
	Vector u_new;
	double initialStepSize;
	Vector Direction;


	// Problem size 
	int nrv = u.Size();

	
	// Unit matrix
	Matrix I(nrv,nrv);
	for (i=0; i<nrv; i++) {
		for (j=0; j<nrv; j++) {
			if (i==j) {
				I(i,j) = 1.0;
			}
			else {
				I(i,j) = 0.0;
			}
		}
	}


	// Matrix of "outer" product of gradient vector
	Matrix dGdG(nrv,nrv);
	for (i=0; i<nrv; i++) {
		for (j=0; j<nrv; j++) {
			dGdG(i,j) = gradG(i)*gradG(j);
		}
	}

	// Get initial step size from the step size algorithm
	initialStepSize = theStepSizeRule->getInitialStepSize();


	// As long as it is not the first step; do the usual thing
	// (shouldn't happen if the user restarts the search...)
	if (stepNumber != 1) {

		// Compute the initial search direction vector
		Vector direction = (-1)* (I - (1.0/(gradG^gradG))*dGdG ) * u;

		// Initial step 
		u_new = u + initialStepSize*direction;

		// Set the direction of the Newton search
		Direction = gradG;
	}
	// If it's the first step; do the Newton thing from the
	// start point in the direction of the iHLRF search direction. 
	else {

		u_new = u;

		// Compute the alpha-vector
		Vector alpha = gradG * ( (-1) / gradG.Norm() );

		// Compute the direction vector
		double alpha_times_u = alpha ^ u ;
		Vector direction = alpha * ( passed_g / gradG.Norm() + alpha_times_u ) - u;

		// Set the direction of the Newton search
		Direction = (-1)*direction;
	}


	// Do the search to bring the trial point 'u_new' onto the lsf surface
	//const double tangent= gradG.Norm();
	Vector u_newest= theRootFindingAlgorithm->findLimitStateSurface(2,passed_g, Direction, u_new);


	// Return the final search direction
	// (remember to scale it so that u_new = u_old + lambda*d really brings us to u_new
	searchDirection = (1.0/initialStepSize)*(u_newest-u);

	return 0;

}




