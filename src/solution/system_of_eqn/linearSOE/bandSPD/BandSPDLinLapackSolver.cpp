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
// $Date: 2003/04/02 22:02:52 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/bandSPD/BandSPDLinLapackSolver.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/bandSPD/BandSPDLinLapackSolver.h
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the implementation of XC::BandSPDLinLapackSolver.
//
// What: "@(#) BandSPDLinLapackSolver.h, revA"

#include <solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinLapackSolver.h>
#include <solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinSOE.h>

//! @brief Constructor.
XC::BandSPDLinLapackSolver::BandSPDLinLapackSolver(void)
  :BandSPDLinSolver(SOLVER_TAGS_BandSPDLinLapackSolver)
  {}

//! @brief Virtual constructor.
XC::LinearSOESolver *XC::BandSPDLinLapackSolver::getCopy(void) const
   { return new BandSPDLinLapackSolver(*this); }

//! @brief Computes the solution to a real system of linear equations
//!     A * X = B,
//!  where A is an N-by-N symmetric positive definite band matrix and X
//!  and B are N-by-NRHS matrices.
//! 
//!  The Cholesky decomposition is used to factor A as
//!     A = U**T * U,  if UPLO = 'U', or
//!     A = L * L**T,  if UPLO = 'L',
//!  where U is an upper triangular band matrix, and L is a lower
//!  triangular band matrix, with the same number of superdiagonals or
//!  subdiagonals as A.  The factored form of A is then used to solve the
//!  system of equations A * X = B.
extern "C" int dpbsv_(char *UPLO, int *N, int *KD, int *NRHS, 
		      double *A, int *LDA, double *B, int *LDB, 
		      int *INFO);

//! @brief Solves a system of linear equations A*X = B with a symmetric
//! positive definite band matrix A using the Cholesky factorization
//! A = U**T*U or A = L*L**T computed by DPBTRF.
extern "C" int dpbtrs_(char *UPLO, int *N, int *KD, int *NRHS, 
		       double *A, int *LDA, double *B, int *LDB, 
		       int *INFO);

//! @brief Computes the Cholesky factorization of a real symmetric
//! positive definite band matrix A.
//!
//! The factorization has the form
//!    A = U**T * U,  if UPLO = 'U', or
//!    A = L  * L**T,  if UPLO = 'L',
//! where U is an upper triangular matrix and L is lower triangular.
extern "C" int dpbtrf_(char *UPLO, int *N, int *KD, 
		       double *A, int *LDA, int *INFO);

//! @brief  Returns the value of the 1-norm, or the Frobenius norm, or the infinity norm,
//! or the element of largest absolute value of a symmetric band matrix. 
extern "C" double dlansb_(const char *norm, const char *uplo,
			  const int *n, const int *k,
			  const double *a, const int *lda, double *work);

//! @brief Estimates the reciprocal of the condition number (in the
//! 1-norm) of a real symmetric positive definite band matrix using the
//! Cholesky factorization A = U**T*U or A = L*L**T computed by DPBTRF.
//!
//! An estimate is obtained for norm(inv(A)), and the reciprocal of the
//! condition number is computed as RCOND = 1 / (ANORM * norm(inv(A))).
extern "C" int dpbcon_(char *UPLO, int *N, int *KD, double *A,
		       int *LDA, const double *anorm, double *rcond,
		       double *work, int *iwork, int *INFO);
//! Compute solution.
//! 
//! The solver first copies the B vector into X and then solves the
//! BandSPDLinSOE system by calling the LAPACK routines {\em 
//! dpbsv()}, if the system is marked as not having been factored,
//! and dpbtrs() if system is marked as having been factored. 
//! If the solution is successfully obtained, i.e. the LAPACK routines
//! return \f$0\f$ in the INFO argument, it marks the system has having been 
//! factored and returns \f$0\f$, otherwise it prints a warning message and
//! returns INFO. The solve process changes \f$A\f$ and \f$X\f$.   
int XC::BandSPDLinLapackSolver::solve(void)
  {
    int retval= 0;
    if(!theSOE)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; no LinearSOE object has been set\n";
	retval= -1;
      }
    else
      {
	int n = theSOE->size;
	int kd = theSOE->half_band -1;
	int ldA = kd +1;
	int nrhs = 1;
	int ldB = n;
	int info;
	double *Aptr = theSOE->A.getDataPtr();
	double *Xptr = theSOE->getPtrX();
	double *Bptr = theSOE->getPtrB();

	// first copy B into X
	for(int i=0; i<n; i++)
	  *(Xptr++) = *(Bptr++);
	Xptr= theSOE->getPtrX();

	char strU[]= "U";
	// now solve AX = Y
	{ if (theSOE->factored == false)          
	    dpbsv_(strU,&n,&kd,&nrhs,Aptr,&ldA,Xptr,&ldB,&info);
	  else
	    dpbtrs_(strU,&n,&kd,&nrhs,Aptr,&ldA,Xptr,&ldB,&info);
	}

	// check if successful
	if(info != 0)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING - the LAPACK"
		      << " routines returned " << info << std::endl;
	    retval= -info;
	  }
	theSOE->factored = true;
      }
    return retval;
  }

//! Compute solution.
//! 
//! The solver first copies the B vector into X and then solves the
//! BandSPDLinSOE system by calling the LAPACK routines {\em 
//! dpbsv()}, if the system is marked as not having been factored,
//! and dpbtrs() if system is marked as having been factored. 
//! If the solution is successfully obtained, i.e. the LAPACK routines
//! return \f$0\f$ in the INFO argument, it marks the system has having been 
//! factored and returns \f$0\f$, otherwise it prints a warning message and
//! returns INFO. The solve process changes \f$A\f$ and \f$X\f$.   
double XC::BandSPDLinLapackSolver::getRCond(const char &c)
  {
    double retval= 0.0;
    if(!theSOE)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; no LinearSOE object has been set\n";
	retval= -1.0;
      }
    else
      {
	int n = theSOE->size;
	int kd = theSOE->half_band -1;
	int ldA = kd+1;
	int info;
	double *Aptr = theSOE->A.getDataPtr();

	char strU[]= "U";
	// now compute condition number
	if(theSOE->factored == false) // factorize
	  {
	    dpbtrf_(strU,&n,&kd,Aptr,&ldA,&info);
	    theSOE->factored= true;
	  }
	if(info != 0)
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; LaPack dpbtrf_ failure with error: " << info
		    << std::endl;
	else
	  {
	    char norm[1];
	    norm[0]= c;
            Vector wrk(3*n);
            double *wrkPtr= wrk.getDataPtr();
	    ID iwrk(n);
	    int *iwrkPtr= iwrk.getDataPtr();
            const double anorm= dlansb_(norm, strU, &n, &kd, Aptr, &ldA, wrkPtr);
	    dpbcon_(norm,&n,&kd,Aptr,&ldA,&anorm,&retval,wrkPtr,iwrkPtr,&info);
	  }
	// check if successful
	if(info != 0)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING - the LAPACK"
		      << " routines returned " << info << std::endl;
	    retval= -info;
	  }
      }
    return retval;
  }


//! @brief Does nothing but return \f$0\f$.
int XC::BandSPDLinLapackSolver::setSize()
  {
    // nothing to do    
    return 0;
  }

//! @brief Does nothing but return \f$0\f$.
int XC::BandSPDLinLapackSolver::sendSelf(Communicator &comm)
  {
    // nothing to do
    return 0;
  }

//! @brief Does nothing but return \f$0\f$.
int XC::BandSPDLinLapackSolver::recvSelf(const Communicator &comm)
  {
    // nothing to do
    return 0;
  }



