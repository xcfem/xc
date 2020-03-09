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
// $Source: /usr/local/cvs/OpenSees/SRC/actor/machineBroker/AlphaMachineBroker.cpp,v $
                                                                        
                                                                        
// File: ~/actor/broker/AlphaMachineBroker.h
//
// Written: fmk
// Created: Fri Sept 20 12:27:47: 1996
// Revision: A
//
// Purpose: This file contains the class definition for AlphaMachineBroker.
// AlphaMachineBroker is a machine broker for use with the cluster of
// alpha machines in 509 Davis Hall.
//
// What: "@(#) AlphaMachineBroker.h, revA"

#include <utility/actor/machineBroker/AlphaMachineBroker.h>
#include <cstdlib>

#include "utility/remote/remote.h"
#include "utility/actor/channel/Channel.h"

//! @brief Constructor.
XC::AlphaMachineBroker::AlphaMachineBroker(FEM_ObjectBroker *theBroker)
  :AlphaBaseMachineBroker(theBroker,0,8)
  {
    machines[0]= "alpha-1";
    machines[1]= "alpha-2";
    machines[2]= "alpha-3";
    machines[3]= "alpha-4";
    machines[4]= "alpha-5";
    machines[5]= "alpha-6";    
    machines[6]= "alpha-7";    
    machines[7]= "alpha-8";        
  }

int XC::AlphaMachineBroker::startActor(const std::string &actorProgram, Channel &theChannel, int compDemand)
  { 
    // get the next machine, a round-robin approach
    std::string machine;
    if(currentMachine < maxNumMachines)
      machine = machines[currentMachine];
    else
      {
	currentMachine = 0;
	machine = machines[currentMachine];
      }
    currentMachine++;
    //std::cerr << "CurrentMachine : " << machine <<  std::endl;
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

