//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
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
//ConjVariables.cc

#include "ConjVariables.h"
#include <iostream>

//! @brief Devuelve verdadero si la variable pertenece al conjunto.
bool ConjVariables::In(const Variable *var) const
  {
    return (find(var)!=end());
  }
//! @brief Devuelve verdadero si la variable pertenece al conjunto.
bool ConjVariables::In(const Variable &var) const
  {
    return (find(&var)!=end());
  }

//! @brief Return the names of the variables.
std::vector<std::string> ConjVariables::getNames(void) const
  {
    std::vector<std::string> retval(size());
    size_t conta= 0;
    for(const_iterator i= begin();i!=end();i++,conta++)
      retval[conta]= (*i)->getName();
    return retval;
  }

std::ostream &operator<<(std::ostream &stream,const ConjVariables &cv)
  { return stream << "operador salida ConjVariables no implementado." << std::endl; }

