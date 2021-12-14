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
                                                                        
// $Revision: 1.1 $
// $Date: 2003/08/29 07:17:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/machineBroker/MPI_MachineBroker.h,v $
                                                                        
                                                                        
// Written: fmk
// Revision: A
//
// Purpose: This file contains the class definition for MPI_MachineBroker.
// MPI_MachineBroker is the broker responsible for monitoring the usage of
// the processes in an mpi run.
//
// What: "@(#) MPI_MachineBroker.h, revA"

#ifndef MPI_MachineBroker_h
#define MPI_MachineBroker_h

#include "MachineBroker.h"
#include <cstdlib>

namespace XC {
class ID;
class MPI_Channel;
class FEM_ObjectBroker;

//! @ingroup IPComm
//
//! @brief MPI_MachineBroker is the broker responsible for monitoring
//! the usage of the processes in an mpi run.
class MPI_MachineBroker: public MachineBroker
  {
  private:
    size_t rank;
    ID *usedChannels;
    std::vector<MPI_Channel *> theChannels;

    void free_mem(void);
    void alloc(const std::size_t &);
    MPI_MachineBroker(const MPI_MachineBroker &);
    MPI_MachineBroker &operator=(const MPI_MachineBroker &);
  public:
    MPI_MachineBroker(FEM_ObjectBroker *theBroker, int argc,char *argv[]);
    ~MPI_MachineBroker(void);

    // methods to return info about local process id and num processes
    int getPID(void);
    int getNP(void);

    // methods to get and free Channels (processes)
    Channel *getMyChannel(void);
    Channel *getRemoteProcess(void);
    int freeProcess(Channel *);
  };
} // end of XC namespace

#endif
