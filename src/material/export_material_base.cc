//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
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
//utils_python_interface.cxx

#include "python_interface.h"

void export_material_base(void)
  {
    using namespace boost::python;
    docstring_options doc_options;

    class_<XC::Material, bases<XC::MovableObject,XC::TaggedObject>, boost::noncopyable >("Material", no_init)
        .def("commitState", &XC::Material::commitState,"Commits material's state.")
        .def("revertToLastCommit", &XC::Material::revertToLastCommit,"Returns the material to the last commited state.")
        .def("revertToStart", &XC::Material::revertToStart,"Returns the material to its initial state.")
        .def("getName",&XC::Material::getName,"Returns the name of the material.")
       ;
  }

