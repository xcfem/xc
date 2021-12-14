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
// $Date: 2003/10/27 23:05:30 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/FEsensitivity/SensitivityAlgorithm.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/FEsensitivity/SensitivityAlgorithm.h>
#include <solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h>
#include <reliability/FEsensitivity/SensitivityIntegrator.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <utility/matrix/Vector.h>
#include <reliability/domain/components/ReliabilityDomain.h>
#include <reliability/domain/components/RandomVariablePositioner.h>
#include "solution/analysis/integrator/IncrementalIntegrator.h"


XC::SensitivityAlgorithm::SensitivityAlgorithm(ReliabilityDomain *passedReliabilityDomain,
										   EquiSolnAlgo *passedAlgorithm,
										   SensitivityIntegrator *passedSensitivityIntegrator,
										   int passedAnalysisTypeTag)
{
	// The reliability domain is needed to get hold 
	// of the random variable positioners:
	theReliabilityDomain = passedReliabilityDomain;

	// The finite element equation solution algorithm is 
	// needed to get hold of the system of equations (SOE):
	theAlgorithm = passedAlgorithm;

	// The sensitivity integrator is needed to assemble the 
	// new right-hand side of the system of equations:
	theSensitivityIntegrator = passedSensitivityIntegrator;

	// Tag to tell whether grads should be computed at each step
	// and whether they should be computed wrt. random variables
	analysisTypeTag = passedAnalysisTypeTag;

}


int XC::SensitivityAlgorithm::computeSensitivities(void)
  {
	// Meaning of analysisTypeTag:
	// 1: compute at each step wrt. random variables
	// 2: compute at each step wrt. parameters
	// 3: compute by command wrt. random variables
	// 4: compute by command wrt. parameters

	
	// Initial declarations
	int numGrads, numPos, i, gradNumber, posNumber;
	RandomVariablePositioner *theRandomVariablePositioner;
	ParameterPositioner *theParameterPositioner;


	// Get pointer to the system of equations (SOE)
	LinearSOE *theSOE = theAlgorithm->getLinearSOEPtr();


	// Get pointer to incremental integrator
	IncrementalIntegrator *theIncInt = theAlgorithm->getIncrementalIntegratorPtr();


	// Form current tangent at converged state
	// (would be nice with an if-statement here in case
	// the current tangent is already formed)
	if (theIncInt->formTangent(CURRENT_TANGENT) < 0){
		std::cerr << "WARNING XC::SensitivityAlgorithm::computeGradients() -";
		std::cerr << "the XC::Integrator failed in formTangent()\n";
		return -1;
	}


	// Get number of random variables and random variable positioners
	if (analysisTypeTag==1 || analysisTypeTag==3) {
		numGrads = theReliabilityDomain->getNumberOfRandomVariables();
		numPos = theReliabilityDomain->getNumberOfRandomVariablePositioners();
	}
	else {
		numPos = theReliabilityDomain->getNumberOfParameterPositioners();
		numGrads = numPos;
	}
	

	// Zero out the old right-hand side of the SOE
	theSOE->zeroB();
		

	// Form the part of the RHS which are indepent of parameter
	theSensitivityIntegrator->formIndependentSensitivityRHS();


	// Loop where one gradient vector is computed per iteration
	for (gradNumber=1; gradNumber<=numGrads; gradNumber++ )  {


		// Clear sensitivity flags
		for (i=1; i<=numPos; i++ ) {
			if (analysisTypeTag==1 || analysisTypeTag==3) {
				theRandomVariablePositioner = theReliabilityDomain->getRandomVariablePositionerPtr(i);
				theRandomVariablePositioner->activate(false);
			}
			else {
				theParameterPositioner = theReliabilityDomain->getParameterPositionerPtr(i);
				theParameterPositioner->activate(false);
			}
		}


		// Loop over positioners to check which are "active" this time
		// and set flag for phase 1
		for (posNumber=1; posNumber<=numPos; posNumber++ ) {

			if (analysisTypeTag==1 || analysisTypeTag==3) {
				// Get the random positioner and its rv#
				theRandomVariablePositioner = theReliabilityDomain->getRandomVariablePositionerPtr(posNumber);
				int rvNumber = theRandomVariablePositioner->getRvNumber();

				// Check if rv# of this rv positioner is the same as the gradient#
				if ( rvNumber==gradNumber ) {

					// Set sensitivity flag so that this one contributes to the RHS
					theRandomVariablePositioner->activate(true);

				} // End if rv# == gradient#
			}
			else {
				theParameterPositioner = theReliabilityDomain->getParameterPositionerPtr(posNumber);
				theParameterPositioner->activate(true);
			}

		} // End loop over r.v. positioners to check if it contributes


		// Zero out the old right-hand side
		theSOE->zeroB();

		
		// Form new right-hand side
		theSensitivityIntegrator->formSensitivityRHS(gradNumber);


		// Solve the system of equation with the new right-hand side
		theSOE->solve();


		// Save 'v' to the nodes for a "sensNodeDisp node? dof?" command
		theSensitivityIntegrator->saveSensitivity( theSOE->getX(), gradNumber, numGrads );


		// Commit unconditional history variables (also for elastic problems; strain sens may be needed anyway)
		theSensitivityIntegrator->commitSensitivity(gradNumber, numGrads);

	} // End loop for each gradient

    return 0;
}

bool 
XC::SensitivityAlgorithm::shouldComputeAtEachStep(void)
{
	if (analysisTypeTag==1 || analysisTypeTag==2) {
		return true;
	}
	else {
		return false;
	}
}
