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
// $Date: 2001/07/20 22:36:37 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/LinearSOE.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/LinearSOE.C
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the implementation of XC::LinearSOE.
//
// What: "@(#) LinearSOE.C, revA"

#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/system_of_eqn/linearSOE/LinearSOESolver.h>

#include <solution/system_of_eqn/linearSOE/bandGEN/BandGenLinSolver.h>
#include <solution/system_of_eqn/linearSOE/bandGEN/BandGenLinLapackSolver.h>

#include <solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinSolver.h>
#include <solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinLapackSolver.h>
//#include <solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinThreadSolver.h>

#include <solution/system_of_eqn/linearSOE/DomainSolver.h>

#include <solution/system_of_eqn/linearSOE/cg/ConjugateGradientSolver.h>

#include <solution/system_of_eqn/linearSOE/diagonal/DiagonalSolver.h>
#include <solution/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSolver.h>
#include <solution/system_of_eqn/linearSOE/diagonal/DiagonalDirectSolver.h>

#include <solution/system_of_eqn/linearSOE/fullGEN/FullGenLinSolver.h>
#include <solution/system_of_eqn/linearSOE/fullGEN/FullGenLinLapackSolver.h>

//#include <solution/system_of_eqn/linearSOE/itpack/ItpackLinSolver.h>

#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSolver.h>
#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectBlockSolver.h>
//#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectThreadSolver.h>
//#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectSkypackSolver.h>
#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectSolver.h>
//#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSubstrSolver.h>

#include <solution/system_of_eqn/linearSOE/sparseGEN/DistributedSparseGenRowLinSolver.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSolver.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenRowLinSolver.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/SuperLU.h>

#include <solution/system_of_eqn/linearSOE/sparseSYM/SymSparseLinSolver.h>

#include "utility/matrix/Vector.h"

//#include <solution/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSolver.h>

//! @brief Constructor. The integer \p classTag} is passed to
//! the constructor of the base class. 
//!
//! @param owr: analysis aggregation that owns this object.
//! @param classTag: identifier of the class.
XC::LinearSOE::LinearSOE(AnalysisAggregation *owr,int classTag)
  :SystemOfEqn(owr,classTag), theSolver(nullptr) {}

//! @brief Frees memory.
void XC::LinearSOE::free_memory(void)
  {
    if(theSolver)
      {
        delete theSolver;
        theSolver= nullptr;
      }
  }

//! @brief Copy the solver.
void XC::LinearSOE::copy(const LinearSOESolver *newSolver)
  {
    if(newSolver)
      {
        free_memory();
        LinearSOESolver *tmp= newSolver->getCopy();
        if(tmp)
          setSolver(tmp);
        else
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; can't create solver." << std::endl;
      }
  }

//! @brief Sets the solver for the system of equations.
//!
//! This is invoked to set the currently associated LinearSOESolver object to 
//! be \p newSolver. Each subclass will provide it's own variation of 
//! setSolver() method (needed so subclasses can verify type of Solver
//! object passed). the subclasses in their variation of the setSolver()
//! method (unless they wish to implement their own solve()
//! method) invoke this method. Returns $0$.
bool XC::LinearSOE::setSolver(LinearSOESolver *newSolver)
  {
    bool retval= false;
    if(newSolver)
      {
        free_memory();
        theSolver= newSolver;
        theSolver->setLinearSOE(this);
        const int solverOK= theSolver->setSize();
        if(solverOK < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING - solver failed setSize().\n";
          }
        retval= true;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; null pointer to solver." << std::endl;
    return retval;
  }

XC::LinearSOESolver &XC::LinearSOE::newSolver(const std::string &tipo)
  {
    if(tipo=="band_gen_lin_lapack_solver")
      setSolver(new BandGenLinLapackSolver());
    else if(tipo=="band_spd_lin_lapack_solver")
      setSolver(new BandSPDLinLapackSolver());
//     else if(tipo=="band_spd_lin_thread_solver")
//       setSolver(new BandSPDLinThreadSolver());
//     else if(tipo=="conjugate_gradient_solver")
//       setSolver(new ConjugateGradientSolver());
    else if(tipo=="diagonal_direct_solver")
      setSolver(new DiagonalDirectSolver());
    else if(tipo=="distributed_diagonal_solver")
      setSolver(new DistributedDiagonalSolver());
    else if(tipo=="full_gen_lin_lapack_solver")
      setSolver(new FullGenLinLapackSolver());
//     else if(tipo=="itpack_lin_solver")
//       setSolver(new ItpackLinSolver());
    else if(tipo=="profile_spd_lin_direct_solver")
      setSolver(new ProfileSPDLinDirectSolver());
    else if(tipo=="profile_spd_lin_direct_block_solver")
      setSolver(new ProfileSPDLinDirectBlockSolver());
//     else if(tipo=="profile_spd_lin_direct_skypack_solver")
//      setSolver(new ProfileSPDLinDirectSkypackSolver());
//     else if(tipo=="profile_spd_lin_direct_thread_solver")
//       setSolver(new ProfileSPDLinDirectThreadSolver());
//     else if(tipo=="profile_spd_lin_substr_solver")
//       setSolver(new ProfileSPDLinSubstrSolver());
    else if(tipo=="super_lu_solver")
      setSolver(new SuperLU());
    else if(tipo=="sym_sparse_lin_solver")
      setSolver(new SymSparseLinSolver());
//     else if(tipo=="umfpack_gen_lin_solver")
//       setSolver(new UmfpackGenLinSolver());
    else
      std::cerr << "Solver of type: '"
                << tipo << "' unknown." << std::endl;
    assert(theSolver);
    return *theSolver;
  }

//! @brief Destructor.
XC::LinearSOE::~LinearSOE(void)
  { free_memory(); }

//! @brief Computes the solution of the system of equations.
//!
//! Causes the SystemOfEqn object to invoke solve() on the currently
//! associated solver object. Returns a $0$ if successful,
//! negative number if not; the actual value depending on the
//! LinearSOESolver. To solve a linear system of equations means to find
//! $x$ such that the equation $Ax=b$ is satisfied. 
int XC::LinearSOE::solve(void)
  { return (getSolver()->solve()); }

//! @brief Returns the determinant of the system matrix.
double XC::LinearSOE::getDeterminant(void)
  { return getSolver()->getDeterminant(); }


//! @brief Returns a pointer to the solver.
XC::LinearSOESolver *XC::LinearSOE::getSolver(void)
  { return theSolver; }

//! @brief invoke setSize() on the Solver
int XC::LinearSOE::setSolverSize(void)
  {
    const int retval = theSolver->setSize();
    if(retval < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; solver failed setSize().\n";
      }
    return retval;
  }

