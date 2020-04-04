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
                                                                        
// $Revision: 1.2 $
// $Date: 2006/01/10 00:42:51 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/sparseGEN/SparseGenRowLinSOE.h,v $
                                                                        
#ifndef SparseGenRowLinSOE_h
#define SparseGenRowLinSOE_h

// Written: fmk 
// Created: 04/05
// Revision: A
//
// Description: This file contains the class definition for SparseGenRowLinSOE
// SparseGenRowLinSOE is a subclass of LinearSOE. It stores the matrix equation
// Ax=b using the sparse row-compacted storage scheme for storing the 
// matrix A. 
//
// What: "@(#) SparseGenRowLinSOE.h, revA"

#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenSOEBase.h>

namespace XC {
class SparseGenRowLinSolver;

//! @ingroup SOE
//
//! @brief Sparse nonsymmetric matrix linear system of equations.
class SparseGenRowLinSOE: public SparseGenSOEBase
  {
  private:
    ID colA;
    ID rowStartA; //!< int arrays containing info about coeficientss in A
  protected:
    virtual bool setSolver(LinearSOESolver *);

    friend class AnalysisAggregation;
    SparseGenRowLinSOE(AnalysisAggregation *);        
    SystemOfEqn *getCopy(void) const;
  public:
    int setSize(Graph &theGraph);
    int addA(const Matrix &, const ID &, double fact = 1.0);
    
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    friend class PetscSparseSeqSolver;    
  };
inline SystemOfEqn *SparseGenRowLinSOE::getCopy(void) const
  { return new SparseGenRowLinSOE(*this); }
} // end of XC namespace


#endif

