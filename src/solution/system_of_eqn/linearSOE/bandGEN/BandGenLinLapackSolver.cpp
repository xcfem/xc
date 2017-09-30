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
// $Date: 2003/04/02 22:02:51 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/bandGEN/BandGenLinLapackSolver.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/bandGEN/BandGenLinLapackSolver.h
//
// Written: fmk 
// Created: Tue Sep 26 16:27:47: 1996
// Revision: A
//
// Description: This file contains the class definition for 
// BandGenLinLapackSolver. It solves the XC::BandGenLinSOE object by calling
// Lapack routines.
//
// What: "@(#) BandGenLinLapackSolver.h, revA"

#include <solution/system_of_eqn/linearSOE/bandGEN/BandGenLinLapackSolver.h>
#include <solution/system_of_eqn/linearSOE/bandGEN/BandGenLinSOE.h>


//! A unique class tag defined in classTags.h is passed to the
//! BandGenLinSolver constructor.
XC::BandGenLinLapackSolver::BandGenLinLapackSolver(void)
  :BandGenLinSolver(SOLVER_TAGS_BandGenLinLapackSolver) {}


extern "C" int dgbsv_(int *N, int *KL, int *KU, int *NRHS, double *A, 
		      int *LDA, int *iPiv, double *B, int *LDB, int *INFO);
		      

extern "C" int dgbtrs_(char *TRANS, int *N, int *KL, int *KU, int *NRHS, 
		       double *A, int *LDA, int *iPiv, double *B, int *LDB, 
		       int *INFO);

//! @brief Performs the solution of the system of equations.
//!
//! The solver first copies the B vector into X and then solves the
//! BandGenLinSOE system by calling the LAPACK routines 
//! dgbsv(), if the system is marked as not having been factored, and
//! dgbtrs() if system is marked as having been factored. If the
//! solution is successfully obtained, i.e. the LAPACK routines return 0
//! in the INFO argument, it marks the system has having been
//! factored and returns $0$, otherwise it prints a warning message and
//! returns INFO. The solve process changes A and X.
int XC::BandGenLinLapackSolver::solve(void)
  {
    if(theSOE == 0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; no LinearSOE object has been set.\n";
	return -1;
      }

    int n = theSOE->size;    
    // check iPiv is large enough
    if(iPiv.Size() < n)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; iPiv not large enough - has setSize() been called?\n";
	return -1;
      }

    int kl = theSOE->numSubD;
    int ku = theSOE->numSuperD;
    int ldA = 2*kl + ku +1;
    int nrhs = 1;
    int ldB = n;
    int info;
    double *Aptr = theSOE->A.getDataPtr();
    double *Xptr = theSOE->getPtrX();
    double *Bptr = theSOE->getPtrB();
    int    *iPIV = iPiv.getDataPtr();
    
    // first copy B into X
    for(int i=0; i<n; i++)
      {	*(Xptr++) = *(Bptr++); }
    Xptr = theSOE->getPtrX();

    // now solve AX = B

    {
      if(theSOE->factored == false) // factor and solve 	
        dgbsv_(&n,&kl,&ku,&nrhs,Aptr,&ldA,iPIV,Xptr,&ldB,&info);
      else // solve only using factored matrix
        {
          char ene[]= "N";
          dgbtrs_(ene,&n,&kl,&ku,&nrhs,Aptr,&ldA,iPIV,Xptr,&ldB,&info);
        }
    }

    // check if successfull
    if(info != 0)
      {
	std::cerr << "WARNING XC::BandGenLinLapackSolver::solve() -";
	std::cerr << "LAPACK routine returned " << info << std::endl;
	return -info;
      }

    theSOE->factored = true;
    return 0;
  }
    

//! @brief Sets the size of #iPiv.
//!
//! Is used to construct a 1d integer array, #iPiv that is needed by
//! the LAPACK solvers. It checks to see if current size of #iPiv is
//! large enough, if not resizes it. Returns 0 if sucessfull, prints
//! a warning message and returns a -1 if not enough memory is
//! available for this new array.
int XC::BandGenLinLapackSolver::setSize()
  {
    // if iPiv not big enough, resize it.
    if(iPiv.Size() < theSOE->size)
      iPiv.resize(theSOE->size);
    return 0;
  }

int XC::BandGenLinLapackSolver::sendSelf(CommParameters &cp)
  { return 0; }

int XC::BandGenLinLapackSolver::recvSelf(const CommParameters &cp)
  {
    // nothing to do
    return 0;
  }
