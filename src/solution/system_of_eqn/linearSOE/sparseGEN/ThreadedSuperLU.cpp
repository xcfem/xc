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
// $Date: 2003/02/14 23:02:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/sparseGEN/ThreadedSuperLU.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/sparseGEN/ThreadedSuperLU.h
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for 
// ThreadedSuperLU. It solves the XC::FullGenLinSOE object by calling
// SuperLU_MT routines.
//
// What: "@(#) ThreadedSuperLU.h, revA"

#include <solution/system_of_eqn/linearSOE/sparseGEN/ThreadedSuperLU.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSOE.h>

XC::ThreadedSuperLU:: XC::ThreadedSuperLU(int numT, int perm, int relx, 
			     int panel, double thrsh) 
:SparseGenColLinSolver(SOLVER_TAGS_ThreadedSuperLU), perm_r(0),perm_c(0), 
 etree(0), sizePerm(0), relax(relx), permSpec(perm), panelSize(panel),
 thresh(thrsh), numThreads(numT), refact(NO), usepr(NO), fact(DOFACT), work(nullptr), lwork(0)
{
  refact = NO;
  fact = DOFACT;
  usepr = NO;
  work = nullptr;
  lwork = 0;
}


XC::ThreadedSuperLU::~ThreadedSuperLU(void)
  {
    if(lwork >= 0)
      {
        Destroy_SuperNode_SCP(&L);
        Destroy_CompCol_NCP(&U);
      }
    StatFree(&gStat);
  }

int
XC::ThreadedSuperLU::solve(void)
{
    if (theSOE == 0) {
	std::cerr << "WARNING XC::ThreadedSuperLU::solve(void)- ";
	std::cerr << " No XC::LinearSOE object has been set\n";
	return -1;
    }
    
    int n = theSOE->size;
    
    // check for XC::quick return
    if (n == 0)
	return 0;

    if (sizePerm == 0) {
	std::cerr << "WARNING XC::ThreadedSuperLU::solve(void)- ";
	std::cerr << " size for row and col permutations 0 - has setSize() been called?\n";
	return -1;
    }

    // first copy B into X
    double *Xptr = theSOE->X;
    double *Bptr = theSOE->B;
    for (int i=0; i<n; i++)
	*(Xptr++) = *(Bptr++);

    if (theSOE->factored == false) {
	// factor the matrix
	int info;

	StatInit(n, numThreads, &gStat);

	pdgstrf_init(numThreads, refact, panelSize, relax, thresh, usepr,
		     0.0, perm_c, perm_r, work, lwork, &A, &AC, 
		     &pdgstrf_options, &gStat);

	pdgstrf(&pdgstrf_options, &AC, perm_r, &L, &U, &gStat, &info);

	if (info != 0) {	
	   std::cerr << "WARNING XC::ThreadedSuperLU::solve(void)- ";
	   std::cerr << " Error " << info << " returned in factorization pdgstrf()\n";
	   return info;
	}

	refact = YES;	
	theSOE->factored = true;
    }	

    // do forward and backward substitution
    trans = NOTRANS;
    int info;
    dgstrs (trans, &L, &U, perm_r, perm_c, &B, &gStat, &info);    

    if (info != 0) {	
       std::cerr << "WARNING XC::ThreadedSuperLU::solve(void)- ";
       std::cerr << " Error " << info << " returned in substitution dgstrs()\n";
       return info;
    }

    return 0;
}




int XC::ThreadedSuperLU::setSize()
  {
    int n = theSOE->size;
    if (n > 0) {

      // create space for the permutation vectors 
      // and the elimination tree
      if (sizePerm < n) {

        perm_r.resize(n);
        perm_c.resize(n);
        etree.resize(n);

	sizePerm = n;
      }

      // initialisation
      StatAlloc(n, numThreads, panelSize, relax, &gStat);

      // create the SuperMatrixMT A	
      dCreate_CompCol_Matrix(&A, n, n, theSOE->nnz, theSOE->A, 
			     theSOE->rowA, theSOE->colStartA, 
			     NC, _D, GE);

      // obtain and apply column permutation to give SuperMatrixMT AC
      get_perm_c(permSpec, &A, perm_c);
      //      sp_preorder(refact, &A, perm_c, etree, &AC);

      // create the rhs SuperMatrixMT B 
      dCreate_Dense_Matrix(&B, n, 1, theSOE->X, n, DN, _D, GE);
	
      // set the refact variable to 'N' after first factorization with new_ size 
      // can set to 'Y'.
      refact = NO;

    } else if (n == 0)
	return 0;
    else
      {
	std::cerr << "WARNING XC::ThreadedSuperLU::setSize()";
	std::cerr << " - order of system <  0\n";
	return -1;	
      }
    return 0;
  }

int XC::ThreadedSuperLU::sendSelf(CommParameters &cp)
  {
    // nothing to do
    return 0;
  }

int XC::ThreadedSuperLU::recvSelf(const CommParameters &cp)
  {
    // nothing to do
    return 0;
  }

