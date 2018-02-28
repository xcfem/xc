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
                                                                        
// $Revision: 1.1 $
// $Date: 2003/08/29 07:17:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/machineBroker/MPI_MachineBroker.cpp,v $
                                                                        
                                                                        
// Written: fmk
// Revision: A


#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/actor/machineBroker/MPI_MachineBroker.h>
#include <utility/actor/channel/MPI_Channel.h>
#include <utility/matrix/ID.h>

#include <mpi/mpi.h>

void XC::MPI_MachineBroker::free_mem(void)
  {
    const size_t sz= theChannels.size();
    for(size_t i=0; i<sz; i++)
      {
        if(theChannels[i])
          {
            delete theChannels[i];
            theChannels[i]= nullptr;
          }
        else
	  std::cerr << "Puntero a canal nulo." << std::endl;
      }
    theChannels.clear();
    if(usedChannels)
      {
        delete usedChannels;
        usedChannels= nullptr;
      }
  }

void XC::MPI_MachineBroker::alloc(const std::size_t &sz)
  {
    free_mem();
    theChannels= std::vector<MPI_Channel *>(sz,nullptr);
    for(size_t i=0; i<sz; i++)
      theChannels[i]= new MPI_Channel(i);
    usedChannels= new ID(sz);
    usedChannels->Zero();
  }

XC::MPI_MachineBroker::MPI_MachineBroker(FEM_ObjectBroker *theBroker, int argc,char *argv[])
  :MachineBroker(theBroker)
  {
    MPI_Init(&argc, &argv);
    int r= rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &r);
    rank= r;
    int sz= theChannels.size();
    MPI_Comm_size(MPI_COMM_WORLD, &sz);
    alloc(sz);
  }


XC::MPI_MachineBroker::~MPI_MachineBroker(void)
  {
    free_mem();
    MPI_Finalize();
  }


int XC::MPI_MachineBroker::getPID(void)
  { return rank; }


int XC::MPI_MachineBroker::getNP(void)
  { return theChannels.size(); }



XC::Channel *XC::MPI_MachineBroker::getMyChannel(void)
  { return theChannels[0]; }



XC::Channel *XC::MPI_MachineBroker::getRemoteProcess(void)
  {
    if(rank != 0)
      {
        std::cerr << "XC::MPI_MachineBroker::getRemoteProcess() - child process cannot not yet allocate processes\n";
        return 0;
      }
    const size_t sz= theChannels.size();
    for(size_t i=0; i<sz; i++)
      if(i != rank)
        if((*usedChannels)(i) == 0)
          {
            (*usedChannels)(i) = 1;
            return theChannels[i];
          }
  
    // no processes available
    return 0;
  }



int XC::MPI_MachineBroker::freeProcess(Channel *theChannel)
  {
    const size_t sz= theChannels.size();
    for(size_t i=0; i<sz; i++)
      if(i != rank)
        if(theChannels[i] == theChannel)
          {
            (*usedChannels)(i)= 0;
            return 0;
          }
    // channel not found!
    return -1;
  }

