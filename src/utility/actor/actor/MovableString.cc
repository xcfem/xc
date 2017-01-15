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
//MovableString.cc

#include "MovableString.h"
#include "utility/actor/channel/Channel.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/matrix/ID.h"

//! @brief Envía la cadena de caracteres through the channel being passed as parameter.
const XC::ID &stringToId(const std::string &str)
  {
    static XC::ID retval;
    static const int ratio= sizeof(int)/sizeof(char);
    const int *intPtr= reinterpret_cast<const int *>(str.c_str());
    const size_t sz= str.size()/ratio;
    retval= XC::getIDFromIntPtr(intPtr,sz+1);
    return retval;
  }

//! @brief Recibe la cadena de caracteres through the channel being passed as parameter.
const std::string &idToString(const XC::ID &id)
  {
    static std::string retval;
    const size_t sz= id.Size();
    if(sz>0)
      {
        const char *charPtr= reinterpret_cast<const char *>(id.getDataPtr());
        retval= std::string(charPtr);
      }
    else
      retval= "";
    return retval;
  }

//! @brief Constructor.
XC::MovableString::MovableString(const std::string &v)
  :MovableID(1) 
  { setString(v); }

//! @brief Asigna la cadena de caracteres.
void XC::MovableString::setString(const std::string &v)
  { setID(stringToId(v)); }

const std::string &XC::MovableString::getString(void)
  { return idToString(*this); }




