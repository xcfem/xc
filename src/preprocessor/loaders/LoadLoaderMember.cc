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
//LoadLoaderMember.cc

#include "LoadLoaderMember.h"
#include "preprocessor/loaders/LoadLoader.h"


//! @brief Default constructor.
XC::LoadLoaderMember::LoadLoaderMember(LoadLoader *owr)
  : EntCmd(owr), MovableObject(0) {}


//! @brief Returns the owner.
const XC::LoadLoader *XC::LoadLoaderMember::getLoadLoader(void) const
  {
    const LoadLoader *loader= dynamic_cast<const LoadLoader *>(Owner());
    if(!loader)
      std::cerr << "(LoadLoaderMember) need an owner." << std::endl;
    return loader;
  }

//! @brief Returns the owner.
XC::LoadLoader *XC::LoadLoaderMember::getLoadLoader(void)
  {
    LoadLoader *loader= dynamic_cast<LoadLoader *>(Owner());
    if(!loader)
      std::cerr << "(LoadLoaderMember) need an owner." << std::endl;
    return loader;
  }

//! @brief Returns the domain.
const XC::Domain *XC::LoadLoaderMember::getDomain(void) const
  {
    const Domain *retval= nullptr;
    const LoadLoader *loader= getLoadLoader();
    if(loader)
      retval= loader->getDomain();
    return retval;
  }

//! @brief Returns the domain.
XC::Domain *XC::LoadLoaderMember::getDomain(void)
  {
    Domain *retval= nullptr;
    LoadLoader *loader= getLoadLoader();
    if(loader)
      retval= loader->getDomain();
    return retval;
  }

//! @brief Returns the tag para el siguiente load pattern.
int &XC::LoadLoaderMember::getTagLP(void)
  { return getLoadLoader()->getTagLP(); }

//! @brief Returns the tag para el siguiente load pattern.
const int &XC::LoadLoaderMember::getTagLP(void) const
  { return getLoadLoader()->getTagLP(); }

