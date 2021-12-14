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

class_<Point2d, bases<GeomObj2d> >("Point2d")
  .def(init<Point2d>())
  .def(init<double, double>())
  .def("getPos2d", &Point2d::getPos,"Return the position of the point.")  
  ;

class_<Point3d, bases<GeomObj3d> >("Point3d")
  .def(init<Point3d>())
.def(init<double, double, double>())
  .def("getPos3d", &Point3d::getPos,"Return the position of the point.")  
  ;
