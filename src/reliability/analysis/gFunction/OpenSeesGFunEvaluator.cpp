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
                                                                        
// $Revision: 1.9 $
// $Date: 2003/10/27 23:45:43 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/gFunction/OpenSeesGFunEvaluator.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/analysis/gFunction/OpenSeesGFunEvaluator.h>
#include <utility/matrix/Vector.h>
#include <reliability/domain/components/ReliabilityDomain.h>
#include <reliability/domain/components/LimitStateFunction.h>
#include <reliability/domain/components/RandomVariablePositioner.h>

#include <tcl.h>
#include <string.h>

#include <fstream>
#include <iomanip>
#include <iostream>
using std::ifstream;
using std::ios;
using std::setw;
using std::setprecision;

XC::OpenSeesGFunEvaluator::OpenSeesGFunEvaluator(Tcl_Interp *passedTclInterp,
					ReliabilityDomain *passedReliabilityDomain,
					const std::string &passedFileName)
:GFunEvaluator(passedTclInterp, passedReliabilityDomain)
{
	fileName= passedFileName;
	nsteps = 0;
	dt = 0.0;
	// (here the user has provided a file with the analysis commands)

	createRecorders();

}

XC::OpenSeesGFunEvaluator::OpenSeesGFunEvaluator(Tcl_Interp *passedTclInterp,
					ReliabilityDomain *passedReliabilityDomain,
					int p_nsteps, double p_dt)
:GFunEvaluator(passedTclInterp, passedReliabilityDomain)
{
	nsteps = p_nsteps;
	dt = p_dt;
	// (here the user has specified number of steps and possibly dt)

	createRecorders();
}

int
XC::OpenSeesGFunEvaluator::runGFunAnalysis(Vector x)
{
	// Zero out the response in the structural domain to make ready for next analysis
	char theRevertToStartCommand[10] = "reset";
	Tcl_Eval( theTclInterp, theRevertToStartCommand );


	// Put random variables into the structural domain according to the RandomVariablePositioners
	int numberOfRandomVariablePositioners = theReliabilityDomain->getNumberOfRandomVariablePositioners();
	RandomVariablePositioner *theRandomVariablePositioner;
	int rvNumber;
	int i;
	for ( i=1 ; i<=numberOfRandomVariablePositioners ; i++ )  {
		theRandomVariablePositioner = theReliabilityDomain->getRandomVariablePositionerPtr(i);
		rvNumber				= theRandomVariablePositioner->getRvNumber();
		theRandomVariablePositioner->update(x(rvNumber-1));
	}


	// Run the structural analysis according to user specified scheme
	double result = 0;
	if (dt==0.0 && nsteps==0 && !strcmp(fileName.c_str(),"0") ) {
		// Run up to max time in fFuncs
		std::cerr << "OpenSeesGFunEvaluator: The option -runToMaxTimeInGFun " << std::endl
			<< " is not yet implemented." << std::endl;
	}
	else if (dt==0.0 && nsteps==0) {
		// Read commands from file and execute them
		// (The reason for doing it like this is that we want to check
		// the flag returned by the analysis to see if it converged). 
		char theAnalyzeCommand[30];
		sprintf(theAnalyzeCommand,"[source %s]",fileName.c_str());
		Tcl_ExprDouble( theTclInterp, theAnalyzeCommand, &result);
	}
	else {
		// User has given "nsteps" and possibly "dt"
		char theAnalyzeCommand[30];
		if (dt == 0.0) {
			sprintf(theAnalyzeCommand,"[analyze %d]",nsteps);
			Tcl_ExprDouble( theTclInterp, theAnalyzeCommand, &result);
		}
		else {
			sprintf(theAnalyzeCommand,"[analyze %d %10.5f]", nsteps, dt);
			Tcl_ExprDouble( theTclInterp, theAnalyzeCommand, &result);
		}
	
	}

	return ((int)result);
}




