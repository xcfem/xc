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
// $Date: 2005/01/27 22:22:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/diagonal/DiagonalDirectSolver.h,v $

// Written: fmk 
// Created: Jan 2005
// Revision: A
//
// Description: This file contains the class definition for 
// DiagonalDirectSolver. DiagonalDirectSolver is a subclass 
// of LinearSOESOlver. It solves diagonal system directly!

// What: "@(#) DiagonalDirectSolver.h, revA"

#ifndef DiagonalDirectSolver_h
#define DiagonalDirectSolver_h

#include <solution/system_of_eqn/linearSOE/diagonal/DiagonalSolver.h>
namespace XC {
class DiagonalSOE;

//! @ingroup Solver
//
//! @brief Direct solver for diagonal matrix SOE.
class DiagonalDirectSolver: public DiagonalSolver
  {
  protected:
    double minDiagTol;

    friend class LinearSOE;
    DiagonalDirectSolver(double tol=1.0e-18);    
    virtual LinearSOESolver *getCopy(void) const;
  public:
    virtual int solve(void);        
    virtual int setSize(void);    
    double getDeterminant(void);
    
    virtual int setDiagonalSOE(DiagonalSOE &theSOE);

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  };

inline LinearSOESolver *DiagonalDirectSolver::getCopy(void) const
   { return new DiagonalDirectSolver(*this); }
} // end of XC namespace


#endif

