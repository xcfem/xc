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
                                                                        
// $Revision: 1.4 $
// $Date: 2005/12/01 01:12:55 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/LinearSOE.h,v $
                                                                        
                                                                        
#ifndef LinearSOE_h
#define LinearSOE_h

// File: ~/system_of_eqn/LinearSOE.h
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
//
// What: "@(#) LinearSOE.h, revA"

#include <solution/system_of_eqn/SystemOfEqn.h>

namespace XC {
class LinearSOESolver;
class Matrix;
class Vector;
class ID;

//!  @ingroup SOE
//! 
//! @defgroup LinearSOE linear systems of equations.
//
//! @ingroup LinearSOE
//
//! @brief Linear system of equations.
//!
//! Abstract base class and thus no objects of it's type
//! can be instantiated. It has pure virtual functions which must be
//! implemented in it's derived classes. LinearSystemOfEqn is an abstraction 
//! of the linear system of equation given by : [A]{X} = {B} - {C},
//! where A is a matrix and B,C and X are vectors. To solve the equation means
//! given A, B and C to find the unknown X such that the equation is satisfied.
//! LinearSOE object is responsible for storing these equations and for
//! providing methods at the interface to set up and obtain the
//! equations. Each LinearSOE object will be associated with a
//! LinearSOESolver object. It is the LinearSOESolver objects that is
//! responsible for solving the linear system of equations.
class LinearSOE : public SystemOfEqn
  {
  private:
    LinearSOESolver *theSolver;
    void free_memory(void);
    void copy(const LinearSOESolver *);
  protected:
    friend class FEM_ObjectBroker;
    virtual bool setSolver(LinearSOESolver *);
    int setSolverSize(void);

    LinearSOE(AnalysisAggregation *,int classTag);
  public:
    virtual ~LinearSOE(void);

    virtual int solve(void);    

    //! @brief Determines and sets the size of the system.
    //!
    //! Invoked to allow the LinearSOE object to determine the size
    //! and sparsity of the matrix $A$ and vectors $x$ and $b$. This
    //! information can be deduced from the number of vertices and
    //! the connectivity between the vertices in the Graph object \p theGraph.
    //! To return $0$ if successful, a negative number if not.
    virtual int setSize(Graph &theGraph) =0;
    //! @brief Returns the number of equations in the system.
    virtual int getNumEqn(void) const =0;
    
    //! The LinearSOE object assembles \p fact times the Matrix \p M
    //! into the matrix $A$. The Matrix is assembled into $A$ at the
    //! locations given by the ID object \p loc, i.e.
    //! $a_{loc(i),loc(j)}+= M(i,j)$. Numbering in $A$ starts
    //! at $(0,0)$, i.e. C style. If allocation specified is outside
    //! the range, i.e. $(-1,-1)$ the corresponding entry in \p M
    //! is not added to $A$. To return $0$ if successful, a
    //! negative number if not.
    virtual int addA(const Matrix &M, const ID &loc, double fact = 1.0) =0;

    //! The LinearSOE object assembles \p fact times the Vector \p V into
    //! the vector $b$. The Vector is assembled into $b$ at the locations
    //! given by the ID object {\em loc}, i.e. $b_{loc(i)} += V(i)$. If a
    //! location specified is outside the range, e.g. $-1$, the corresponding
    //! entry in {\em V} is not added to $b$. To return $0$ if successful, a
    //! negative number if not.
    virtual int addB(const Vector &V, const ID &loc,const double &fact= 1.0) =0;    

    //! The LinearSOE object sets the vector \p b to be \p fact times
    //! the vector \p V. To return $0$ if successful, a negative number if
    //! not.
    virtual int setB(const Vector &V, const double &fact= 1.0) =0;        

    //! @brief To zero the matrix $A$, i.e. set all the components of $A$ to $0$.
    virtual void zeroA(void) =0;
    //! @brief To zero the vector $b$, i.e. set all the components of $b$ to $0$.
    virtual void zeroB(void) =0;

    //! @brief Return a const reference to the vector $x$.
    virtual const Vector &getX(void) const= 0;
    //! @brief Return a const reference to the vector $b$.
    virtual const Vector &getB(void) const= 0;    
    virtual double getDeterminant(void);
    //! @brief Return the 2-norm of the vector $x$.
    virtual double normRHS(void) const= 0;

    //! The LinearSOE object is responsible for setting $x(loc) = value$. This
    //! is needed in domain decomposition methods and could be useful in
    //! iterative solution strategies when an initial approximation is known.
    virtual void setX(int loc, double value) =0;
    //! @brief Sets the vector $x$.
    virtual void setX(const Vector &X) =0;
    
    LinearSOESolver *getSolver(void);
    LinearSOESolver &newSolver(const std::string &);
  };
} // end of XC namespace


#endif