int
XC::OpenSeesGFunEvaluator::tokenizeSpecials(const std::string &theExpression)
{
	// Set value of OpenSees finite element response quantities 
	// appearing in the limit-state function in the Tcl domain
	char buf[500]="";
	char tempchar[100]="";
	int i;
	double fileValue = 0.0;
	char tclAssignment[500]="";
	char temp[120];


	char separators[5] = "}{";
	char *lsf_forTokenizing = new char[1000];
	strcpy(lsf_forTokenizing,theExpression.c_str());
	const std::string dollarSign = "$";
	const std::string underscore = "_";
	char *tokenPtr = strtok( lsf_forTokenizing, separators);
	while ( tokenPtr != nullptr ) {

		strcpy(tempchar,tokenPtr);

		// If a nodal velocity is detected
		if ( strncmp(tokenPtr, "ud", 2) == 0) {

			// Get node number and dof number
			int nodeNumber, direction;
			sscanf(tempchar,"ud_%i_%i", &nodeNumber, &direction);

			// Assign value to the displacement quantity
			char tclAssignment[100];
			sprintf(tclAssignment,"set ud_%d_%d [nodeVel %d %d ]",nodeNumber,direction,nodeNumber,direction);
			Tcl_Eval( theTclInterp, tclAssignment);
		}
		// If a nodal displacement is detected
		else if ( strncmp(tokenPtr, "u", 1) == 0) {

			// Get node number and dof number
			int nodeNumber, direction;
			sscanf(tempchar,"u_%i_%i", &nodeNumber, &direction);

			// Assign value to the displacement quantity
			char tclAssignment[100];
			sprintf(tclAssignment,"set u_%d_%d [nodeDisp %d %d ]",nodeNumber,direction,nodeNumber,direction);
			Tcl_Eval( theTclInterp, tclAssignment);

		}
		else if ( strncmp(tokenPtr, "rec",3) == 0) {

			// Determine file name 
			int lineNum = 0;
			int colNum = 0;
			char *variableName= nullptr;
			if( strncmp(tokenPtr, "rec_node",8) == 0) {
				variableName = rec_node_occurrence(tempchar, false, lineNum, colNum);
			}
			else if ( strncmp(tokenPtr, "rec_element",11) == 0) {
				variableName = rec_element_occurrence(tempchar, false, lineNum, colNum);
			}
			char fileName[100];
			sprintf(fileName,"%s.out",variableName);

			// Open the file
			ifstream inputFile(fileName,ios::in);
			if (!inputFile) {
				std::cerr << "Could not open file with quantities for limit-state function." << std::endl;
			}

			// Count number of lines
			int numberOfLines = 0;
			while (inputFile.getline(buf,120)) {
				numberOfLines++;
			}
			
			// Close and re-open file
			inputFile.close();
			ifstream inputFile2(fileName,ios::in);
			
			// Now pick last line and the right column
			for (i=1; i<numberOfLines; i++) {
				inputFile2.getline(buf,120);
			}
			for (i=1; i<=colNum; i++) {
				inputFile2 >> temp;
			}
			fileValue = (double)atof(temp);
			if (fileValue == 0.0) {
				std::cerr << "ERROR: Could not find quantity in recorder file." << std::endl;
				return -1;
			}
			inputFile2.close();


			// Make the variable assignment
			sprintf(tclAssignment, "set %s  %25.20e", variableName, fileValue);
			Tcl_Eval( theTclInterp, tclAssignment);

			delete variableName;

		}

		tokenPtr = strtok( nullptr, separators);
	}

	delete [] lsf_forTokenizing;

	// Re-create possible recorders for subsequent analyses
	createRecorders();

	return 0;
}


void
XC::OpenSeesGFunEvaluator::setNsteps(int p_nsteps)
{
	nsteps = p_nsteps;
}

double
XC::OpenSeesGFunEvaluator::getDt()
{
	return dt;
}





int
XC::OpenSeesGFunEvaluator::createRecorders()
{


//	removeRecorders();



	// Download active limit-state function
	int lsf = theReliabilityDomain->getTagOfActiveLimitStateFunction();
	LimitStateFunction *theLimitStateFunction = theReliabilityDomain->getLimitStateFunctionPtr(lsf);
	std::string theExpression = theLimitStateFunction->getExpression();

	// Initial declarations
	char tempchar[100]="";
	char separators[5] = "}{";
	char *lsf_forTokenizing = new char[500];
	strcpy(lsf_forTokenizing,theExpression.c_str());

	// Go through the limit-state function
	char *tokenPtr = strtok( lsf_forTokenizing, separators);
	while ( tokenPtr != nullptr ) {

		strcpy(tempchar,tokenPtr);

		if ( strncmp(tokenPtr, "rec_node",8) == 0) {
			int dummy1, dummy2;
			rec_node_occurrence(tempchar, true, dummy1, dummy2);
		}
		else if ( strncmp(tokenPtr, "rec_element",11) == 0) {
			int dummy1, dummy2;
			rec_element_occurrence(tempchar, true, dummy1, dummy2);
		}

		tokenPtr = strtok( nullptr, separators);
	}

	delete [] lsf_forTokenizing;

	return 0;
}




