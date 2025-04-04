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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/02/14 23:00:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/beam_loads/BeamPointLoad.cpp,v $
                                                                        
// Written: fmk 

// Purpose: This file contains the class implementation XC::BeamPointLoad.

#include <domain/load/beam_loads/BeamPointLoad.h>

#include "utility/actor/actor/MovableVector.h"

XC::BeamPointLoad::BeamPointLoad(int tag, int classTag, double Pt, double dist, const XC::ID &theElementTags, double Pa)
  :BeamMecLoad(tag, classTag,Pt,Pa,theElementTags),x(dist) {}

XC::BeamPointLoad::BeamPointLoad(int tag, int classTag)
  :BeamMecLoad(tag, classTag), x(0.0) {}

std::string XC::BeamPointLoad::Category(void) const
  { return "punctual"; }

//! @brief Send data through the communicator argument.
int XC::BeamPointLoad::sendData(Communicator &comm)
  {
    int res= BeamMecLoad::sendData(comm);
    res+= comm.sendDouble(x,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receive data through the communicator argument.
int XC::BeamPointLoad::recvData(const Communicator &comm)
  {
    int res= BeamMecLoad::recvData(comm);
    res+= comm.receiveDouble(x,getDbTagData(),CommMetaData(6));
    return res;
  }
