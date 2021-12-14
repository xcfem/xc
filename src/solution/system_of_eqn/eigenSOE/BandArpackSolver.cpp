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
// File: ~/system_of_eqn/eigenSOE/BandArpackSolver.C
//
// Written: Jun Peng
// Created: Feb. 11, 1999
// Revision: A
//
// Description: This file contains the class definition for 
// BandArpackSolver. It solves the XC::BandArpackSOE object by calling
// Arpack routines.

#include <solution/system_of_eqn/eigenSOE/BandArpackSolver.h>
#include <solution/system_of_eqn/eigenSOE/BandArpackSOE.h>
#include <cstdio>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/FE_EleIter.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/analysis/integrator/Integrator.h>
#include <cstring>
#include "utility/matrix/Vector.h"
#include "utility/utils/misc_utils/mchne_eps.h"

//! @brief Constructor.
XC::BandArpackSolver::BandArpackSolver(int numE)
  :EigenSolver(EigenSOLVER_TAGS_BandArpackSolver,numE), tol(mchne_eps_dbl), maxitr(1000) {}

//! @brief LAPACK Computes the solution to the system of linear equations
//! with a band matrix A and multiple right-hand sides.
extern "C" int dgbsv_(int *N, int *KL, int *KU, int *NRHS, double *A, int *LDA,
                      int *iPiv, double *B, int *LDB, int *INFO);

//! @brief Computes an LU factorization of a real m-by-n band matrix A
//! using partial pivoting with row interchanges.
extern "C" int dgbtrf_(const int *M, const int *N, const int *KL,
		       const int *KU, double *A, const int *LDA,
		       int *iPiv, int *INFO);

//! @brief LAPACK Solves a system of linear equations with an LU-factored
//! band matrix, with multiple right-hand sides.
extern "C" int dgbtrs_(char *TRANS, int *N, int *KL, int *KU, int *NRHS,
                       double *A, int *LDA, int *iPiv, double *B, int *LDB,
                       int *INFO);

//! @brief Reverse communication interface for the Implicitly Restarted Arnoldi Iteration.
extern "C" int dsaupd_(int *ido, const char* bmat, const int *n, const char *which, const int *nev,
                       const double *tol, double *resid, const int *ncv, double *v, int *ldv,
                       int *iparam, int *ipntr, double *workd, double *workl,
                       int *lworkl, int *info);
//! @brief Postprocess of eigenvalues and eigenvectors.
extern "C" int dseupd_(bool *rvec, const char *howmny, long int *select, double *d, double *z,
                       int *ldz, double *sigma, const char *bmat, int *n, const char *which,
                       int *nev, double *tol, double *resid, int *ncv, double *v,
                       int *ldv, int *iparam, int *ipntr, double *workd,
                       double *workl, int *lworkl, int *info);

//! @brief Returns the value of the 1-norm, Frobenius norm, infinity-norm,
//! or the largest absolute value of any element of general band matrix. 
extern "C" double dlangb_(const char *norm, const int *n,
			  const int *kl, const int *ku,
			  const double *a, const int *lda, double *work);

//! @brief Estimates the reciprocal of the condition number of a real
//! general band matrix A, in either the 1-norm or the infinity-norm,
//! using the LU factorization computed by DGBTRF.
extern "C" int dgbcon_(char *norm, const int *N, const int *KL, const int *KU, const double *ab, const int *ldab, const int *iPiv, const double *anorm, double *rcond, double *work, int *iwork, int *INFO);

//! @brief Constructor.
//!
//! @param n: dimension of the eigenproblem
//! @param ncv: number of columns of the matrix v (<=n) t. This will indicate how many Lanczos vectors are generated at each iteration (see Arpack manual).
//! @param nev: number of eigenvalues to be computed.
//! @param maxitr: maximum number of iterations.
//! @param mode: type of eigenproblem to be solved (see dsaupd description in Arpack manual).
XC::ArpackAuxVars::ArpackAuxVars(int n, int ncv, int nev, int maxitr, int mode)
  : ldv(n), lworkl(ncv*ncv+8*ncv), v(n*ncv), workl(ncv*ncv+8*ncv+1),
  workd(3*n+1), d(nev), z(n*nev), resid(n), select(ncv)
  {
    iparam[0] = 1;
    iparam[2] = maxitr;
    iparam[6] = mode;
  }

