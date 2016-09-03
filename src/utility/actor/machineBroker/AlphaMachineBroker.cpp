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
// $Source: /usr/local/cvs/OpenSees/SRC/actor/machineBroker/AlphaMachineBroker.cpp,v $
                                                                        
                                                                        
// File: ~/actor/broker/AlphaMachineBroker.h
//
// Written: fmk
// Created: Fri Sept 20 12:27:47: 1996
// Revision: A
//
// Purpose: This file contains the class definition for XC::AlphaMachineBroker.
// AlphaMachineBroker is a machine broker for use with the cluster of
// alpha machines in 509 Davis Hall.
//
// What: "@(#) AlphaMachineBroker.h, revA"

#include <utility/actor/machineBroker/AlphaMachineBroker.h>
#include <stdlib.h>

#include <string.h>
#include <remote.h>
#include <utility/actor/channel/Channel.h>

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

int XC::AlphaMachineBroker::startActor(char *actorProgram, Channel &theChannel, int compDemand)
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
    std::string remotecmd= REMOTE + " " + machine + " " + actorProgram 
                           + " " + theChannel.addToProgram() + "\n";
    
    system(remotecmd.c_str());
    return 0;
  }

