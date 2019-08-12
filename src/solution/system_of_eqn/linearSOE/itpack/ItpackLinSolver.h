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
// $Date: 2002/06/08 16:17:26 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/itpack/ItpackLinSolver.h,v $

#ifndef ItpackLinSolver_h
#define ItpackLinSolver_h

// Written: MHS
// Created: Sept 2001
//
// Description: This file contains the class definition for ItpackLinSolver.
// ItpackLinSolver is a concrete subclass of LinearSOE. It stores full
// unsymmetric linear system of equations using 1d arrays in Fortran style

#include <solution/system_of_eqn/linearSOE/LinearSOESolver.h>

// Adaptive methods
#define ItpackJCG         1
#define ItpackJSI         2
#define ItpackSOR         3
#define ItpackSSORCG      4
#define ItpackSSORSI      5
#define ItpackRSCG        6
#define ItpackRSSI        7

// Textbook methods
#define ItpackJ           8
#define ItpackGS          9
#define ItpackSORFixed   10
#define ItpackSSORFixed  11
#define ItpackRS         12

namespace XC {
class ItpackLinSOE;

//! @ingroup Solver
//
//! @brief <a href="http://rene.ma.utexas.edu/CNA/ITPACK/" target="_new"> ITPACK</a> based linear SOE solver.
class ItpackLinSolver : public LinearSOESolver
  {
  private:
    
    ItpackLinSOE *theSOE; //!< Sparse Ax=b represntation 
  
    ID IA;
    ID JA;
     
    int n; // Size of system, i.e., the number of equations
      // Parameter arrays sent to ITPACK subroutines
    int iparm[12];
    double rparm[12];
    // Workspace arrays sent to ITPACK subroutines
    ID iwksp;
    Vector wksp;
     
    int nwksp; //!< Length of wksp array
    int maxIter; //!< Maximum number of iterations
    int method; //!< Integer indicating which method to use
    double omega; //!< Parameter for SOR and SSOR fixed omega methods

    ItpackLinSolver(int method, int maxIter = 100, double omega = 1.0);
    ItpackLinSolver(void);
    virtual LinearSOESolver *getCopy(void) const;
    virtual bool setLinearSOE(LinearSOE *theSOE);
  public:
  
    int solve(void);
    int setSize(void);
    bool setLinearSOE(ItpackLinSOE &theSOE);
  
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

inline LinearSOESolver *ItpackLinSolver::getCopy(void) const
   { return new ItpackLinSolver(*this); }
} // end of XC namespace

#endif
