//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//python_interface.tcc

class_<XC::Cell, XC::Cell *, boost::noncopyable >("Cell", no_init)
  .def("getArea",&XC::Cell::getArea,"Return the area of the cell.")
  .def("getCentroidPosition", make_function( &XC::Cell::getCentroidPosition, return_internal_reference<>() ), "Return the position of the cell centroid.")
  ;

class_<XC::QuadCell, bases<XC::Cell>, boost::noncopyable >("QuadCell", no_init)
  ;

class_<XC::TriangCell, bases<XC::Cell>, boost::noncopyable >("TriangCell", no_init)
  ;

typedef std::vector<XC::Cell *> vector_cell_ptrs;
class_<vector_cell_ptrs, boost::noncopyable>("vector_cell_ptrs")
  .def(vector_indexing_suite<vector_cell_ptrs>() )
  ;

class_<XC::VectorCells, bases<vector_cell_ptrs>, boost::noncopyable >("VectorCells", no_init)
  ;
