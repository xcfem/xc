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
// $Date: 2003/08/29 07:19:26 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/machineBroker/MillMachineBroker.cpp,v $
                                                                        
                                                                        
// File: ~/actor/broker/MillMachineBroker.h
//
// Written: fmk
// Created: March 1998
// Revision: A
//
// Purpose: This file contains the class definition for XC::MillMachineBroker.
// MillMachineBroker is a machine broker for use with the cluster of
// Millennium machines in Sida Hall.
//
// What: "@(#) MillMachineBroker.h, revA"

#include <utility/actor/machineBroker/MillMachineBroker.h>
#include <cstdlib>

#include "utility/remote/remote.h"
#include "utility/actor/channel/Channel.h"

XC::MillMachineBroker::MillMachineBroker(FEM_ObjectBroker *theBroker)
  :AlphaBaseMachineBroker(theBroker,0,5)
  {
    machines[0] = "mill0";
    machines[1] = "mill1";
    machines[2] = "mill2";
    machines[3] = "mill3";
    machines[4] = "mill4";
  }


int XC::MillMachineBroker::startActor(const std::string &actorProgram, 
			       Channel &theChannel,
			       int compDemand)
  { 

    // get the next machine, a round-robin approach
    std::string machine;
    if (currentMachine < maxNumMachines)
	machine = machines[currentMachine];
    else
      {
	currentMachine = 0;
	machine = machines[currentMachine];
      }
    currentMachine++;
    
    std::string remotecmd= REMOTE;
    remotecmd+= " ";          
    remotecmd+= machine;
    remotecmd+= " ";
    remotecmd+= actorProgram;
    remotecmd+= " ";
    remotecmd+= theChannel.addToProgram();
    remotecmd+= "\n";

    int retval= system(remotecmd.c_str());
    return retval;
  }

