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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/DomainSolver.h,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/DomainSolver.h
//
// Written: fmk 
// Created: Tue Sep 26 16:27:47: 1996
// Revision: A
//
// Description: This file contains the class definition for DomainSolver.
// DomainSolver is an abstract base class and thus no objects of it's type
// can be instantiated. It has pure virtual functions which must be
// implemented in it's derived classes.  Instances of DomainSolver are used
// to solve a system of equations and to do static condensation operations on the
// linear system of equations.
//
// What: "@(#) DomainSolver.h, revA"

#ifndef DomainSolver_h
#define DomainSolver_h

#include <solution/system_of_eqn/linearSOE/LinearSOESolver.h>
namespace XC {
class Matrix;
class Vector;

//! @ingroup LinearSolver
//
//! @brief used to solve a system of equations and to do static
//! condensation operations on the linear system of equations.
//!
//! DomainSolver objects are responsible for performing the numerical
//! operations required for the domain decomposition methods.
class DomainSolver: public virtual LinearSOESolver
  {
  protected:
    DomainSolver(int classTag);
  public:

    //! Causes the condenser to form
    //! \f$A_{ee}^* = A_{ee} -A_{ei} A_{ii}^{-1} A_{ie}\f$, where
    //! \f$A_{ii}\f$ is the first \p numInt rows of the \f$A\f$ matrix.
    //! The original \f$A\f$ is changed as a result. \f$A_{ee}^*\f$ is
    //! to be stored in \f$A_{ee}\f$.
    virtual int condenseA(int numInt) =0;
    //! Causes the condenser to form
    //! \f$B_e^* = B_e - A_{ei} A_{ii}^{-1} B_i\f$, where \f$A_{ii}\f$ 
    //! is the first \p numInt rows of \f$A\f$. The original \f$B\f$
    //! is changed as a result. \f$B_e^*\f$ is to be stored in \f$B_e\f$.
    virtual int condenseRHS(int numInt, Vector *u= nullptr) =0;
    //! @brief Causes the condenser to form \f$A_{ee} u\f$.
    virtual int computeCondensedMatVect(int numInt, const Vector &u) =0;
    //! @brief Returns the contents of \f$A_{ee}\f$ as a matrix.
    virtual const Matrix &getCondensedA(void) =0;
    //! @brief Returns the contents of \f$B_e\f$ as a Vector.
    virtual const Vector &getCondensedRHS(void) =0;
    //! @brief Returns the contents of the last call to {\em
    //! computeCondensedMatVect()}.
    virtual const Vector &getCondensedMatVect(void) =0;

    //! @brief Sets the computed value of the unknowns in \f$X_e\f$
    //! corresponding to the external equations to \p u. The number of
    //! external equations is given by the size of vector \f$u\f$.
    virtual int setComputedXext(const Vector &) =0;
    //! @brief To compute the internal equation numbers \f$X_i\f$ given
    //! the value set for the external equations in the last call to
    //! setComputedXext().
    virtual int solveXint(void) =0;
  };
} // end of XC namespace

#endif

