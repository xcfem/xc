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
//IllConditioningAnalysis.cpp

#include <solution/analysis/analysis/IllConditioningAnalysis.h>
#include <solution/analysis/algorithm/eigenAlgo/KEigenAlgo.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/eigenSOE/EigenSOE.h>
#include <solution/analysis/numberer/DOF_Numberer.h>
#include <solution/analysis/handler/ConstraintHandler.h>
#include <solution/analysis/integrator/eigen/KEigenIntegrator.h>
#include <domain/domain/Domain.h>

//! @brief Constructor.
XC::IllConditioningAnalysis::IllConditioningAnalysis(SolutionStrategy *analysis_aggregation)
  :EigenAnalysis(analysis_aggregation) {}

//! @brief Sets the algorithm to use in the analysis.
int XC::IllConditioningAnalysis::setAlgorithm(KEigenAlgo &theAlgo)
  { return EigenAnalysis::setAlgorithm(theAlgo); }

//! @brief Sets the integrator to use in the analysis.
int XC::IllConditioningAnalysis::setIntegrator(KEigenIntegrator &theIntegrator)
  { return EigenAnalysis::setIntegrator(theIntegrator); }

//! @brief Sets the system of eigenvalues to use in the analysis.
int XC::IllConditioningAnalysis::setEigenSOE(EigenSOE &theSOE)
  { return EigenAnalysis::setEigenSOE(theSOE); }

