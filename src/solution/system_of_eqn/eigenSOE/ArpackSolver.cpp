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
// File: ~/system_of_eqn/eigenSOE/ArpackSolver.C
//
// Written: fmk
// Created: 05.09
//
// This is the solver that works on the ArpackSOE. It uses the LinearSOE
// in the SOE to perform the solve() operation if required.
// It uses the ARPACK library to perform the eigenvalue analysis.
// ARPACK is an eigen analysis package which was developed by 
// R.B.Lehoucq, D.C.Sorensen and C.Yang at Rice University. ARPACK is a
// collection of FORTRAN77 subroutines designed to solve large scale eigen
// problems. ARPACK is capable of solving large scale non-Hermitian standard 
// and generalized eigen problems. When the matrix <B>K</B> is symmetric, 
// the method is a variant of the Lanczos process called Implicitly Restarted
// Lanczos Method (IRLM).

//
// It is based on previous work of Jun Peng(Stanford)
//


#include <solution/system_of_eqn/eigenSOE/ArpackSolver.h>
#include "solution/system_of_eqn/eigenSOE/ArpackSOE.h"
#include "solution/system_of_eqn/linearSOE/LinearSOE.h"
#include <cstdio>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/FE_EleIter.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/analysis/integrator/Integrator.h>
#include <cstring>
#include "utility/utils/misc_utils/mchne_eps.h"
#include <f2c.h>
#include "utility/actor/channel/Channel.h"

static std::vector<double> workArea;

extern "C" int dsaupd_(int *ido, char* bmat, int *n, char *which, int *nev, 
		       double *tol, double *resid, int *ncv, double *v, int *ldv,
		       int *iparam, int *ipntr, double *workd, double *workl,
		       int *lworkl, int *info);

extern "C" int dseupd_(bool *rvec, char *howmny, logical *select, double *d, double *z,
		       int *ldz, double *sigma, char *bmat, int *n, char *which,
		       int *nev, double *tol, double *resid, int *ncv, double *v,
		       int *ldv, int *iparam, int *ipntr, double *workd, 
		       double *workl, int *lworkl, int *info);

//! @brief Constructor.
XC::ArpackSolver::ArpackSolver(const int &nModes)
  :EigenSolver(EigenSOLVER_TAGS_ArpackSolver, nModes),
 theSOE(nullptr), numModesMax(0), numMode(0), size(0)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; Work in progres. Not yet ready." << std::endl;
  }

void XC::ArpackSolver::saupd_error_msg(const int &info)
  {
      std::cerr << getClassName() << "::" << __FUNCTION__ << "ArpackSolver::Error with _saupd info = " << info << std::endl;
      switch(info)
	{
	case -1: 
	  std::cerr << "N must be positive.\n";
	  break;
	case -2: 
	  std::cerr << "NEV must be positive.\n";
	  break;
	case -3: 
	  std::cerr << "NCV must be greater than NEV and less than or equal to N.\n";
	  break;
	case -4:
	  std::cerr << "The maximum number of Arnoldi update iterations allowed\n";
	  break;
	case -5: 
	  std::cerr << "WHICH must be one of 'LM', 'SM', 'LA', 'SA' or 'BE'.\n";
	  break;
	case -6: 
	  std::cerr << "BMAT must be one of 'I' or 'G'.\n";
	  break;
	case -7: 
	  std::cerr << "Length of private work array WORKL is not sufficient.\n";
	  break;
	case -8: 
	  std::cerr << "Error return from trid. eigenvalue calculation\n";
	  std::cerr << "Informatinal error from LAPACK routine dsteqr.\n";
	  break;
	case -9: 
	  std::cerr << "Starting vector is zero.\n";
	  break;
	case -10: 
	  std::cerr << "IPARAM(7) must be 1,2,3,4,5.\n";
	  break;
	case -11: 
	  std::cerr << "IPARAM(7) = 1 and BMAT = 'G' are incompatible.\n";
	  break;
	case -12: 
	  std::cerr << "IPARAM(1) must be equal to 0 or 1.\n";
	  break;
	case -13:
	  std::cerr << "NEV and WHICH = 'BE' are incompatible.\n";
	  break;
	case -9999:
	  std::cerr << "Could not build an Arnoldi factorization.\n";
	  std::cerr << "IPARAM(5) - the size of the current Arnoldi factorization is " << iparam[4] << ".\n";
	  std::cerr << "The user is advised to check that enough workspace and array storage have been allocated.\n";
	  break;
	default:
	  std::cerr << "unrecognised return value\n";
	}
  }

