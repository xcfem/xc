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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/02/14 23:02:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectSolver.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/ProfileSPD/ProfileSPDLinSOESolver.C
//
// Written: fmk 
// Created: February 1997
// Revision: A
//
// Description: This file contains the implementation for ProfileSPDLinSOESolver

// What: "@(#) ProfileSPDLinDirectSolver.C, revA"

#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectSolver.h>
#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSOE.h>
#include <cmath>

//! @brief Constructor. A unique class tag defined in classTags.h
//! is passed to the base class constructor.
XC::ProfileSPDLinDirectSolver::ProfileSPDLinDirectSolver(double tol)
  : ProfileSPDLinDirectBase(SOLVER_TAGS_ProfileSPDLinDirectSolver,tol){}

int XC::ProfileSPDLinDirectSolver::setSize(void)
  {

    if(!theSOE)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; No system of equations has been set.\n";
	return -1;
      }

    // check for quick return 
    if(theSOE->size == 0)
      return 0;
    
    size = theSOE->size;
    
    RowTop= ID(size);
    topRowPtr= std::vector<double *>(size);
    invD= Vector(size); 
	

    // set some pointers
    double *A = theSOE->A.getDataPtr();
    int *iDiagLoc = theSOE->iDiagLoc.getDataPtr();

    // set RowTop and topRowPtr info

    RowTop[0]= 0;
    topRowPtr[0]= A;
    for(int j=1; j<size; j++)
      {
	int icolsz = iDiagLoc[j] - iDiagLoc[j-1];
	RowTop[j] = j - icolsz +  1;
	topRowPtr[j] = &A[iDiagLoc[j-1]]; // FORTRAN array indexing in iDiagLoc
      }

    size = theSOE->size;
    return 0;
  }

