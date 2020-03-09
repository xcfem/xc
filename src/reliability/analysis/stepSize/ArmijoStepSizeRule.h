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
// $Date: 2003/10/27 23:45:44 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/stepSize/ArmijoStepSizeRule.h,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#ifndef ArmijoStepSizeRule_h
#define ArmijoStepSizeRule_h

#include <reliability/analysis/stepSize/StepSizeRule.h>
#include <reliability/analysis/transformation/ProbabilityTransformation.h>
#include <reliability/analysis/meritFunction/MeritFunctionCheck.h>
#include <reliability/analysis/rootFinding/RootFinding.h>

namespace XC {
class GFunEvaluator;
class GradGEvaluator;

//! @ingroup ReliabilityAnalysis Objects to perform reliability analisys.
//!
//! @brief Implementation of Armijo rule inexact line search method.
//!
//! The armijo rule is employed to determine the step size as:
//! \f$\lambda= b^k\f$. The user selects a value0.0< b <1.0, while
//! \f$k\f$ is an integer with initial value 0. \f$k\f$ increases by unit
//! steps until an acceptable step size is found.
class ArmijoStepSizeRule: public StepSizeRule
  {
  private:
    double stepSize;
    GFunEvaluator *theGFunEvaluator;
    GradGEvaluator *theGradGEvaluator;
    ProbabilityTransformation *theProbabilityTransformation;
    MeritFunctionCheck *theMeritFunctionCheck;
    RootFinding *theRootFindingAlgorithm;
    double gFunValue;
    double base;
    int maxNumReductions;
    double b0;
    int numberOfShortSteps;
    double radius;
    double surfaceDistance;
    double evolution;
    bool isCloseToSphere;
    int printFlag;

  public:
	ArmijoStepSizeRule(GFunEvaluator *passedGFunEvaluator, 
		ProbabilityTransformation *theProbabilityTransformation,
		MeritFunctionCheck *theMeritFunctionCheck,
		RootFinding *theRootFindingAlgorithm, 
		double base,
		int maxNumReductions,
		double b0,
		int numberOfShortSteps,
		double radius,
		double surfaceDistance,
		double evolution,
		int printFlag);
		
    int computeStepSize(Vector u, Vector grad_G, double G, Vector d, int stepNumber);
    double getStepSize();
    double getInitialStepSize();
    double getGFunValue();
  };
} // end of XC namespace

#endif
