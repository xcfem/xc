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

extern "C" int dpbsv_(char *UPLO, int *N, int *KD, int *NRHS, 
		      double *A, int *LDA, double *B, int *LDB, 
		      int *INFO);

extern "C" int dpbtrs_(char *UPLO, int *N, int *KD, int *NRHS, 
		       double *A, int *LDA, double *B, int *LDB, 
		       int *INFO);
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
    if(!theSOE)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; no LinearSOE object has been set\n";
	return -1;
      }

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
	return -info;
      }
    theSOE->factored = true;

    return 0;
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



