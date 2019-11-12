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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/03/04 00:38:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/curvature/FirstPrincipalCurvature.cpp,v $

//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu) 
//

#include <reliability/analysis/curvature/FirstPrincipalCurvature.h>
#include <reliability/analysis/curvature/FindCurvatures.h>
#include <reliability/domain/components/LimitStateFunction.h>
#include <utility/matrix/Vector.h>


XC::FirstPrincipalCurvature::FirstPrincipalCurvature()
:FindCurvatures(), curvatures(1)
{
}

int XC::FirstPrincipalCurvature::computeCurvatures(ReliabilityDomain *theReliabilityDomain)
{

	// "Download" limit-state function from reliability domain
	int lsf = theReliabilityDomain->getTagOfActiveLimitStateFunction();
	LimitStateFunction *theLimitStateFunction = 
		theReliabilityDomain->getLimitStateFunctionPtr(lsf);

	// Get hold of 'u' and 'alpha' at the two last steps
	Vector last_u = theLimitStateFunction->designPoint_u_inStdNormalSpace;
	Vector secondLast_u = theLimitStateFunction->secondLast_u;
	Vector lastAlpha = theLimitStateFunction->normalizedNegativeGradientVectorAlpha;
	Vector secondLastAlpha = theLimitStateFunction->secondLastAlpha;

	// Compute curvature according to Der Kiureghian & De Stefano (1992), Eq.26:

	// Initial computations
	Vector uLastMinus_u = last_u - secondLast_u;
	double signumProduct = secondLastAlpha ^ uLastMinus_u;
	double alphaProduct = secondLastAlpha ^ lastAlpha;
	double sumSquared = 0.0;

	// Compute norm of the difference vector
	for ( int i=0; i<last_u.Size(); i++ ) {
		sumSquared += uLastMinus_u(i)*uLastMinus_u(i);
	}

	double norm_uLastMinus_u = sqrt(sumSquared);

	// Check sign and compute curvature
	if (fabs(signumProduct)==(signumProduct)) {
		curvatures(0) = acos(alphaProduct) / norm_uLastMinus_u;
	}
	else {
		curvatures(0) = -acos(alphaProduct) / norm_uLastMinus_u;
	}

	return 0;
}


XC::Vector XC::FirstPrincipalCurvature::getCurvatures()
  { return curvatures; }


