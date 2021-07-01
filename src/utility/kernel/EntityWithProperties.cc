//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
//EntityWithProperties.cc

#include "EntityWithProperties.h"
#include <fstream>
#include <iostream>
#include "utility/utils/misc_utils/memoria.h"
#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string.hpp"
#include <cxxabi.h>
#include <sys/stat.h>

#include <cctype>
#include "CImg.h"

//Variables estáticas de EntityWithProperties.
int EntityWithProperties::verbosity= 1;

//! @brief Default constructor.
EntityWithProperties::EntityWithProperties(EntityWithProperties *owr)
  : owner(owr)
  {
    if(this == owner)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ¡ojo!, object is owner of itself." << std::endl;
  }

//! @brief Copy constructor
EntityWithProperties::EntityWithProperties(const EntityWithProperties &other)
  : owner(other.owner)
  {
    if(this == owner)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ¡ojo!, object is owner of itself." << std::endl;
  }

//! @brief Assignment operator.
EntityWithProperties &EntityWithProperties::operator=(const EntityWithProperties &other)
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
bool EntityWithProperties::operator==(const EntityWithProperties &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      { retval= (owner==other.owner); }
    return retval;
  }

//! @brief Returns demangled class name.
std::string EntityWithProperties::getClassName(void) const
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
int EntityWithProperties::getVerbosityLevel(void)
  { return verbosity; }

//! @brief Set the value of the verbosity level.
void EntityWithProperties::setVerbosityLevel(const int &vl)
  { verbosity= vl; }

//! @brief Asigna el propietario del objeto.
void EntityWithProperties::set_owner(EntityWithProperties *owr)
  { owner= owr; }