int
XC::OpenSeesGFunEvaluator::removeRecorders()
{
	char tclAssignment[50] = "";
	sprintf(tclAssignment , "remove recorders");
	Tcl_Eval( theTclInterp, tclAssignment);

	return 0;
}

char *
XC::OpenSeesGFunEvaluator::rec_node_occurrence(char tempchar[100], bool createRecorder, int &line, int &col)
{
	// Initial declarations
	char tclAssignment[500]="";


	// Get node and dof number etc.
	int nodeNumber, direction;
	char dispOrWhat[10];
	char restString[100];
	sscanf(tempchar,"rec_node_%s",restString);
	if ( strncmp(restString, "disp",4) == 0) {
		// recorder XC::Node fileName.out disp  -time -node 7 -dof 1  {rec_node_disp_7_1}
		strcpy(dispOrWhat,"disp");
		sscanf(restString,"disp_%i_%i", &nodeNumber, &direction);
	}
	else if ( strncmp(restString, "vel",3) == 0) {
		// recorder XC::Node fileName.out vel   -time -node 7 -dof 1  {rec_node_vel_7_1}
		strcpy(dispOrWhat,"vel");
		sscanf(restString,"vel_%i_%i", &nodeNumber, &direction);
	}
	else if ( strncmp(restString, "accel",5) == 0) {
		// recorder XC::Node fileName.out accel -time -node 7 -dof 1  {rec_node_accel_7_1}
		strcpy(dispOrWhat,"accel");
		sscanf(restString,"accel_%i_%i", &nodeNumber, &direction);
	}
	else {
		std::cerr << "ERROR in syntax of limit-state function with recorder." << std::endl;
	}

	// Determine variable name 
	char *variableName;
	char tempString[100];
	sprintf(tempString, "rec_node_%s_%d_%d", dispOrWhat, nodeNumber, direction);
	variableName = new char[100];
	strcpy(variableName,tempString);

	if (createRecorder) {

		// Create the recorder
		sprintf(tclAssignment , "recorder XC::Node %s.out %s -time -node %d -dof %d",variableName,dispOrWhat,nodeNumber,direction);
		Tcl_Eval( theTclInterp, tclAssignment);

		// Possibly create DDM gradient recorders				
		sprintf(tclAssignment , "recorder NodeGrad %s_ddm.out %s -time -node %d -dof %d",variableName,dispOrWhat,nodeNumber,direction);
//		Tcl_Eval( theTclInterp, tclAssignment);

		delete variableName;
		return 0;
	}

	line = 0;
	col = 2;
	return variableName;
}

