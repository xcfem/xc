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

// $Revision: 1.4 $
// $Date: 2008/09/23 22:52:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/eigenSOE/FullGenEigenSolver.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 11/07
// Revision: A
//
// Description: This file contains the implementation of the
// FullGenEigenSolver class.

#include "FullGenEigenSolver.h"
#include "FullGenEigenSOE.h"
#include <f2c.h>
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/FE_EleIter.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/analysis/integrator/Integrator.h>
#include "utility/matrix/Vector.h"





extern "C" int dggev_(char *JOBVL, char *JOBVR, int *N, double *A, int *LDA,
                      double *B, int *LDB, double *ALPHAR, double *ALPHAI,
                      double *BETA, double *VL, int *LDVL, double *VR,
                      int *LDVR, double *WORK, int *LWORK, int *INFO);

XC::FullGenEigenSolver::FullGenEigenSolver(void)
   : EigenSolver(EigenSOLVER_TAGS_FullGenEigenSolver), theSOE(nullptr), eigenvalue(0), sortingID(0)
 {}


int XC::FullGenEigenSolver::solve(void)
  {return this->solve(theSOE->size);}


int XC::FullGenEigenSolver::solve(int nEigen)
  {
    if(theSOE == 0)
      {
        std::cerr << "FullGenEigenSolver::solve()- "
            << " No EigenSOE object has been set yet\n";
        return -1;
      }

    // check for quick return
    if(nEigen < 1)
      {
        numModes = 0;
        return 0;
      }

    // get the number of equations
    int n= theSOE->size;

    // set the number of eigenvalues
    numModes = nEigen;
    if(numModes > n)
      {
	std::clog << "FullGenEigenSolver::solve; number of nodes: " << numModes
                  << " is greater than the maximum: " << n 
                  << " we take the maximum." << std::endl;
        numModes = n;
      }

    // do not compute left eigenvalues and eigenvectors
    char jobvl[]= "N";

    // compute right eigenvalues and eigenvectors
    char jobvr[]= "V";

    // stiffness matrix data
    double *Kptr = theSOE->A.getDataPtr();

    // leading dimension of K
    int ldK = n;

    // mass matrix data
    double *Mptr = theSOE->M.getDataPtr();

    // leading dimension of M
    int ldM = n;

    // allocate memory for eigenvalues
    std::vector<double> alphaR(n);
    std::vector<double> alphaI(n);
    std::vector<double> beta(n);

    eigenvalue.resize(n);

    // allocate memory for sorting index array
    sortingID.resize(n);

    // dummy left eigenvectors
    double vl[1];

    // leading dimension of dummy left eigenvectors
    int ldvl = 1;

    // allocate memory for right eigenvectors
    eigenvector.resize(n*n);

    // leading dimension of right eigenvectors
    int ldvr = n;

    // dimension of the workspace array
    int lwork = n*(8+64);

    // allocate memory for workspace array
    std::vector<double> work(lwork);

    // output information
    int info = 0;

    // call the LAPACK eigenvalue subroutine
    dggev_(jobvl, jobvr, &n, Kptr, &ldK, Mptr, &ldM, &alphaR[0], &alphaI[0], &beta[0],
           vl, &ldvl, eigenvector.getDataPtr(), &ldvr, &work[0], &lwork, &info);

    if(info < 0)
      {
        std::cerr << "FullGenEigenSolver::solve() - invalid argument number "
                  << -info << " passed to LAPACK dggev routine\n";
        return info;
      }

    if(info > 0) {
        std::cerr << "FullGenEigenSolver::solve() - the LAPACK dggev routine "
            << "returned error code " << info << std::endl;
        return -info;
    }

    theSOE->factored = true;

    for(int i=0; i<n; i++)
      {
        const double mag = sqrt(alphaR[i]*alphaR[i]+alphaI[i]*alphaI[i]);
        if(mag*DBL_EPSILON < fabs(beta[i]))
          {
            if(alphaI[i] == 0.0)
              { eigenvalue[i] = alphaR[i]/beta[i]; }
            else
              {
                eigenvalue[i] = -mag/beta[i];
                std::cerr << "FullGenEigenSolver::solve() - the eigenvalue "
                    << i+1 << " is complex with magnitude "
                    << -eigenvalue[i] << std::endl;
              }
          }
        else
          { eigenvalue[i]= DBL_MAX; }
        sortingID[i] = i;
      }


    // sort eigenvalues in ascending order and return sorting ID
    this->sort(n, eigenvalue, sortingID);

    for(int i=0; i<numModes; i++)
      {
        if(eigenvalue[i] == DBL_MAX)
          {
	    std::cerr << "FullGenEigenSolver::solve() - the eigenvalue "
		    << i+1 << " is numerically undetermined or infinite\n";
          }
      }

    int lworkOpt = (int) work[0];
    if(lwork < lworkOpt) {
        std::cerr << "FullGenEigenSolver::solve() - optimal workspace size "
                << lworkOpt << " is larger than provided workspace size "
                << lwork << " consider increasing workspace\n";
    }

    // clean up the memory
    return 0;
  }


