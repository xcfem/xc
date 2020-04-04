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
// $Date: 2000/09/15 08:23:15 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/actor/MovableObject.cpp,v $
                                                                        
                                                                        
// File: ~/actor/MovableObject.C
//
// Written: fmk
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the implementation of MovableObject.
//
// What: "@(#) MovableObject.C, revA"

#include "MovableObject.h"
#include "utility/actor/actor/Communicator.h"
#include "utility/actor/channel/Channel.h"
#include "utility/matrix/ID.h"

//! @brief Constructor. Sets the class tag and the database tag.
//!
//! Sets the objects class identifier to \p cTag:
//! this is a unique id for each class of instantiable movable
//! objects. The identifier will allow an object broker to recognize the
//! object type to be instantiated. Sets the objects database tag to \p
//! dTag: this is a unique id for identifying the object in a database.
XC::MovableObject::MovableObject(int cTag, int dTag)
  :classTag(cTag), dbTag(dTag) {}


//! @brief Constructor. Sets the objects class identifier to \p theTag 
//! and sets the objects database tag to 0.
XC::MovableObject::MovableObject(int theTag)
  :classTag(theTag), dbTag(0) {}

//! @brief Copy constructor. Doesn't copy the dbTag.
XC::MovableObject::MovableObject(const MovableObject &other)
  :classTag(other.classTag), dbTag(0) {}

//! @brief Assignment operator. Doesn't copy the dbTag.
XC::MovableObject &XC::MovableObject::operator=(const MovableObject &other)
  {
    classTag= other.classTag;
    return *this;
  }

//! @brief Return the class identifier.
int XC::MovableObject::getClassTag(void) const
  { return classTag; }

//! @brief Return the object identifier in the database.
int XC::MovableObject::getDbTag(void) const
  { return dbTag; }

//! @brief Return the object identifier in the database.
int XC::MovableObject::getDbTag(Communicator &comm)
  {
    if(dbTag==0)
      dbTag = comm.getDbTag();
    return dbTag;
  }

//! @brief Sets the object identifier in the database.
void XC::MovableObject::setDbTag(int newTag)
  { dbTag = newTag; }

//! @brief Sets the object identifier in the database if not
//! already set.
void XC::MovableObject::setDbTag(Communicator &comm)
  {
    if(dbTag==0)
      dbTag = comm.getDbTag();
  }

//! @brief Sets the value \p param to the parameter \p argv. 
int XC::MovableObject::setParameter(const std::vector<std::string> &argv, Parameter &param)
  { return 0; }

//! @brief Updates the parameter identified by \p parameterID with \p info. 
int XC::MovableObject::updateParameter(int parameterID, Information &info)
  { return 0; }

//! @brief Activates the parameter identified by \p parameterID
int XC::MovableObject::activateParameter(int parameterID)
  { return 0; }

//! @brief Set the value of the variable idenfified by \p var.
int XC::MovableObject::setVariable(const std::string &var, Information &theInfo)
  { return -1; }

//! @brief Return the value of the variable idenfified by \p var.
int XC::MovableObject::getVariable(const std::string &variable, Information &theInfo)
  { return -1; }

