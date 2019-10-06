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
// $Date: 2000/09/15 08:23:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/Solver.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/Solver.C
//
// Written: fmk 
// Created: Tue Sep 26 16:27:47: 1996
// Revision: A
//
// Description: This file contains the class definition for XC::Solver.
// Solver is an abstract base class and thus no objects of it's type
// can be instantiated. It has pure virtual functions which must be
// implemented in it's derived classes.  Instances of XC::Solver are used
// to solve a system of equations.
//
// What: "@(#) Solver.C, revA"

#include <solution/system_of_eqn/Solver.h>

//! @brief Constructor. The integer \p classTag is passed to the
//! base class constructor. 
XC::Solver::Solver(int classTag)
  : MovableObject(classTag), CommandEntity() {}


//! @brief Estimates the reciprocal of the condition number of the
//! matrix A, in either the 1-norm or the infinity-norm,
//! using the LU factorization computed by DGBTRF.
//!
//! @param c: Specifies whether the 1-norm condition number or the
//!           infinity-norm condition number is required:
//!           = '1' or 'O':  1-norm;
//!           = 'I':         Infinity-norm.
double XC::Solver::getRCond(const char &)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet." << std::endl;
    return 0.0;
  }

//! @brief Return the determinant of the system matrix.
double XC::Solver::getDeterminant(void)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet." << std::endl;
    return 1.0;
  }
