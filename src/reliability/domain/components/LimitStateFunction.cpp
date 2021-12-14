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
                                                                        
// $Revision: 1.7 $
// $Date: 2003/10/27 23:04:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/components/LimitStateFunction.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/domain/components/LimitStateFunction.h>
#include <utility/matrix/Vector.h>
#include <string.h>
#include <classTags.h>

XC::LimitStateFunction::LimitStateFunction(int passedTag,const std::string &passedExpression)
  :ReliabilityDomainComponent(passedTag, LIMIT_STATE_FUNCTION)
  {
    originalExpression= passedExpression;
    expressionWithAddition= passedExpression;
    tokenizedExpression= passedExpression;
  }


const std::string &XC::LimitStateFunction::getExpression(void) const
  { return expressionWithAddition; }


const std::string &XC::LimitStateFunction::getTokenizedExpression(void) const
  { return tokenizedExpression; }

int XC::LimitStateFunction::addExpression(const std::string &addition)
  {
     expressionWithAddition+= addition;
     tokenizeIt(expressionWithAddition);
     return 0;
  }

int XC::LimitStateFunction::removeAddedExpression(void)
  {
    expressionWithAddition= originalExpression;
    tokenizeIt(expressionWithAddition);
    return 0;
  }


int XC::LimitStateFunction::tokenizeIt(const std::string &originalExpression)
  {
    // Also store the tokenized expression (with dollar signs in front of variable names)
    char *lsf_forTokenizing = new char[500];
    char separators[5] = "}{";
    char dollarSign[] = "$";
    strcpy(lsf_forTokenizing,originalExpression.c_str());
    char lsf_expression[500] = "";
    char *tokenPtr2 = strtok( lsf_forTokenizing, separators);
    while( tokenPtr2 != nullptr )
      {
    	if(strncmp(tokenPtr2, "a",1) == 0
    	   || strncmp(tokenPtr2, "b",1) == 0
    		|| strncmp(tokenPtr2, "c",1) == 0
    		|| strncmp(tokenPtr2, "d",1) == 0
    		|| strncmp(tokenPtr2, "e",1) == 0
    		|| strncmp(tokenPtr2, "f",1) == 0
    		|| strncmp(tokenPtr2, "g",1) == 0
    		|| strncmp(tokenPtr2, "h",1) == 0
    		|| strncmp(tokenPtr2, "i",1) == 0
    		|| strncmp(tokenPtr2, "j",1) == 0
    		|| strncmp(tokenPtr2, "k",1) == 0
    		|| strncmp(tokenPtr2, "l",1) == 0
    		|| strncmp(tokenPtr2, "m",1) == 0
    		|| strncmp(tokenPtr2, "n",1) == 0
    		|| strncmp(tokenPtr2, "o",1) == 0
    		|| strncmp(tokenPtr2, "p",1) == 0
    		|| strncmp(tokenPtr2, "q",1) == 0
    		|| strncmp(tokenPtr2, "r",1) == 0
    		|| strncmp(tokenPtr2, "s",1) == 0
    		|| strncmp(tokenPtr2, "t",1) == 0
    		|| strncmp(tokenPtr2, "u",1) == 0
    		|| strncmp(tokenPtr2, "v",1) == 0
    		|| strncmp(tokenPtr2, "w",1) == 0
    		|| strncmp(tokenPtr2, "x",1) == 0
    		|| strncmp(tokenPtr2, "y",1) == 0
    		|| strncmp(tokenPtr2, "z",1) == 0) {
    		strcat(lsf_expression, dollarSign);
    		strcat(lsf_expression, tokenPtr2);
    	  }
    	else
          { strcat(lsf_expression, tokenPtr2); }
    	tokenPtr2 = strtok( nullptr, separators);
      }
    delete [] lsf_forTokenizing;

    tokenizedExpression= std::string(lsf_expression);

    return 0;
  }
