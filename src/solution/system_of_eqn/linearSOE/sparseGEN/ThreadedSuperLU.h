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
                                                                        
// $Revision: 1.2 $
// $Date: 2002/01/25 20:27:11 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/sparseGEN/ThreadedSuperLU.h,v $
                                                                        
                                                                        
#ifndef ThreadedSuperLU_h
#define ThreadedSuperLU_h

// File: ~/system_of_eqn/linearSOE/sparseGEN/ThreadedSuperLU.h
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class interface for ThreadedSuperLU.
// This is a class that uses the threads version of SuperLU
//
// What: "@(#) ThreadedSuperLU.h, revA"

#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSolver.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/pdsp_defs.h>
#include <superlu/supermatrix.h>

namespace XC {

//! @ingroup LinearSolver
//
//! @brief Threaded <a href="https://launchpad.net/ubuntu/+source/superlu" target="_new"> SuperLU</a> based sparse general matrix linear SOE solver.
class ThreadedSuperLU : public SparseGenColLinSolver
  {
  private:
    SuperMatrix A,L,U,B,AC;
    ID perm_r;
    ID perm_c;
    ID etree;
    int sizePerm;
    int relax, permSpec, panelSize;
    float thresh;

    int numThreads;
    pdgstrf_options_t pdgstrf_options;
    yes_no_t refact, usepr;
    fact_t fact;
    trans_t trans;
    void *work;
    int lwork;
    Gstat_t gStat;
  public:
    ThreadedSuperLU(int numThreads = 2,
		    int permSpec = 0, int panelSize = 6, 
		    int relax = 6, double thresh = 0.0);     

    ~ThreadedSuperLU();

    int solve(void);
    int setSize(void);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);        
  };
} // end of XC namespace

#endif


