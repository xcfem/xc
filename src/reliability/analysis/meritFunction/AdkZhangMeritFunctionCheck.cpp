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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/10/27 23:45:43 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/meritFunction/AdkZhangMeritFunctionCheck.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/analysis/meritFunction/AdkZhangMeritFunctionCheck.h>
#include <reliability/analysis/meritFunction/MeritFunctionCheck.h>
#include <cmath>
#include <utility/matrix/Vector.h>


XC::AdkZhangMeritFunctionCheck::AdkZhangMeritFunctionCheck(double pmulti, double padd, double pa)
:MeritFunctionCheck()
{
	multi = pmulti;
	add = padd;
	a = pa;
}

int
XC::AdkZhangMeritFunctionCheck::check(Vector u_old, 
								  double g_old, 
								  Vector grad_G_old, 
								  double stepSize,
								  Vector stepDirection,
								  double g_new)
{

	// Update penalty parameter 'c' (should remain constant along the search direction)
	this->updateMeritParameters(u_old,g_old,grad_G_old);

	
	// New point in standard normal space
	Vector u_new = u_old + stepSize*stepDirection;


	// Compute value of merit functions
	Vector dummy(1);
	double merit_old = this->getMeritFunctionValue(u_old,g_old,dummy);
	double merit_new = this->getMeritFunctionValue(u_new,g_new,dummy);

	
	// Gradient of the merit function
	double signumG;
	if (g_old != 0.0) {
		signumG = g_old/fabs(g_old);
	}
	else {
		signumG = 1.0;
	}
	Vector gradM_old = u_old + c * signumG * grad_G_old;


	// Do the check
	if (  (merit_new-merit_old)  <=  a*stepSize*(gradM_old^stepDirection)  ) {
		return 0;  // ok
	}
	else {
		return -1; // not ok
	}
	
}





int
XC::AdkZhangMeritFunctionCheck::updateMeritParameters(Vector u, 
												  double g,
												  Vector grad_G)
{
	// Update penalty factor 'c'
	c = (u.Norm() / grad_G.Norm()) * multi + add;

	return 0;
}


double
XC::AdkZhangMeritFunctionCheck::getMeritFunctionValue(Vector u, 
												  double g,
												  Vector grad_G)
{
	// Note that it is correct to keep 'c' constant

	// Compute merit function
	double merit = 0.5 * (u ^ u) + c * fabs(g);


	// Return the result
	return merit;
}