//! @brief Reverse communication interface for the Implicitly Restarted Arnoldi Iteration.
//!
//! @param ido: Reverse communication flag
//! @param n: dimension of the eigenproblem
//! @param nev: number of eigenvalues to be computed.
//! @param tol: Stopping criterion: the relative accuracy of the Ritz value is considered acceptable if BOUNDS(I)<=TOL*ABS(RITZ(I)). If tol<0. is passed a default is set:
//! @param ncv: number of columns of the matrix v (<=n) t. This will ndicate how many Lanczos vectors are generated at each iteration (see Arpack manual).
//! @param info: On input: see Arpack manual. On output: error flag (if any). 
int XC::ArpackAuxVars::dsaupd(int &ido, const int &n, const std::string &which, const int &nev, const double &tol, const int &ncv, int &info)
  {
    int retval= dsaupd_(&ido, &bmat, &n, which.c_str(), &nev, &tol, &resid[0], &ncv, &v[0], &ldv,iparam, ipntr, &workd[0], &workl[0], &lworkl, &info);
    if(info!= 0)
      std::cerr << "ArpackAuxVars::" << __FUNCTION__
		<< "; error with dsaupd; info = " 
		<< info << std::endl;
    return retval;
  }

//! @brief Print error message.
std::string XC::BandArpackSolver::get_err_string(int info)
  {
    std::string retval= "";
    switch(info)
      {
      case -1:
	retval= "N must be positive.";
	break;
      case -2:
	retval= "NEV must be positive.";
	break;
      case -3:
	retval= "NCV must be greater than NEV and less than or equal to N.";
	break;
      case -4:
	retval= "The maximum number of Arnoldi update iterations allowed";
	break;
      case -5:
	retval= "WHICH must be one of 'LM', 'SM', 'LA', 'SA' or 'BE'.";
	break;
      case -6:
	retval= "BMAT must be one of 'I' or 'G'.";
	break;
      case -7:
	retval= "Length of private work array WORKL is not sufficient.";
	break;
      case -8:
	retval= "Error return from trid. eigenvalue calculation; informational error from LAPACK routine dsteqr.";
	break;
      case -9:
	retval= "Starting vector is zero.";
	break;
      case -10:
	retval= "IPARAM(7) must be 1,2,3,4,5.";
	break;
      case -11:
	retval= "IPARAM(7) = 1 and BMAT = 'G' are incompatible.";
	break;
      case -12:
	retval= "IPARAM(1) must be equal to 0 or 1.";
	break;
      case -13:
	retval= "NEV and WHICH = 'BE' are incompatible.";
	break;
      }
    return retval;
  }

//! @brief Get error message.
std::string XC::BandArpackSolver::dsaupd_err_string(int info)
  {
    std::string retval= get_err_string(info);
    if(retval=="")
      {
	switch(info)
	  {
	  case -9999:
	    retval= "Could not build an Arnoldi factorization.";
	    retval+= " IPARAM(5) returns the size of the current Arnoldi\n";
	    retval+= "factorization. The user is advised to check that";
	    retval+= "enough workspace and array storage has been allocated.";
	    break;
	  default:
	    retval= "unrecognised return value";
	  }
      }
    return retval;
  }

//! @brief Get error message.
std::string XC::BandArpackSolver::dseupd_err_string(int info)
  {
    std::string retval= get_err_string(info);
    if(retval=="")
      {
	switch(info)
	  {
	  case -14:
	    retval= "DSAUPD did not find any eigenvalues to sufficient accuracy.";
	    break;
	  case -15:
	    retval= "HOWMNY must be one of 'A' or 'S' if RVEC = .true.";
	    break;
	  case -16:
	    retval= "HOWMNY = 'S' not yet implemented";
	    break;
	  default:
	    retval= "unrecognised return value";
	  }
      }
    return retval;
  }

