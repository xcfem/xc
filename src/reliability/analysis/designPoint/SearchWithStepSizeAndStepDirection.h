// -*-c++-*-
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
// $Date: 2003/10/27 23:45:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/designPoint/SearchWithStepSizeAndStepDirection.h,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu) 
//

#ifndef SearchWithStepSizeAndStepDirection_h
#define SearchWithStepSizeAndStepDirection_h

#include <reliability/analysis/designPoint/FindDesignPointAlgorithm.h>
#include <reliability/analysis/stepSize/StepSizeRule.h>
#include <reliability/analysis/direction/SearchDirection.h>
#include <reliability/analysis/transformation/ProbabilityTransformation.h>
#include <reliability/analysis/hessianApproximation/HessianApproximation.h>
#include <reliability/analysis/convergenceCheck/ReliabilityConvergenceCheck.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <reliability/domain/components/ReliabilityDomain.h>

#include <fstream>
using std::ofstream;

namespace XC {
  class GFunEvaluator;
  class GradGEvaluator;
//! @ingroup ReliabilityAnalysis
//!
//! @brief Design point search with step size and step direction.
class SearchWithStepSizeAndStepDirection: public FindDesignPointAlgorithm
  {
  private:
    // The reliability domain and tools for the analysis
    ReliabilityDomain *theReliabilityDomain;
    GFunEvaluator *theGFunEvaluator;
    GradGEvaluator *theGradGEvaluator;
    StepSizeRule *theStepSizeRule;
    SearchDirection *theSearchDirection;
    ProbabilityTransformation *theProbabilityTransformation;
    HessianApproximation *theHessianApproximation;
    ReliabilityConvergenceCheck *theReliabilityConvergenceCheck;

    // Private member functions to do the job
    int doTheActualSearch(bool doRvProjection);
    int doRvProjection(Vector uOld, Vector uNew);

    // Data members set when the object is created
    int maxNumberOfIterations;

    // Data members where the results are to be stored
    Vector x;
    Vector u;
    Vector alpha;
    Vector gradientInStandardNormalSpace;
    Vector gamma;
    Vector uSecondLast;
    Vector alphaSecondLast;
    int i;
    Vector searchDirection;
    double Gfirst;
    double Glast;

    // Data members set through the call when a job is to be done
    Vector *startPoint;
    Vector *designPoint_uStar;

    int printFlag;
    std::string fileNamePrint;
    int numberOfEvaluations;
  public:
    // Constructor and destructor
    SearchWithStepSizeAndStepDirection(
				    int passedMaxNumberOfIterations, 
				    GFunEvaluator *passedGFunEvaluator,
				    GradGEvaluator *passedGradGEvaluator,
				    StepSizeRule *passedStepSizeRule,
				    SearchDirection *passedSearchDirection,
				    ProbabilityTransformation *passedProbabilityTransformation,
				    HessianApproximation *theHessianApproximation,
				    ReliabilityConvergenceCheck *theReliabilityConvergenceCheck,
				    int printFlag,
				    const std::string &fileNamePrint,
				    Vector *startPoint);

    int findDesignPoint(ReliabilityDomain *theReliabilityDomain);

    const Vector &get_x(void) const;
    const Vector &get_u(void) const;
    const Vector &get_alpha(void) const;
    Vector get_gamma(void) const;
    int getNumberOfSteps(void) const;
    const Vector &getSecondLast_u(void) const;
    const Vector &getSecondLast_alpha(void) const;
    const Vector &getLastSearchDirection(void) const;
    double getFirstGFunValue(void) const;
    double getLastGFunValue(void) const;
    const Vector &getGradientInStandardNormalSpace(void) const;
    int    getNumberOfEvaluations(void) const;
  };
} // end of XC namespace

#endif
