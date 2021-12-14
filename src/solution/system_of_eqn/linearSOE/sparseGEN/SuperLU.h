// -*-c++-*-
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
// $Date: 2005/03/17 20:47:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/sparseGEN/SuperLU.h,v $
                                                                        
                                                                        
#ifndef SuperLU_h
#define SuperLU_h

// File: ~/system_of_eqn/linearSOE/sparseGEN/SuperLU.h
//
// Written: fmk 
// Created: 11/96
//
// Description: This file contains the class definition for SuperLU.
// A SuperLU object can be constructed to solve a SparseGenColLinSOE
// object. It obtains the solution by making calls on the
// the SuperLU library developed at UC Berkeley by Prof. James Demmel, 
// Xiaoye S. Li and John R. Gilbert.
// The SuperLU library contains a set of subroutines to solve a sparse
// linear system  $AX=B$. It uses Gaussian elimination with partial
// pivoting (GEPP). The columns of A may be preordered before
// factorization; the preordering for sparsity is completely separate
// from the factorization and a number of ordering schemes are provided. 
//
// What: "@(#) SuperLU.h, revA"

#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSolver.h>
//#include "superlu/slu_cdefs.h"
#include "superlu/slu_ddefs.h"
#include "superlu/supermatrix.h"

namespace XC {

//! @ingroup LinearSolver
//
//! @brief <a href="https://launchpad.net/ubuntu/+source/superlu" target="_new"> SuperLU</a> based sparse general matrix linear SOE solver.
//!
//! A SuperLU object can be constructed to solve
//! a SparseGenColLinSOE object. It obtains the solution by making calls on the
//! the SuperLU library developed at UC Berkeley by Prof. James Demmel, 
//! Xiaoye S. Li and John R. Gilbert.
//! The SuperLU library contains a set of subroutines to solve a sparse
//! linear system  \f$AX=B\f$. It uses Gaussian elimination with partial
//! pivoting (GEPP). The columns of A may be preordered before
//! factorization; the preordering for sparsity is completely separate
//! from the factorization and a number of ordering schemes are provided.
class SuperLU: public SparseGenColLinSolver
  {
  private:
    SuperMatrix A;
    SuperMatrix B; //!< On entry, the right hand side matrix; on exit, the solution matrix if info = 0;
    SuperMatrix AC; //!< The resulting matrix after applied the column
                    //! permutation perm_c[] to matrix A.
    SuperMatrix L; //!< The factor L from the factorization Pr*A*Pc=L*U as
                   // computed by dgstrf(). Use compressed row subscripts
                   // storage for supernodes, i.e., L has types:
                   // Stype = SLU_SC, Dtype = SLU_D, Mtype = SLU_TRLU.
    SuperMatrix U; //!< The factor U from the factorization Pr*A*Pc=L*U as
                   // computed by dgstrf(). Use compressed row subscripts
                   // storage for supernodes, i.e., U has types:
                   // Stype = SLU_SC, Dtype = SLU_D, Mtype = SLU_TRLU.
    ID perm_r; //!< row permutation vector dimension(L->nrow)
    ID perm_c; //!< column permutation vector dimension(L->ncol)
    ID etree;
    int relax, permSpec, panelSize;
    char symmetric;
    superlu_options_t options; //! @brief Specifies whether or not the elimination tree will be re-used.
    SuperLUStat_t stat; //!< Record the statistics on runtime and
                        // floating-point operation count.
    void free_matricesLU(void);
    void free_matricesABAC(void);
    void free_matrices(void);
    void free_mem(void);
    void alloc_permutation_vectors(const size_t &n);
    void alloc_matrices(const size_t &n);
    void alloc(const size_t &n);
    int factorize(void);

    friend class LinearSOE;
    friend class FEM_ObjectBroker;
    SuperLU(int permSpec = 0, int panelSize = 6, int relax = 6,char symmetric = 'N');
    SuperLU(const SuperLU &);
    SuperLU &operator=(const SuperLU &);
    virtual LinearSOESolver *getCopy(void) const;
  public:
    ~SuperLU(void);

    int solve(void);
    int setSize(void);

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);

    void Print(std::ostream &os) const;
  };
} // end of XC namespace

#endif