//! @brief Implicitly Restarted Arnoldi Iteration loop
//! @param ncv: number of columns of the matrix v (<=n) t. This will indicate how many Lanczos vectors are generated at each iteration (see Arpack manual).
//! @param nev: number of eigenvalues to be computed.
int XC::BandArpackSolver::dsaupd_loop(const int &ncv, const int &nev, ArpackAuxVars &av)
  {
    int n = theSOE->size; //number of rows of the matrix (and of columns because it is an square matrix).
    int kl = theSOE->numSubD; //Number of subdiagonals of the matrix.
    int ku = theSOE->numSuperD; //Number of superdiagonals of the matrix.
    int ldA = 2*kl + ku +1;
    int ldB = n;
    double *Aptr = theSOE->A.getDataPtr(); //Pointer to A matrix.
    int *iPIV = iPiv.getDataPtr(); //Pivots indexes.
    int info= 0;
    char ene[] = "N"; 
    int ido= 0; // first call to the reverse communication interface.
    int nrhs = 1;
    int ierr= 0;
    while(1)
      {
        //dsaupd_(&ido, &av.bmat, &n, which.c_str(), &nev, &tol, &av.resid[0], &ncv, &av.v[0], &av.ldv,av.iparam, av.ipntr, &av.workd[0], &av.workl[0], &av.lworkl, &info);
	av.dsaupd(ido, n, which, nev, tol, ncv, info);
        if(ido == -1) //Initialization phase
          {
            myMv(n, &av.workd[av.ipntr[0]-1], &av.workd[av.ipntr[1]-1]);
            dgbtrs_(ene, &n, &kl, &ku, &nrhs, Aptr, &ldA, iPIV,&av.workd[av.ipntr[1] - 1], &ldB, &ierr);
            if(ierr != 0)
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; error with dgbtrs_ 1" << std::endl;
                exit(0);
              }
            continue;
          }
        else if(ido == 1) //compute  Y = OP * Z  and Z = B * X  (see dsaupd in arpack manual).
          {

            //          double ratio = 1.0;
            myCopy(n, &av.workd[av.ipntr[2]-1], &av.workd[av.ipntr[1]-1]);
            dgbtrs_(ene, &n, &kl, &ku, &nrhs, Aptr, &ldA, iPIV,&av.workd[av.ipntr[1] - 1], &ldB, &ierr);
            if(ierr != 0)
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; error with dgbtrs_ 2" <<std::endl;
                exit(0);
              }
            continue;
          }
        else if(ido == 2) //compute  Y = B * X (see dsaupd in arpack manual).
          {
            myMv(n, &av.workd[av.ipntr[0]-1], &av.workd[av.ipntr[1]-1]);
            continue;
          }
        break; // ido==99: done.
      }
    return info;
  }

