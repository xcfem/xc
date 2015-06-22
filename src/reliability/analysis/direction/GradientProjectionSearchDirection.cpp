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




