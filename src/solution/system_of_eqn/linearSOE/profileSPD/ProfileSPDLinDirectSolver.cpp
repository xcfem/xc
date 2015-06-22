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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/02/14 23:02:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectSolver.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/ProfileSPD/ProfileSPDLinSOESolver.C
//
// Written: fmk 
// Created: Febuary 1997
// Revision: A
//
// Description: This file contains the implementation for ProfileSPDLinSOESolver
// Description: This file contains the class definition for 
// ProfileSPDLinDirectSolver. ProfileSPDLinDirectSolver is a subclass 
// of LinearSOESOlver. It solves a XC::ProfileSPDLinSOE object using
// the LDL^t factorization.

// What: "@(#) ProfileSPDLinDirectSolver.C, revA"

#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectSolver.h>
#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSOE.h>
#include <cmath>

XC::ProfileSPDLinDirectSolver::ProfileSPDLinDirectSolver(double tol)
  :ProfileSPDLinDirectBase(SOLVER_TAGS_ProfileSPDLinDirectSolver,tol){}

int XC::ProfileSPDLinDirectSolver::setSize(void)
  {

    if (theSOE == 0) {
	std::cerr << "XC::ProfileSPDLinDirectSolver::setSize()";
	std::cerr << " No system has been set\n";
	return -1;
    }

    // check for XC::quick return 
    if (theSOE->size == 0)
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


int 
XC::ProfileSPDLinDirectSolver::solve(void)
{

    // check for XC::quick returns
    if (theSOE == 0) {
	std::cerr << "XC::ProfileSPDLinDirectSolver::solve(void): ";
	std::cerr << " - No ProfileSPDSOE has been assigned\n";
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

	double a00 = theSOE->A[0];
	if (a00 <= 0.0) {
	  std::cerr << "XC::ProfileSPDLinDirectSolver::solve() - ";
	  std::cerr << " aii < 0 (i, aii): (0,0)\n"; 
	  return(-2);
	}    
	
        invD[0] = 1.0/theSOE->A[0];	
	
	// for every col across 
	for (int i=1; i<theSize; i++) {

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
	    double *bjPtr  = &X[rowitop];  
	    double tmp = 0;	    
	    
	    for (int jj=rowitop; jj<i; jj++) {
		double aji = *ajiPtr;
		double lij = aji * invD[jj];
		tmp -= lij * *bjPtr++; 		
		*ajiPtr++ = lij;
		aii = aii - lij*aji;
	    }
	    
	    // check that the diag > the tolerance specified
	    if (aii == 0.0) {
		std::cerr << "XC::ProfileSPDLinDirectSolver::solve() - ";
		std::cerr << " aii < 0 (i, aii): (" << i << ", " << aii << ")\n"; 
		return(-2);
	    }
	    if (fabs(aii) <= minDiagTol) {
		std::cerr << "XC::ProfileSPDLinDirectSolver::solve() - ";
		std::cerr << " aii < minDiagTol (i, aii): (" << i;
		std::cerr << ", " << aii << ")\n"; 
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
	for (int k=(theSize-1); k>0; k--) {

	    int rowktop = RowTop[k];
	    double bk = X[k];
	    double *ajiPtr = topRowPtr[k]; 		

	    for (int j=rowktop; j<k; j++) 
		X[j] -= *ajiPtr++ * bk;
	}   	 	
    }

    else {

	// JUST DO SOLVE

	// do forward substitution 
	for (int i=1; i<theSize; i++) {
	    
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
	for (int k=(theSize-1); k>0; k--) {

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

double XC::ProfileSPDLinDirectSolver::getDeterminant(void) 
{
   int theSize = theSOE->size;
   double determinant = 1.0;
   for (int i=0; i<theSize; i++)
     determinant *= invD[i];
   determinant = 1.0/determinant;
   return determinant;
}

int XC::ProfileSPDLinDirectSolver::setProfileSOE(ProfileSPDLinSOE &theNewSOE)
  {
    if (theSOE != 0) {
	std::cerr << "XC::ProfileSPDLinDirectSolver::setProfileSOE() - ";
	std::cerr << " has already been called \n";	
	return -1;
    }
    
    theSOE = &theNewSOE;
    return 0;
}
	

int XC::ProfileSPDLinDirectSolver::factor(int n)
{

    // check for XC::quick returns
    if (theSOE == 0) {
	std::cerr << "XC::ProfileSPDLinDirectSolver::factor: ";
	std::cerr << " - No ProfileSPDSOE has been assigned\n";
	return -1;
    }

    int theSize = theSOE->size;    
    if (n > theSize) {
	std::cerr << "XC::ProfileSPDLinDirectSolver::factor: ";
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
		std::cerr << "XC::ProfileSPDLinDirectSolver::solve() - ";
		std::cerr << " aii < 0 (i, aii): (" << i << ", " << aii << ")\n"; 
		return(-2);
	    }
	    if (aii <= minDiagTol) {
		std::cerr << "XC::ProfileSPDLinDirectSolver::solve() - ";
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

int
XC::ProfileSPDLinDirectSolver::sendSelf(CommParameters &cp)
{
    return 0;
}


int 
XC::ProfileSPDLinDirectSolver::recvSelf(const CommParameters &cp)
{
    return 0;
}


