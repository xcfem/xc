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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/10/27 23:45:44 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/sensitivity/OpenSeesGradGEvaluator.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/analysis/sensitivity/OpenSeesGradGEvaluator.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <reliability/domain/components/ReliabilityDomain.h>
#include <reliability/domain/components/LimitStateFunction.h>
#include <reliability/domain/components/RandomVariable.h>
#include <tcl.h>
#include <string.h>

#include <fstream>
#include <iomanip>
#include <iostream>
using std::ifstream;
using std::ios;
using std::setw;
using std::setprecision;
using std::setiosflags;


XC::OpenSeesGradGEvaluator::OpenSeesGradGEvaluator(
					Tcl_Interp *passedTclInterp,
					ReliabilityDomain *passedReliabilityDomain,
					bool PdoGradientCheck)
  :GradGEvaluator(passedReliabilityDomain, passedTclInterp, PdoGradientCheck)
    {
      theReliabilityDomain= passedReliabilityDomain;
      const int nrv= passedReliabilityDomain->getNumberOfRandomVariables();
      grad_g= Vector(nrv);
    }


int XC::OpenSeesGradGEvaluator::computeGradG(double g, const Vector &passed_x)
  {
	// Zero out the previous result matrix
    if(!DgDdispl.isEmpty())
      {	DgDdispl= Matrix(); }

	// Call base class method
	computeParameterDerivatives(g);

	// Initial declaractions
	double perturbationFactor= 0.001; // (is multiplied by stdv and added to others...)
	char tclAssignment[500];
	//const char dollarSign[]= "$";
	//const char underscore[]= "_";
	//char lsf_expression[500]= "";
	char separators[5]= "}{";
	int nrv= theReliabilityDomain->getNumberOfRandomVariables();
	RandomVariable *theRandomVariable;
	char tempchar[100]="";
	//char newSeparators[5]= "_";
	double g_perturbed;
	int i;
	double onedudx;
	Vector dudx(nrv);
	

	// Compute gradients if this is a path-INdependent analysis
	// (This command only has effect if it IS path-independent.)
	sprintf(tclAssignment,"computeGradients");
	Tcl_Eval( theTclInterp, tclAssignment );


	// Initialize gradient vector
	grad_g.Zero();


	// "Download" limit-state function from reliability domain
	int lsf= theReliabilityDomain->getTagOfActiveLimitStateFunction();
	LimitStateFunction *theLimitStateFunction= 
		theReliabilityDomain->getLimitStateFunctionPtr(lsf);
	std::string theExpression= theLimitStateFunction->getExpression();
	char *lsf_copy= new char[500];
	strcpy(lsf_copy,theExpression.c_str());


	// Tokenize the limit-state function and COMPUTE GRADIENTS
	char *tokenPtr= strtok( lsf_copy, separators); 
	while ( tokenPtr != nullptr ) {

		strcpy(tempchar,tokenPtr);

		if ( strncmp(tokenPtr, "x",1) == 0) {

			// Get random variable number
			int rvNum;
			sscanf(tempchar,"x_%i",&rvNum);

			// Perturb its value according to its standard deviation
			theRandomVariable= theReliabilityDomain->getRandomVariablePtr(rvNum);
			double stdv= theRandomVariable->getStdv();
			sprintf(tclAssignment , "set x_%d  %35.20f", rvNum, (passed_x(rvNum-1)+perturbationFactor*stdv) );
			Tcl_Eval( theTclInterp, tclAssignment);

			// Evaluate limit-state function again
			std::string theTokenizedExpression= theLimitStateFunction->getTokenizedExpression();
			Tcl_ExprDouble( theTclInterp, theTokenizedExpression.c_str(), &g_perturbed );

			// Make assignment back to its original value
			sprintf(tclAssignment , "set x_%d  %35.20f", rvNum, passed_x(rvNum-1) );
			Tcl_Eval( theTclInterp, tclAssignment);

			// Add gradient contribution
			grad_g(rvNum-1)+= (g_perturbed-g)/(perturbationFactor*stdv);
		}
		// If a nodal velocity is detected
		else if ( strncmp(tokenPtr, "ud", 2) == 0) {

			// Get node number and dof number
			int nodeNumber, direction;
			sscanf(tempchar,"ud_%i_%i", &nodeNumber, &direction);

			// Keep the original value
			double originalValue;
			sprintf(tclAssignment,"$ud_%d_%d", nodeNumber, direction);
			Tcl_ExprDouble( theTclInterp, tclAssignment, &originalValue);

			// Set perturbed value in the Tcl workspace
			double newValue= originalValue*(1.0+perturbationFactor);
			sprintf(tclAssignment,"set ud_%d_%d %35.20f", nodeNumber, direction, newValue);
			Tcl_Eval( theTclInterp, tclAssignment);

			// Evaluate the limit-state function again
			std::string theTokenizedExpression= theLimitStateFunction->getTokenizedExpression();
			Tcl_ExprDouble( theTclInterp, theTokenizedExpression.c_str(), &g_perturbed );

			// Compute gradient
			double onedgdu= (g_perturbed-g)/(originalValue*perturbationFactor);

			// Make assignment back to its original value
			sprintf(tclAssignment,"set ud_%d_%d %35.20f", nodeNumber, direction, originalValue);
			Tcl_Eval( theTclInterp, tclAssignment);

			// Obtain DDM gradient vector
			for (int i=1; i<=nrv; i++) {
				sprintf(tclAssignment , "set sens [sensNodeVel %d %d %d ]",nodeNumber,direction,i);
				Tcl_Eval( theTclInterp, tclAssignment);
				sprintf(tclAssignment , "$sens ");
				Tcl_ExprDouble( theTclInterp, tclAssignment, &onedudx );
				dudx( (i-1) )= onedudx;
			}

			// Add gradient contribution
			grad_g+= onedgdu*dudx;

		}
		// If a nodal displacement is detected
		else if ( strncmp(tokenPtr, "u", 1) == 0) {

			// Get node number and dof number
			int nodeNumber, direction;
			sscanf(tempchar,"u_%i_%i", &nodeNumber, &direction);

			// Keep the original value
			double originalValue;
			sprintf(tclAssignment,"$u_%d_%d", nodeNumber, direction);
			Tcl_ExprDouble( theTclInterp, tclAssignment, &originalValue);

			// Set perturbed value in the Tcl workspace
			double newValue= originalValue*(1.0+perturbationFactor);
			sprintf(tclAssignment,"set u_%d_%d %35.20f", nodeNumber, direction, newValue);
			Tcl_Eval( theTclInterp, tclAssignment);

			// Evaluate the limit-state function again
			std::string theTokenizedExpression= theLimitStateFunction->getTokenizedExpression();
			Tcl_ExprDouble( theTclInterp, theTokenizedExpression.c_str(), &g_perturbed );

			// Compute gradient
			double onedgdu= (g_perturbed-g)/(originalValue*perturbationFactor);

			// Store the DgDdispl in a matrix
			if(DgDdispl.isEmpty())
			  {
			    DgDdispl= Matrix(1, 3);
			    DgDdispl(0,0)= static_cast<double>(nodeNumber);
			    DgDdispl(0,1)= static_cast<double>(direction);
			    DgDdispl(0,2)= onedgdu;
			  }
			else
			  {
			    int oldSize= DgDdispl.noRows();
			    Matrix tempMatrix= DgDdispl;
			    DgDdispl= Matrix(oldSize+1, 3);
			    for (i=0; i<oldSize; i++)
			      {
				DgDdispl(i,0)= tempMatrix(i,0);
				DgDdispl(i,1)= tempMatrix(i,1);
				DgDdispl(i,2)= tempMatrix(i,2);
			      }
			    DgDdispl(oldSize,0)= static_cast<double>(nodeNumber);
			    DgDdispl(oldSize,1)= static_cast<double>(direction);
			    DgDdispl(oldSize,2)= onedgdu;
			  }


			// Make assignment back to its original value
			sprintf(tclAssignment,"set u_%d_%d %35.20f", nodeNumber, direction, originalValue);
			Tcl_Eval( theTclInterp, tclAssignment);

			// Obtain DDM gradient vector
			for (int i=1; i<=nrv; i++) {
				sprintf(tclAssignment , "set sens [sensNodeDisp %d %d %d ]",nodeNumber,direction,i);
				Tcl_Eval( theTclInterp, tclAssignment);
				sprintf(tclAssignment , "$sens ");
				Tcl_ExprDouble( theTclInterp, tclAssignment, &onedudx );
				dudx( (i-1) )= onedudx;
			}

			// Add gradient contribution
			grad_g+= onedgdu*dudx;

		}
		else if ( strncmp(tokenPtr, "rec",3) == 0) {
		}

		tokenPtr= strtok( nullptr, separators);  // read next token and go up and check the while condition again
	} 

	delete [] lsf_copy;

	if(doGradientCheck)
	  {
		char myString[100];
		std::ofstream outputFile( "DDMgradients.out", ios::out );
		std::cerr << std::endl;
		for(int ddm=0; ddm<grad_g.Size(); ddm++)
		  {
		    std::cerr << "DDM("<< (ddm+1) << ")= " << grad_g(ddm)
			      << std::endl;
			sprintf(myString,"%20.16e ",grad_g(ddm));
			outputFile << myString << std::endl;
		}
		outputFile.close();
		std::cerr << "PRESS Ctrl+C TO TERMINATE APPLICATION!" << std::endl;
		while(true) {
		}
	}



	return 0;

}


int XC::OpenSeesGradGEvaluator::computeAllGradG(Vector gFunValues, const Vector &passed_x)
  {

	// Allocate result matrix
	Vector gradG(passed_x.Size());
	if(grad_g_matrix.isEmpty())
	  {
	    grad_g_matrix= Matrix(passed_x.Size(), gFunValues.Size());
	  }
	else
	  {
	    grad_g_matrix.Zero();
	  }


	// Loop over performance functions
	for (int j=1; j<=gFunValues.Size(); j++)
	  {

		// Set tag of active limit-state function
		theReliabilityDomain->setTagOfActiveLimitStateFunction(j);

		this->computeGradG(gFunValues(j-1),passed_x);
		gradG= this->getGradG();

		for(int i=1; i<=passed_x.Size(); i++)
		  { grad_g_matrix(i-1,j-1)= gradG(i-1); }
	}

	return 0;
  }



