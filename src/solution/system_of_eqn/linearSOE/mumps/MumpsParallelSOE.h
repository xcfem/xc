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
                                                                        
// $Revision: 1.5 $
// $Date: 2009-05-11 20:56:11 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/mumps/MumpsParallelSOE.h,v $
                                             
#ifndef MumpsParallelSOE_h
#define MumpsParallelSOE_h

// Written: fmk 
// Description: This file contains the class definition for MumpsParallelSOE
// MumpsParallelSOE is a subclass of LinearSOE. It uses the sparse column
// storage scheme. The matrix A is kept distributed, X and B kept on all processors.
//
// matrix types (matType): 0 Unsymmetrc
//                         1 Symmetrix positive definite
//                         2 General Symmetric

// What: "@(#) MumpsParallelSOE.h, revA"

#include "MumpsSOE.h"

namespace XC {
class MumpsParallelSolver;

class MumpsParallelSOE : public MumpsSOE
  {
  private:
    int processID;
    std::vector<Channel *> theChannels;
    std::vector<ID> localCol;
    
  protected:
    mutable std::vector<double> workArea;
    Vector myB;
    
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    MumpsParallelSOE(SolutionStrategy *owr= nullptr);
    //MumpsParallelSOE(MumpsParallelSolver &theSolver, int matType=0);
    
    ~MumpsParallelSOE();

    // these methods need to be rewritten
    int setSize(Graph &theGraph);
    int addB(const Vector &, const ID &, double fact = 1.0);    
    int setB(const Vector &, double fact = 1.0);            
    const Vector &getB(void) const;
    void zeroB(void);
    int solve(void);

    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);

    friend class MumpsParallelSolver;
    
    int setProcessID(int processTag);
    int setChannels(const std::vector<Channel *> &theChannels);
  };
} // end of XC namespace

#endif

