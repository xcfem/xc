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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/bandGEN/BandGenLinSolver.h,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/bandGEN/BandGenLinSolver.h
//
// Written: fmk 
// Created: Tue Sep 26 16:27:47: 1996
// Revision: A
//
// Description: This file contains the class definition for BandGenLinSolver.
// BandGenLinSolver is an abstract base class and thus no objects of it's type
// can be instantiated. It has pure virtual functions which must be
// implemented in it's derived classes.  Instances of BandGenLinSolver 
// are used to solve a system of equations of type BandGenLinSOE.
//
// What: "@(#) BandGenLinSolver.h, revA"

#ifndef BandGenLinSolver_h
#define BandGenLinSolver_h

#include <solution/system_of_eqn/linearSOE/LinearSOESolver.h>
namespace XC {
class BandGenLinSOE;

//! @ingroup Solver
//
//! @brief Base class for band general linear SOE solvers.
//!
//! BandGenLinSolver is an abstract class.  The BandGenLinSolver
//! class provides access for each subclass to the BandGenLinSOE object
//! through the pointer #theSOE, which is a protected pointer.
class BandGenLinSolver: public LinearSOESolver
  {
  protected:
    BandGenLinSOE *theSOE;
    virtual bool setLinearSOE(LinearSOE *theSOE);
  public:
    BandGenLinSolver(int classTag);    

    virtual int solve(void) = 0;
    virtual bool setLinearSOE(BandGenLinSOE &theSOE);
  };
} // end of XC namespace

#endif

