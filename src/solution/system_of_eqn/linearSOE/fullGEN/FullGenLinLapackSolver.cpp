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
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/fullGEN/FullGenLinLapackSolver.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/FullGEN/FullGenLinLapackSolver.h
//
// Written: fmk 
// Created: Tue Sep 26 16:27:47: 1996
// Revision: A
//
// Description: This file contains the class definition for 
// FullGenLinLapackSolver. It solves the XC::FullGenLinSOE object by calling
// Lapack routines.
//
// What: "@(#) FullGenLinLapackSolver.h, revA"

#include <solution/system_of_eqn/linearSOE/fullGEN/FullGenLinLapackSolver.h>
#include <solution/system_of_eqn/linearSOE/fullGEN/FullGenLinSOE.h>

//! @brief Constructor.
//!
//! A unique class tag defined in classTags.h is passed to the
//! FullGenLinSolver constructor. Sets the size of #iPiv to 0, 
//! \p iPiv being an integer array needed by the LAPACK routines.
XC::FullGenLinLapackSolver::FullGenLinLapackSolver(void)
  : FullGenLinSolver(SOLVER_TAGS_FullGenLinLapackSolver) {}


extern "C" int dgesv_(int *N, int *NRHS, double *A, int *LDA, int *iPiv, 
		      double *B, int *LDB, int *INFO);

extern "C" int dgetrs_(char *TRANS, int *N, int *NRHS, double *A, int *LDA, 
		       int *iPiv, double *B, int *LDB, int *INFO);

//! @brief Computes the solution.
//!
//! First copies B into X and then solves the FullGenLinSOE system 
//! it is associated with (pointer kept by parent class) by calling the LAPACK 
//! routines dgesv(), if the system is marked as not having been factored,
//! or dgetrs(), if system is marked as having been factored. If the
//! solution is successfully obtained, i.e. the LAPACK routines return 0
//! in the INFO argument, it marks the system has having been
//! factored and returns 0, otherwise it prints a warning message and
//! returns INFO. The solve process changes A and X.
int XC::FullGenLinLapackSolver::solve(void)
  {
    if(!theSOE)
      {
	std::cerr << "WARNING XC::FullGenLinLapackSolver::solve(void)- ";
	std::cerr << " No XC::LinearSOE object has been set\n";
	return -1;
      }
    
    int n= theSOE->size;
    
    // check for quick return
    if(n == 0)
      return 0;
    
    // check iPiv is large enough
    if(iPiv.Size() < n)
      {
	std::cerr << "WARNING XC::FullGenLinLapackSolver::solve(void)- ";
	std::cerr << " iPiv not large enough - has setSize() been called?\n";
	return -1;
      }	
	
    int ldA= n;
    int nrhs= 1;
    int ldB= n;
    int info;
    double *Aptr = theSOE->A.getDataPtr();
    double *Xptr = theSOE->getPtrX();
    double *Bptr = theSOE->getPtrB();
    int *iPIV= iPiv.getDataPtr();
    
    // first copy B into X
    for(int i=0; i<n; i++)
      *(Xptr++) = *(Bptr++);
    Xptr= theSOE->getPtrX();

    // now solve AX = Y

    char strN[]= "N";
    {if (theSOE->factored == false)      
	dgesv_(&n,&nrhs,Aptr,&ldA,iPIV,Xptr,&ldB,&info);
     else
	dgetrs_(strN, &n,&nrhs,Aptr,&ldA,iPIV,Xptr,&ldB,&info);
    }
    
    // check if successfull
    if(info != 0)
      {
	std::cerr << nombre_clase() << "::" << __FUNCTION__
	          << "; lapack solver failed - " << info
		  << " returned.\n";
	return -info;
    }
    
    theSOE->factored = true;
    return 0;
  }
//! @brief Sets the size of #iPiv from the size of the system of equations.
//!
//! Is used to construct a 1d integer array, #iPiv that is needed by
//! the LAPACK solvers. It checks to see if current size of #iPiv is
//! large enough, if not it resizes it. Returns 0 if sucessfull,
//! prints a warning message and returns a -1 if not enough memory
//! is available for this new array.
int XC::FullGenLinLapackSolver::setSize(void)
  {
    const int n = theSOE->size;
    if(n > 0)
      {
	if(iPiv.Size() < n)
          iPiv.resize(n);
      }
    else if(n == 0)
      return 0;
    else
      {
	std::cerr << nombre_clase() << "::" << __FUNCTION__
	          << "; ran out of memory.\n";
	return -1;	
      }
    return 0;
  }

//! @brief Does nothing.
int XC::FullGenLinLapackSolver::sendSelf(CommParameters &)

  {
    // nothing to do
    return 0;
  }

//! @brief Does nothing.
int XC::FullGenLinLapackSolver::recvSelf(const CommParameters &)
  {
    // nothing to do
    return 0;
  }



