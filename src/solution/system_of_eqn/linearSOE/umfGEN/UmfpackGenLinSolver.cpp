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
// $Date: 2003/04/02 22:02:53 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSolver.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSolver.C
//
// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the class definition for 
// UmfpackGenLinSolver. It solves the UmfpackGenLinSOEobject by calling
// UMFPACK2.2.1 routines.
//
// What: "@(#) UmfpackGenLinSolver.C, revA"

#include <solution/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSOE.h>
#include <solution/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSolver.h>

void XC::UmfpackGenLinSolver::free_symbolic(void)
  {
    if(Symbolic)
      {
	umfpack_di_free_symbolic(&Symbolic);
	Symbolic= nullptr;
      }
  }

XC::UmfpackGenLinSolver::UmfpackGenLinSolver()
 : LinearSOESolver(SOLVER_TAGS_UmfpackGenLinSolver),
   Symbolic(nullptr), theSOE(nullptr)
  {}

XC::LinearSOESolver *XC::UmfpackGenLinSolver::getCopy(void) const
   { return new UmfpackGenLinSolver(*this); }

//! @brief Destructor.
XC::UmfpackGenLinSolver::~UmfpackGenLinSolver()
  { free_symbolic(); }

int XC::UmfpackGenLinSolver::solve(void)
  {
    const int n = theSOE->X.Size();
    const int nnz = static_cast<int>(theSOE->Ai.size());
    if(n == 0 || nnz==0)
      return 0;
    
    int *Ap= &(theSOE->Ap[0]);
    int *Ai= &(theSOE->Ai[0]);
    double *Ax = &(theSOE->Ax[0]);
    double *X = theSOE->X.getDataPtr();
    double *B = theSOE->B.getDataPtr();

    // check if symbolic is done
    if(!Symbolic)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING: setSize has not been called.\n";
	return -1;
      }
    
    // numerical analysis
    void *Numeric= nullptr;
    int status = umfpack_di_numeric(Ap,Ai,Ax,Symbolic,&Numeric,Control,Info);

    // check error
    if(status!=UMFPACK_OK)
      {
	std::cerr  << getClassName() << "::" << __FUNCTION__
		   <<"; WARNING: numeric analysis returns "<< status
		   << std::endl;
	if(status==UMFPACK_WARNING_singular_matrix)
	  std::cerr << " Singular matrix. Numeric factorization was successful, but the matrix is singular." << std::endl;
	if(status==UMFPACK_ERROR_out_of_memory)
	  std::cerr << " Insufficient memory to complete the numeric factorization." << std::endl;
	if(status==UMFPACK_ERROR_argument_missing)
	  std::cerr << " One or more required arguments are missing." << std::endl;
	if(status==UMFPACK_ERROR_invalid_Symbolic_object)
	  std::cerr << " Symbolic object provided as input is invalid." << std::endl;
	if(status==UMFPACK_ERROR_different_pattern)
	  std::cerr << " Different pattern." << std::endl;
	return -1;
      }

    // solve
    status= umfpack_di_solve(UMFPACK_A,Ap,Ai,Ax,X,B,Numeric,Control,Info);

    // delete Numeric
    if(Numeric)
      { umfpack_di_free_numeric(&Numeric); }
    
    // check error
    if(status!=UMFPACK_OK)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING: solving returns "<< status
		  << std::endl;
	return -1;
      }

    return 0;
  }


int XC::UmfpackGenLinSolver::setSize()
  {
    // set default control parameters
    umfpack_di_defaults(Control);
    Control[UMFPACK_PIVOT_TOLERANCE] = 1.0;
    Control[UMFPACK_STRATEGY] = UMFPACK_STRATEGY_SYMMETRIC;

    const int n = theSOE->X.Size();
    const int nnz = static_cast<int>(theSOE->Ai.size());
    if (n == 0 || nnz==0) return 0;
    
    int *Ap= &(theSOE->Ap[0]);
    int *Ai= &(theSOE->Ai[0]);
    double *Ax= &(theSOE->Ax[0]);

    // symbolic analysis
    if(Symbolic)
      { free_symbolic(); }
    int status = umfpack_di_symbolic(n,n,Ap,Ai,Ax,&Symbolic,Control,Info);

    // check error
    if (status!=UMFPACK_OK)
      {
	std::cerr  << getClassName() << "::" << __FUNCTION__
		   << "; WARNING: symbolic analysis returns "
		   << status
		   << std::endl;
	Symbolic= nullptr;
	return -1;
      }
    return 0;
  }

//! @brief Sets the system of equations to solve.
bool XC::UmfpackGenLinSolver::setLinearSOE(LinearSOE *soe)
  {
    bool retval= false;
    UmfpackGenLinSOE *tmp= dynamic_cast<UmfpackGenLinSOE *>(soe);
    if(tmp)
      {
        theSOE= tmp;
        retval= true;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< " not a suitable system of equations"
		<< std::endl;
    return retval;
  }

bool XC::UmfpackGenLinSolver::setLinearSOE(UmfpackGenLinSOE &theLinearSOE)
  { return setLinearSOE(&theLinearSOE); }

int XC::UmfpackGenLinSolver::sendSelf(Communicator &comm)
  {
    // nothing to do
    return 0;
  }

int XC::UmfpackGenLinSolver::recvSelf(const Communicator &comm)
  {
    // nothing to do
    return 0;
  }

