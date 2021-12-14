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
** (C) Copyright 1999, The Regents of the University of California    **
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
** ****************************************************************** */
                                                                        
// $Revision: 1.6 $
// $Date: 2005/11/29 23:36:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/StaticAnalysis.h,v $
                                                                        
                                                                        
#ifndef StaticAnalysis_h
#define StaticAnalysis_h

// Written: fmk 
// Revision: A
//
// Description: This file contains the interface for the StaticAnalysis
// class. StaticAnalysis is a subclass of Analysis, it is used to perform 
// a static analysis on the FE\_Model.
//
// What: "@(#) StaticAnalysis.h, revA"

#include <solution/analysis/analysis/Analysis.h>

namespace XC {
class ConvergenceTest;

// AddingSensitivity:BEGIN ///////////////////////////////
#ifdef _RELIABILITY
class SensitivityAlgorithm;
#endif
// AddingSensitivity:END ///////////////////////////////

//! @ingroup AnalysisType
//
//! @brief performs a static analysis on the FE\_Model.
class StaticAnalysis: public Analysis
  {
  protected:
    int domainStamp;

// AddingSensitivity:BEGIN ///////////////////////////////
#ifdef _RELIABILITY
    SensitivityAlgorithm *theSensitivityAlgorithm;
#endif
// AddingSensitivity:END ///////////////////////////////

    int new_domain_step(int num_step);
    int check_domain_change(int num_step,int numSteps);
    int new_integrator_step(int num_step);
    int solve_current_step(int num_step);
    int compute_sensitivities_step(int num_step);
    int commit_step(int num_step);
    int run_analysis_step(int num_step,int numSteps);

    friend class SolutionProcedure;
    StaticAnalysis(SolutionStrategy *analysis_aggregation);
    Analysis *getCopy(void) const;
  public:

    ~StaticAnalysis(void);

    void clearAll(void);	    
    
    virtual int analyze(int numSteps);
    int initialize(void);
    int domainChanged(void);

    int setNumberer(DOF_Numberer &theNumberer);
    int setAlgorithm(EquiSolnAlgo &theAlgorithm);
    int setIntegrator(StaticIntegrator &theIntegrator);
    int setLinearSOE(LinearSOE &theSOE);
    int setConvergenceTest(ConvergenceTest &theTest);

    ConvergenceTest *getConvergenceTest(void);

    // AddingSensitivity:BEGIN ///////////////////////////////
#ifdef _RELIABILITY
    int setSensitivityAlgorithm(SensitivityAlgorithm *theSensitivityAlgorithm);
#endif
    // AddingSensitivity:END /////////////////////////////////
  };

//! @brief Virtual constructor.
inline Analysis *StaticAnalysis::getCopy(void) const
  { return new StaticAnalysis(*this); }
} // end of XC namespace

#endif
