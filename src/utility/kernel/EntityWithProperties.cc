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
//EntityWithProperties.cc

#include "EntityWithProperties.h"
#include <iostream>

//! @brief Default constructor.
EntityWithProperties::EntityWithProperties(EntityWithProperties *owr)
  : EntityWithOwner(owr)
  {}

//! @brief Comparison operator.
bool EntityWithProperties::operator==(const EntityWithProperties &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= EntityWithOwner::operator==(other);
        if(retval)
          retval= (python_dict==other.python_dict);
       }
    return retval;
  }

//! @brief Clear python properties map.
void EntityWithProperties::clearPyProps(void)
  { python_dict.clear(); }

//! @brief Returns true if property exists.
bool EntityWithProperties::hasPyProp(const std::string &str)
  { return (python_dict.find(str) != python_dict.end()); }

//! @brief Return the Python object with the name being passed as parameter.
boost::python::object EntityWithProperties::getPyProp(const std::string &str)
   {
     boost::python::object retval; //Defaults to None.
     // Python checks the class attributes before it calls __getattr__
     // so we don't have to do anything special here.
     PythonDict::const_iterator i= python_dict.find(str);
     if(i == python_dict.end())
       {
         std::clog << getClassName() << "::" << __FUNCTION__
	           << "; Warning, property: '" << str
		   << "' not found. Returning None."
		   << std::endl;
         // PyErr_SetString(PyExc_AttributeError, str.c_str());
         // throw boost::python::error_already_set();
       }
     else
       retval= i->second;
     return retval;
   }

//! @brief Sets/appends a value tho the Python object's dictionary.
// However, with __setattr__, python doesn't do anything with the class attributes first,
// it just calls __setattr__.
// Which means anything that's been defined as a class attribute won't be modified
// here - including things set with
//add_property(), def_readwrite(), etc.
void EntityWithProperties::setPyProp(std::string str, boost::python::object val)
  {
    python_dict[str] = val;
  }

//! @brief Return the names of the object properties weightings.
boost::python::list EntityWithProperties::getPropNames(void) const
  {
    boost::python::list retval;
    for(PythonDict::const_iterator i= python_dict.begin();i!= python_dict.end();i++)
      retval.append((*i).first);
    return retval;
  }

//! @brief Return a Python dictionary containing the object members values.
boost::python::dict EntityWithProperties::getPyDict(void) const
  {
    boost::python::dict retval;
    // Store properties.
    for(PythonDict::const_iterator i= python_dict.begin();i!= python_dict.end();i++)
      {
	const std::string key= py_prop_prefix+(*i).first;
        retval[key]= (*i).second;
      }
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void EntityWithProperties::setPyDict(const boost::python::dict &d)
  {
    auto items = d.attr("items")(); // just plain d.items or d.iteritems for Python 2!
    for(auto it = boost::python::stl_input_iterator<boost::python::tuple>(items); it != boost::python::stl_input_iterator<boost::python::tuple>(); ++it)
      {
	boost::python::tuple kv = *it;
	std::string key= boost::python::extract<std::string>(kv[0]);
	if(key.rfind(py_prop_prefix, 0) == 0) // it's a property.
	  {
	    const int sz= py_prop_prefix.size();
	    key.erase(0,sz); // remove prefix
            auto value= kv[1];
	    setPyProp(key, value);
          }    
      }
  }
