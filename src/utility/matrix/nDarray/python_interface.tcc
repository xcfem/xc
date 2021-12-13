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
//python_interface.tcc

class_<XC::nDarray>("nDarray")
  .def(init<int, double>())
  ;

class_<XC::BJmatrix , bases<XC::nDarray>, boost::noncopyable >("BJmatrix", no_init);

class_<XC::BJtensor, bases<XC::nDarray>, boost::noncopyable >("BJtensor", no_init);

class_<XC::Cosseratstraintensor , bases<XC::BJtensor>, boost::noncopyable >("Cosseratstraintensor", no_init);

class_<XC::Cosseratstresstensor , bases<XC::BJtensor>, boost::noncopyable >("Cosseratstresstensor", no_init);

class_<XC::straintensor , bases<XC::BJtensor> >("straintensor");

class_<XC::stresstensor , bases<XC::BJtensor> >("stresstensor")
  .def(init<int,double>())
  .def(init<boost::python::list>())
  .def(self_ns::str(self_ns::self))
  .def("Iinvariant1",&XC::stresstensor::Iinvariant1)
  .def("Iinvariant2",&XC::stresstensor::Iinvariant2)
  .def("Iinvariant3",&XC::stresstensor::Iinvariant3)
  .def("Jinvariant1",&XC::stresstensor::Jinvariant1)
  .def("Jinvariant2",&XC::stresstensor::Jinvariant2)
  .def("Jinvariant3",&XC::stresstensor::Jinvariant3)
  .def("deviator",&XC::stresstensor::deviator)
  .def("principal",&XC::stresstensor::principal)
  .def("sigma_octahedral",&XC::stresstensor::sigma_octahedral)
  .def("tau_octahedral",&XC::stresstensor::tau_octahedral)
  ;

