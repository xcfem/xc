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

