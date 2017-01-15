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
// $Date: 2005/11/30 23:47:00 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/subdomain/ActorSubdomain.h,v $
                                                                        
                                                                        
#ifndef ActorSubdomain_h
#define ActorSubdomain_h

// Written: fmk 
// Revision: A
//
// Description: This file contains the class definition for ActorSubdomain.
// ActorSubdomain is a container class. The class is responsible for holding
// and providing access to the Elements, Nodes, LoadCases, SFreedom_Constraints 
// and MFreedom_Constraints that have been added to the ActorSubdomain.
//
// What: "@(#) ActorSubdomain.h, revA"

#include "Subdomain.h"
#include "utility/actor/actor/Actor.h"
#include "utility/matrix/ID.h"

namespace XC {
class ActorSubdomain: public Subdomain, public Actor
  {
  private:
    ID msgData;
    Vector lastResponse;
  public:
    ActorSubdomain(Channel &, FEM_ObjectBroker &,DataOutputHandler::map_output_handlers *,EntCmd *);
    
    virtual int run(void);
    virtual const Vector &getLastExternalSysResponse(void);

    virtual int  update(void);    
    virtual int  updateTimeDt(void);    
    virtual int  barrierCheck(int res);    
  };
} // end of XC namespace
	
		   
#endif
