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
// $Source: /usr/local/cvs/OpenSees/SRC/actor/machineBroker/DecMachineBroker.cpp,v $
                                                                        
                                                                        
// File: ~/actor/broker/DecMachineBroker.h
//
// Written: fmk
// Created: Fri Sept 20 12:27:47: 1996
// Revision: A
//
// Purpose: This file contains the class definition for XC::DecMachineBroker.
// DecMachineBroker is a machine broker for use with the cluster of
// dec machines in 509 Davis Hall.
//
// What: "@(#) DecMachineBroker.h, revA"

#include <utility/actor/machineBroker/DecMachineBroker.h>
#include <stdlib.h>

#include <string.h>
#include <remote.h>
#include <utility/actor/channel/Channel.h>


XC::DecMachineBroker::DecMachineBroker(FEM_ObjectBroker *theBroker)
:AlphaBaseMachineBroker(theBroker,0,9)
  {
    char *dec1 = "dec-1";
    char *dec2 = "dec-2";
    char *dec3 = "dec-3";
    char *dec4 = "dec-4";
    char *dec5 = "dec-5";
    char *dec6 = "dec-6";    
    char *dec7 = "dec-7";    
    char *dec8 = "dec-8";        
    char *dec9 = "dec-9";            
    
    char **theMachines = (char **)malloc(9*sizeof(char *));
    theMachines[0] = dec9;
    theMachines[1] = dec8;
    theMachines[2] = dec7;
    theMachines[3] = dec6;
    theMachines[4] = dec5;
    theMachines[5] = dec4;
    theMachines[6] = dec3;    
    theMachines[7] = dec2;    
    theMachines[8] = dec1;        
    
    machines = theMachines;
}


int XC::DecMachineBroker::startActor(char *actorProgram, 
			       Channel &theChannel,
			       int compDemand)
{ 
    char  remotecmd[400];

    // get the next machine, a round-robin approach
    char *machine;
    if (currentMachine < maxNumMachines)
	machine = machines[currentMachine];
    else {
	currentMachine = 0;
	machine = machines[currentMachine];
    }

    currentMachine++;
    
    strcpy(remotecmd,REMOTE);
    strcat(remotecmd," ");          
    strcat(remotecmd,machine);
    strcat(remotecmd," ");
    strcat(remotecmd,actorProgram);
    strcat(remotecmd," ");
    strcat(remotecmd,theChannel.addToProgram());    
    strcat(remotecmd,"\n");

    system(remotecmd);

    return 0;
}

