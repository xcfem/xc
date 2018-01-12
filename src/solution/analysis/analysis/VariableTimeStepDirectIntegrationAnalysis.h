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
                                                                        
// $Revision: 1.3 $
// $Date: 2005/11/29 23:36:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/VariableTimeStepDirectIntegrationAnalysis.h,v $
                                                                        
                                                                        
#ifndef VariableTimeStepDirectIntegrationAnalysis_h
#define VariableTimeStepDirectIntegrationAnalysis_h

// File: ~/analysis/analysis/VariableTimeStepDirectIntegrationAnalysis.h
// 
// Written: fmk 
// Created: 10/00
// Revision: A
//
// Description: This file contains the class definition for 
// VariableTimeStepDirectIntegrationAnalysis. VariableTimeStepDirectIntegrationAnalysis 
// is a subclass of DirectIntegrationAnalysis. It is used to perform a 
// dynamic analysis on the FE\_Model using a direct integration scheme.  
//
// What: "@(#) VariableTimeStepDirectIntegrationAnalysis.h, revA"

#include <solution/analysis/analysis/DirectIntegrationAnalysis.h>

namespace XC {
class TransientIntegrator;
class ConvergenceTest;

//! @ingroup AnalysisType
//
//! @brief perform a dynamic analysis on the FE\_Model
//! using a direct integration scheme.
class VariableTimeStepDirectIntegrationAnalysis: public DirectIntegrationAnalysis
  {
  protected:
    virtual double determineDt(double dT, double dtMin, double dtMax, int Jd,ConvergenceTest *theTest);

    friend class ProcSolu;
    VariableTimeStepDirectIntegrationAnalysis(AnalysisAggregation *analysis_aggregation);
    Analysis *getCopy(void) const;
  public:

    int analyze(int numSteps, double dT, double dtMin, double dtMax, int Jd);
  };

//! @brief Virtual constructor.
inline Analysis *VariableTimeStepDirectIntegrationAnalysis::getCopy(void) const
  { return new VariableTimeStepDirectIntegrationAnalysis(*this); }
} // end of XC namespace

#endif

