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
// $Date: 2003/02/14 23:01:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/ElementalLoad.cpp,v $
                                                                        
                                                                        
// Written: fmk 11/95
//          modified 11/01 for new design

// Purpose: This file contains the methods for class XC::ElementalLoad.

#include "domain/load/ElementalLoad.h"
#include "utility/matrix/Vector.h"
#include "preprocessor/set_mgmt/SetBase.h"


//! @brief Constructor.
//!
//! @param tag: elemental load identifier.
//! @param classTag: identifier of the class.
XC::ElementalLoad::ElementalLoad(int tag, int classTag)
  : Load(tag, classTag) {}

//! @brief Constructor.
//!
//! @param tag: elemental load identifier.
//! @param classTag: identifier of the class.
//! @param tags_elems: identifiers of the loaded elements.
XC::ElementalLoad::ElementalLoad(int tag, int classTag,const ID &tags_elems)
  :Load(tag, classTag), elemTags(tags_elems) {}

//! @brief Return the number of loaded elements.
int XC::ElementalLoad::numElements(void) const
  { return elemTags.Size(); }

//! @brief Return the identifiers of the loaded elements.
const XC::ID &XC::ElementalLoad::getElementTags(void) const
  { return elemTags; }

//! @brief Return the category of the load.
//!
//! Return a string like 'uniform', 'punctual',...
std::string XC::ElementalLoad::Category(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet." << std::endl;
    return "unknown";
  }


//! @brief Sets the identifiers of the loaded elements.
int XC::ElementalLoad::setElementTags(const ID &theEleTags)
  {
    elemTags= theEleTags;
    return 0;
  }

//! @brief Sets the identifiers of the loaded elements.
int XC::ElementalLoad::setElementTagsFromSet(const SetBase &s)
  {
    const std::set<int> elementTags= s.getElementTags();
    elemTags= ID(elementTags.begin(), elementTags.end());
    return 0;
  }

const XC::Vector &XC::ElementalLoad::getSensitivityData(const int &gradIndex) const
  {
    static Vector trash(10);
    return trash;
  }

//! @brief Send data through the communicator argument.
int XC::ElementalLoad::sendData(Communicator &comm)
  {
    int res= Load::sendData(comm);
    res+= comm.sendID(elemTags,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receive data through the communicator argument.
int XC::ElementalLoad::recvData(const Communicator &comm)
  {
    int res= Load::recvData(comm);
    res+= comm.receiveID(elemTags,getDbTagData(),CommMetaData(2));
    return res;
  }
