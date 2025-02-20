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
// $Date: 2003/02/14 23:02:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectThreadSolver.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/ProfileSPD/ProfileSPDLinDirectThreadSolver.C
//
// Written: fmk 
// Created: Mar 1998
// Revision: A
//
// Description: This file contains the class definition for 
// ProfileSPDLinDirectThreadSolver. ProfileSPDLinDirectThreadSolver will solve
// a linear system of equations stored using the profile scheme using threads.
// It solves a XC::ProfileSPDLinSOE object using the LDL^t factorization and a block approach.

// What: "@(#) ProfileSPDLinDirectThreadSolver.C, revA"

#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectThreadSolver.h>
#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSOE.h>
#include <cmath>

#define _REENTRANT    /* basic 3-lines for threads */
// #include <pthread.h>
#include <thread.h>

#include <utility/Timer.h>


// global data that will be needed by the threads
struct thread_control_block {
  mutex_t start_mutex;
  mutex_t end_mutex;
  mutex_t startBlock_mutex;
  mutex_t endBlock_mutex;

  cond_t start_cond;
  cond_t end_cond;
  cond_t startBlock_cond;
  cond_t endBlock_cond;

  double *A,*X,*B;
  int *iDiagLoc;
  int size;
  int blockSize;
  int  maxColHeight;
  double minDiagTol;
  int *RowTop;
  double **topRowPtr, *invD;

  int currentBlock;
  int workToDo;
  int info;
  int threadID;
  int numThreads;
  int threadsDone;
  int threadsDoneBlock;
} TCB_ProfileSPDDirectThreadSolver;


void *ProfileSPDLinDirectThreadSolver_Worker(void *arg);

//! @brief Default constructor.
XC::ProfileSPDLinDirectThreadSolver::ProfileSPDLinDirectThreadSolver(void)
  :ProfileSPDLinDirectBase(SOLVER_TAGS_ProfileSPDLinDirectThreadSolver,1.0e-12),
 NP(2), running(false), blockSize(4), maxColHeight(0) {}

//! @brief Constructor.
//!
//! @param numProcessors: number of processors to use.
XC::ProfileSPDLinDirectThreadSolver::ProfileSPDLinDirectThreadSolver(int numProcessors, int blckSize, double tol) 
:ProfileSPDLinDirectBase(SOLVER_TAGS_ProfileSPDLinDirectThreadSolver,tol),
 NP(numProcessors), running(false), blockSize(blckSize), maxColHeight(0)
  {}

//! @brief Set system size.    
int XC::ProfileSPDLinDirectThreadSolver::setSize(void)
  {
    if(!theSOE)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__;
	std::cerr << " No system has been set\n";
	return -1;
      }

    // check for XC::quick return 
    if (theSOE->size == 0)
	return 0;
    if(size != theSOE->size)
      {    
        size = theSOE->size;
        RowTop= ID(size);
        topRowPtr= std::vector<double *>(size);
        invD= Vector(size); 
      }


    // set some pointers
    double *A = theSOE->A.getDataPtr();
    int *iDiagLoc = theSOE->iDiagLoc.getDataPtr();

    // set RowTop and topRowPtr info

    maxColHeight = 0;
    RowTop[0] = 0;
    topRowPtr[0] = A;
    for (int j=1; j<size; j++) {
	int icolsz = iDiagLoc[j] - iDiagLoc[j-1];
        if (icolsz > maxColHeight) maxColHeight = icolsz;
	RowTop[j] = j - icolsz +  1;
	topRowPtr[j] = &A[iDiagLoc[j-1]]; // FORTRAN array indexing in iDiagLoc
    }

    size = theSOE->size;
    return 0;
}


