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
                                                                        
// $Revision: 1.1 $
// $Date: 2005/12/06 22:21:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/sparseGEN/DistributedSuperLU.h,v $
                                                                        
                                                                        
#ifndef DistributedSuperLU_h
#define DistributedSuperLU_h

// Written: fmk 
//
// Description: This file contains the class definition for DistributedSuperLU.
// A DistributedSuperLU object can be constructed to solve a SparseGenColLinSOE
// object. It obtains the solution by making calls on the
// the SuperLU library developed at UC Berkeley by Prof. James Demmel, 
// Xiaoye S. Li and John R. Gilbert.
// The SuperLU library contains a set of subroutines to solve a sparse
// linear system  $AX=B$. It uses Gaussian elimination with partial
// pivoting (GEPP). The columns of A may be preordered before
// factorization; the preordering for sparsity is completely separate
// from the factorization and a number of ordering schemes are provided. 
//
// What: "@(#) DistributedSuperLU.h, revA"

#include <other/superLU_dist_2.0/src/superlu_ddefs.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSolver.h>


namespace XC {

//! @ingroup LinearSolver
//
//! @brief <a href="https://launchpad.net/ubuntu/+source/superlu" target="_new"> SuperLU</a> based solver for distributed sparse matrix linear systems of equations. It uses Gaussian elimination with partial
//! pivoting (GEPP). The columns of A may be preordered before
//! factorization; the preordering for sparsity is completely separate
//! from the factorization and a number of ordering schemes are provided.
class DistributedSuperLU: public SparseGenColLinSolver
  {
  private:
    superlu_options_t_Distributed options;
    SuperLUStat_t stat;
    SuperMatrix A;
    ScalePermstruct_t ScalePermstruct;
    LUstruct_t LUstruct;
    gridinfo_t grid;

    bool gridInit;
    int npRow, npCol;

    int processID;
    int numChannels;
    std::vector<Channel *> theChannels;

    MPI_Comm comm_SuperLU;
    Vector b;
    ID rowA;
  public:
    DistributedSuperLU(int npRow, int npCol);
    DistributedSuperLU(void);
    ~DistributedSuperLU(void);

    int solve(void);
    int setSize(void);

    virtual int setProcessID(int domainTag);
    virtual int setChannels(int numChannels, Channel **theChannels);
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  };
} // end of XC namespace

#endif

