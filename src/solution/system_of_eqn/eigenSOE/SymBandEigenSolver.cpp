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
                                                                        
// $Revision: 1.7 $
// $Date: 2005/07/06 22:00:20 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/eigenSOE/SymBandEigenSolver.cpp,v $

// Written: MHS
// Created: Oct 2001
//
// Description: This file contains the class definition for
// SymBandEigenSolver, which computes the eigenvalues and eigenvectors
// of a symmetric banded matrix using the LAPACK subroutine dsbevx.

#include <solution/system_of_eqn/eigenSOE/SymBandEigenSolver.h>
#include <solution/system_of_eqn/eigenSOE/SymBandEigenSOE.h>
#include <cmath>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/FE_EleIter.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/analysis/integrator/Integrator.h>
#include "utility/matrix/Vector.h"

XC::SymBandEigenSolver::SymBandEigenSolver()
:EigenSolver(EigenSOLVER_TAGS_SymBandEigenSolver), theSOE(nullptr) {}

extern "C" int dsbevx_(char *jobz, char *range, char *uplo, int *n, int *kd,
		       double *ab, int *ldab, double *q, int *ldq,
		       double *vl, double *vu, int *il, int *iu, double *abstol,
		       int *m, double *w, double *z, int *ldz,
		       double *work, int *iwork, int *ifail, int *info);

//! @brief Returns the value of the 1-norm, Frobenius norm, infinity-norm,
//! or the largest absolute value of any element of  a real symmetric band
//! matrix. 
extern "C" double dlansb_(const char *norm, const char *uplo, const int *n,
			  const int *k, const double *a, const int *lda,
			  double *work);
//! @brief Computes the Cholesky factorization of a real symmetric
//! positive definite band matrix A.
extern "C" int dpbtrf_(const char *uplo, const int *n, const int *kd,
		       double *ab, const int *ldab, int *INFO);

//! @brief estimates the reciprocal of the condition number (in the
//! 1-norm) of a real symmetric positive definite band matrix A
//! using the factorization A = U*D*U**T or A = L*D*L**T computed by DDPBRF.
extern "C" int dpbcon_(const char *uplo, const int *n, const int *kd,
		       const double *ab, const int *ldab, const double *anorm,
		       double *rcond, double *work, int *iwork, int *INFO);

int XC::SymBandEigenSolver::solve(void)
  {return this->solve(theSOE->size);}

