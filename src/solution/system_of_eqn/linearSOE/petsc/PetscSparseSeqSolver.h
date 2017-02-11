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
// $Date: 2005/05/18 19:26:59 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/petsc/PetscSparseSeqSolver.h,v $
                                                                        
// Written: fmk 
// Created: 04/05

//
// Description: This file contains the class definition for 
// PetscSparseSeqSolver. It solves the SparseGenLinSOE object by calling Petsc routines
//
// What: "@(#) PetscSparseSeqSolver.h, revA"

#ifndef PetscSparseSeqSolver_h
#define PetscSparseSeqSolver_h

#include <petscksp.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenRowLinSolver.h>

#define PetscMinRelTol 1.0e-6

namespace XC {
class SparseGenRowLinSOE;

//! @ingroup Solver
//
//! @brief <a href="http://www.mcs.anl.gov/petsc/petsc-as" target="_new"> PETSC</a> based sparse general matrix SOE solver.
class PetscSparseSeqSolver: public SparseGenRowLinSolver
  {
  private:
    KSP ksp;                      // linear solver context
    PC  pc;
    int its;
    KSPType method;
    PCType  preconditioner;
    double rTol;
    double aTol;
    double dTol; 
    int maxIts;

    Mat A;
    Vec x, b;
  protected:
    SparseGenRowLinSOE *theSOE;

    PetscSparseSeqSolver(KSPType method, PCType preconditioner);    
    PetscSparseSeqSolver(KSPType method, PCType preconditioner, double rTol, double aTol, double dTol, int maxIts);    
    virtual LinearSOESolver *getCopy(void) const;
    virtual bool setLinearSOE(LinearSOE *theSOE);
  public:
    ~PetscSparseSeqSolver();

    int solve(void);
    int setSize(void);
    int getNumIterations(void);
    virtual bool setLinearSOE(SparseGenRowLinSOE &theSOE);
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

inline LinearSOESolver *PetscSparseSeqSolver::getCopy(void) const
   { return new PetscSparseSeqSolver(*this); }
} // end of XC namespace

#endif

