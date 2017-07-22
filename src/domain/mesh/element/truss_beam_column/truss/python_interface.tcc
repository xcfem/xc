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

class_<XC::ProtoTruss, bases<XC::Element1D>, boost::noncopyable >("ProtoTruss", no_init)
  .def("getDim", &XC::ProtoTruss::getNumDIM,"Return the dimension of the space in which the element is defined (1, 2 or 3).")
  .def("getMaterial",&XC::ProtoTruss::getMaterialRef,return_internal_reference<>(),"Returns element's material")
  ;

class_<XC::TrussBase, bases<XC::ProtoTruss>, boost::noncopyable >("TrussBase", no_init)
  .def("getL", &XC::TrussBase::getL, return_value_policy<copy_const_reference>(),"Return the longitud of the element.")
   ;

class_<XC::TrussSection , bases<XC::TrussBase>, boost::noncopyable >("TrussSection", no_init);

class_<XC::Spring , bases<XC::ProtoTruss>, boost::noncopyable >("Spring", no_init)
  .def("getN", &XC::Spring::getAxialForce,"Returns axial (N) internal force.")
  ;


class_<XC::Truss, bases<XC::TrussBase>, boost::noncopyable >("Truss")
  .add_property("area", make_function( &XC::Truss::getArea, return_value_policy<copy_const_reference>() ), &XC::Truss::setArea)
  .def("getN", &XC::Truss::getAxialForce,"Returns axial (N) internal force.")
  .add_property("getN1", &XC::Truss::getAxialForce,"Returns axial (N) internal force at node 1.")
  .add_property("getN2", &XC::Truss::getAxialForce,"Returns axial (N) internal force at node 2.")
   ;

class_<XC::CorotTrussBase, bases<XC::ProtoTruss>, boost::noncopyable >("CorotTrussBase", no_init);

class_<XC::CorotTruss, bases<XC::CorotTrussBase>, boost::noncopyable >("CorotTruss", no_init)
  .add_property("area", make_function( &XC::CorotTruss::getArea, return_value_policy<copy_const_reference>() ), &XC::CorotTruss::setArea)
  .def("getN", &XC::CorotTruss::getAxialForce,"Returns N internal force.")
  .add_property("getN1", &XC::CorotTruss::getAxialForce,"Returns axial (N) internal force at node 1.")
  .add_property("getN2", &XC::CorotTruss::getAxialForce,"Returns axial (N) internal force at node 2.")
  ;

class_<XC::CorotTrussSection , bases<XC::CorotTrussBase>, boost::noncopyable >("CorotTrussSection", no_init)
  ;



