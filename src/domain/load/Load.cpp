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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:19 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/Load.cpp,v $
                                                                        
                                                                        
// File: ~/domain/load/Load.C
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the implementation for the XC::Load class.
//
// What: "@(#) Load.C, revA"


#include <domain/load/Load.h>

#include "utility/matrix/ID.h"

//! @brief Constructor.
//! 
//! Constructs a load with a tag given by \p tag and a class tag is
//! given by \p classTag. These are passed to the DomainComponent constructor.
XC::Load::Load(int tag, int classTag)
  : DomainComponent(tag, classTag), loadPatternTag(-1) {}

//! @brief Sets the identifier of the pattern to which the load belongs.
void XC::Load::setLoadPatternTag(int tag)
  { loadPatternTag= tag; }

//! @brief Return the identifier of the pattern to which the load belongs. If no
//! load pattern tag has been set \f$-1\f$ is returned.
int XC::Load::getLoadPatternTag(void) const
  { return loadPatternTag; }

//! @brief Send data through the channel being passed as parameter.
int XC::Load::sendData(Communicator &comm)
  {
    setDbTagDataPos(0,getTag());
    int res= comm.sendInt(loadPatternTag,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receive data through the channel being passed as parameter.
int XC::Load::recvData(const Communicator &comm)
  {
    setTag(getDbTagDataPos(0));
    int res= comm.receiveInt(loadPatternTag,getDbTagData(),CommMetaData(1));
    return res;
  }
