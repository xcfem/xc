//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//EntityWithOwner.cc

#include "EntityWithOwner.h"

#include <iostream>
#include <cxxabi.h>

// Static variables.
int EntityWithOwner::verbosity= 1;

//! @brief Default constructor.
EntityWithOwner::EntityWithOwner(EntityWithOwner *owr)
  : owner(owr)
  {
    if(this == owner)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ¡ojo!, object is owner of itself." << std::endl;
  }

//! @brief Copy constructor
EntityWithOwner::EntityWithOwner(const EntityWithOwner &other)
  : owner(other.owner)
  {
    if(this == owner)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ¡ojo!, object is owner of itself." << std::endl;
  }

//! @brief Assignment operator.
EntityWithOwner &EntityWithOwner::operator=(const EntityWithOwner &other)
  {
    owner= other.owner;
    if(this == owner)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; ¡ojo!, objeto of class: '"
		  << getClassName() 
                  << "', owns itself." << std::endl;
        owner= nullptr;
      }
    return *this;
  }

//! @brief Comparison operator.
bool EntityWithOwner::operator==(const EntityWithOwner &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      { retval= (owner==other.owner); }
    return retval;
  }

//! @brief Returns demangled class name.
std::string EntityWithOwner::getClassName(void) const
  {
    std::string tmp= typeid(*this).name();
    std::bad_exception  e;
    int status;
    char *realname= abi::__cxa_demangle(tmp.c_str(), 0, 0, &status);
    if(realname)
      tmp= std::string(realname);
    free(realname);
    return tmp;
  }

//! @brief Get the value of the verbosity level.
int EntityWithOwner::getVerbosityLevel(void)
  { return verbosity; }

//! @brief Set the value of the verbosity level.
void EntityWithOwner::setVerbosityLevel(const int &vl)
  { verbosity= vl; }

//! @brief Asigna el propietario del objeto.
void EntityWithOwner::set_owner(EntityWithOwner *owr)
  { owner= owr; }



