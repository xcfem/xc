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
// $Date: 2003/08/29 07:16:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/machineBroker/MachineBroker.h,v $
                                                                        
                                                                        
// Written: fmk
// Revision: A
//
// Purpose: This file contains the class definition for MachineBroker.
// MachineBroker is an abstract base class, a subclass of which must
// be written for each parallel machine.
//
// What: "@(#) MachineBroker.h, revA"

#ifndef MachineBroker_h
#define MachineBroker_h

#include <vector>
#include "utility/actor/ObjectWithObjBroker.h"
#include "utility/matrix/ID.h"

namespace XC {
class Channel;
class FEM_ObjectBroker;

//! @ingroup IPComm
//
//! @brief A MachineBroker is responsible
//! for getting an actor process running on the parallel machine.
class MachineBroker: public ObjectWithObjBroker
  {
  private:
    std::vector<Channel *> actorChannels; //!< channels owned with running actor processes
    int numActorChannels;
    int numActiveChannels;
    ID activeChannels;

    MachineBroker(const MachineBroker &);
    MachineBroker &operator=(const MachineBroker &);
  public:
    MachineBroker(FEM_ObjectBroker *);
    virtual ~MachineBroker();

    // methods to return info about local process id and num processes
    virtual int getPID(void) = 0;
    virtual int getNP(void)  = 0;

    // methods to get and free Actors
    virtual int shutdown(void);    
    virtual int runActors(void);
    virtual Channel *startActor(int actorType, int compDemand = 0);
    virtual int finishedWithActor(Channel *);

    // methods to get and free Channels (processes)
    virtual Channel *getMyChannel(void) =0;
    virtual Channel *getRemoteProcess(void) =0;
    virtual int freeProcess(Channel *) =0;
  };
} // end of XC namespace

#endif
