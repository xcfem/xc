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
// $Date: 2005/12/06 22:11:37 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSOE.h,v $
                                                                        
                                                                        
#ifndef SparseGenColLinSOE_h
#define SparseGenColLinSOE_h

// Written: fmk 
// Created: 04/98
// Revision: A
//
// Description: This file contains the class definition for SparseGenColLinSOE
// SparseGenColLinSOE is a subclass of LinearSOE. It stores the matrix equation
// Ax=b using the sparse column-compacted storage scheme for storing the 
// matrix A. 
//
// What: "@(#) SparseGenColLinSOE.h, revA"

#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenSOEBase.h>

namespace XC {
class SparseGenColLinSolver;

/**
 @brief Sparse nonsymmetric matrix linear system of equations.
 @ingroup SOE

 SparseGenColLinSOE is class which is used to store the matrix
 equation \f$Ax=b\f$ of order \f$size\f$ using a sparse column-compacted
 storage scheme for \f$A\f$. The \f$A\f$ matrix is stored in a 1d double
 array with \f$nnz\f$ elements, where \f$nnz\f$ is the number of non-zeroes
 in the matrix \f$A\f$. Two additional 1d integer arrays \f$rowA\f$
 and \f$colStartA\f$ are used to store information about the location of
 the coefficients, with \f$colStartA(i)\f$ storing the location in the 1d
 double array of the start of column \f$i\f$ and \f$rowA(j)\f$ identifying
 the row in \f$A\f$ to which the \f$j'th\f$ component in the 1d double
 array. \f$colStartA\f$ is of dimension \f$size+1\f$ and \f$rowA\f$ of
 dimension \f$nnz\f$. For example
 
 \f$
 \left[
 \begin{array}{ccccc}
 a_{0,0} & 0 & a_{0,2}  & a_{0,3} & 0\\
 a_{1,0} & a_{1,1} & 0 & 0 & 0\\
 0 & a_{2,1} & a_{2,2} & 0 & 0\\
 0 & 0 & 0 & a_{3,3} & a_{3,4}\\
 a_{4,0} & a_{4,1} & 0 & 0 & a_{4,4}
 \end{array}
 \right] 
 \f$
 
 is stored in:
 
 \f$
 \left[
 \begin{array}{cccccccccccccc}
 a_{0,0} & a_{1,0}  & a_{4,0} & a_{1,1} & a_{2,1} & a_{4,1} &
 a_{0,2} & a_{2,2} & a_{0,3} & a_{3,3} & a_{3,4} & a_{4,4}
 \end{array}
 \right] 
 \f$
 
 with
 
 \f$
 colStartA =
 \left[
 \begin{array}{cccccccccccccc}
 0 & 3 & 6 & 8 & 10 & 12
 \end{array}
 \right] 
 \f$
 
 and
 
 \f$
 rowA =
 \left[
 \begin{array}{cccccccccccccc}
 0 & 1 & 4 & 1 & 2 & 4 & 0 & 2 & 0 & 3 & 3 & 4 
 \end{array}
 \right] 
 \f$
 The \f$x\f$ and \f$b\f$ vectors are stored in 1d double arrays of
 length \f$n\f$.
*/
class SparseGenColLinSOE: public SparseGenSOEBase
  {
  protected:
    ID rowA;
    ID colStartA; //!< int arrays containing info about coeficientss in A
  protected:
    virtual bool setSolver(LinearSOESolver *);

    friend class SolutionStrategy;
    friend class FEM_ObjectBroker;
    SparseGenColLinSOE(SolutionStrategy *);        
    SparseGenColLinSOE(SolutionStrategy *,int classTag);
    SystemOfEqn *getCopy(void) const;
  public:
    virtual int setSize(Graph &theGraph);
    virtual int addA(const Matrix &, const ID &, double fact = 1.0);
    
    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);
#ifdef _PARALLEL_PROCESSING
    friend class SuperLU;    
    friend class ThreadedSuperLU;        
    friend class DistributedSuperLU;        
#else
    friend class SuperLU;    
#endif
  };
} // end of XC namespace


#endif