void XC::ArpackSolver::seupd_error_msg(const int &info)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__ << "ArpackSolver::Error with dseupd_" << info;
    switch(info)
      {
      case -1: 
	std::cerr << " N must be positive.\n";
	break;
      case -2: 
	std::cerr << " NEV must be positive.\n";
	break;
      case -3: 
	std::cerr << " NCV must be greater than NEV and less than or equal to N.\n";
	break;
      case -5: 
	std::cerr << " WHICH must be one of 'LM', 'SM', 'LA', 'SA' or 'BE'.\n";
	break;
      case -6: 
	std::cerr << " BMAT must be one of 'I' or 'G'.\n";
	break;
      case -7: 
	std::cerr << " Length of private work WORKL array is not sufficient.\n";
	break;
      case -8: 
	std::cerr << " Error return from trid. eigenvalue calculation";
	std::cerr << "Information error from LAPACK routine dsteqr.\n";
	break;
      case -9: 
	std::cerr << " Starting vector is zero.\n";
	break;
      case -10: 
	std::cerr << " IPARAM(7) must be 1,2,3,4,5.\n";
	break;
      case -11: 
	std::cerr << " IPARAM(7) = 1 and BMAT = 'G' are incompatibl\n";
	break;
      case -12: 
	std::cerr << " NEV and WHICH = 'BE' are incompatible.\n";
	break;
      case -14: 
	std::cerr << " DSAUPD did not find any eigenvalues to sufficient accuracy.\n";
	break;
      case -15: 
	std::cerr << " HOWMNY must be one of 'A' or 'S' if RVEC = .true.\n";
	break;
      case -16: 
	std::cerr << " HOWMNY = 'S' not yet implemented\n";
	break;
      default:
	;
      }
  }

