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
// $Date: 2003/10/27 23:45:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/convergenceCheck/StandardReliabilityConvergenceCheck.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/analysis/convergenceCheck/StandardReliabilityConvergenceCheck.h>
#include <reliability/analysis/convergenceCheck/ReliabilityConvergenceCheck.h>
#include <cmath>
#include <utility/matrix/Vector.h>

#include <fstream>
#include <iomanip>
#include <iostream>
using std::ifstream;
using std::ios;
using std::setw;
using std::setprecision;
using std::setiosflags;


XC::StandardReliabilityConvergenceCheck::StandardReliabilityConvergenceCheck(double passedE1, 
																		 double passedE2, 
																		 double pscaleValue,
																		 int print)
:ReliabilityConvergenceCheck()
{
	e1 = passedE1;
	e2 = passedE2;
	criterium1 = 0.0;
	criterium2 = 0.0;
	scaleValue = pscaleValue;
	printFlag = print;
}

int	
XC::StandardReliabilityConvergenceCheck::setScaleValue(double passedScaleValue)
{
	if (scaleValue == 0.0) {
		scaleValue = passedScaleValue;
	}

	return 0;
}


int	
XC::StandardReliabilityConvergenceCheck::check(Vector u, double g, Vector gradG)
{
	if (scaleValue == 0.0) {
		std::cerr << "XC::StandardReliabilityConvergenceCheck::check() --" << std::endl
			<< " scale value has not been set!" << std::endl;
	}


	// Alpha vector
	Vector alpha = gradG *  ( (-1.0) / gradG.Norm() );


	// The scaling factor (rather new development)
	double temp0=u.Norm();
	if (temp0 < 1.0) {
		temp0 = 1.0; 
	}


	// Scaled u-vector
	Vector u_scaled = (1.0/temp0) * u;


	// Convergence criteria
	Vector temp1 = u_scaled - (alpha^u_scaled)*alpha;
	criterium1 = fabs(g / scaleValue);
	criterium2 = temp1.Norm();


	// Inform user about convergence status 
	static ofstream logfile( "ConvergenceCheckLog.txt", ios::out );
	char outputString[100];
	sprintf(outputString,"check1=(%11.3e), check2=(%10.3e), dist=%16.14f",criterium1,criterium2,u.Norm());
	if (printFlag!=0) {
		std::cerr << outputString << std::endl;
	}
	logfile << outputString << std::endl;
	logfile.flush();


	// Return '1' if the analysis converged ('-1' otherwise)
	if ( ( criterium1 < e1 ) && ( criterium2 < e2 ) ) {
		return 1;
	}
	else {
		return -1;
	}
}

int 
XC::StandardReliabilityConvergenceCheck::getNumberOfCriteria()
{
	return 2;
}

double 
XC::StandardReliabilityConvergenceCheck::getCriteriaValue(int whichCriteria)
{
	if (whichCriteria == 1) {
		return criterium1; 
	}
	else if (whichCriteria == 2) {
		return criterium2;
	}
	else {
		std::cerr << "XC::StandardReliabilityConvergenceCheck::getCriteriaValue() -- " << std::endl
			<< " criterium number " << whichCriteria << " does not exist!" << std::endl;
		return 0.0;
	}

}
