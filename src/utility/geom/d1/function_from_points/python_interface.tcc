//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//python_interface.cxx

class_<FunctionFromPointsR_R >("FunctionGraph1D")
  .def("__call__", &FunctionFromPointsR_R::Valor)
  .def("size", &FunctionFromPointsR_R::size)
  .def("empty", &FunctionFromPointsR_R::empty)
  .def("append", &FunctionFromPointsR_R::insert)
  .def("clear", &FunctionFromPointsR_R::clear)
  .def("Max", &FunctionFromPointsR_R::Max)
  .def("AbcisaMax", &FunctionFromPointsR_R::AbcisaMax)
  .def("Min", &FunctionFromPointsR_R::Min)
  .def("AbcisaMin", &FunctionFromPointsR_R::AbcisaMin)
  .def("getArea", &FunctionFromPointsR_R::getArea,"Return the area enclosed by the function.")
  .def("firstX", &FunctionFromPointsR_R::firstX)
  .def("firstY", &FunctionFromPointsR_R::firstY)
  .def("lastX", &FunctionFromPointsR_R::lastX)
  .def("lastY", &FunctionFromPointsR_R::lastY)
  .def("sub", &FunctionFromPointsR_R::sub)
  .def("getNeg", &FunctionFromPointsR_R::getNeg)
  .def(self * double())
  .def(double() * self)
  .def(self / double())
  .def(self + self)
  .def(self - self)
  .def(self *= self)
  .def(self += double())
  .def(self += self)
  .def(self -= double())
  .def(self -= self)
  .def(self *= double())
  .def(self /= double())
  .def(self /= self)
  .def("integra", &FunctionFromPointsR_R::integra)
  ;

class_<FunctionFromPointsR2_R >("FunctionGraph2D")
  ;
