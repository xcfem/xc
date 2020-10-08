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
// $Date: 2003/04/02 22:02:53 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSolver.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSolver.C
//
// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the class definition for 
// UmfpackGenLinSolver. It solves the UmfpackGenLinSOEobject by calling
// UMFPACK2.2.1 routines.
//
// What: "@(#) UmfpackGenLinSolver.C, revA"

#include <solution/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSOE.h>
#include <solution/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSolver.h>
#include <f2c.h>

extern "C" int umd21i_(int *keep, double *cntl, int *icntl);


XC::UmfpackGenLinSolver::UmfpackGenLinSolver()
:LinearSOESolver(SOLVER_TAGS_UmfpackGenLinSolver),
 copyIndex(0), lIndex(0), work(0), theSOE(nullptr)
  {
    // perform the initialisation needed in UMFpack
    umd21i_(keep, cntl, icntl);
  }

XC::LinearSOESolver *XC::UmfpackGenLinSolver::getCopy(void) const
   { return new UmfpackGenLinSolver(*this); }

extern "C" int umd2fa_(const int *n, int *ne, int *job, logical *transa,
		       int *lvalue, int *lindex, double *value,
		       int *index, int *keep, double *cntl, int *icntl,
		       int *info, double *rinfo);

extern "C" int umd2so_(const int *n, int *job, logical *transa,
		       int *lvalue, int *lindex, double *value,
		       int *index, int *keep, double *b, double *x, 
		       double *w, double *cntl, int *icntl,
		       int *info, double *rinfo);

int XC::UmfpackGenLinSolver::solve(void)
  {
    if(!theSOE)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING no LinearSOE object has been set"
	          << std::endl;
	return -1;
      }
    
    const int n = theSOE->size;
    int ne = theSOE->nnz;
    int lValue = theSOE->lValue;

    // check for quick return
    if (n == 0)
	return 0;

    // first copy B into X
    double *Xptr = theSOE->getPtrX();
    double *Bptr = theSOE->getPtrB();
    double *Aptr = theSOE->A.getDataPtr();

    int job =0; // set to 1 if wish to do iterative refinement
    logical trans = FALSE_;

    if(theSOE->factored == false)
      {
        // make a copy of index
        for(int i=0; i<2*ne; i++)
          { copyIndex[i] = theSOE->index[i]; }

      // factor the matrix
      umd2fa_(&n, &ne, &job, &trans, &lValue, &lIndex, Aptr,
	      copyIndex.getDataPtr(), keep, cntl, icntl, info, rinfo);
      
      if (info[0] != 0) {	
	std::cerr << "WARNING UmfpackGenLinSolver::solve(void)- ";
	std::cerr << info[0] << " returned in factorization UMD2FA()\n";
	return -info[0];
      }
      theSOE->factored = true;
    }	

    // do forward and backward substitution
    umd2so_(&n, &job, &trans, &lValue, &lIndex, Aptr, copyIndex.getDataPtr(), 
	    keep, Bptr, Xptr, work.getDataPtr(), cntl, icntl, info, rinfo);

    if (info[0] != 0) {	
       std::cerr << "WARNING UmfpackGenLinSolver::solve(void)- ";
       std::cerr << info[0] << " returned in substitution dgstrs()\n";
       return -info[0];
    }

    return 0;
}


int XC::UmfpackGenLinSolver::setSize()
  {
    int n = theSOE->size;
    int ne = theSOE->nnz;
    if(n > 0)
      {
        work.resize(4*n);

        lIndex = 37*n + 4*ne + 10;
        copyIndex= ID(lIndex);
      }	
    return 0;
  }

//! @brief Sets the system of equations to solve.
bool XC::UmfpackGenLinSolver::setLinearSOE(LinearSOE *soe)
  {
    bool retval= false;
    UmfpackGenLinSOE *tmp= dynamic_cast<UmfpackGenLinSOE *>(soe);
    if(tmp)
      {
        theSOE= tmp;
        retval= true;
      }
    else
      std::cerr << getClassName() << "::setLinearSOE: not a suitable system of equations" << std::endl;
    return retval;
  }

bool XC::UmfpackGenLinSolver::setLinearSOE(UmfpackGenLinSOE &theLinearSOE)
  { return setLinearSOE(&theLinearSOE); }

int XC::UmfpackGenLinSolver::sendSelf(Communicator &comm)
  {
    // nothing to do
    return 0;
  }

int XC::UmfpackGenLinSolver::recvSelf(const Communicator &comm)
  {
    // nothing to do
    return 0;
  }

