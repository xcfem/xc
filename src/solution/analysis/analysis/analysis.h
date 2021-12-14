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
//analysis.h

//Headers for the analysis type.
#include "solution/analysis/analysis/StaticAnalysis.h"
#include "solution/analysis/analysis/DomainDecompositionAnalysis.h"
#include "solution/analysis/analysis/DirectIntegrationAnalysis.h"
#include "solution/analysis/analysis/LinearBucklingAnalysis.h"
#include "solution/analysis/analysis/IllConditioningAnalysis.h"
#include "solution/analysis/analysis/EigenAnalysis.h"
#include "solution/analysis/analysis/ModalAnalysis.h"
//#include "solution/analysis/analysis/SubdomainAnalysis.h"
//#include "solution/analysis/analysis/SubstructuringAnalysis.h"
#include "solution/analysis/analysis/TransientAnalysis.h"
#include "solution/analysis/analysis/VariableTimeStepDirectIntegrationAnalysis.h"

#ifdef _PARALLEL_PROCESSING
#include "solution/analysis/analysis/StaticDomainDecompositionAnalysis.h"
#include "solution/analysis/analysis/TransientDomainDecompositionAnalysis.h"
#endif
