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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:00:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/domainDecompAlgo/DomainDecompAlgo.cpp,v $
                                                                        
                                                                        
// File: ~/OOP/analysis/algorithm/DomainDecompAlgo.C
// 
// Written: fmk 
// Created: Sun Sept 15 15:06:47: 1996 
// Revision: A 
//

// Description: This file contains the class definition for 
// DomainDecompAlgo. DomainDecompAlgo is an abstract base class, 
// i.e. no objects of it's type can be created.  Its subclasses deifine
// the sequence of operations to be performed in the analysis by static
// equilibrium of a finite element model.  
// 
// What: "@(#)DomainDecompAlgo.h, revA"

#include <solution/analysis/algorithm/domainDecompAlgo/DomainDecompAlgo.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/system_of_eqn/linearSOE/DomainSolver.h>
#include "domain/domain/subdomain/Subdomain.h"

//! @brief Constructor.
XC::DomainDecompAlgo::DomainDecompAlgo(SoluMethod *owr)
  :SolutionAlgorithm(owr,DomDecompALGORITHM_TAGS_DomainDecompAlgo) {}

//! @brief Solve current step.
int XC::DomainDecompAlgo::solveCurrentStep(void)
  {
    IncrementalIntegrator *theIntegrator= dynamic_cast<IncrementalIntegrator *>(getIntegratorPtr());
    LinearSOE *theLinearSOE= getLinearSOEPtr();
    DomainSolver *theSolver= getDomainSolverPtr();    
    Subdomain *theSubdomain= getSubdomainPtr();
    
    if(theIntegrator != 0 && theLinearSOE != 0 &&
	theSolver != 0 && theSubdomain != 0 )
      {
	const Vector &extResponse= theSubdomain->getLastExternalSysResponse();

	theSolver->setComputedXext(extResponse);
	theSolver->solveXint();

	theIntegrator->update(theLinearSOE->getX());
	return 0;
      }
    else
      {
	std::cerr << "XC::DomainDecompAlgo::solveCurrentStep() ";
	std::cerr << "no links have been set\n";
	return -1;
      }
  }

int XC::DomainDecompAlgo::sendSelf(CommParameters &cp)
  { return 0; }

int XC::DomainDecompAlgo::recvSelf(const CommParameters &cp)
  { return 0; }