//! The solver first copies the B vector into X.
//! FILL IN
//! The solve process changes \f$A\f$ and \f$X\f$.   
int XC::ProfileSPDLinDirectThreadSolver::solve(void)
  {
    // check for XC::quick returns
    if (theSOE == 0) {
	std::cerr << getClassName() << "::" << __FUNCTION__;
	std::cerr << " - No ProfileSPDSOE has been assigned\n";
	return -1;
    }
    
    if (theSOE->size == 0)
	return 0;

    // set some pointers
    double *A = theSOE->A;
    double *B = theSOE->B;
    double *X = theSOE->X;
    int *iDiagLoc = theSOE->iDiagLoc;
    int size = theSOE->size;

    // copy B into X
    for (int ii=0; ii<size; ii++)
	X[ii] = B[ii];
    
    if (theSOE->isAfactored == false)  {
      // start threads running
      if (running == false) {
	mutex_init(&TCB_ProfileSPDDirectThreadSolver.start_mutex, USYNC_THREAD, 0);
	mutex_init(&TCB_ProfileSPDDirectThreadSolver.end_mutex, USYNC_THREAD, 0);
	mutex_init(&TCB_ProfileSPDDirectThreadSolver.startBlock_mutex, USYNC_THREAD, 0);
	mutex_init(&TCB_ProfileSPDDirectThreadSolver.endBlock_mutex, USYNC_THREAD, 0);
	cond_init(&TCB_ProfileSPDDirectThreadSolver.start_cond, USYNC_THREAD, 0);
	cond_init(&TCB_ProfileSPDDirectThreadSolver.end_cond, USYNC_THREAD, 0);
	cond_init(&TCB_ProfileSPDDirectThreadSolver.startBlock_cond, USYNC_THREAD, 0);
	cond_init(&TCB_ProfileSPDDirectThreadSolver.endBlock_cond, USYNC_THREAD, 0);
	TCB_ProfileSPDDirectThreadSolver.workToDo = 0;

        
	// Create the threads - Bound daemon threads
	for (int j = 0; j < NP; j++) 
	  thr_create(nullptr,0, ProfileSPDLinDirectThreadSolver_Worker, 
		     nullptr, THR_BOUND|THR_DAEMON, nullptr);

        running = true;
      }
      
  
      mutex_lock(&TCB_ProfileSPDDirectThreadSolver.start_mutex); 

	invD[0] = 1.0/A[0];	

        // assign the global variables the threads will need
        TCB_ProfileSPDDirectThreadSolver.A = A;
	TCB_ProfileSPDDirectThreadSolver.X = X;
	TCB_ProfileSPDDirectThreadSolver.B = B;
	TCB_ProfileSPDDirectThreadSolver.size = size;
	TCB_ProfileSPDDirectThreadSolver.minDiagTol = minDiagTol;
	TCB_ProfileSPDDirectThreadSolver.maxColHeight = maxColHeight;
	TCB_ProfileSPDDirectThreadSolver.RowTop = RowTop;
	TCB_ProfileSPDDirectThreadSolver.topRowPtr = topRowPtr;
	TCB_ProfileSPDDirectThreadSolver.invD = invD;
        TCB_ProfileSPDDirectThreadSolver.blockSize = blockSize;
        TCB_ProfileSPDDirectThreadSolver.iDiagLoc = iDiagLoc;
	
	TCB_ProfileSPDDirectThreadSolver.info = 0;
        TCB_ProfileSPDDirectThreadSolver.workToDo = NP;
        TCB_ProfileSPDDirectThreadSolver.currentBlock = -1;
	TCB_ProfileSPDDirectThreadSolver.threadID = 0;
	TCB_ProfileSPDDirectThreadSolver.numThreads = NP;
        TCB_ProfileSPDDirectThreadSolver.threadsDone = 0;
        TCB_ProfileSPDDirectThreadSolver.threadsDoneBlock = NP;

        // wake up the threads
        cond_broadcast(&TCB_ProfileSPDDirectThreadSolver.start_cond);
      mutex_unlock(&TCB_ProfileSPDDirectThreadSolver.start_mutex); 

      // yield the LWP
      thr_yield();
 
      // wait for all the threads to finish
      mutex_lock(&TCB_ProfileSPDDirectThreadSolver.end_mutex);

         while (TCB_ProfileSPDDirectThreadSolver.threadsDone < NP) 
           cond_wait(&TCB_ProfileSPDDirectThreadSolver.end_cond, &TCB_ProfileSPDDirectThreadSolver.end_mutex);

      mutex_unlock(&TCB_ProfileSPDDirectThreadSolver.end_mutex);


      theSOE->isAfactored = true;
	
      // divide by diag term 
      double *bjPtr = X; 
      double *aiiPtr = invD;
      for (int j=0; j<size; j++) 
	*bjPtr++ = *aiiPtr++ * X[j];

    
      // now do the back substitution storing result in X
      for (int k=(size-1); k>0; k--) {
      
	int rowktop = RowTop[k];
	double bk = X[k];
	double *ajiPtr = topRowPtr[k]; 		
      
	for (int j=rowktop; j<k; j++) 
	  X[j] -= *ajiPtr++ * bk;
      }   	 
    } else { // just do forward and back substitution
      
      // do forward substitution 
      for (int i=1; i<size; i++) {
	    
	int rowitop = RowTop[i];	    
	double *ajiPtr = topRowPtr[i];
	double *bjPtr  = &X[rowitop];  
	double tmp = 0;	    
	    
	for (int j=rowitop; j<i; j++) 
	  tmp -= *ajiPtr++ * *bjPtr++; 
	    
	X[i] += tmp;
      }

      // divide by diag term 
      double *bjPtr = X; 
      double *aiiPtr = invD;
      for (int j=0; j<size; j++) 
	*bjPtr++ = *aiiPtr++ * X[j];

    
      // now do the back substitution storing result in X
      for (int k=(size-1); k>0; k--) {
      
	int rowktop = RowTop[k];
	double bk = X[k];
	double *ajiPtr = topRowPtr[k]; 		

	for (int j=rowktop; j<k; j++) 
	  X[j] -= *ajiPtr++ * bk;
      }   	 
    }    
    return 0;
  }