int XC::SymBandEigenSolver::solve(int nModes)
  {
    if(!theSOE)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; no EigenSOE has been set yet.\n";
        return -1;
      }
  
    // Set number of modes
    numModes= nModes;

    // Number of equations
    int n= theSOE->size;

    // Check for quick return
    if(numModes < 1)
      {
        numModes= 0;
        return 0;
      }

    // Simple check
    if(numModes > n)
      numModes= n;

    // Allocate storage for eigenvalues
    eigenvalue.resize(n);

    // Real work array (see LAPACK dsbevx subroutine documentation)
    work.resize(7*n);

    // Integer work array (see LAPACK dsbevx subroutine documentation)
    std::vector<int> iwork(5*n);

    // Leading dimension of eigenvectors
    int ldz = n;

    // Allocate storage for eigenvectors
    eigenvector.resize(ldz*numModes);

    // Number of superdiagonals
    int kd= theSOE->numSuperD;

    // Matrix data
    double *ab= theSOE->A.getDataPtr();

    // Leading dimension of the matrix
    int ldab= kd + 1;

    // Leading dimension of q
    int ldq= n;

    // Orthogonal matrix used in reduction to tridiagonal form
    // (see LAPACK dsbevx subroutine documentation)
    std::vector<double> q(ldq*n);

    // Index ranges [1,numModes] of eigenpairs to compute
    int il = 1;
    int iu = numModes;

    // Compute eigenvalues and eigenvectors
    char jobz[] = "V";

    // Selected eigenpairs are based on index range [il,iu]
    char range[] = "I";

    // Upper triagle of matrix is stored
    char uplo[] = "U";
  
    // Return value
    std::vector<int> ifail(n);
    int info= 0;

    // Number of eigenvalues returned
    int m= 0;

    // Not used
    double vl = 0.0;
    double vu = 1.0;

    // Not used ... I think!
    double abstol = -1.0;


    // if Mass matrix we make modifications to A:
    //         A -> M^(-1/2) A M^(-1/2)
    double *M= theSOE->M.getDataPtr();
    double *A= theSOE->A.getDataPtr();
    int numSuperD = theSOE->numSuperD;
    int size = n;
    if(M) //Its seems that the M matrix must be DIAGONAL.
      {
        int i,j;
        bool singular = false;
        // form M^(-1/2) and check for singular mass matrix
        for(int k=0; k<size; k++)
          {
            if(M[k] == 0.0)
              {
	        singular = true;
	        // alternative is to set as a small no ~ 1e-10 times smallest m(i,i) != 0.0
	        std::cerr << "SymBandEigenSolver::solve() - M matrix singular\n";
	        return -1;
              }
            else
              {
	        M[k] = 1.0/sqrt(M[k]);
              }
          }
        // make modifications to A
        // Aij -> Mi Aij Mj  (based on new_ M)
        for(i=0; i<size; i++)
          {
            double *AijPtr = A +(i+1)*(numSuperD+1) - 1;
            int minColRow = i - numSuperD;
            if(minColRow < 0) minColRow = 0;
            for(j=i; j>=minColRow; j--)
              {
	        *AijPtr *= M[j]*M[i];
	        AijPtr--;
              }
          }
      }

    // Calls the LAPACK routine that computes the eigenvalues and eigenvectors
    // of the matrix A previously transforme.
    dsbevx_(jobz, range, uplo, &n, &kd, ab, &ldab,
	    &q[0], &ldq, &vl, &vu, &il, &iu, &abstol, &m,
	    eigenvalue.getDataPtr(), eigenvector.getDataPtr(), &ldz, work.getDataPtr(), &iwork[0], &ifail[0], &info);

    if(info < 0)
      {
        std::cerr << "SymBandEigenSolver::solve() -- invalid argument number " << -info << " passed to LAPACK dsbevx\n";
        return info;
      }

    if(info > 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; LAPACK dsbevx returned error code " << info << std::endl;
        return -info;
      }

    if(m < numModes)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; LAPACK dsbevx only computed " << m << " eigenvalues, " <<
        numModes << "were requested\n";
        numModes = m;
      }

    theSOE->factored = true;

    // make modifications to the eigenvectors
    //   Eij -> Mi Eij  (based on new_ M)

    M= theSOE->M.getDataPtr();
    if(M)
      {
        for(int j=0; j<numModes; j++)
          {
            double *eigVectJptr = &eigenvector[j*ldz];
            double *MPtr = M;
            for(int i=0; i<size; i++) 
	      *eigVectJptr++ *= *MPtr++;
          }
      }
    return 0;
  }

//! @brief Sets the eigenproblem to solve.
bool XC::SymBandEigenSolver::setEigenSOE(EigenSOE *soe)
  {
    bool retval= false;
    SymBandEigenSOE *tmp= dynamic_cast<SymBandEigenSOE *>(soe);
    if(tmp)
      {
        theSOE= tmp;
        retval= true;
      }
    else
      std::cerr << getClassName() << "::setEigenSOE: not a suitable system of equations." << std::endl;
    return retval;
  }

//! @brief Sets the eigenproblem to solve.
bool XC::SymBandEigenSolver::setEigenSOE(SymBandEigenSOE &theBandSOE)
  { return setEigenSOE(&theBandSOE); }

