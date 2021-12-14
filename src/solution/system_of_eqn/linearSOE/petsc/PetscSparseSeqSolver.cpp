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
                                                                        
// $Revision: 1.2 $
// $Date: 2005/05/18 19:26:59 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/petsc/PetscSparseSeqSolver.cpp,v $
                                                                        
// Written: fmk 
// Created: 04/05
                                                                        
// What: "@(#) PetscSparseSeqSolver.h, revA"

#include <solution/system_of_eqn/linearSOE/petsc/PetscSparseSeqSolver.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenRowLinSOE.h>
#include <utility/Timer.h>
#include "PetscSOE.h"

XC::PetscSparseSeqSolver::PetscSparseSeqSolver(KSPType meth, PCType pre)
  :SparseGenRowLinSolver(SOLVER_TAGS_PetscSparseSeqSolver), 
   rTol(PETSC_DEFAULT), aTol(PETSC_DEFAULT), dTol(PETSC_DEFAULT), maxIts(PETSC_DEFAULT)
{
  PetscInitialize(0, PETSC_NULL, (char *)0, PETSC_NULL);

  method = meth;
  preconditioner = pre;
}

XC::PetscSparseSeqSolver::PetscSparseSeqSolver(KSPType meth, PCType pre, double relTol, double absTol, double divTol, int maxIterations)
  :SparseGenRowLinSolver(SOLVER_TAGS_PetscSparseSeqSolver), 
   rTol(relTol), aTol(absTol), dTol(divTol), maxIts(maxIterations)
{
  PetscInitialize(0, PETSC_NULL, (char *)0, PETSC_NULL);

  method = meth;
  preconditioner = pre;
}


XC::PetscSparseSeqSolver::~PetscSparseSeqSolver(void)
  {
#if (PETSC_VERSION_MAJOR == 3) && (PETSC_VERSION_MINOR >= 2)
    MatDestroy(&A);
    VecDestroy(&x);
    VecDestroy(&b);
    KSPDestroy(&ksp);
#else
    MatDestroy(A);
    VecDestroy(x);
    VecDestroy(b);
    KSPDestroy(ksp);
#endif
  }



int XC::PetscSparseSeqSolver::solve(void)
  {
    PetscErrorCode ierr = KSPSolve(ksp, b, x); CHKERRQ(ierr); 
    return ierr;
  }


int XC::PetscSparseSeqSolver::getNumIterations(void)
  {
    PetscErrorCode ierr= KSPGetIterationNumber(ksp, &its);
    return ierr;
  }

//! @brief Set the system size.
int XC::PetscSparseSeqSolver::setSize()
  {
    //int nnz = theSOE->nnz;


    /* 
     * Call Petsc VecCreate & MatCreate; NOTE: using previously allocated storage
     * 
     */

    PetscBool flg;
    int n= theSOE->size;
    //Modified LCPT 6/9/2018
    PetscErrorCode ierr= PetscOptionsGetInt(PETSC_NULL,nullptr,"-n", &n, &flg); CHKERRQ(ierr);
    //End of modification
    const double *Xdata = theSOE->getPtrX();
    ierr = VecCreateSeqWithArray(PETSC_COMM_WORLD, n, n, Xdata, &x); CHKERRQ(ierr); //LCPT
    const double *Bdata = theSOE->getPtrB();
    ierr = VecCreateSeqWithArray(PETSC_COMM_WORLD, n, n, Bdata, &b); CHKERRQ(ierr);  //LCPT
    int *colA = theSOE->colA.getDataPtr();
    int *rowStartA = theSOE->rowStartA.getDataPtr();
    double *Adata = theSOE->A.getDataPtr();
    ierr = MatCreateSeqAIJWithArrays(PETSC_COMM_WORLD, n, n, rowStartA, colA,  Adata , &A);

    /* 
     * Create linear solver context
     */

    KSPCreate(PETSC_COMM_WORLD, &ksp);

    /* 
     *  Set operators. NOTE: matrix that defines the linear system
     *  also serves as the preconditioning matrix.
     */

    KSPSetOperators(ksp, A, A); //, DIFFERENT_NONZERO_PATTERN);

    /* 
     *  Set solution scheme & tolerances
     */

    ierr = KSPSetType(ksp, method); CHKERRQ(ierr); 
    ierr = KSPSetTolerances(ksp, rTol, aTol, dTol, maxIts); 

    /* 
     *  Set preconditioning scheme
     */

    KSPGetPC(ksp, &pc);
    ierr = PCSetType(pc,  preconditioner); CHKERRQ(ierr); 

    /* 
     *  Finally mark so that uses last solution as initial guess in each solution
     *    NOTE: maybe change this as another user supplied option
     */

    ierr = KSPSetInitialGuessNonzero(ksp, PETSC_TRUE); CHKERRQ(ierr); 

    return ierr;
  }

//! @brief Sets the system of equations to solve.
bool XC::PetscSparseSeqSolver::setLinearSOE(LinearSOE *soe)
  {
    bool retval= false;
    SparseGenRowLinSOE *tmp= dynamic_cast<SparseGenRowLinSOE *>(soe);
    if(tmp)
      {
        theSOE= tmp;
        retval= true;
      }
    else
      std::cerr << getClassName() << "::setLinearSOE: not a suitable system of equations" << std::endl;
    return retval;
  }

bool XC::PetscSparseSeqSolver::setLinearSOE(SparseGenRowLinSOE &theSystem)
{
  theSOE = &theSystem;
  return 0;
}


int
XC::PetscSparseSeqSolver::sendSelf(Communicator &comm)
{
    // nothing to do
    return 0;
}

int
XC::PetscSparseSeqSolver::recvSelf(const Communicator &comm)
{
    // nothing to do
    return 0;
}



