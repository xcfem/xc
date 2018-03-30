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
//LoadHandlerMember.cc

#include "LoadHandlerMember.h"
#include "preprocessor/prep_handlers/LoadHandler.h"


//! @brief Default constructor.
XC::LoadHandlerMember::LoadHandlerMember(LoadHandler *owr)
  : EntCmd(owr), MovableObject(0) {}


//! @brief Returns the owner.
const XC::LoadHandler *XC::LoadHandlerMember::getLoadHandler(void) const
  {
    const LoadHandler *handler= dynamic_cast<const LoadHandler *>(Owner());
    if(!handler)
      std::cerr << "(LoadHandlerMember) need an owner." << std::endl;
    return handler;
  }

//! @brief Returns the owner.
XC::LoadHandler *XC::LoadHandlerMember::getLoadHandler(void)
  {
    LoadHandler *handler= dynamic_cast<LoadHandler *>(Owner());
    if(!handler)
      std::cerr << "(LoadHandlerMember) need an owner." << std::endl;
    return handler;
  }

//! @brief Returns the domain.
const XC::Domain *XC::LoadHandlerMember::getDomain(void) const
  {
    const Domain *retval= nullptr;
    const LoadHandler *handler= getLoadHandler();
    if(handler)
      retval= handler->getDomain();
    return retval;
  }

//! @brief Returns the domain.
XC::Domain *XC::LoadHandlerMember::getDomain(void)
  {
    Domain *retval= nullptr;
    LoadHandler *handler= getLoadHandler();
    if(handler)
      retval= handler->getDomain();
    return retval;
  }

//! @brief Returns the tag para el siguiente load pattern.
int &XC::LoadHandlerMember::getTagLP(void)
  { return getLoadHandler()->getTagLP(); }

//! @brief Returns the tag para el siguiente load pattern.
const int &XC::LoadHandlerMember::getTagLP(void) const
  { return getLoadHandler()->getTagLP(); }

