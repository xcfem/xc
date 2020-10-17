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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:16 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/domainDecompAlgo/DomainDecompAlgo.h,v $
                                                                        
                                                                        
// File: ~/OOP/analysis/algorithm/DomainDecompAlgo.h 
// 
// Written: fmk 
// Created: 10/96 
// Revision: A 
//

// Description: This file contains the class definition for 
// DomainDecompAlgo. DomainDecompAlgo is an abstract base class, 
// i.e. no objects of it's type can be created.  Its subclasses deifine
// the sequence of operations to be performed in the analysis by static
// equilibrium of a finite element model.  
// 
// What: "@(#)DomainDecompAlgo.h, revA"

#ifndef DomainDecompAlgo_h
#define DomainDecompAlgo_h

#include <solution/analysis/algorithm/SolutionAlgorithm.h>

namespace XC {
class AnalysisModel;
class IncrementalIntegrator;
class LinearSOE;
class DomainSolver;
class DomainDecompAnalysis;
class Subdomain;

//! @ingroup AnalAlgo
//
//! @brief Solution algorithm for domain decomposition analysis.
//!
//! The DomainDecompAlgo class is the SolutionAlgorithm subclass
//! used in a DomainDecompAnalysis. The class is responsible for updating
//! the DOF\_Group responses given the current solution to the interface
//! problem.
class DomainDecompAlgo: public SolutionAlgorithm
  {
  protected:
    friend class FEM_ObjectBroker;
    friend class SolutionStrategy;
    DomainDecompAlgo(SolutionStrategy *);
    SolutionAlgorithm *getCopy(void) const;
  public:
    // public functions defined for subclasses
    int solveCurrentStep(void);
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  };

inline SolutionAlgorithm *DomainDecompAlgo::getCopy(void) const
  { return new DomainDecompAlgo(*this); }

} // end of XC namespace

#endif


