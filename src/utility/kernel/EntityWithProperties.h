// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
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
//EntityWithProperties.h

#ifndef ENTPROP_H
#define ENTPROP_H

#include "EntityWithOwner.h"
#include <map>
#include <boost/python.hpp>
#include <boost/python/dict.hpp>

//! @ingroup NUCLEO
//
//! @brief Object that can return properties as Python objects.
class EntityWithProperties: public EntityWithOwner
  {
  private:
    static inline const std::string py_prop_prefix= "py_prop";
    typedef std::map<std::string, boost::python::object> PythonDict;
    
    PythonDict python_dict; //!< Python variables.
  public:
    EntityWithProperties(EntityWithProperties *owr= nullptr);
    
    virtual bool operator==(const EntityWithProperties &) const;
    
    void clearPyProps(void);
    bool hasPyProp(const std::string &);
    boost::python::object getPyProp(const std::string &str);
    void setPyProp(std::string str, boost::python::object val);
    boost::python::list getPropNames(void) const;
    virtual boost::python::dict getPyDict(void) const;
    virtual void setPyDict(const boost::python::dict &);
  };

#endif
