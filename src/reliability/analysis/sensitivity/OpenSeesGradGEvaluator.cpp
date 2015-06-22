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
:GradGEvaluator(passedReliabilityDomain, passedTclInterp)
{
	theReliabilityDomain = passedReliabilityDomain;
	doGradientCheck = PdoGradientCheck;

	int nrv = passedReliabilityDomain->getNumberOfRandomVariables();
	grad_g = new XC::Vector(nrv);
	grad_g_matrix = 0;

	DgDdispl = 0;
}

XC::OpenSeesGradGEvaluator::~OpenSeesGradGEvaluator()
  {
    if(grad_g != 0) 
		delete grad_g;

	if (DgDdispl != 0)
		delete DgDdispl;

	if (grad_g_matrix != 0)
		delete grad_g_matrix;
}




 XC::Vector
XC::OpenSeesGradGEvaluator::getGradG()
{
	return (*grad_g);
}


 XC::Matrix
XC::OpenSeesGradGEvaluator::getAllGradG()
{
	if (grad_g_matrix==0) {
		Matrix dummy(1,1);
		return dummy;
	}
	else {
		return (*grad_g_matrix);
	}
}


int XC::OpenSeesGradGEvaluator::computeGradG(double g, Vector passed_x)
  {
	// Zero out the previous result matrix
	if (DgDdispl != 0) {
		delete DgDdispl;
		DgDdispl = 0;
	}

	// Call base class method
	computeParameterDerivatives(g);

	// Initial declaractions
	double perturbationFactor = 0.001; // (is multiplied by stdv and added to others...)
	char tclAssignment[500];
	//const char dollarSign[] = "$";
	//const char underscore[] = "_";
	//char lsf_expression[500] = "";
	char separators[5] = "}{";
	int nrv = theReliabilityDomain->getNumberOfRandomVariables();
	RandomVariable *theRandomVariable;
	char tempchar[100]="";
	//char newSeparators[5] = "_";
	double g_perturbed;
	int i;
	double onedudx;
	Vector dudx(nrv);
	

	// Compute gradients if this is a path-INdependent analysis
	// (This command only has effect if it IS path-independent.)
	sprintf(tclAssignment,"computeGradients");
	Tcl_Eval( theTclInterp, tclAssignment );


	// Initialize gradient vector
	grad_g->Zero();


	// "Download" limit-state function from reliability domain
	int lsf = theReliabilityDomain->getTagOfActiveLimitStateFunction();
	LimitStateFunction *theLimitStateFunction = 
		theReliabilityDomain->getLimitStateFunctionPtr(lsf);
	std::string theExpression = theLimitStateFunction->getExpression();
	char *lsf_copy = new char[500];
	strcpy(lsf_copy,theExpression.c_str());


	// Tokenize the limit-state function and COMPUTE GRADIENTS
	char *tokenPtr = strtok( lsf_copy, separators); 
	while ( tokenPtr != nullptr ) {

		strcpy(tempchar,tokenPtr);

		if ( strncmp(tokenPtr, "x",1) == 0) {

			// Get random variable number
			int rvNum;
			sscanf(tempchar,"x_%i",&rvNum);

			// Perturb its value according to its standard deviation
			theRandomVariable = theReliabilityDomain->getRandomVariablePtr(rvNum);
			double stdv = theRandomVariable->getStdv();
			sprintf(tclAssignment , "set x_%d  %35.20f", rvNum, (passed_x(rvNum-1)+perturbationFactor*stdv) );
			Tcl_Eval( theTclInterp, tclAssignment);

			// Evaluate limit-state function again
			std::string theTokenizedExpression = theLimitStateFunction->getTokenizedExpression();
			Tcl_ExprDouble( theTclInterp, theTokenizedExpression.c_str(), &g_perturbed );

			// Make assignment back to its original value
			sprintf(tclAssignment , "set x_%d  %35.20f", rvNum, passed_x(rvNum-1) );
			Tcl_Eval( theTclInterp, tclAssignment);

			// Add gradient contribution
			(*grad_g)(rvNum-1) += (g_perturbed-g)/(perturbationFactor*stdv);
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
			double newValue = originalValue*(1.0+perturbationFactor);
			sprintf(tclAssignment,"set ud_%d_%d %35.20f", nodeNumber, direction, newValue);
			Tcl_Eval( theTclInterp, tclAssignment);

			// Evaluate the limit-state function again
			std::string theTokenizedExpression = theLimitStateFunction->getTokenizedExpression();
			Tcl_ExprDouble( theTclInterp, theTokenizedExpression.c_str(), &g_perturbed );

			// Compute gradient
			double onedgdu = (g_perturbed-g)/(originalValue*perturbationFactor);

			// Make assignment back to its original value
			sprintf(tclAssignment,"set ud_%d_%d %35.20f", nodeNumber, direction, originalValue);
			Tcl_Eval( theTclInterp, tclAssignment);

			// Obtain DDM gradient vector
			for (int i=1; i<=nrv; i++) {
				sprintf(tclAssignment , "set sens [sensNodeVel %d %d %d ]",nodeNumber,direction,i);
				Tcl_Eval( theTclInterp, tclAssignment);
				sprintf(tclAssignment , "$sens ");
				Tcl_ExprDouble( theTclInterp, tclAssignment, &onedudx );
				dudx( (i-1) ) = onedudx;
			}

			// Add gradient contribution
			(*grad_g) += onedgdu*dudx;

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
			double newValue = originalValue*(1.0+perturbationFactor);
			sprintf(tclAssignment,"set u_%d_%d %35.20f", nodeNumber, direction, newValue);
			Tcl_Eval( theTclInterp, tclAssignment);

			// Evaluate the limit-state function again
			std::string theTokenizedExpression= theLimitStateFunction->getTokenizedExpression();
			Tcl_ExprDouble( theTclInterp, theTokenizedExpression.c_str(), &g_perturbed );

			// Compute gradient
			double onedgdu = (g_perturbed-g)/(originalValue*perturbationFactor);

			// Store the DgDdispl in a matrix
			if (DgDdispl == 0) {
				DgDdispl = new XC::Matrix(1, 3);
				(*DgDdispl)(0,0) = (double)nodeNumber;
				(*DgDdispl)(0,1) = (double)direction;
				(*DgDdispl)(0,2) = onedgdu;
			}
			else {
				int oldSize = DgDdispl->noRows();
				Matrix tempMatrix = *DgDdispl;
				delete DgDdispl;
				DgDdispl = new XC::Matrix(oldSize+1, 3);
				for (i=0; i<oldSize; i++) {
					(*DgDdispl)(i,0) = tempMatrix(i,0);
					(*DgDdispl)(i,1) = tempMatrix(i,1);
					(*DgDdispl)(i,2) = tempMatrix(i,2);
				}
				(*DgDdispl)(oldSize,0) = (double)nodeNumber;
				(*DgDdispl)(oldSize,1) = (double)direction;
				(*DgDdispl)(oldSize,2) = onedgdu;
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
				dudx( (i-1) ) = onedudx;
			}

			// Add gradient contribution
			(*grad_g) += onedgdu*dudx;

		}
		else if ( strncmp(tokenPtr, "rec",3) == 0) {
		}

		tokenPtr = strtok( nullptr, separators);  // read next token and go up and check the while condition again
	} 

	delete [] lsf_copy;

	if (doGradientCheck) {
		char myString[100];
		std::ofstream outputFile( "DDMgradients.out", ios::out );
		std::cerr << std::endl;
		for (int ddm=0; ddm<grad_g->Size(); ddm++) {
			std::cerr << "DDM("<< (ddm+1) << ") = " << (*grad_g)(ddm) << std::endl;
			sprintf(myString,"%20.16e ",(*grad_g)(ddm));
			outputFile << myString << std::endl;
		}
		outputFile.close();
		std::cerr << "PRESS Ctrl+C TO TERMINATE APPLICATION!" << std::endl;
		while(true) {
		}
	}



	return 0;

}



int
XC::OpenSeesGradGEvaluator::computeAllGradG(Vector gFunValues, Vector passed_x)
{

	// Allocate result matrix
	Vector gradG(passed_x.Size());
	if (grad_g_matrix == 0) {
		grad_g_matrix = new XC::Matrix(passed_x.Size(), gFunValues.Size());
	}
	else {
		grad_g_matrix->Zero();
	}


	// Loop over performance functions
	for (int j=1; j<=gFunValues.Size(); j++) {

		// Set tag of active limit-state function
		theReliabilityDomain->setTagOfActiveLimitStateFunction(j);

		this->computeGradG(gFunValues(j-1),passed_x);
		gradG = this->getGradG();

		for (int i=1; i<=passed_x.Size(); i++) {
	
			(*grad_g_matrix)(i-1,j-1) = gradG(i-1);
		}
	}

	return 0;
}


 XC::Matrix 
XC::OpenSeesGradGEvaluator::getDgDdispl()
{
	return (*DgDdispl);
}

