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
//FrenetTrihedron.cc

#include "FrenetTrihedron.h"

//! @brief Default constructor.
FrenetTrihedron::FrenetTrihedron(void)
  : ProtoGeom(), path() 
  {}

//! @brief Constructor.
FrenetTrihedron::FrenetTrihedron(const Polyline3d &pth)
  : ProtoGeom(), path(pth) 
  {}


//! @brief Prints the matrix.
void FrenetTrihedron::Print(std::ostream &os) const
  { os << "path= " << this->path; }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict FrenetTrihedron::getPyDict(void) const
  {
    boost::python::dict retval= ProtoGeom::getPyDict();
    retval["path"]= path.getPyDict();
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void FrenetTrihedron::setPyDict(const boost::python::dict &d)
  {
    ProtoGeom::setPyDict(d);
    this->path= boost::python::extract<Polyline3d>(d["path"]);
  }

std::ostream &operator<<(std::ostream &os,const FrenetTrihedron &sc)
  {
    sc.Print(os);
    return os;
  }
