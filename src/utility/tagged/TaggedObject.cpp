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
// $Date: 2003/02/14 23:02:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/utility/tagged/TaggedObject.cpp,v $
                                                                        
                                                                        
// File: ~/taggedt/TaggedObject.C
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class implementation for XC::TaggedObject.
// A XC::TaggedObject is an object with an integer identifier. It is used as
// a base class by XC::DomainComponent, Graph and other classes in the framework.
//
// What: "@(#) TaggedObject.C, revA"

#include "utility/tagged/TaggedObject.h"
#include "xc_basic/src/matrices/m_double.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "boost/lexical_cast.hpp"


//! @brief Constructor.
//!
//! Constructs a TaggedObject with a tag given by \p tag. The tag of
//! a component is some unique means of identifying the component among
//! like components, i.e. the tag of a node would be its unique node number.
//!
//! @param tag: object identifier.
//! @param owr: object owner (this object is somewhat contained by).
XC::TaggedObject::TaggedObject(int tag,EntCmd *owr)
  : EntCmd(owr), theTag(tag) {}

//! @brief Virtual constructor.
XC::TaggedObject *XC::TaggedObject::getCopy(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented yet." << std::endl;
    return nullptr;
  }

//! @brief Set tag.
//!
//! Sets the tag of the object to be \p newTag. It is provided so that
//! MovableObjects can set their tag in recvSelf().
void XC::TaggedObject::setTag(int newTag) 
  { theTag = newTag; }

//! @brief Set tag (to be called from Python interface).
void XC::TaggedObject::assignTag(int newTag) 
  {
    std::clog << "Call assignTag is not a good idea (usually)." << std::endl;
    setTag(newTag);
  }

//! @brief Print stuff.
void XC::TaggedObject::Print(std::ostream &s, int flag)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented yet." << std::endl;
  }

//! @brief Inserts the object on the output stream argument.
std::ostream &XC::operator<<(std::ostream &s, XC::TaggedObject &m)
  {
    m.Print(s);
    return s;
  }
