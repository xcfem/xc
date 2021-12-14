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
//ProtoGeom.cc

#include "ProtoGeom.h"
#include "utility/geom/pos_vec/Pos2d.h"
#include "utility/geom/pos_vec/Vector2d.h"


#include "FT_matrix.h"
#include <iostream>


//! @brief Constructor.
ProtoGeom::ProtoGeom(void)
  : exts(true) {}

void ProtoGeom::Print(std::ostream &os) const
  { os << "nil_geom_obj"; }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict ProtoGeom::getPyDict(void) const
  {
    boost::python::dict retval= CommandEntity::getPyDict();
    retval["exts"]= exts;
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void ProtoGeom::setPyDict(const boost::python::dict &d)
  {
    CommandEntity::setPyDict(d);
    exts= boost::python::extract<bool>(d["exts"]);
  }
