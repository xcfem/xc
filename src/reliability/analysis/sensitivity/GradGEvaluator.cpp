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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/10/27 23:45:44 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/sensitivity/GradGEvaluator.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include "GradGEvaluator.h"
#include <utility/matrix/Matrix.h>
#include <reliability/domain/components/ReliabilityDomain.h>
#include <tcl.h>

XC::GradGEvaluator::GradGEvaluator(ReliabilityDomain *passedReliabilityDomain,
							   Tcl_Interp *passedTclInterp)
{
	theTclInterp = passedTclInterp;
	theReliabilityDomain = passedReliabilityDomain;
	DgDpar = 0;
}

XC::GradGEvaluator::~GradGEvaluator()
{
	if (DgDpar != 0)
		delete DgDpar;
}



 XC::Matrix
XC::GradGEvaluator::getDgDdispl()
{
	std::cerr << "XC::GradGEvaluator::getDgDdispl() -- This method is not " << std::endl
		<< " implemented for the chosen type of XC::GradGEvaluator." << std::endl;
	
	Matrix dummy(1,1);
	return dummy;
}







int 
XC::GradGEvaluator::computeParameterDerivatives(double g)
{
	// Zero out the previous result matrix
	if (DgDpar != 0) {
		delete DgDpar;
		DgDpar = 0;
	}


	// Initial declarations
	char separators[5] = "}{";
	char tclAssignment[500];
	double onedgdpar;
	int i;


	// "Download" limit-state function from reliability domain
	int lsf = theReliabilityDomain->getTagOfActiveLimitStateFunction();
	LimitStateFunction *theLimitStateFunction = 
		theReliabilityDomain->getLimitStateFunctionPtr(lsf);
	std::string theExpression = theLimitStateFunction->getExpression();
	char *lsf_copy = new char[500];
	strcpy(lsf_copy,theExpression.c_str());
	char *tokenPtr = strtok( lsf_copy, separators); 

	while( tokenPtr != nullptr )
          {

		if ( strncmp(tokenPtr, "par", 3) == 0) {
	
			// Get parameter number
			int parameterNumber;
			sscanf(tokenPtr,"par_%i",&parameterNumber);

			// Store the original parameter value
			double originalValue;
			sprintf(tclAssignment , "($par_%d)",parameterNumber);
			Tcl_ExprDouble( theTclInterp, tclAssignment, &originalValue );
			
			// Assign a perturbed value
			sprintf(tclAssignment,"set par_%d [ expr ($par_%d*1.001) ]",parameterNumber,parameterNumber);
			Tcl_Eval( theTclInterp, tclAssignment);

			// Evaluate limit-state function again
			double g_perturbed;
			std::string theTokenizedExpression = theLimitStateFunction->getTokenizedExpression();
			Tcl_ExprDouble( theTclInterp, theTokenizedExpression.c_str(), &g_perturbed );

			// Compute the gradient 'dgdpar' by finite difference
			onedgdpar = (g_perturbed-g)/(originalValue*0.001);

			// Make assignment back to its original value
			sprintf(tclAssignment,"set par_%d %35.20f",parameterNumber,originalValue);
			Tcl_Eval( theTclInterp, tclAssignment);
			
			// Store the DgDpar in a matrix (make it expand successively)
			if (DgDpar == 0) {
				DgDpar = new XC::Matrix(1, 2);
				(*DgDpar)(0,0) = (double)parameterNumber;
				(*DgDpar)(0,1) = onedgdpar;
			}
			else {
				int oldSize = DgDpar->noRows();
				Matrix tempMatrix = *DgDpar;
				delete DgDpar;
				DgDpar = new XC::Matrix(oldSize+1, 2);
				for (i=0; i<oldSize; i++) {
					(*DgDpar)(i,0) = tempMatrix(i,0);
					(*DgDpar)(i,1) = tempMatrix(i,1);
				}
				(*DgDpar)(oldSize,0) = (double)parameterNumber;
				(*DgDpar)(oldSize,1) = onedgdpar;
			}
		}

		tokenPtr = strtok( nullptr, separators); 
	}

	delete [] lsf_copy;

	return 0;
}



 XC::Matrix 
XC::GradGEvaluator::getDgDpar()
{
	return (*DgDpar);
}