int XC::FullGenEigenSolver::setSize(void)
  {
    int size = theSOE->size;

    if(eigenV.Size() != size)
      eigenV.resize(size);
    return 0;
  }


//! @brief Return the eigenvectors dimension.
const int &XC::FullGenEigenSolver::getSize(void) const
  { return theSOE->size; }

//! @brief Sets the eigenproblem to solve.
bool XC::FullGenEigenSolver::setEigenSOE(EigenSOE *soe)
  {
    bool retval= false;
    FullGenEigenSOE *tmp= dynamic_cast<FullGenEigenSOE *>(soe);
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
bool XC::FullGenEigenSolver::setEigenSOE(FullGenEigenSOE &thesoe)
  { return setEigenSOE(&thesoe); }


const XC::Vector& XC::FullGenEigenSolver::getEigenvector(int mode) const
  {
    if(mode <= 0 || mode > numModes)
      {
        std::cerr << "FullGenEigenSolver::getEigenVector() - mode "
            << mode << " is out of range (1 - " << numModes << ")\n";
        eigenV.Zero();
        return eigenV;
      }

    int size = theSOE->size;
    int index = size*sortingID(mode-1);

    if(!eigenvector.isEmpty())
      {
        for(int i=0; i<size; i++)
          { eigenV(i) = eigenvector[index++]; }	
      }
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__ << "; "
            << "eigenvectors not computed yet\n";
        eigenV.Zero();
      }      
    return eigenV;
  }


const double &XC::FullGenEigenSolver::getEigenvalue(int mode) const
  {
    static const double retval= 0.0;
    if(mode <= 0 || mode > numModes)
      {
        std::cerr << "FullGenEigenSolver::getEigenvalue() - mode " 
            << mode << " is out of range (1 - " << numModes << ")\n";
        return retval;
      }
    else
      {
        if(!eigenvalue.isEmpty())
          { return eigenvalue[mode-1]; }
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__ << "; "
                      << "eigenvalues not yet computed\n";
            return retval;
          }
      }
  }


int XC::FullGenEigenSolver::sendSelf(CommParameters &cp)
  { return 0; }


int XC::FullGenEigenSolver::recvSelf(const CommParameters &cp)
  { return 0; }


void XC::FullGenEigenSolver::sort(int length, Vector &x,ID &id)
  {
    // this is an implementation of shell sort that
    // additionally keeps track of the sorting order
    int flag = 1;
    int d = length;
    int i, idTmp;
    double xTmp;
    
    while (flag || d>1)
      {
        flag = 0;
        d = (d+1)/2;
        for(i=0; i<(length-d); i++)
          {
            if(x[i+d] < x[i])
              {
                // swap items at positions i+d and d
	            xTmp = x[i+d];  idTmp = id[i+d]; 
	            x[i+d] = x[i];  id[i+d] = id[i]; 
	            x[i] = xTmp;    id[i] = idTmp; 
	            // indicate that a swap has occurred
	            flag = 1;
              }
          }
      }
    return;
  }
