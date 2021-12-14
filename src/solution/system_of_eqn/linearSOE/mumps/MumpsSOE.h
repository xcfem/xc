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
                                                                        
// $Revision: 1.7 $
// $Date: 2009-05-11 20:56:11 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/mumps/MumpsSOE.h,v $
                                                                        
#ifndef MumpsSOE_h
#define MumpsSOE_h

// Written: fmk 
// Created: 02/06
//
// Description: This file contains the class definition for MumpsSOE
// MumpsSOE is a subclass of LinearSOE. It stores the matrix equation
// Ax=b using the sparse column-compacted storage scheme for storing the 
// matrix A. 
//
// matrix types (matType): 0 Unsymmetrc
//                         1 Symmetrix positive definite
//                         2 General Symmetric
//
// What: "@(#) MumpsSOE.h, revA"

#include "solution/system_of_eqn/linearSOE/sparseGEN/SparseGenSOEBase.h"

namespace XC {
class MumpsSolver;
class MumpsParallelSolver;
class LinearSOESolver;

class MumpsSOE : public SparseGenSOEBase
  {
  protected:
    ID colA, rowA, colStartA; //!< int arrays containing info about coeficientss in A
    int matType;

  public:
    MumpsSOE(SolutionStrategy *);       
    MumpsSOE(SolutionStrategy *, int classTag);        
    SystemOfEqn *getCopy(void) const;
    //MumpsSOE(SolutionStrategy *, MumpsSolver &theSolver, int matType=0);        
    //MumpsSOE(SolutionStrategy *, LinearSOESolver &theSolver, int classTag, int matType = 0);        
    virtual ~MumpsSOE(void);

    void fortranIndexing(void);
    void cppIndexing(void);

    virtual int setSize(Graph &theGraph);
    virtual int addA(const Matrix &, const ID &, double fact = 1.0);
    
    virtual int setMumpsSolver(MumpsSolver &newSolver);    

    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);

    friend class MumpsSolver;    
    friend class MumpsParallelSolver;    
  };
} // end of XC namespace

#endif

