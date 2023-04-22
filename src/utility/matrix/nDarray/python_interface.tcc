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

class_<XC::nDarray>("nDarray")
  .def(init<list, list>())
  .def(init<int, double>())
  .def(init<int, int, list>())
  ;

class_<XC::BJmatrix , bases<XC::nDarray>>("BJmatrix")
  .def(init<list>())
  .def(init<int, int, list>())
  .def(self_ns::str(self_ns::self))
  ;

class_<XC::BJvector, bases<XC::BJmatrix>>("BJvector")
  .def(init<list>())
  .def(init<int, double>())
  ;

class_<XC::BJtensor, bases<XC::nDarray> >("BJtensor")
  .def(init<list, list>())
  ;

class_<XC::Cosseratstraintensor , bases<XC::BJtensor> >("Cosseratstraintensor")
  .def(init<double>())
  .def(init<list>())
  ;

class_<XC::Cosseratstresstensor , bases<XC::BJtensor> >("Cosseratstresstensor")
  .def(init<double>())
  .def(init<list>())
  ;

class_<XC::stressstraintensor , bases<XC::BJtensor> >("stressstraintensor")
  .def(init<double>())
  .def(init<boost::python::list>())
  .def(self_ns::str(self_ns::self))
  .def("Iinvariant1",&XC::stressstraintensor::Iinvariant1)
  .def("Iinvariant2",&XC::stressstraintensor::Iinvariant2)
  .def("Iinvariant3",&XC::stressstraintensor::Iinvariant3)
  .def("Jinvariant1",&XC::stressstraintensor::Jinvariant1)
  .def("Jinvariant2",&XC::stressstraintensor::Jinvariant2)
  .def("Jinvariant3",&XC::stressstraintensor::Jinvariant3)
  .def("sigma_octahedral",&XC::stressstraintensor::sigma_octahedral)
  .def("tau_octahedral",&XC::stressstraintensor::tau_octahedral)
  ;

class_<XC::straintensor, XC::straintensor *, bases<XC::stressstraintensor> >("straintensor")
  ;

class_<XC::stresstensor, XC::stresstensor *, bases<XC::stressstraintensor> >("stresstensor")
  .def(init<double>())
  .def(init<list>())
  .def("deviator",&XC::stresstensor::deviator)
  .def("principal",&XC::stresstensor::principal)
  ;