int 
XC::ProfileSPDLinDirectThreadSolver::setProfileSOE(ProfileSPDLinSOE &theNewSOE)
{
    if (theSOE != 0) {
	std::cerr << getClassName() << "::" << __FUNCTION__ << "; ";
	std::cerr << " has already been called \n";	
	return -1;
    }
    
    theSOE = &theNewSOE;
    return 0;
}
	
int XC::ProfileSPDLinDirectThreadSolver::sendSelf(Communicator &comm)
{
    if (size != 0)
	std::cerr << "XC::ProfileSPDLinDirectThreadSolver::sendSelf - does not send itself YET\n"; 
    return 0;
}


int 
XC::ProfileSPDLinDirectThreadSolver::recvSelf(const Communicator &comm)
{
    return 0;
}



void *ProfileSPDLinDirectThreadSolver_Worker(void *arg)
{
  // Do this loop forever - or until all the Non-Daemon threads have exited
  while(true)
    {

      // Wait for some work to do
      mutex_lock(&TCB_ProfileSPDDirectThreadSolver.start_mutex);

        while (TCB_ProfileSPDDirectThreadSolver.workToDo == 0)
          cond_wait(&TCB_ProfileSPDDirectThreadSolver.start_cond, &TCB_ProfileSPDDirectThreadSolver.start_mutex);

        TCB_ProfileSPDDirectThreadSolver.workToDo--;

        // get an id;
	int myID = TCB_ProfileSPDDirectThreadSolver.threadID++;

      mutex_unlock(&TCB_ProfileSPDDirectThreadSolver.start_mutex);

      // do some initialization for the factorization
      double *A = TCB_ProfileSPDDirectThreadSolver.A;
      double *B = TCB_ProfileSPDDirectThreadSolver.B;
      double *X = TCB_ProfileSPDDirectThreadSolver.X;
      int *iDiagLoc = TCB_ProfileSPDDirectThreadSolver.iDiagLoc;
      int size = TCB_ProfileSPDDirectThreadSolver.size;
      double minDiagTol = TCB_ProfileSPDDirectThreadSolver.minDiagTol;
      int maxColHeight = TCB_ProfileSPDDirectThreadSolver.maxColHeight;
      int *RowTop = TCB_ProfileSPDDirectThreadSolver.RowTop;
      double **topRowPtr = TCB_ProfileSPDDirectThreadSolver.topRowPtr;
      double *invD = TCB_ProfileSPDDirectThreadSolver.invD;
      int blockSize = TCB_ProfileSPDDirectThreadSolver.blockSize;

      int currentBlock =0;
      int numThreads = TCB_ProfileSPDDirectThreadSolver.numThreads;


      // FACTOR 
      int startRow = 0;
      int lastRow = startRow+blockSize-1;
      int lastColEffected = lastRow+maxColHeight -1;
      int nBlck = size/blockSize;
      if ((size % blockSize) != 0)
	nBlck++;

      // for every block across      
      for (int i=0; i<nBlck; i++) {

        int currentDiagThread = i%numThreads;
        if (myID == currentDiagThread) {

	  // first factor the diagonal block int Ui,i and Di
          int j;
	  for (j=0; j<blockSize; j++) {
            int currentRow = startRow + j;

	    if (currentRow < size) { // this is for case when size%blockSize != 0

	      int rowjTop = RowTop[currentRow];
	      //	      double *akjPtr = &A[iDiagLoc[currentRow-1]];
	      double *akjPtr = topRowPtr[currentRow];
	      int maxRowijTop;
	      if (rowjTop < startRow) {
		akjPtr += startRow-rowjTop; // pointer to start of block row
		maxRowijTop = startRow;
	      } else
		maxRowijTop = rowjTop;

	      int k;
	      for (k=maxRowijTop; k<currentRow; k++) {
		double tmp = *akjPtr;
		int rowkTop = RowTop[k];
		int maxRowkjTop;
		double *alkPtr, *aljPtr;
		if (rowkTop < rowjTop) {
		  alkPtr = topRowPtr[k] + (rowjTop - rowkTop);
		  aljPtr = topRowPtr[currentRow];
		  maxRowkjTop = rowjTop;
		} else {
		  alkPtr = topRowPtr[k];
		  aljPtr = topRowPtr[currentRow] + (rowkTop - rowjTop);
		  maxRowkjTop = rowkTop;
		}

		for (int l = maxRowkjTop; l<k; l++) 
		  tmp -= *alkPtr++ * *aljPtr++;
		
		*akjPtr++ = tmp;
	      }

	      double ajj = *akjPtr;
	      akjPtr = topRowPtr[currentRow];
	      //	      akjPtr = &A[iDiagLoc[currentRow-1]];
	      double *bjPtr  = &X[rowjTop];  
	      double tmp = 0;	    

	      for (k=rowjTop; k<currentRow; k++){
		double akj = *akjPtr;
		double lkj = akj * invD[k];
		tmp -= lkj * *bjPtr++; 		
		*akjPtr++ = lkj;
		ajj = ajj -lkj * akj;
	      }

	      X[currentRow] += tmp;

	      // check that the diag > the tolerance specified
	      if (ajj <= 0.0) {
		std::cerr << getClassName() << "::" << __FUNCTION__ << "; ";
		std::cerr << " aii < 0 (i, aii): (" << currentRow << ", " << ajj << ")\n"; 
		TCB_ProfileSPDDirectThreadSolver.info = -2;
		return nullptr;
	      }
	      if (ajj <= minDiagTol) {
		std::cerr << getClassName() << "::" << __FUNCTION__ << "; ";
		std::cerr << " aii < minDiagTol (i, aii): (" << currentRow;
		std::cerr << ", " << ajj << ")\n"; 
		TCB_ProfileSPDDirectThreadSolver.info = -2;
		return nullptr;
	      }		
	      invD[currentRow] = 1.0/ajj; 

	    } else 
	      j = blockSize;

	  }

          // allow other threads to now proceed
          mutex_lock(&TCB_ProfileSPDDirectThreadSolver.startBlock_mutex);
	    TCB_ProfileSPDDirectThreadSolver.currentBlock = i;

            cond_broadcast(&TCB_ProfileSPDDirectThreadSolver.startBlock_cond);
          mutex_unlock(&TCB_ProfileSPDDirectThreadSolver.startBlock_mutex);        


	  // now do rest of i'th block row belonging to thread
	  // doing a block of columns at a time forming Ui,j*Di
	  int currentCol = startRow + blockSize;
	  for (j=i+1; j<nBlck; j++) {

	    if (j%numThreads == myID) {

	      for (int k=0; k<blockSize; k++) {

		if (currentCol < size) { // this is for case when size%blockSize != 0

		  int rowkTop = RowTop[currentCol];
		  double *alkPtr = topRowPtr[currentCol];
		  // double *alkPtr = &A[iDiagLoc[currentCol-1]];
		  int maxRowikTop;
		  if (rowkTop < startRow) {
		    alkPtr += startRow-rowkTop; // pointer to start of block row
		    maxRowikTop = startRow;
		  } else
		    maxRowikTop = rowkTop;

		  for (int l=maxRowikTop; l<=lastRow; l++) {
		    double tmp = *alkPtr;
		    int rowlTop = RowTop[l];
		    int maxRowklTop;
		    double *amlPtr, *amkPtr;
		    if (rowlTop < rowkTop) {
		      amlPtr = topRowPtr[l] + (rowkTop - rowlTop);
		      amkPtr = topRowPtr[currentCol];
		      maxRowklTop = rowkTop;
		    } else {
		      amlPtr = topRowPtr[l];
		      amkPtr = topRowPtr[currentCol] + (rowlTop - rowkTop);
		      maxRowklTop = rowlTop;
		    }
		  
		    for (int m = maxRowklTop; m<l; m++) 
		      tmp -= *amkPtr++ * *amlPtr++;
		  
		    *alkPtr++ = tmp;
		  }
		  currentCol++;
		  if (currentCol > lastColEffected) {
		    k = blockSize;
		    j = nBlck;
		  }
		  
		} else
		  k = blockSize;
	      }
	    } else
	      currentCol += blockSize;
	  }
	} else {
          // wait till diag i is done 
          mutex_lock(&TCB_ProfileSPDDirectThreadSolver.startBlock_mutex);
             while (TCB_ProfileSPDDirectThreadSolver.currentBlock < i)
                cond_wait(&TCB_ProfileSPDDirectThreadSolver.startBlock_cond, &TCB_ProfileSPDDirectThreadSolver.startBlock_mutex);
          mutex_unlock(&TCB_ProfileSPDDirectThreadSolver.startBlock_mutex);

          // FACTOR REST OF BLOCK ROW BELONGING TO THREAD
	  // now do rest of i'th block row belonging to thread
	  // doing a block of columns at a time forming Ui,j*Di
	  int currentCol = startRow + blockSize;
	  for (int j=i+1; j<nBlck; j++) {

	    if (j%numThreads == myID) {

	      for (int k=0; k<blockSize; k++) {

		if (currentCol < size) { // this is for case when size%blockSize != 0

		  int rowkTop = RowTop[currentCol];
		  double *alkPtr = topRowPtr[currentCol];
		  // double *alkPtr = &A[iDiagLoc[currentCol-1]];
		  int maxRowikTop;
		  if (rowkTop < startRow) {
		    alkPtr += startRow-rowkTop; // pointer to start of block row
		    maxRowikTop = startRow;
		  } else
		    maxRowikTop = rowkTop;

		  for (int l=maxRowikTop; l<=lastRow; l++) {
		    double tmp = *alkPtr;
		    int rowlTop = RowTop[l];
		    int maxRowklTop;
		    double *amlPtr, *amkPtr;
		    if (rowlTop < rowkTop) {
		      amlPtr = topRowPtr[l] + (rowkTop - rowlTop);
		      amkPtr = topRowPtr[currentCol];
		      maxRowklTop = rowkTop;
		    } else {
		      amlPtr = topRowPtr[l];
		      amkPtr = topRowPtr[currentCol] + (rowlTop - rowkTop);
		      maxRowklTop = rowlTop;
		    } 

		    for (int m = maxRowklTop; m<l; m++) 
		      tmp -= *amkPtr++ * *amlPtr++;
		  
		    *alkPtr++ = tmp;
		  }
		  currentCol++;
		  if (currentCol > lastColEffected) {
		    k = blockSize;
		    j = nBlck;
		  }
		  
		} else
		  k = blockSize;
	      }
	    } else
	      currentCol += blockSize;
	  
	  }

	}

	// update the data for the next block
	startRow += blockSize;
	lastRow = startRow + blockSize -1;
	lastColEffected = lastRow + maxColHeight -1;
      }

      // signal the XC::main thread that this thread is done with the work
      mutex_lock(&TCB_ProfileSPDDirectThreadSolver.end_mutex);
         TCB_ProfileSPDDirectThreadSolver.threadsDone++;

         cond_signal(&TCB_ProfileSPDDirectThreadSolver.end_cond);
      mutex_unlock(&TCB_ProfileSPDDirectThreadSolver.end_mutex);
    } 
}