//! @brief Solves the eigenproblem.
int XC::ArpackSolver::solve(void)
  {
    if(generalized == false)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; at moment only solves generalized problems.\n";
	return -1;
      }
    theSOE = theArpackSOE->theSOE;
    if(!theSOE)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; no LinearSOE object has been set\n";
        return -1;
      }

    // set up the space for ARPACK functions.
    // this is done each time method is called!! .. this needs to be cleaned up

    int n = size;
    int nev = numModes;
    int ncv = getNCV(n, nev);
    int ldv = n;
    int lworkl = ncv*ncv + 8*ncv;

    int processID = theArpackSOE->processID;

    // set up the space for ARPACK functions.
    // this is done each time method is called!! .. this needs to be cleaned up
    if(numModes > numModesMax)
      {

	v.resize(ldv * ncv);
	workl.resize(lworkl + 1);
	workd.resize(3 * n + 1);
	eigenvalues.resize(nev);
	eigenvectors.resize(n * nev);
	resid.resize(n);
	select.resize(ncv);

        for(int i=0; i<lworkl+1; i++)
	  workl[i]= 0;
        for(int i=0; i<3*n+1; i++)
	  workd[i]= 0;
        for(int i=0; i<ldv*ncv; i++)
	  v[i] = 0;
        numModesMax = numModes;
      }

    std::string which= getWhich();

    char bmat = 'G';
    char howmy = 'A';

    // some more variables
    double tol = 0.0;
    int info = 0;
    int maxitr = 1000;
    int mode = 3;

    iparam[0] = 1;
    iparam[2] = maxitr;
    iparam[6] = mode; 


    bool rvec = true;

    int ido = 0;
    int ierr = 0;

    while(1)
      {
	char *which_no_const= const_cast<char *>(which.c_str());
	dsaupd_(&ido, &bmat, &n, which_no_const, &nev, &tol, resid.data(), &ncv, v.data(), &ldv,
		iparam, ipntr, workd.data(), workl.data(), &lworkl, &info);

	  if(theArpackSOE->checkSameInt(ido) != 1)
	    {
		  std::cerr << getClassName() << "::" << __FUNCTION__
			    << "; ido values not the same .. ido, processID: "
			    << ido << " " << processID << std::endl;
		  return -1;
	    }

        if(ido == -1)
	  {
            myMv(n, &workd[ipntr[0]-1], &workd[ipntr[1]-1]); 

	    eigenV.setData(&workd[ipntr[1] - 1], size);

	    if (processID > 0)
	      theSOE->zeroB();
	    else
	      theSOE->setB(eigenV);

	    ierr = theSOE->solve();
	    const Vector &X = theSOE->getX();
	    eigenV = X;

	    continue;
          }
	else if(ido == 1)
	  {
	    //  double ratio = 1.0;
	    myCopy(n, &workd[ipntr[2]-1], &workd[ipntr[1]-1]);
	    //std::cerr << "ArpackSOE::solver) - 1 before SOE-setVector\n"; 
	    eigenV.setData(&workd[ipntr[1] - 1], size);
		 //std::cerr << "ArpackSOE::solver) - 1 after SOE-setEVctor\n";
	    if(processID > 0)
	      theSOE->zeroB();
	    else
	      theSOE->setB(eigenV);

	    theSOE->solve();

	    const Vector &X = theSOE->getX();
	    eigenV = X;
	    //      eigenV.setData(&workd[ipntr[1] - 1], size);

	    continue;
          }
	else if(ido == 2)
	  {     
  	    myMv(n, &workd[ipntr[0]-1], &workd[ipntr[1]-1]);
            continue;
          }
        break;
      }

    if(info < 0)
      {
	saupd_error_msg(info);

        eigenvalues.resize(0);
        eigenvectors.resize(0);

        return info;
      }
    else
      {
	if (info == 1)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; maximum number of iteration reached." << std::endl;
	  }
	else if(info == 3)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; no shifts could be applied during implicit,"
		      << "Arnoldi update, try increasing NCV." << std::endl;
	  }

	double sigma= shift;
	if(iparam[4] > 0)
	  {
	    rvec = true;
	    n = size;    
	    ldv = n;
            char *which_no_const= const_cast<char *>(which.c_str());
	    dseupd_(&rvec, &howmy, (logical*)(select.data()), eigenvalues.data(), eigenvectors.data(), &ldv, &sigma, &bmat, &n, which_no_const, &nev, &tol, resid.data(), &ncv, v.data(), &ldv, iparam, ipntr, workd.data(), workl.data(), &lworkl, &info);
	    if(info != 0)
	      {
		seupd_error_msg(info);
                this->setPyProp("info", boost::python::object(info));
		return info;
	      }
	  }
      }

    numMode = numModes;

    /*
    ofstream outfile;
    outfile.open("eigenvectors.dat", ios::out);
    for (int i=0; i<n; i++) {
      for (int j=0; j<numMode; j++) {
	//  int index = (mode - 1) * size;
	//  eigenV.setData(&eigenvectors[index], size);
	outfile << eigenvectors[j*n + i] << " ";
      }
      outfile << "\n";
    }
    outfile.close();

    bool mDiagonal = theArpackSOE->mDiagonal;
    if (mDiagonal == true) {
      outfile.open("mass.dat", ios::out);
      int Msize = theArpackSOE->Msize;
      double *M = theArpackSOE->M;
      if (n <= Msize) {
	for (int i=0; i<n; i++)
	  outfile << M[i] << " ";
      }
      outfile.close();
    }
    */

    // clean up the memory
    return 0;
  }

//! @brief Compute a suitable value for ncv.
int XC::ArpackSolver::getNCV(int n, int nev)
  {
    int result;
    if(2*nev > nev+8)
      { result = nev+8; }
    else
      { result = 2*nev; }
    if(result >= n)
      { result = n; }
    return result;
  }