const XC::Vector &XC::SymBandEigenSolver::getEigenvector(int mode) const
  {
    if(mode < 1 || mode > numModes)
      {
        std::cerr << "SymBandEigenSolver::getEigenVector() -- mode " << mode << " is out of range (1 - "
	          << numModes << ")\n";
        eigenV.Zero();
        return eigenV;  
      }
  
    int size = theSOE->size;
    int index = (mode - 1) * size;
    Vector &vec = eigenV;
    if(!eigenvector.isEmpty())
      {
        for(int i = 0; i < size; i++)
          { vec(i) = eigenvector[index++]; }	
      }
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; eigenvectors not yet computed\n";
        eigenV.Zero();
      }      
    return eigenV;  
  }

const double &XC::SymBandEigenSolver::getEigenvalue(int mode) const
  {
    static double retval= 0.0;
    if(mode < 1 || mode > numModes)
      std::cerr << "SymBandEigenSolver::getEigenvalue() -- mode " 
                << mode << " is out of range (1 - " << numModes << ")\n";
    if(!eigenvalue.isEmpty())
      retval= eigenvalue[mode-1];
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; eigenvalues not yet computed\n";
    return retval;
  }

int XC::SymBandEigenSolver::setSize(void)
  {
    const int size = theSOE->size;    
    if(eigenV.Size() != size)
      eigenV.resize(size);
    return 0;
  }

//! @brief Return the eigenvectors dimension.
const int &XC::SymBandEigenSolver::getSize(void) const
  { return theSOE->size; }

//! @brief Estimates the reciprocal of the condition number of a real
//! general band matrix A, in either the 1-norm or the infinity-norm,
//! using the LU factorization computed by DGBTRF.
//!
//! @param c: Specifies whether the 1-norm condition number or the
//!           infinity-norm condition number is required:
//!           = '1' or 'O':  1-norm;
//!           = 'I':         Infinity-norm.
double XC::SymBandEigenSolver::getRCond(const char &c)
  {
    double retval= 0.0;
    if((c!='1') && (c!='0') && (c!='o'))
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; only 1-norm is supported.\n";
      }
    if(!theSOE)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; no LinearSOE object has been set.\n";
	return -1;
      }
    else
      {
	if(theSOE->factored == false) // factorize
	  {
	    theSOE->save(); //Avoid conflict with solve()
	    int n = theSOE->size;
	    const int k= theSOE->numSuperD;
	    const int ldA= k+1;// Leading dimension of the matrix
	    int info;
	    double *Aptr= theSOE->A.getDataPtr();
	    char uplo[] = "U"; // Upper triangle of matrix is stored
	    dpbtrf_(uplo,&n,&k,Aptr,&ldA,&info);
	    if(info > 0) //Singular matrix.
	      { retval= DBL_MAX; }
	    else if(info < 0)
	      std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; LaPack dpbtrf_ failure with error: " << info
			<< std::endl;
	    else //solve
	      {
		char norm[1];
		norm[0]= c;
		Vector wrk(3*n);
		double *wrkPtr= wrk.getDataPtr();
		ID iwrk(n);
		int *iwrkPtr= iwrk.getDataPtr();
		const double anorm= dlansb_(norm, uplo, &n, &k, Aptr, &ldA, wrkPtr);
		dpbcon_(uplo,&n,&k,Aptr,&ldA,&anorm,&retval,wrkPtr,iwrkPtr,&info);
		// check if successful
		if(info != 0)
		  {
		    std::cerr << getClassName() << "::" << __FUNCTION__
			      << ";LAPACK dpbcon routine returned "
			      << info << std::endl;
		    return -info;
		  }
	      }
            theSOE->restore(); //Avoid conflict with solve()
	  }
	else
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; matrix already factored, conflict with solve()."
		    << std::endl;
	return retval;
      }
  }

int XC::SymBandEigenSolver::sendSelf(CommParameters &cp)
  { return 0; }

int XC::SymBandEigenSolver::recvSelf(const CommParameters &cp)
  { return 0; }

