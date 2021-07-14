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
                                                                        
#ifndef MumpsSolverBase_h
#define MumpsSolverBase_h

#include <solution/system_of_eqn/linearSOE/LinearSOESolver.h>
extern "C" {
#include <dmumps_c.h>
}

namespace XC {

class MumpsSolverBase : public LinearSOESolver
  {
  private:
    static bool MPIInitialized(void);
    static int initializeMPI(void);
    static int finalizeMPI(void);
  protected:
    DMUMPS_STRUC_C id;
    bool init;
    int icntl14;
    int icntl7;
    bool needsSetSize;

    virtual int initializeMumps(void)= 0;
    virtual int solveAfterInitialization(void)= 0;
    
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    MumpsSolverBase(int classTag, int ICNTL7=7, int ICNTL14=20);
    virtual ~MumpsSolverBase(void);

    int solve(void);
    int setSize(void);
    
    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);
  };
} // end of XC namespace

#endif

