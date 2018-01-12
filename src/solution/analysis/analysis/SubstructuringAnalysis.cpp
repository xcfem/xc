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
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/SubstructuringAnalysis.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/Analysis/SubstructuringAnalysis.C
// 
// Written: fmk 
// Created: Tue Sept 17 16:34:47: 1996
// Revision: A
//
// Description: This file contains the class definition for 
// SubstructuringAnalysis. SubstructuringAnalysis is a subclass 
// of XC::DomainDecompositionAnalysis, it is used to perform the operations
// required of a domain decomposition substructuring method.
//
// What: "@(#) SubstructuringAnalysis.C, revA"

#include <solution/analysis/analysis/SubstructuringAnalysis.h>
#include <solution/analysis/handler/ConstraintHandler.h>
#include <solution/analysis/numberer/DOF_Numberer.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/system_of_eqn/linearSOE/DomainSolver.h>
#include "solution/AnalysisAggregation.h"
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include "domain/domain/subdomain/Subdomain.h"

//! @brief Constructor.
//! 
//! The constructor is responsible for ensuring a Substructuring solver is
//! passed in as an argument. The base class does the rest. For this reason
//! WE WILL FORGET THIS CLASS.
XC::SubstructuringAnalysis::SubstructuringAnalysis(Subdomain &the_Domain,DomainSolver &theSolver,AnalysisAggregation *s)
  :DomainDecompositionAnalysis(the_Domain,theSolver,s)
  {}

//! @brief Virtual constructor.
XC::Analysis *XC::SubstructuringAnalysis::getCopy(void) const
  { return new SubstructuringAnalysis(*this); }

//! @brief Performs the analysis.
int XC::SubstructuringAnalysis::analyze(void)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; does nothing and should not have been called\n";
    return -1;
  }

