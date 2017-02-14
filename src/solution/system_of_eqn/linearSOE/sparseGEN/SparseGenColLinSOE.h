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

//! @ingroup SOE
//
//! @brief Sparse nonsymmetric matrix linear system of equations.
class SparseGenColLinSOE : public SparseGenSOEBase
  {
  protected:
    ID rowA;
    ID colStartA;//!< int arrays containing info about coeficientss in A
  protected:
    virtual bool setSolver(LinearSOESolver *);

    friend class SoluMethod;
    friend class FEM_ObjectBroker;
    SparseGenColLinSOE(SoluMethod *);        
    SparseGenColLinSOE(SoluMethod *,int classTag);
    SystemOfEqn *getCopy(void) const;
  public:
    virtual int setSize(Graph &theGraph);
    virtual int addA(const Matrix &, const ID &, double fact = 1.0);
    
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
#ifdef _PARALLEL_PROCESSING
    friend class SuperLU;    
    friend class ThreadedSuperLU;        
    friend class DistributedSuperLU;        
#else
    friend class SuperLU;    
#endif

  };
inline SystemOfEqn *SparseGenColLinSOE::getCopy(void) const
  { return new SparseGenColLinSOE(*this); }
} // end of XC namespace


#endif

