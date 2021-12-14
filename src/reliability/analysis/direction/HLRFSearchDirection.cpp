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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/03/04 00:39:04 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/direction/HLRFSearchDirection.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu) during Spring 2000
// Revised: haukaas 06/00 (core code)
//			haukaas 06/01 (made part of official OpenSees)
//

#include <reliability/analysis/direction/HLRFSearchDirection.h>
#include <reliability/analysis/direction/SearchDirection.h>
#include <utility/matrix/Vector.h>


XC::HLRFSearchDirection::HLRFSearchDirection()
:SearchDirection(), searchDirection(1)
{
}

XC::Vector
XC::HLRFSearchDirection::getSearchDirection()
{
	return searchDirection;
}



int
XC::HLRFSearchDirection::computeSearchDirection(
							int stepNumber,
							Vector u, 
							double gFunctionValue, 
							Vector gradientInStandardNormalSpace )
{

	// Compute the norm of the gradient
	double normOfGradient = gradientInStandardNormalSpace.Norm();


	// Check that the norm is not zero
	if (normOfGradient == 0.0) {
		std::cerr << "XC::HLRFSearchDirection::computeSearchDirection() - " << std::endl
			<< " the norm of the gradient is zero. " << std::endl;
		return -1;
	}

	
	// Compute the alpha-vector
	Vector alpha = gradientInStandardNormalSpace * ( (-1) / normOfGradient );


	// Compute the direction vector
	double alpha_times_u = alpha ^ u ;
	Vector direction = alpha * ( gFunctionValue / normOfGradient + alpha_times_u ) - u;

	searchDirection = direction;

	return 0;
}

