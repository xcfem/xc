// -*-c++-*-
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
                                                                        
// Description: This file contains the implementation of MumpsSolver

// $Revision: 1.6 $
// $Date: 2008-04-01 00:35:04 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/mumps/MumpsSolver.cpp,v $

// Written: fmk 
// Created: 02/06

#include "solution/system_of_eqn/linearSOE/mumps/MumpsSOE.h"
#include "solution/system_of_eqn/linearSOE/mumps/MumpsSolver.h"

#define ICNTL(I) icntl[(I)-1] /* macro s.t. indices match documentation */

#include <mpi/mpi.h>

XC::MumpsSolver::MumpsSolver(int ICNTL7, int ICNTL14)
  :MumpsSolverBase(SOLVER_TAGS_MumpsSolver, ICNTL7, ICNTL14),
   theMumpsSOE(nullptr)
  {
    id.ICNTL(7)= icntl7;
    id.ICNTL(14)= icntl14;
  }

//! @brief Virtual constructor.
XC::LinearSOESolver *XC::MumpsSolver::getCopy(void) const
  { return new MumpsSolver(*this); }

XC::MumpsSolver::~MumpsSolver(void)
  { }

int XC::MumpsSolver::initializeMumps(void)
  {
    if(needsSetSize == false)
      { return 0; }
    else
      {
	terminateMumps(); // If MUMPS already running terminate it.
	id.job=-1; 
	id.par=1; // host involved in calcs
	id.sym= theMumpsSOE->matType;

	id.comm_fortran=0; // OPENMPI

        dmumps_c(&id);
	mumps_init= true;
	
	// increment row and col A values by 1 for mumps fortran indexing
	theMumpsSOE->fortranIndexing();
	
	// analyze the matrix
	id.n = theMumpsSOE->size;
	id.nz = theMumpsSOE->nnz;
	id.irn = theMumpsSOE->rowA.getDataPtr();
	id.jcn = theMumpsSOE->colA.getDataPtr();
	id.a = theMumpsSOE->A.getDataPtr();
	id.rhs = theMumpsSOE->X.getDataPtr();
	

	// No outputs 
	id.ICNTL(1) = -1; id.ICNTL(2) = -1; id.ICNTL(3) = -1; id.ICNTL(4) = 0;

	// Call the MUMPS package to factor & solve the system
	id.job= 1; // analysis.
	dmumps_c(&id);

	int info = id.infog[0];
	if(info != 0)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
	 	      << "; WARNING Error " << info
		      << " returned in substitution dmumps()\n";
	    return info;
	  }

	// decrement row and col A values by 1 to return to C++ indexing
	theMumpsSOE->cppIndexing();

        needsSetSize= false;
        return info;
      }
  }

int XC::MumpsSolver::solveAfterInitialization(void)
  {
    const int n= theMumpsSOE->size;

    Vector &X= theMumpsSOE->X;
    Vector &B= theMumpsSOE->B;

    // increment row and col A values by 1 for mumps fortran indexing
    theMumpsSOE->fortranIndexing();

    for(int i=0; i<n; i++)
      X[i] = B[i];

    int info = 0;
    id.n   = theMumpsSOE->size;
    id.nz  = theMumpsSOE->nnz; 
    id.irn = theMumpsSOE->rowA.getDataPtr();
    id.jcn = theMumpsSOE->colA.getDataPtr();
    id.a   = theMumpsSOE->A.getDataPtr(); 
    id.rhs = theMumpsSOE->X.getDataPtr();
    
    // No outputs 
    id.ICNTL(1)=-1; id.ICNTL(2)=-1; id.ICNTL(3)=-1; id.ICNTL(4)=0;
    if(theMumpsSOE->factored == false)
      {
	// Call the MUMPS package to factor & solve the system
	id.job = 5; // (JOB= 3+2= 5) factorize and solve 
	dmumps_c(&id);
	theMumpsSOE->factored = true;
      }
    else
      {
	// Call the MUMPS package to solve the system
	id.job = 3; // (JOB= 3) solve
	dmumps_c(&id);
      }

    info = id.infog[0];
    if(info != 0)
      {	
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING Error " << info
		  << " returned in substitution dmumps()\n"
		  << getMUMPSErrorMessage() << std::endl;
	return info;
      }

    // decrement row and col A values by 1 to return to C++ indexing
    theMumpsSOE->cppIndexing();

    return 0;
  }

bool XC::MumpsSolver::setLinearSOE(LinearSOE *theSOE)
  {
    bool retval= false;
    MumpsSOE *tmp= dynamic_cast<MumpsSOE *>(theSOE);
    if(tmp)
      {
        theMumpsSOE= tmp;
	retval= true;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; not a suitable system of equations object."
	        << std::endl;
    return retval;
  }