//! @brief Computes the solution.
//!
//! The solver first copies the B vector into X.
//! The solve process changes $A$ and $X$.
int XC::ProfileSPDLinDirectSolver::solve(void)
  {

    // check for quick returns
    if(!theSOE)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no system of equations has been assigned\n";
	return -1;
      }
    
    if (theSOE->size == 0)
	return 0;

    // set some pointers
    double *B = theSOE->getPtrB();
    double *X = theSOE->getPtrX();
    int theSize = theSOE->size;
    // copy B into X
    for (int ii=0; ii<theSize; ii++)
	X[ii] = B[ii];

    /*
      for (int iii=0; iii<theSize; iii++) {
      int rowiiitop = RowTop[iii];
      double *ajiptr = topRowPtr[iii];
      std::cerr << "\n COLUMN " << iii << " TopRow " << rowiiitop << " -> ";
      for (int jjj = rowiiitop; jjj <=iii; jjj++)
      std::cerr << *ajiptr++ << " ";
      }
      std::cerr << std::endl;

      for (int iii=0; iii<theSOE->size; iii++) {
      std::cerr << "COLUMN " << iii << " Biii -> " << X[iii] << std::endl;
      }
      std::cerr << std::endl;
      */

    
    if(theSOE->factored == false)
      {

	// FACTOR & SOLVE
	double *ajiPtr, *akjPtr, *akiPtr, *bjPtr;    
	
	// if the matrix has not been factored already factor it into U^t D U
	// storing D^-1 in invD as we go

	const double &a00 = theSOE->A[0];
	if(a00 <= 0.0)
	  {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; aii < 0 (i, aii): (0,0)\n"; 
	    return(-2);
	  }    
	
        invD[0] = 1.0/theSOE->A[0];	
	
	// for every col across 
	for(int i=1; i<theSize; i++)
	  {
	    int rowitop = RowTop[i];
	    ajiPtr = topRowPtr[i];

	    for(int j=rowitop; j<i; j++)
	      {
		double tmp = *ajiPtr;
		int rowjtop = RowTop[j];

		if(rowitop > rowjtop)
		  {

		    akjPtr = topRowPtr[j] + (rowitop-rowjtop);
		    akiPtr = topRowPtr[i];

		    for (int k=rowitop; k<j; k++) 
			tmp -= *akjPtr++ * *akiPtr++ ;

		    *ajiPtr++ = tmp;
		  }
		else
		  {
		    akjPtr = topRowPtr[j];
		    akiPtr = topRowPtr[i] + (rowjtop-rowitop);

		    for (int k=rowjtop; k<j; k++) 
			tmp -= *akjPtr++ * *akiPtr++ ;

		    *ajiPtr++ = tmp;
		  }
	      }

	    /* now form i'th col of [U] and determine [dii] */

	    double aii = theSOE->A[theSOE->iDiagLoc[i] -1]; // FORTRAN ARRAY INDEXING
	    ajiPtr = topRowPtr[i];
	    double *bjPtr  = &X[rowitop];  
	    double tmp = 0;	    
	    
	    for (int jj=rowitop; jj<i; jj++)
	      {
		double aji = *ajiPtr;
		double lij = aji * invD[jj];
		tmp -= lij * *bjPtr++; 		
		*ajiPtr++ = lij;
		aii = aii - lij*aji;
	      }
	    
	    // check that the diag > the tolerance specified
	    if(aii == 0.0)
	      {
		std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; aii < 0 (i, aii): (" << i << ", "
			  << aii << ")\n"; 
		return(-2);
	      }
	    if (fabs(aii) <= minDiagTol)
	      {
		std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; aii < minDiagTol (i, aii): (" << i
			  << ", " << aii << ")\n"; 
		return(-2);
	      }		
	    invD[i] = 1.0/aii; 
	    X[i] += tmp;	    
	  }

	theSOE->factored = true;
	theSOE->numInt = 0;
	
	
	// divide by diag term 
	bjPtr = X; 
	double *aiiPtr= invD.getDataPtr();
	for (int j=0; j<theSize; j++) 
	    *bjPtr++ = *aiiPtr++ * X[j];


	// now do the back substitution storing result in X
	for(int k=(theSize-1); k>0; k--)
	  {

	    int rowktop = RowTop[k];
	    double bk = X[k];
	    double *ajiPtr = topRowPtr[k]; 		

	    for (int j=rowktop; j<k; j++) 
		X[j] -= *ajiPtr++ * bk;
	  }   	 	
      }

    else
      {

	// JUST DO SOLVE

	// do forward substitution 
	for (int i=1; i<theSize; i++)
	  {
	    
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
	for (int j=0; j<theSize; j++) 
	    *bjPtr++ = *aiiPtr++ * X[j];


	// now do the back substitution storing result in X
	for (int k=(theSize-1); k>0; k--)
	  {

	    int rowktop = RowTop[k];
	    double bk = X[k];
	    double *ajiPtr = topRowPtr[k]; 		

	    for (int j=rowktop; j<k; j++) 
		X[j] -= *ajiPtr++ * bk;
	  }  	 
      }    
    
    /*
    std::cerr << "BBBB " << theSOE->getB();
    std::cerr << "XXXX " << theSOE->getX();
    */
    
    return 0;
  }

//! @brief Returns the determinant.
double XC::ProfileSPDLinDirectSolver::getDeterminant(void) 
  {
    const int theSize = theSOE->size;
    double determinant = 1.0;
    for (int i=0; i<theSize; i++)
      determinant *= invD[i];
    determinant = 1.0/determinant;
    return determinant;
  }

//! @brief Sets the system of equations to solve.
int XC::ProfileSPDLinDirectSolver::setProfileSOE(ProfileSPDLinSOE &theNewSOE)
  {
    int retval= 0;
    if(theSOE)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << ";  has already been called \n";	
	retval= -1;
      }
    else
      theSOE= &theNewSOE;
    return retval;
  }
	

int XC::ProfileSPDLinDirectSolver::factor(int n)
  {

    // check for quick returns
    if(theSOE == 0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__;
	std::cerr << " - No ProfileSPDSOE has been assigned\n";
	return -1;
      }

    int theSize = theSOE->size;    
    if(n > theSize)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__;
	std::cerr << " - n " << n << " greater than size of system" << theSize << std::endl;
	return -1;
      }

    if(theSize == 0 || n == 0)
	return 0;


    // set some pointers
    if(theSOE->factored == false)
      {
	// FACTOR & SOLVE
	double *ajiPtr, *akjPtr, *akiPtr;    
	
	// if the matrix has not been factored already factor it into U^t D U
	// storing D^-1 in invD as we go
    
	invD[0] = 1.0/theSOE->A[0];	
	
	// for every col across 
	for (int i=1; i<n; i++) {

	    int rowitop = RowTop[i];
	    ajiPtr = topRowPtr[i];

	    for (int j=rowitop; j<i; j++) {
		double tmp = *ajiPtr;
		int rowjtop = RowTop[j];

		if (rowitop > rowjtop) {

		    akjPtr = topRowPtr[j] + (rowitop-rowjtop);
		    akiPtr = topRowPtr[i];

		    for (int k=rowitop; k<j; k++) 
			tmp -= *akjPtr++ * *akiPtr++ ;

		    *ajiPtr++ = tmp;
		}
		else {
		    akjPtr = topRowPtr[j];
		    akiPtr = topRowPtr[i] + (rowjtop-rowitop);

		    for (int k=rowjtop; k<j; k++) 
			tmp -= *akjPtr++ * *akiPtr++ ;

		    *ajiPtr++ = tmp;
		}
	    }

	    /* now form i'th col of [U] and determine [dii] */

	    double aii = theSOE->A[theSOE->iDiagLoc[i] -1]; // FORTRAN ARRAY INDEXING
	    ajiPtr = topRowPtr[i];
	    
	    for (int jj=rowitop; jj<i; jj++) {
		double aji = *ajiPtr;
		double lij = aji * invD[jj];
		*ajiPtr++ = lij;
		aii = aii - lij*aji;
	    }
	    
	    // check that the diag > the tolerance specified
	    if (aii <= 0.0) {
		std::cerr << getClassName() << "::" << __FUNCTION__ << "; ";
		std::cerr << " aii < 0 (i, aii): (" << i << ", " << aii << ")\n"; 
		return(-2);
	    }
	    if (aii <= minDiagTol) {
		std::cerr << getClassName() << "::" << __FUNCTION__ << "; ";
		std::cerr << " aii < minDiagTol (i, aii): (" << i;
		std::cerr << ", " << aii << ")\n"; 
		return(-2);
	    }		
	    invD[i] = 1.0/aii; 
	}

	theSOE->factored = true;
	theSOE->numInt = n;
	
    }	
    return 0;
  }


