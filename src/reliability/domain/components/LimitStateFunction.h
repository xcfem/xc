// -*-c++-*-
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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/10/27 23:04:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/components/LimitStateFunction.h,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#ifndef LimitStateFunction_h
#define LimitStateFunction_h

#include <reliability/domain/components/ReliabilityDomainComponent.h>
#include <utility/matrix/Vector.h>
#include <string>

namespace XC {
//! @ingroup ReliabilityAnalysis
// 
//! @brief Function that defines the failure of the structure.
//!
//! The limit state function for estimating the failure probability
//! may be defined as \f$g(X)= R(X) - L(X)\f$ where R is the structure
//! strength and L(X) is the total load over the structure. The limit
//! state function \f$g(X)\f$ defines a failure criterion that is
//! a function of all random variables X. 
class LimitStateFunction: public ReliabilityDomainComponent
  {
  private:
    int tokenizeIt(const std::string &expression);

    std::string originalExpression;
    std::string tokenizedExpression;
    std::string expressionWithAddition;
  public:
    // FORM: first order reliability analysis:
    double GFunValueAtStartPt;
    double GFunValueAtEndPt;
    double FORMReliabilityIndexBeta;
    double FORMProbabilityOfFailure_pf1;
    Vector designPoint_x_inOriginalSpace;
    Vector designPoint_u_inStdNormalSpace;
    Vector normalizedNegativeGradientVectorAlpha;
    Vector importanceVectorGamma;
    int numberOfStepsToFindDesignPointAlgorithm;
    
    // From Simulation analysis:
    double SimulationReliabilityIndexBeta;
    double SimulationProbabilityOfFailure_pfsim;
    double CoefficientOfVariationOfPfFromSimulation;
    int NumberOfSimulations;
    
    // From SORM analysis:
    double SORMCurvatureFittingBetaBreitung;
    double SORMCurvatureFittingPf2Breitung;
    double SORMPointFittingBetaBreitung;
    double SORMPointFittingPf2Breitung;
    double SORMUsingSearchBetaBreitung;
    double SORMUsingSearchPf2Breitung;
    Vector lastSearchDirection;
    int numberOfCurvatauresUsed;
    Vector secondLast_u;
    Vector secondLastAlpha;
  public:
    LimitStateFunction(int tag,const std::string &expression);

    // Method to get/add limit-state function
    const std::string &getExpression(void) const;
    const std::string &getTokenizedExpression(void) const;
    int addExpression(const std::string &expression);
    int removeAddedExpression(void);
  };
} // end of XC namespace

#endif
