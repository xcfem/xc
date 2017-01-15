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
// $Date: 2003/03/04 00:39:23 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/meritFunction/CriteriaReductionMeritFunctionCheck.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/analysis/meritFunction/CriteriaReductionMeritFunctionCheck.h>
#include <reliability/analysis/meritFunction/MeritFunctionCheck.h>
#include <reliability/analysis/convergenceCheck/ReliabilityConvergenceCheck.h>
#include <cmath>
#include <utility/matrix/Vector.h>


XC::CriteriaReductionMeritFunctionCheck::CriteriaReductionMeritFunctionCheck(ReliabilityConvergenceCheck *passedReliabilityConvergenceCheck)
:MeritFunctionCheck()
{
	theReliabilityConvergenceCheck = passedReliabilityConvergenceCheck;
}

int XC::CriteriaReductionMeritFunctionCheck::check(Vector u_old, 
								  double g_old, 
								  Vector grad_G_old, 
								  double stepSize,
								  Vector stepDirection,
								  double g_new, 
								  Vector grad_G_new)
{
	// New point in standard normal space
	Vector u_new = u_old + stepSize*stepDirection;


	// Number of convergence criteria
	int numCrit = theReliabilityConvergenceCheck->getNumberOfCriteria();


	// Initial declarations
	int i;
	Vector oldCriteriaValues(numCrit);
	Vector newCriteriaValues(numCrit);


	// Convergence checks for the old point
	theReliabilityConvergenceCheck->check(u_old,g_old,grad_G_old);
	for (i=1; i<=numCrit; i++) {
		oldCriteriaValues(i-1) = theReliabilityConvergenceCheck->getCriteriaValue(i);
	}
	

	// Convergence checks for the new point
	theReliabilityConvergenceCheck->check(u_new,g_new,grad_G_new);
	for (i=1; i<=numCrit; i++) {
		newCriteriaValues(i-1) = theReliabilityConvergenceCheck->getCriteriaValue(i);
	}


	// All criteria must have improved for the step to be OK
	bool OK = true;
	for (i=1; i<=numCrit; i++) {
		if (newCriteriaValues(i-1)>oldCriteriaValues(i-1)) {
			OK = false;
		}
	}


	// Return
	if (OK) {
		return 1;
	}
	else {
		return -1;
	}
}





double
XC::CriteriaReductionMeritFunctionCheck::getMeritFunctionValue(Vector u, 
												  double g, 
												  Vector grad_G)
{
	std::cerr << "XC::CriteriaReductionMeritFunctionCheck::getMeritFunctionValue() -- this method" << std::endl
		<< " is not implemented in this specific class." << std::endl;

	return 0.0;
}

int
XC::CriteriaReductionMeritFunctionCheck::updateMeritParameters(Vector u, 
												  double g, 
												  Vector grad_G)
{
	std::cerr << "XC::CriteriaReductionMeritFunctionCheck::updateMeritParameters() -- this method" << std::endl
		<< " is not implemented in this specific class." << std::endl;

	return 0;
}
