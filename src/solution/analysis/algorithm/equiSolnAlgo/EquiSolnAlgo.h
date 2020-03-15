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
                                                                        
// $Revision: 1.5 $
// $Date: 2005/11/29 22:42:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h,v $
                                                                        
                                                                        
#ifndef EquiSolnAlgo_h
#define EquiSolnAlgo_h

// File: ~/OOP/analysis/algorithm/EquiSolnAlgo.h 
// 
// Written: fmk 
// Created: 11/96 
// Revision: A 
//

// Description: This file contains the class definition for 
// EquiSolnAlgo. EquiSolnAlgo is an abstract base class, 
// i.e. no objects of it's type can be created.  Its subclasses deifine
// the sequence of operations to be performed in the analysis by static
// equilibrium of a finite element model.  
// 
// What: "@(#)EquiSolnAlgo.h, revA"

#include <solution/analysis/algorithm/SolutionAlgorithm.h>
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include <iostream>

namespace XC {
class AnalysisModel;
class LinearSOE;
class ConvergenceTest;

//! @ingroup EQSolAlgo
//
//! @brief EquiSolnAlgo is an abstract base class, 
//! i.e. no objects of it's type can be created. An
//! EquiSolnAlgo object defines the sequence of operations 
//! performed by the the Integrator and the LinearSOE objects in
//! solving the equilibrium equation \f$R(U) = 0\f$ given the current state of
//! the domain at each time step in a direct integration analysis or load
//! increment in a static analysis.
class EquiSolnAlgo: public SolutionAlgorithm
  {
  protected:
    EquiSolnAlgo(AnalysisAggregation *,int classTag);
  public:
    // virtual functions
    //! @brief steps taken in order to get the system into an
    //! equilibrium state.
    //! 
    //! A method implemented by each subclass which specifies the steps taken
    //! in order to get the system into an equilibrium state. It is a pure
    //! virtual function, i.e. all subclasses or their descendents must
    //! implement this routine. To return \f$0\f$ if algorithm succeeds, a negative
    //! value otherwise. 
    virtual int solveCurrentStep(void) =0;
    virtual ConvergenceTest *getConvergenceTestPtr(void);     
    virtual const ConvergenceTest *getConvergenceTestPtr(void) const;
    virtual void Print(std::ostream &, int flag =0) const;    

    // the following are not protected as convergence test
    // may need access to them
    IncrementalIntegrator *getIncrementalIntegratorPtr(void);
  };
} // end of XC namespace

#endif


