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
//NamedEntity.cc

#include "NamedEntity.h"

//! @brief Return a Python dictionary with the object members values.
boost::python::dict NamedEntity::getPyDict(void) const
  {
    boost::python::dict retval= CommandEntity::getPyDict();
    retval["name"]= name;
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void NamedEntity::setPyDict(const boost::python::dict &d)
  {
    CommandEntity::setPyDict(d);
    name= boost::python::extract<std::string>(d["name"]);
  }

//! @brief += operator.
NamedEntity &NamedEntity::operator+=(const NamedEntity &other)
  {
    name= '('+ name + '+' + other.getName()+')';
    return *this;
  }

//! @brief -= operator.
NamedEntity &NamedEntity::operator-=(const NamedEntity &other)
  {
    name= '('+ name + '-' + other.getName()+')';
    return *this;
  }

//! @brief *= operator.
NamedEntity &NamedEntity::operator*=(const NamedEntity &other)
  {
    name= '('+ name + '*' + other.getName()+')';
    return *this;
  }

//! @brief Return the object named as: '(a.name+b.name)'
NamedEntity operator+(const NamedEntity &a,const NamedEntity &b)
  {
    NamedEntity retval(a);
    retval+=b;
    return retval;
  }

//! @brief Return the object named as: '(a.name-b.name)'
NamedEntity operator-(const NamedEntity &a,const NamedEntity &b)
  {
    NamedEntity retval(a);
    retval-= b;
    return retval;
  }

//! @brief Return the object named as: '(a.name*b.name)'
NamedEntity operator*(const NamedEntity &a,const NamedEntity &b)
  {
    NamedEntity retval(a);
    retval*= b;
    return retval;    
  }

