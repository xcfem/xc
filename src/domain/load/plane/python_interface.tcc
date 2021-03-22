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

class_<XC::BidimLoad, bases<XC::ElementBodyLoad>, boost::noncopyable >("BidimLoad", no_init)
  .add_property("category", &XC::BidimLoad::Category)
  ;

class_<XC::BidimMecLoad, bases<XC::BidimLoad>, boost::noncopyable >("BidimMecLoad", no_init)
  .def("getResultant", &XC::BidimMecLoad::getResultant,"Returns load resultant (force and moment integration over the elements).")
  ;

class_<XC::ShellMecLoad, bases<XC::BidimMecLoad>, boost::noncopyable >("ShellMecLoad", no_init)
  .add_property("getForceVectorDimension", &XC::ShellMecLoad::getForceVectorDimension)
  .add_property("getMomentVectorDimension", &XC::ShellMecLoad::getMomentVectorDimension)
  .def("getLocalForce",&XC::ShellMecLoad::getLocalForce,"Returns local coordinates or force vector.")
  .def("getLocalMoment",&XC::ShellMecLoad::getLocalMoment,"Returns local coordinates of bending moment vector.")
  .def("getVector3dLocalForce",&XC::ShellMecLoad::getVector3dLocalForce,"Returns a Vector3d with the local coordinates of the force vector.")
  .def("getVector3dLocalMoment",&XC::ShellMecLoad::getVector3dLocalMoment,"Returns a Vector3d with the local coordinates of bending moment vector.")
  .def("getLocalForces",make_function(&XC::ShellMecLoad::getLocalForces, return_internal_reference<>() ))
  .def("getLocalMoments",make_function(&XC::ShellMecLoad::getLocalMoments, return_internal_reference<>() ))
  .def("getGlobalVectors",make_function(&XC::ShellMecLoad::getGlobalVectors, return_internal_reference<>() ))
  .def("getGlobalForces",make_function(&XC::ShellMecLoad::getGlobalForces, return_internal_reference<>() ),"Returns force vector(s) in global coordinates.")
  .def("getGlobalMoments",make_function(&XC::ShellMecLoad::getGlobalMoments, return_internal_reference<>() ),"Returns force vector(s) in global coordinates.")
  ;

class_<XC::ShellUniformLoad, bases<XC::ShellMecLoad>, boost::noncopyable >("ShellUniformLoad", no_init)
  .add_property("axial1Component", &XC::ShellUniformLoad::getAxial1Component, &XC::ShellUniformLoad::setAxial1Component)
  .add_property("axial2Component", &XC::ShellUniformLoad::getAxial2Component, &XC::ShellUniformLoad::setAxial2Component)
  .add_property("transComponent", &XC::ShellUniformLoad::getTransComponent, &XC::ShellUniformLoad::setTransComponent)
  .add_property("Wx",make_function(&XC::ShellUniformLoad::Wx, return_value_policy <copy_const_reference> ()))
  .add_property("Wy",make_function(&XC::ShellUniformLoad::Wy, return_value_policy <copy_const_reference> ()))
  .add_property("Wz",make_function(&XC::ShellUniformLoad::Wz, return_value_policy <copy_const_reference> ()))
  .def("getLocalPressures",make_function(&XC::ShellUniformLoad::getLocalPressures, return_internal_reference<>() ),"Returns pressure vectors (one for each element) expressed in element local coordinates.")
  .def("getDistributedLocalMoments",make_function(&XC::ShellUniformLoad::getDistributedLocalMoments, return_internal_reference<>() ),"Returns distributed moment vectors (one for each element) expressed in element local coordinates.")
  .def("getGlobalPressures",make_function(&XC::ShellUniformLoad::getGlobalPressures, return_internal_reference<>() ),"Returns pressure vectors (one for each element) expressed in global coordinates.")
  .def("getDistributedGlobalMoments",make_function(&XC::ShellUniformLoad::getDistributedGlobalMoments, return_internal_reference<>() ),"Returns distributed moment vectors (one for each element) expressed in global coordinates.")
  .def("getResultant",&XC::ShellUniformLoad::getResultant,"Returns the load resultant (integration of the pressures over the elements).")
  ;

class_<XC::ShellRawLoad, bases<XC::ShellMecLoad>, boost::noncopyable >("ShellRawLoad", no_init)
  ;

class_<XC::BidimStrainLoad, bases<XC::BidimLoad>, boost::noncopyable >("BidimStrainLoad", no_init)
  .def("getStrain",make_function(&XC::BidimStrainLoad::getStrain, return_internal_reference<>() ))
  .def("getStrains",make_function(&XC::BidimStrainLoad::getStrains, return_internal_reference<>() ))
  .def("setStrainComp",&XC::BidimStrainLoad::setStrainComp)
  ;

class_<XC::ShellStrainLoad, bases<XC::BidimStrainLoad>, boost::noncopyable >("ShellStrainLoad", no_init)
  ;



