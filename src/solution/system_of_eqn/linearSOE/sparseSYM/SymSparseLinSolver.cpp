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
// File: ~/system_of_eqn/linearSOE/symLinSolver/SymSparseLinSolver.C
//
// Written: Jun Peng  (junpeng@stanford.edu)
//          Prof. Kincho H. Law
//          Stanford University
// Created: 12/98
// Revision: A
//
// Description: This file contains the class definition for 
// SymSparseinSolver. It solves the XC::SymSparseLinSOE object by calling
// some "C" functions. The solver used here is generalized sparse
// solver. The user can choose three different ordering schema.
//
// What: "@(#) SymSparseLinSolver.C, revA"


#include "solution/system_of_eqn/linearSOE/sparseSYM/SymSparseLinSOE.h"
#include "solution/system_of_eqn/linearSOE/sparseSYM/SymSparseLinSolver.h"

extern "C" {
  #include "solution/system_of_eqn/linearSOE/sparseSYM/nmat.h"
  #include "solution/system_of_eqn/linearSOE/sparseSYM/FeStructs.h"
}



XC::SymSparseLinSolver::SymSparseLinSolver()
:LinearSOESolver(SOLVER_TAGS_SymSparseLinSolver),
 theSOE(0)
{
    // nothing to do.
}


int XC::SymSparseLinSolver::solve(void)
{ 
    if (theSOE == 0) {
	std::cerr << "WARNING XC::SymSparseLinSolver::solve(void)- ";
	std::cerr << " No XC::LinearSOE object has been set\n";
	return -1;
    }

    int      nblks = theSOE->nblks;
    int      *xblk = theSOE->xblk;
    int      *invp = theSOE->invp;
    double   *diag = theSOE->diag;
    double   **penv = theSOE->penv;
    int      *rowblks = theSOE->rowblks;
    OFFDBLK  **begblk = theSOE->begblk;
    OFFDBLK  *first = theSOE->first;

    int neq = theSOE->size;

    // check for quick return
    if (neq == 0)
	return 0;

    // first copy B into X

    for(int i=0; i<neq; i++)
      theSOE->getX(i)= theSOE->getB(i);
    double *Xptr = theSOE->getPtrX();

    if(theSOE->factored == false)
      {
        //factor the matrix
        //call the "C" function to do the numerical factorization.
        int factor;
	factor = pfsfct(neq, diag, penv, nblks, xblk, begblk, first, rowblks);
	if(factor > 0)
          {
	    std::cerr << "In XC::SymSparseLinSolver: error in factorization.\n";
	    return -1;
	  }
	theSOE->factored = true;
      }

    // do forward and backward substitution.
    // call the "C" function.

    pfsslv(neq, diag, penv, nblks, xblk, Xptr, begblk);

    // Since the X we get by solving AX=B is P*X, we need to reordering
    // the Xptr to ge the wanted X.

    std::vector<double> tempX(neq);

    for(int m=0; m<neq; m++)
      { tempX[m] = Xptr[invp[m]]; }
	
    for(int k=0; k<neq; k++)
      { Xptr[k] = tempX[k]; }
    return 0;
  }


int XC::SymSparseLinSolver::setSize(void)
  {
    // nothing to do
    return 0;
  }

//! @brief Sets the system of equations to solve.
bool XC::SymSparseLinSolver::setLinearSOE(LinearSOE *soe)
  {
    bool retval= false;
    SymSparseLinSOE *tmp= dynamic_cast<SymSparseLinSOE *>(soe);
    if(tmp)
      {
        theSOE= tmp;
        retval= true;
      }
    else
      std::cerr << getClassName() << "::setLinearSOE: not a suitable system of equations" << std::endl;
    return retval;
  }

bool XC::SymSparseLinSolver::setLinearSOE(SymSparseLinSOE &theLinearSOE)
  { return setLinearSOE(&theLinearSOE); }


int XC::SymSparseLinSolver::sendSelf(CommParameters &cp)
  {
    // doing nothing
    return 0;
  }


int XC::SymSparseLinSolver::recvSelf(const CommParameters &cp)
  {
    // nothing to do
    return 0;
  }




