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
#include "utility/utils/misc_utils/colormod.h"

// Static variables.
int EntityWithOwner::verbosity= 1;

//! @brief Default constructor.
EntityWithOwner::EntityWithOwner(EntityWithOwner *owr)
  : owner(owr)
  {
    if(this == owner)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
                << "; warning!, object is owner of itself."
		<< Color::def << std::endl;
  }

//! @brief Copy constructor
EntityWithOwner::EntityWithOwner(const EntityWithOwner &other)
  : owner(other.owner)
  {
    if(this == owner)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
                << "; warning!, object is owner of itself."
		<< Color::def << std::endl;
  }

//! @brief Assignment operator.
EntityWithOwner &EntityWithOwner::operator=(const EntityWithOwner &other)
  {
    owner= other.owner;
    if(this == owner)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; warning!, object of class: '"
		  << getClassName() 
                  << "', owns itself."
		  << Color::def << std::endl;
        owner= nullptr;
      }
    return *this;
  }

//! @brief Return true if both objects are equal.
bool EntityWithOwner::isEqual(const EntityWithOwner &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
	retval= (typeid(*this) == typeid(other));
	if(retval)
	  {
	    retval= (this->owner==other.owner);
	  }
      }
    return retval;
  }

//! @brief Comparison operator.
bool EntityWithOwner::operator==(const EntityWithOwner &other) const
  { return this->isEqual(other); }

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

//! @brief Assigns the owner of the object.
void EntityWithOwner::set_owner(EntityWithOwner *owr)
  { owner= owr; }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict EntityWithOwner::getPyDict(void) const
  {
    boost::python::dict retval;
    //retval["ownerAddress"]= reinterpret_cast<size_t>(owner); NOT USED.
    retval["className"]= getClassName();
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void EntityWithOwner::setPyDict(const boost::python::dict &d)
  {
    // const size_t ownerAddr= boost::python::extract<size_t>(d["ownerAddress"]);
    // //XXX we need a mechanism to restore the owner from its address.
    // if(ownerAddr!=0)
    //   std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
    // 	        << "; mechanism to restore the owner from its address"
    // 	        << " not implemented yet."
    // 	        << Color::def << std::endl;
  }

