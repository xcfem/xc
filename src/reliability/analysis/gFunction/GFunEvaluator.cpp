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
                                                                    
// $Revision: 1.4 $
// $Date: 2003/10/27 23:45:43 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/gFunction/GFunEvaluator.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include "GFunEvaluator.h"
#include <reliability/domain/components/ReliabilityDomain.h>
#include <tcl.h>

#include <fstream>
#include <iomanip>
#include <iostream>
using std::ifstream;
using std::ios;
using std::setw;
using std::setprecision;

XC::GFunEvaluator::GFunEvaluator(Tcl_Interp *passedTclInterp, ReliabilityDomain *passedReliabilityDomain)
{
    theTclInterp = passedTclInterp;
    theReliabilityDomain = passedReliabilityDomain;
    numberOfEvaluations = 0;
}

double XC::GFunEvaluator::getG()
  { return g; }

int XC::GFunEvaluator::initializeNumberOfEvaluations()
  {
    numberOfEvaluations = 0;
    return 0;
  }

int XC::GFunEvaluator::getNumberOfEvaluations()
  { return numberOfEvaluations; }


int XC::GFunEvaluator::evaluateG(Vector x)
  {
    numberOfEvaluations++;


    // "Download" limit-state function from reliability domain
    int lsf = theReliabilityDomain->getTagOfActiveLimitStateFunction();
    LimitStateFunction *theLimitStateFunction = theReliabilityDomain->getLimitStateFunctionPtr(lsf);


    // Get the limit-state function expression
    std::string theExpression = theLimitStateFunction->getExpression();


    // Set value of GFun-specific parameters in the limit-state function
    this->tokenizeSpecials(theExpression);


    // Initial declarations
    int i;
    double fileValue = 0.0;

    char buf[500]="";
    char tclAssignment[500]="";
    char tempchar[100]="";
    char temp[120];

    char separators[5] = "}{";
    const std::string dollarSign = "$";
    const std::string underscore = "_";

    char lsf_forTokenizing[500];
    strcpy(lsf_forTokenizing,theExpression.c_str());


    // Set values of basic random variables and file quantities
    // (Other quantities will have to be set by specific implementations of this class)
    char *tokenPtr = strtok( lsf_forTokenizing, separators);
    while ( tokenPtr != nullptr ) {

            // Copy the token pointer over to a temporary storage
            strcpy(tempchar,tokenPtr);

            if ( strncmp(tokenPtr, "x",1) == 0) {
                    int rvNum;
                    sscanf(tempchar,"x_%i",&rvNum);
                    sprintf(tclAssignment , "set x_%d  %15.5f", rvNum, x(rvNum-1) );
                    Tcl_Eval( theTclInterp, tclAssignment);
            }
            else if ( strncmp(tokenPtr, "file",4) == 0) {
                    int rowNum = 0;
                    int colNum = 0;
                    char fileName[256];
                    sscanf(tempchar,"file_%s",fileName);
                    int rowloc = strcspn(fileName,"_");
                    char rowstr[10] = "";
                    int rowcnt = 0;
                    for (i=rowloc+1; fileName[i]!='\0'; i++) {
                            rowstr[rowcnt] = fileName[i];
                            rowcnt++;
                    }
                    rowstr[rowcnt] = '\0';
                    sscanf(rowstr,"%i_%i",&rowNum,&colNum);
                    fileName[rowloc] = '\0';

                    ifstream inputFile(fileName,ios::in);
                    if (!inputFile) {
                            std::cerr << "Could not open file with quantities for limit-state function." << std::endl;
                    }
                    for (i=1; i<rowNum; i++) {
                            inputFile.getline(buf,120);
                    }
                    for (i=1; i<=colNum; i++) {
                            inputFile >> temp;
                    }
                    fileValue = (double)atof(temp);
                    if (fileValue == 0.0) {
                            std::cerr << "ERROR: Could not find quantity in performance function file." << std::endl;
                            return -1;
                    }
                    inputFile.close();
                    sprintf(tclAssignment , "set file_%s_%d_%d  %15.5f",fileName,rowNum,colNum,fileValue);

                    Tcl_Eval( theTclInterp, tclAssignment);
            }
            
            tokenPtr = strtok( nullptr, separators);
    }

    // Compute value of g-function
    std::string theTokenizedExpression = theLimitStateFunction->getTokenizedExpression();
    g = 0.0;
    Tcl_ExprDouble( theTclInterp, theTokenizedExpression.c_str(), &g );


    return 0;
}






void XC::GFunEvaluator::setNsteps(int nsteps)
{
    std::cerr << "GFunEvaluator::set_nsteps() -- This method is not " << std::endl
            << " implemented for the chosen type of XC::GFunEvaluator." << std::endl;
}


double XC::GFunEvaluator::getDt()
  {
    std::cerr << "GFunEvaluator::getDt() -- This method is not " << std::endl
              << " implemented for the chosen type of XC::GFunEvaluator." << std::endl;
    return 0;
  }