//! @brief Solves the eigenproblem.
int XC::BandArpackSolver::solve(void)
  {
    if(!theSOE)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; no LinearSOE object has been set\n";
        return -1;
      }
    int n = theSOE->size; //number of rows of the matrix (and of columns because it is an square matrix).

    // check iPiv is large enough
    if(iPiv.Size() < n)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "WARNING - "
		  << " iPiv not large enough - has setSize() been called?\n";
        return -1;
      }

    // set some variables
    int kl = theSOE->numSubD; //Number of subdiagonals of the matrix.
    int ku = theSOE->numSuperD; //Number of superdiagonals of the matrix.
    int ldA = 2*kl + ku +1;
    double *Aptr = theSOE->A.getDataPtr(); //Pointer to A matrix.

    if(numModes==n)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; number of modes to obtain ("
                << numModes << ") must be lesser than N= "
                << n << ".\n" << std::endl;
    int nev= numModes;
    int ncv= getNCV(n, nev);

    // set up the space for ARPACK functions.
    // this is done each time method is called!! .. this needs to be cleaned up
    int mode = 3;
    ArpackAuxVars av(n,ncv,nev,maxitr,mode);


    bool rvec = true;
    int ierr= 0;

    // Do the factorization of matrix (A-dM) here.
    dgbtrf_(&n, &n, &kl, &ku, Aptr, &ldA, iPiv.getDataPtr(), &ierr);

    if(ierr != 0)
      {
	if(ierr<0) // Error in dgbtrf_ argument.
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; error in dgbtrf_: "
		      << ierr << std::endl;
	  }
        if(ierr>0) // Singular matrix.
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; singular matrix (one or more zero eigenvalues)"
		      << std::endl;
	  }
        return -1;
      }
    int info= dsaupd_loop(ncv,nev,av);
    if(info < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; error with dsaupd loop info = " 
                  << info << ' ' << dsaupd_err_string(info) << std::endl;
        return info;
      }
    else
      {
        if(info == 1)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; maximum number of iterations reached."
                      << std::endl;
          }
        else if (info == 3)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; no Shifts could be applied during implicit,";
            std::cerr << " Arnoldi update, try increasing NCV." << std::endl;
          }

        double sigma = theSOE->shift;
        if(av.iparam[4] > 0)
          {
            rvec = true;
            n= theSOE->size;
            av.ldv = n;

            dseupd_(&rvec, &av.howmy, &av.select[0], av.d.getDataPtr(), av.z.getDataPtr(), &av.ldv, &sigma, &av.bmat, &n,
		    which.c_str(),&nev, &tol, &av.resid[0], &ncv, &av.v[0], &av.ldv, av.iparam, av.ipntr, &av.workd[0],
                    &av.workl[0], &av.lworkl, &info);
            if(info != 0)
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; error with dseupd_" << info << ' '
			  << dseupd_err_string(info) << std::endl;
                return info;
              }
          }
      }
    value= av.d;
    eigenvector= av.z;
    theSOE->factored = true;

    return 0;
  }

//! @brief Compute a suitable value for ncv.
int XC::BandArpackSolver::getNCV(int n, int nev)
  { return std::min(std::min(2*nev,nev+8),n); }


void XC::BandArpackSolver::myMv(int n, double *v, double *result)
  {
    Vector x(v, n);
    Vector y(result,n);

    y.Zero();
    AnalysisModel *theAnalysisModel= theSOE->getAnalysisModelPtr();

    // loop over the FE_Elements
    FE_Element *elePtr;
    FE_EleIter &theEles = theAnalysisModel->getFEs();
    while((elePtr = theEles()) != 0)
      {
        const Vector &b = elePtr->getM_Force(x, 1.0);
        y.Assemble(b, elePtr->getID(), 1.0);
      }

    // loop over the DOF_Groups
    DOF_Group *dofGroupPtr= nullptr;
    DOF_GrpIter &theDofGroups = theAnalysisModel->getDOFGroups();
    while ((dofGroupPtr= theDofGroups()) != 0)
      {
        const Vector &a = dofGroupPtr->getM_Force(x,1.0);
        y.Assemble(a,dofGroupPtr->getID(),1.0);
      }
  }

void XC::BandArpackSolver::myCopy(int n, double *v, double *result)
  {
    for(int i=0; i<n; i++)
      { result[i] = v[i]; }
  }

//! @brief Sets the eigenproblem to solve.
bool XC::BandArpackSolver::setEigenSOE(EigenSOE *soe)
  {
    bool retval= false;
    BandArpackSOE *tmp= dynamic_cast<BandArpackSOE *>(soe);
    if(tmp)
      {
        theSOE= tmp;
        retval= true;
      }
    else
      std::cerr << getClassName() << __FUNCTION__
                << ": not a suitable system of equations." << std::endl;
    return retval;
  }

//! @brief Sets the eigenproblem to solve.
bool XC::BandArpackSolver::setEigenSOE(BandArpackSOE &theBandSOE)
  { return setEigenSOE(&theBandSOE); }

