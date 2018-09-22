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

void export_material_nD(void)
  {
    using namespace boost::python;
    docstring_options doc_options;

#include "python_interface.tcc"

typedef std::vector<XC::NDMaterial *> vectorNDMaterial;
class_<vectorNDMaterial,boost::noncopyable>("vectorNDMaterial")
  .def(vector_indexing_suite<vectorNDMaterial>() )
  ;

class_<material_vector_NDMat,bases<vectorNDMaterial,CommandEntity>,boost::noncopyable>("MaterialVectorUMat", no_init)
  .def("commitState", &material_vector_NDMat::commitState,"Commits materials state.")
  .def("revertToLastCommit", &material_vector_NDMat::revertToLastCommit,"Returns the material to its last commited state.")
  .def("revertToStart", &material_vector_NDMat::revertToStart,"Returns the material to its initial state.")
  .def("getNames",&material_vector_NDMat::getNamesPy,"Returns the names of the materials.")
  ;

  }