/*
    double *ajiPtr, *akjPtr, *akiPtr, *bjPtr;    
	
    // if the matrix has not been factored already factor it into U^t D U
    // storing D^-1 in invD as we go
    
    if (theSOE->isAfactored == false) {
	
	invD[0] = 1.0/theSOE->A[0];	
	
	// for every col across 
	for (int i=1; i<n; i++) {

	    int rowitop = RowTop[i];
	    ajiPtr = topRowPtr[i];

	    for (int j=rowitop; j<i; j++) {
		double tmp = *ajiPtr;
		int rowjtop = RowTop[j];

		if (rowitop > rowjtop) {

		    akjPtr = topRowPtr[j] + (rowitop-rowjtop);
		    akiPtr = topRowPtr[i];

		    for (int k=rowitop; k<j; k++) 
			tmp -= *akjPtr++ * *akiPtr++ ;

		    *ajiPtr++ = tmp;
		}
		else {
		    akjPtr = topRowPtr[j];
		    akiPtr = topRowPtr[i] + (rowjtop-rowitop);

		    for (int k=rowjtop; k<j; k++) 
			tmp -= *akjPtr++ * *akiPtr++ ;

		    *ajiPtr++ = tmp;
		}
	    }

	    // now form i'th col of [U] and determine [dii] 

	    double aii = theSOE->A[theSOE->iDiagLoc[i] -1]; // FORTRAN ARRAY INDEXING
	    ajiPtr = topRowPtr[i];
	    
	    for (int jj=rowitop; jj<i; jj++) {
		double aji = *ajiPtr;
		double lij = aji * invD[jj];
		*ajiPtr++ = lij;
		aii = aii - lij*aji;
	    }
	    
	    // check that the diag > the tolerance specified
	    if (aii <= 0.0)     return(-2);
	    if (aii <= minDiagTol) return(-1);
	    invD[i] = 1.0/aii; 
	}
    }
    
    theSOE->isAfactored = true;
    if (n == theSize)
	theSOE->numInt = 0;
    else
	theSOE->numInt = n;
    
    return 0;
}
*/

int XC::ProfileSPDLinDirectSolver::sendSelf(Communicator &comm)
  {
    return 0;
  }


int XC::ProfileSPDLinDirectSolver::recvSelf(const Communicator &comm)
  {
    return 0;
  }


