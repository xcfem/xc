//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
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


int XC::PetscSparseSeqSolver::setSize()
  {
    //int nnz = theSOE->nnz;


    /* 
     * Call Petsc VecCreate & MatCreate; NOTE: using previously allocated storage
     * 
     */

    PetscBool flg;
    int n = theSOE->size;
    PetscErrorCode ierr= PetscOptionsGetInt(PETSC_NULL,"-n", &n, &flg); CHKERRQ(ierr);
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

//! @brief Asigna el sistema de ecuaciones a resolver.
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
      std::cerr << nombre_clase() << "::setLinearSOE: el sistema de ecuaciones no es del tipo adecuado para este solver." << std::endl;
    return retval;
  }

bool XC::PetscSparseSeqSolver::setLinearSOE(SparseGenRowLinSOE &theSystem)
{
  theSOE = &theSystem;
  return 0;
}


int
XC::PetscSparseSeqSolver::sendSelf(CommParameters &cp)
{
    // nothing to do
    return 0;
}

int
XC::PetscSparseSeqSolver::recvSelf(const CommParameters &cp)
{
    // nothing to do
    return 0;
}



