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

class_<XC::ZeroLength, bases<XC::Element0D>, boost::noncopyable >("ZeroLength", no_init)
  .def("clearMaterials",&XC::ZeroLength::clearMaterials)
  .def("setMaterial",&XC::ZeroLength::setMaterial)
  .def("getMaterials",make_function(&XC::ZeroLength::getMaterials,return_internal_reference<>()))
  ;

class_<XC::ZeroLengthSection, bases<XC::Element0D>, boost::noncopyable >("ZeroLengthSection", no_init)
  .add_property("getOrder",&XC::ZeroLengthSection::getOrder,"Returns element order (order of the underlying section deformation vector).")
  .def("getSection",make_function(&XC::ZeroLengthSection::getSection,return_internal_reference<>()),"Returns element's section.")
  .def("getMaterial",make_function(&XC::ZeroLengthSection::getSection,return_internal_reference<>()),"Returns element's section.")
  .add_property("getInternalForcesTransformation",make_function(&XC::ZeroLengthSection::getInternalForcesTransformation, return_internal_reference<>() ),"Returns element transformation.")
  .def("getVDirStrongAxisLocalCoord",&XC::ZeroLengthSection::getVDirStrongAxisLocalCoord,"Returns the direction vector of element strong axis expressed in the local coordinate system.")
  .def("getVDirWeakAxisLocalCoord",&XC::ZeroLengthSection::getVDirWeakAxisLocalCoord,"Returns the direction vector of element weak axis expressed in the local coordinate system.")
.def("getStrongAxisAngle",&XC::ZeroLengthSection::getStrongAxisAngle,"Returns the angle between element strong axis and local XZ plane.")
  .def("getWeakAxisAngle",&XC::ZeroLengthSection::getWeakAxisAngle,"Returns the angle between element weak axis and local XZ plane.")
  .def("getVDirStrongAxisGlobalCoord",make_function(&XC::ZeroLengthSection::getVDirStrongAxisGlobalCoord, return_value_policy<copy_const_reference>()),"Returns the direction vector of element strong axis expressed in the global coordinate system.")
  .def("getVDirWeakAxisGlobalCoord",make_function(&XC::ZeroLengthSection::getVDirWeakAxisGlobalCoord, return_value_policy<copy_const_reference>()),"Returns the direction vector of element weak axis expressed in the global coordinate system.")
  ;

class_<XC::ZeroLengthContact, bases<XC::Element0D>, boost::noncopyable >("ZeroLengthContact", no_init);

class_<XC::ZeroLengthContact2D, bases<XC::ZeroLengthContact>, boost::noncopyable >("ZeroLengthContact2D", no_init);

//class_<XC::ZeroLengthContact3D, bases<XC::ZeroLengthContact>, boost::noncopyable >("ZeroLengthContact3D", no_init);