void XC::ArpackSolver::myMv(int n, double *v, double *result)
  {
    Vector x(v, n);
    Vector y(result,n);

    bool mDiagonal = theArpackSOE->mDiagonal;

    if (mDiagonal == true)
      {
	const int Msize = theArpackSOE->Msize;
	std::vector<double> &M= theArpackSOE->M;

	/* for output
	DataFileStream dataStream("M.txt");
	dataStream.open();
	for (int i=0; i<n; i++)
	  dataStream << M[i] << endln;
	dataStream.close();
	*/

	if(n <= Msize)
	  {
	    for(int i=0; i<n; i++)
	      result[i] = M[i]*v[i];
	  }
	else
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
	 	      << "; n > Msize!\n";
	    return;
	  }

      }
    else
      {
	y.Zero();

	AnalysisModel *theAnalysisModel = theArpackSOE->theModel;
	// loop over the FE_Elements
	FE_Element *elePtr;
	FE_EleIter &theEles = theAnalysisModel->getFEs();    
	while((elePtr = theEles()) != 0)
	  {
	    const Vector &b = elePtr->getM_Force(x, 1.0);
	    y.Assemble(b, elePtr->getID(), 1.0);
	  }

	// loop over the DOF_Groups
	DOF_Group *dofPtr;
	DOF_GrpIter &theDofs = theAnalysisModel->getDOFGroups();
	while ((dofPtr = theDofs()) != 0)
	  {
	    const Vector &a = dofPtr->getM_Force(x,1.0);      
	    y.Assemble(a, dofPtr->getID(), 1.0);
	  }
      }

    // if paallel we have to merge the results
    int processID = theArpackSOE->processID;
    if (processID != -1)
      {
	ChannelQueue &theChannels= theArpackSOE->theChannels;
	int numChannels = theChannels.size();
	if(processID != 0)
	  {
	    theChannels[0]->sendVector(0, 0, y);
	    theChannels[0]->recvVector(0, 0, y);
	  }
	else
	  {
	    Vector other(workArea.data(), n);
	    // recv contribution from remote & add
	    for(int i=0; i<numChannels; i++)
	      {
	        theChannels[i]->recvVector(0,0,other);
	        y+= other;
	      }
	    // send result back
	    for(int i=0; i<numChannels; i++)
	      { theChannels[i]->sendVector(0,0,y); }
	  }
      }
  }

void XC::ArpackSolver::myCopy(int n, double *v, double *result)
  {
    for(int i=0; i<n; i++)
      { result[i] = v[i]; }
  }

//! @brief Sets the eigenproblem to solve.
bool XC::ArpackSolver::setEigenSOE(EigenSOE *soe)
  {
    bool retval= false;
    ArpackSOE *tmp= dynamic_cast<ArpackSOE *>(soe);
    if(tmp)
      {
        theArpackSOE= tmp;
        shift= theArpackSOE->getShift();
        retval= true;
      }
    else
      std::cerr << getClassName() << __FUNCTION__
                << ": not a suitable system of equations." << std::endl;
    return retval;
  }

//! @brief Sets the eigenproblem to solve.
bool XC::ArpackSolver::setEigenSOE(ArpackSOE &theBandSOE)
  { return setEigenSOE(&theBandSOE); }

//! @brief Returns the autovector corresponding to the mode being passed as parameter.
const XC::Vector &XC::ArpackSolver::getEigenvector(int mode) const
  {
    if(mode <= 0 || mode > numModes)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; mode is out of range(1 - nev)";
        eigenV.Zero();
        return eigenV;
      }

    const int size= theSOE->getNumEqn();
    int index= (mode - 1) * size;

    if(!eigenvectors.empty())
      {
        for(int i=0; i<size; i++)
          eigenV(i)= eigenvectors[index++];
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
const double &XC::ArpackSolver::getEigenvalue(int mode) const
  {
    static double retval= 0.0;
    if(mode <= 0 || mode > numModes)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; mode is out of range(1 - nev)";
        retval= -1.0;
      }
    if(!eigenvalues.empty())
      return eigenvalues[mode-1];
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; eigenvalues not yet determined";
        retval= -2.0;
      }
    return retval;
  }

//! @brief Sets the system size.
int XC::ArpackSolver::setSize(void)
  {
    size = theArpackSOE->Msize;
    const int sizeWork= workArea.size();

    if(sizeWork < size)
      workArea.resize(size);
    return 0;
  }

//! @brief Returns the eigenvectors dimension.
const int &XC::ArpackSolver::getSize(void) const
  { return size; }

//! @brief Estimates the reciprocal of the condition number of a real
//! general band matrix A, in either the 1-norm or the infinity-norm,
//! using the LU factorization computed by DGBTRF.
//!
//! @param c: Specifies whether the 1-norm condition number or the
//!           infinity-norm condition number is required:
//!           = '1' or 'O':  1-norm;
//!           = 'I':         Infinity-norm.
double XC::ArpackSolver::getRCond(const char &c)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet." << std::endl;
    return -1;
  }

int XC::ArpackSolver::sendSelf(Communicator &comm)
  { return 0; }

int XC::ArpackSolver::recvSelf(const Communicator &comm)
  {
    // nothing to do
    return 0;
  }