//! @brief Returns the autovector corresponding to the mode being passed as parameter.
const XC::Vector &XC::BandArpackSolver::getEigenvector(int mode) const
  {
    if(mode <= 0 || mode > numModes)
      {
        std::cerr << "BandArpackSOE::getEigenvector() - mode is out of range(1 - nev)";
        eigenV.Zero();
        return eigenV;
      }

    const int size= theSOE->size;
    int index= (mode - 1) * size;

    if(!eigenvector.isEmpty())
      {
        for(int i=0; i<size; i++)
          eigenV(i)= eigenvector(index++);
      }
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; eigenvectors not yet determined."
	          << std::endl;
        eigenV.Zero();
      }
    return eigenV;
  }


//! @brief Return the eigenvalue corresponding to the mode being passed as parameter.
const double &XC::BandArpackSolver::getEigenvalue(int mode) const
  {
    static double retval= 0.0;
    if(mode <= 0 || mode > numModes)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; mode is out of range(1 - nev)";
        retval= -1.0;
      }
    if(!value.isEmpty())
      return value[mode-1];
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; eigenvalues not yet determined";
        retval= -2.0;
      }
    return retval;
  }

//! @brief Sets the system size.
int XC::BandArpackSolver::setSize(void)
  {
    int size = theSOE->size;
    if(iPiv.Size() < size)
      iPiv.resize(size);

    if(eigenV.Size() != size)
      eigenV.resize(size);

    return 0;
  }

//! @brief Returns the eigenvectors dimension.
const int &XC::BandArpackSolver::getSize(void) const
  { return theSOE->size; }

//! @brief Estimates the reciprocal of the condition number of a real
//! general band matrix A, in either the 1-norm or the infinity-norm,
//! using the LU factorization computed by DGBTRF.
//!
//! @param c: Specifies whether the 1-norm condition number or the
//!           infinity-norm condition number is required:
//!           = '1' or 'O':  1-norm;
//!           = 'I':         Infinity-norm.
double XC::BandArpackSolver::getRCond(const char &c)
  {
    double retval= 0.0;
    if(!theSOE)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; no LinearSOE object has been set.\n";
	return -1;
      }
    else
      {
	int n = theSOE->size;
	// check iPiv is large enough
	if(iPiv.Size() < n)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; iPiv not large enough - has setSize() been called?\n";
	    return -1;
	  }


	//now solve
	if(theSOE->factored == false) // factorize
	  {
	    theSOE->save(); //Avoid conflict with solve()
	    int kl = theSOE->numSubD;
	    int ku = theSOE->numSuperD;
	    const int ldA = 2*kl + ku +1;
	    int info;
	    double *Aptr= theSOE->A.getDataPtr();
	    int *iPIV = iPiv.getDataPtr();
	    dgbtrf_(&n,&n,&kl,&ku,Aptr,&ldA,iPIV,&info);

	    if(info<0) //info>0 means matrix singular but this is not a problem here
	      std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; LaPack dgbtrf_ failure with error: " << info
			<< std::endl;
	    else
	      {
		info= 0; 
		char norm[1];
		norm[0]= c;
		Vector wrk(3*n);
		double *wrkPtr= wrk.getDataPtr();
		ID iwrk(n);
		int *iwrkPtr= iwrk.getDataPtr();
		const double anorm= dlangb_(norm, &n, &kl, &ku, Aptr, &ldA, wrkPtr);
		if(anorm==0)
		  retval= 0.0; //! singular matrix.
		else
		  dgbcon_(norm,&n,&kl,&ku,Aptr,&ldA,iPIV,&anorm,&retval,wrkPtr,iwrkPtr,&info);
	      }

	    // check if successful
	    if(info != 0)
	      {
		std::cerr << getClassName() << "::" << __FUNCTION__
			  << ";LAPACK routine returned " << info << std::endl;
		return -info;
	      }
	    theSOE->restore(); //Avoid conflict with solve()
	  }
	else
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; matrix already factored, conflict with solve()."
		    << std::endl;
      }
    return retval;
  }

int XC::BandArpackSolver::sendSelf(Communicator &comm)
  { return 0; }

int XC::BandArpackSolver::recvSelf(const Communicator &comm)
  {
    // nothing to do
    return 0;
  }



