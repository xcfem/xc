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
// $Date: 2003/02/14 23:02:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectBlockSolver.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/ProfileSPD/ProfileSPDLinDirectBlockSolver.C
//
// Written: fmk 
// Created: Mar 1998
// Revision: A
//
// Description: This file contains the class definition for 
// ProfileSPDLinDirectBlockSolver. ProfileSPDLinDirectBlockSolver is a subclass 
// of LinearSOESOlver. It solves a XC::ProfileSPDLinSOE object using
// the LDL^t factorization and a block approach

// What: "@(#) ProfileSPDLinDirectBlockSolver.C, revA"

#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectBlockSolver.h>
#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSOE.h>
#include <cmath>

#include <utility/Timer.h>

XC::ProfileSPDLinDirectBlockSolver::ProfileSPDLinDirectBlockSolver(double tol, int blckSize)
  :ProfileSPDLinDirectBase(SOLVER_TAGS_ProfileSPDLinDirectBlockSolver,tol), blockSize(blckSize), maxColHeight(0) {}

    

int XC::ProfileSPDLinDirectBlockSolver::setSize(void)
  {

    if(theSOE == 0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__;
	std::cerr << " No system has been set\n";
	return -1;
      }

    // check for quick return 
    if(theSOE->size == 0)
	return 0;
    if(size != theSOE->size)
      {    
        size = theSOE->size;
        RowTop= ID(size);
        topRowPtr= std::vector<double *>(size);
        invD= Vector(size);
      }


    // set some pointers
    double *A= theSOE->A.getDataPtr();
    int *iDiagLoc = theSOE->iDiagLoc.getDataPtr();

    // set RowTop and topRowPtr info

    maxColHeight = 0;
    RowTop(0)= 0;
    topRowPtr[0] = A;
    for(int j=1; j<size; j++)
      {
	int icolsz= iDiagLoc[j] - iDiagLoc[j-1];
        if (icolsz > maxColHeight) maxColHeight = icolsz;
	RowTop(j) = j - icolsz +  1;
	topRowPtr[j] = &A[iDiagLoc[j-1]]; // FORTRAN array indexing in iDiagLoc
      }
    size = theSOE->size;
    return 0;
  }


int XC::ProfileSPDLinDirectBlockSolver::solve(void)
  {
    // check for quick returns
    if(theSOE == 0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__;
	std::cerr << " - No ProfileSPDSOE has been assigned\n";
	return -1;
      }
    
    if(theSOE->size == 0)
      return 0;

    // set some pointers
    double *B = theSOE->getPtrB();
    double *X = theSOE->getPtrX();
    int n = theSOE->size;

    // copy B into X
    for (int ii=0; ii<n; ii++)
	X[ii] = B[ii];
    
    if(theSOE->factored == false)
      {
	// FACTOR 
	invD[0] = 1.0/theSOE->A[0];	
        int startRow = 0;
	int lastRow = startRow+blockSize-1;
        int lastColEffected = lastRow+maxColHeight -1;
        int nBlck = n/blockSize;
	if ((n % blockSize) != 0)
	  nBlck++;

	// for every block across      
	for (int i=0; i<nBlck; i++) {

	  // first factor the diagonal block int Ui,i and Di
          int j;
	  for (j=0; j<blockSize; j++) {
            int currentRow = startRow + j;

	    if (currentRow < n) { // this is for case when size%blockSize != 0

	      int rowjTop= RowTop(currentRow);
	      double *akjPtr= topRowPtr[currentRow];
	      int maxRowijTop;
	      if(rowjTop < startRow)
                {
		  akjPtr += startRow-rowjTop; // pointer to start of block row
		  maxRowijTop = startRow;
	        }
              else
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
		return(-2);
	      }
	      if (ajj <= minDiagTol) {
		std::cerr << getClassName() << "::" << __FUNCTION__ << "; ";
		std::cerr << " aii < minDiagTol (i, aii): (" << currentRow;
		std::cerr << ", " << ajj << ")\n"; 
		return(-2);
	      }		
	      invD[currentRow] = 1.0/ajj; 

	    } else 
	      j = blockSize;

	  }

	  // now do rest of i'th block row doing a block of columns at a time
          // forming Ui,j*Di
	  int currentCol = startRow + blockSize;
	  for (j=i+1; j<nBlck; j++) 
	    for (int k=0; k<blockSize; k++) {

	      if (currentCol < n) { // this is for case when size%blockSize != 0

		int rowkTop = RowTop[currentCol];
		double *alkPtr = topRowPtr[currentCol];
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

	  startRow += blockSize;
	  lastRow = startRow + blockSize -1;
          lastColEffected = lastRow + maxColHeight -1;
	}

	theSOE->factored = true;
	theSOE->numInt = 0;
	
	// divide by diag term 
	double *bjPtr = X; 
	double *aiiPtr= invD.getDataPtr();
	for (int j=0; j<n; j++) 
	  *bjPtr++ = *aiiPtr++ * X[j];

    
	// now do the back substitution storing result in X
	for (int k=(n-1); k>0; k--) {
      
	  int rowktop = RowTop[k];
	  double bk = X[k];
	  double *ajiPtr = topRowPtr[k]; 		

	  for (int j=rowktop; j<k; j++) 
	    X[j] -= *ajiPtr++ * bk;
	}   	 
    } else { // just do forward and back substitution
      
      // do forward substitution 
      for (int i=1; i<n; i++) {
	    
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
      double *aiiPtr = invD.getDataPtr();
      for (int j=0; j<n; j++) 
	*bjPtr++ = *aiiPtr++ * X[j];

    
      // now do the back substitution storing result in X
      for (int k=(n-1); k>0; k--) {
      
	int rowktop = RowTop[k];
	double bk = X[k];
	double *ajiPtr = topRowPtr[k]; 		

	for (int j=rowktop; j<k; j++) 
	  X[j] -= *ajiPtr++ * bk;
      }   	 
    }    
    return 0;
}

int XC::ProfileSPDLinDirectBlockSolver::setProfileSOE(ProfileSPDLinSOE &theNewSOE)
{
    if (theSOE != 0) {
	std::cerr << getClassName() << "::" << __FUNCTION__ << "; ";
	std::cerr << " has already been called \n";	
	return -1;
    }
    
    theSOE = &theNewSOE;
    return 0;
}
	
int XC::ProfileSPDLinDirectBlockSolver::sendSelf(CommParameters &cp)
{
    if (size != 0)
	std::cerr << "XC::ProfileSPDLinDirectBlockSolver::sendSelf - does not send itself YET\n"; 
    return 0;
}


int XC::ProfileSPDLinDirectBlockSolver::recvSelf(const CommParameters &cp)
  { return 0; }


