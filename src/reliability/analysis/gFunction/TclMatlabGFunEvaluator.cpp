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
#include <reliability/analysis/gFunction/TclMatlabGFunEvaluator.h>
#include <utility/matrix/Vector.h>
#include <reliability/domain/components/ReliabilityDomain.h>
#include <reliability/domain/components/LimitStateFunction.h>
#include <reliability/domain/components/RandomVariablePositioner.h>
#include <reliability/domain/components/ReliabilityDomain.h>

#include <tcl.h>
#include <cstring>

#include <fstream>
//#include <engine.h>
//#include <mex.h>



XC::TclMatlabGFunEvaluator::TclMatlabGFunEvaluator(Tcl_Interp *passedTclInterp,
											   ReliabilityDomain *passedReliabilityDomain)

:GFunEvaluator()
{
	theTclInterp			= passedTclInterp;
	theReliabilityDomain	= passedReliabilityDomain;
}

int
XC::TclMatlabGFunEvaluator::evaluate_g(Vector x)
{

	// "Download" limit-state function from reliability domain
	int lsf = theReliabilityDomain->getTagOfActiveLimitStateFunction();
	LimitStateFunction *theLimitStateFunction = 
		theReliabilityDomain->getLimitStateFunctionPtr(lsf);

	if (lsf == 1) {

		// Print the realization of x to file called 'realization.txt'
	  std::ofstream outputFile( "realization.txt", std::ios::out );
		for (int i=0; i<x.Size(); i++) {
			outputFile << x(i) << std::endl;
		}
		outputFile.close();


		// Execute a Tcl file called 'tclgfun.tcl' (remember to "reset" analysis!)
		char theTclCommand[30];
		sprintf(theTclCommand,"source tclgfun.tcl");
		Tcl_Eval( theTclInterp, theTclCommand );

		
		// Start a Matlab engine
		Engine *ep;
		ep = engOpen("\0");


		// Execute a Matlab function called 'matlabgfun'
		char theMatlabCommand[50];
		sprintf(theMatlabCommand,"matlabgfun");
		engEvalString(ep, theMatlabCommand);


		// Shut down the Matlab engine
		engClose(ep);

	}
	else {
		// Does nothing
	}


	// Read value of limit-state functions from file called 'gfun.txt'
	double gvalue;
	ifstream inputFile( "gfun.txt", ios::in );

	for (int i=1; i<=lsf; i++) {
		inputFile >> gvalue;
	}


	// Store the value of the g-function
	g = gvalue;

	return 0;
}


double
XC::TclMatlabGFunEvaluator::get_g()
{
	return g;
}
