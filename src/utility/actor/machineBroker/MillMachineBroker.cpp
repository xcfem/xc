//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
#include <stdlib.h>

#include <string.h>
#include <remote.h>
#include <utility/actor/channel/Channel.h>

XC::MillMachineBroker::MillMachineBroker(FEM_ObjectBroker *theBroker)
  :AlphaBaseMachineBroker(theBroker), currentMachine(0),maxNumMachines(5)
{
    char *mill0 = "mill0";
    char *mill1 = "mill1";
    char *mill2 = "mill2";
    char *mill3 = "mill3";
    char *mill4 = "mill4";
    
    char **theMachines = (char **)malloc(5*sizeof(char *));
    theMachines[0] = mill0;
    theMachines[1] = mill1;
    theMachines[2] = mill2;
    theMachines[3] = mill3;
    theMachines[4] = mill4;
    
    machines = theMachines;
}


int 
XC::MillMachineBroker::startActor(char *actorProgram, 
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

    // std::cerr << "XC::MillMachineBroker::Constructor - command\n"<< remotecmd;
    system(remotecmd);

    return 0;
}

