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
                                                                        
// $Revision: 1.6 $
// $Date: 2008-04-15 07:15:29 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/mumps/MumpsParallelSolver.h,v $
                                                                        
                                                                        
#ifndef MumpsParallelSolver_h
#define MumpsParallelSolver_h

// Written: fmk 
// Created: 02/06
//
// Description: This file contains the class definition for Mumps.
// A Mumps object can be constructed to solve a MumpsParallelSOE. It obtains 
// the solution by making calls on the the Mumps library, which is based 
// on public domain software developed during the Esprit IV European project 
// PARASOL (1996-1999) by CERFACS, ENSEEIHT-IRIT and RAL. Since this first 
// public domain version in 1999, the developments are supported by CERFACS, 
// ENSEEIHT-IRIT, and INRIA Rhone-Alpes. 
// Main contributors to MUMPS are Patrick Amestoy, Iain Duff, Abdou Guermouche,
// Jacko Koster, Jean-Yves L'Excellent, and Stephane Pralet.

// Up-to-date copies of the MUMPS package can be obtained from the Web pages 
// http://www.enseeiht.fr/apo/MUMPS/ or http://graal.ens-lyon.fr/MUMPS

// What: "@(#) Mumps.h, revA"

#include <solution/system_of_eqn/linearSOE/LinearSOESolver.h>
extern "C" {
#include <dmumps_c.h>
}

namespace XC {
class MumpsParallelSOE;

class MumpsParallelSolver : public LinearSOESolver
  {
  private:
    int initializeMumps(void);
    int solveAfterInitialization(void);

    DMUMPS_STRUC_C id;
    MumpsParallelSOE *theMumpsSOE;
    bool init;
    int icntl14;
    int icntl7;
    bool needsSetSize;
    int rank;
    int np;
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    MumpsParallelSolver(int ICNTL7=7, int ICNTL14=20);
    MumpsParallelSolver(int MPI_COMM, int ICNTL7, int ICNTL14);
    LinearSOESolver *getCopy(void) const;
    virtual ~MumpsParallelSolver(void);

    int solve(void);
    int setSize(void);
    bool setLinearSOE(LinearSOE *theSOE); 

    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);
};
} // end of XC namespace

#endif

