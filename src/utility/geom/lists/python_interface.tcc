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
//python_interface.cxx


class_<dq_pos2d>("dq_pos2d")
  .def(vector_indexing_suite<dq_pos2d>() )
  ;

class_<polyPos2d, bases<dq_pos2d> >("PolyPos2d")
  .def("getCenterOfMass", &polyPos2d::getCenterOfMass, "Return the center of mass of the point list.")
  .def("close", &polyPos2d::close, "Close the point list (insert the first point as its last one).")
  ;

class_<dq_pos3d>("dq_pos3d")
  .def(vector_indexing_suite<dq_pos3d>() )
  ;

class_<polyPos3d, bases<dq_pos3d> >("PolyPos3d")
  .def("getCenterOfMass", &polyPos3d::getCenterOfMass, "Return the center of mass of the point list.")
  .def("close", &polyPos3d::close, "Close the point list (insert the first point as its last one).")
  ;