char *
XC::OpenSeesGFunEvaluator::rec_element_occurrence(char tempchar[100], bool createRecorder, int &line, int &col)
{

	// Initial declarations
	char tclAssignment[500]="";
	char restString[100];
	char tempString[100];

	// Start obtaining information about element number etc. 
	int eleNumber;
	sscanf(tempchar,"rec_element_%i_%s", &eleNumber, restString);

	if ( strncmp(restString, "globalForce",11) == 0) {
		// recorder XC::Element 5 fileName.out -time globalForce {rec_element_5_globalForce_2}
		int globalForceComponent;
		sscanf(restString,"globalForce_%i", &globalForceComponent);
		sprintf(tempString, "rec_element_%d_globalForce_%d", eleNumber, globalForceComponent);
		line = 0;
		col = globalForceComponent+1;
		if (createRecorder) {
			sprintf(tclAssignment , "recorder XC::Element %d -file %s.out -time globalForce",eleNumber, tempString);
			Tcl_Eval( theTclInterp, tclAssignment);
		}
	}
	else if ( strncmp(restString, "localForce",10) == 0) {
		// recorder XC::Element 5 fileName.out -time localForce {rec_element_5_localForce_2}
		int localForceComponent;
		sscanf(restString,"localForce_%i", &localForceComponent);
		sprintf(tempString, "rec_element_%d_localForce_%d", eleNumber, localForceComponent);
		line = 0;
		col = localForceComponent+1;
		if (createRecorder) {
			sprintf(tclAssignment , "recorder XC::Element %d -file %s.out -time localForce",eleNumber, tempString);
			Tcl_Eval( theTclInterp, tclAssignment);
		}
	}
	else if ( strncmp(restString, "section",7) == 0) {
		int sectionNumber;
		sscanf(restString,"section_%i_%s", &sectionNumber, restString);
		if ( strncmp(restString, "force",5) == 0) {
			// recorder XC::Element 5 fileName.out -time section 2 force {rec_element_5_section_2_force_1}
			int sectionForceComponent;
			sscanf(restString,"force_%i", &sectionForceComponent);
			sprintf(tempString, "rec_element_%d_section_%d_force_%d", eleNumber, sectionNumber, sectionForceComponent);
			line = 0;
			col = sectionForceComponent+1;
			if (createRecorder) {
				sprintf(tclAssignment , "recorder XC::Element %d -file %s.out -time section %d force",eleNumber, tempString, sectionNumber);
				Tcl_Eval( theTclInterp, tclAssignment);
			}
		}
		else if ( strncmp(restString, "deformation",11) == 0) {
			// recorder XC::Element 5 fileName.out -time section 2 deformation {rec_element_5_section_2_deformation_1}
			int sectionDeformationComponent;
			sscanf(restString,"deformation_%i", &sectionDeformationComponent);
			sprintf(tempString, "rec_element_%d_section_%d_deformation_%d", eleNumber, sectionNumber, sectionDeformationComponent);
			line = 0;
			col = sectionDeformationComponent+1;
			if (createRecorder) {
				sprintf(tclAssignment , "recorder XC::Element %d -file %s.out -time section %d deformation",eleNumber, tempString, sectionNumber);
				Tcl_Eval( theTclInterp, tclAssignment);
			}
		}
		else if ( strncmp(restString, "stiffness",9) == 0) {
			// recorder XC::Element 5 fileName.out -time section 2 stiffness {rec_element_5_section_2_stiffness_1}
			int sectionStiffnessComponent;
			sscanf(restString,"stiffness_%i", &sectionStiffnessComponent);
			sprintf(tempString, "rec_element_%d_section_%d_stiffness_%d", eleNumber, sectionNumber, sectionStiffnessComponent);
			line = 0;
			col = sectionStiffnessComponent+1;
			if (createRecorder) {
				sprintf(tclAssignment , "recorder XC::Element %d -file %s.out -time section %d stiffness",eleNumber, tempString, sectionNumber);
				Tcl_Eval( theTclInterp, tclAssignment);
			}
		}
		else if ( strncmp(restString, "fiber",5) == 0) {
			// recorder XC::Element 5 fileName.out -time section 2 fiber $y $z stressStrain {rec_element_5_section_2_fiber_2_36_6_54_stress}
			int ya, yb, za, zb;
			sscanf(restString,"fiber_%i_%i_%i_%i_%s", &ya, &yb, &za, &zb, restString);
			sprintf(tempString, "rec_element_%d_section_%d_fiber_%d_%d_%d_%d_stressStrain", eleNumber, sectionNumber, ya, yb, za, zb);
			line = 0;
			if (createRecorder) {
				sprintf(tclAssignment , "recorder XC::Element %d -file %s.out -time section %d fiber %d.%d %d.%d stressStrain",eleNumber, tempString, sectionNumber, ya, yb, za, zb);
				Tcl_Eval( theTclInterp, tclAssignment);
			}
			if (strcmp(restString,"stress")==0) {
				col = 2;
			}
			else if (strcmp(restString,"strain")==0) {
				col = 3;
			}
			else {
				std::cerr << "ERROR in syntax of limit-state function with recorder." << std::endl;
			}
		}
	}
	else {
		std::cerr << "ERROR in syntax of limit-state function with recorder." << std::endl;
	}


	char *variableName;
	variableName = new char[100];
	strcpy(variableName,tempString);
	return variableName;
}
