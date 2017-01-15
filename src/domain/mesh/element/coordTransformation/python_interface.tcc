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

//Coordinate transformations

const XC::Vector &(XC::CrdTransf::*getVectorGlobalFromLocal)(const XC::Vector &) const= &XC::CrdTransf::getVectorGlobalCoordFromLocal;
const XC::Vector &(XC::CrdTransf::*getVectorLocalFromGlobal)(const XC::Vector &) const= &XC::CrdTransf::getVectorLocalCoordFromGlobal;
class_<XC::CrdTransf, bases<XC::TaggedObject,XC::MovableObject >, boost::noncopyable >("CrdTransf", no_init)
  .add_property("getInitialLength", &XC::CrdTransf::getInitialLength)
  .add_property("getDeformedLength", &XC::CrdTransf::getDeformedLength)
  .add_property("getIVector", make_function(&XC::CrdTransf::getI, return_internal_reference<>()))
  .add_property("getJVector", make_function(&XC::CrdTransf::getJ, return_internal_reference<>()))
  .def("getVectorGlobalCoordFromLocal", getVectorGlobalFromLocal, return_value_policy<copy_const_reference>())
  .def("getVectorLocalCoordFromGlobal", getVectorLocalFromGlobal,  return_value_policy<copy_const_reference>())
    
  .def("commitState",&XC::CrdTransf::commitState)
  .def("revertToLastCommit",&XC::CrdTransf::revertToLastCommit)
  .def("revertToStart",&XC::CrdTransf::revertToStart)
    
  .def("getBasicTrialDisp",&XC::CrdTransf::getBasicTrialDisp, return_value_policy<copy_const_reference>())
  .def("getBasicIncrDisp",&XC::CrdTransf::getBasicIncrDisp, return_value_policy<copy_const_reference>())
  .def("getBasicIncrDeltaDisp",&XC::CrdTransf::getBasicIncrDeltaDisp, return_value_policy<copy_const_reference>())
  .def("getBasicTrialVel",&XC::CrdTransf::getBasicTrialVel, return_value_policy<copy_const_reference>())
  .def("getBasicTrialAccel",&XC::CrdTransf::getBasicTrialAccel, return_value_policy<copy_const_reference>())
    
  .def("getGlobalResistingForce",&XC::CrdTransf::getGlobalResistingForce, return_value_policy<copy_const_reference>())
  .def("getGlobalStiffMatrix",&XC::CrdTransf::getGlobalStiffMatrix, return_value_policy<copy_const_reference>())
  .def("getInitialGlobalStiffMatrix",&XC::CrdTransf::getInitialGlobalStiffMatrix, return_value_policy<copy_const_reference>())
    

  .def("getPointGlobalCoordFromLocal",&XC::CrdTransf::getPointGlobalCoordFromLocal, return_value_policy<copy_const_reference>())
  .def("getPointGlobalCoordFromBasic",&XC::CrdTransf::getPointGlobalCoordFromBasic, return_value_policy<copy_const_reference>())
  .def("getPointLocalCoordFromGlobal",&XC::CrdTransf::getPointLocalCoordFromGlobal)
  .def("getPointBasicCoordFromGlobal",&XC::CrdTransf::getPointBasicCoordFromGlobal)
  .def("getPointsGlobalCoordFromLocal",&XC::CrdTransf::getPointsGlobalCoordFromLocal, return_value_policy<copy_const_reference>())
  .def("getPointsGlobalCoordFromBasic",&XC::CrdTransf::getPointsGlobalCoordFromBasic, return_value_policy<copy_const_reference>())
  .def("getPointGlobalDisplFromBasic",&XC::CrdTransf::getPointGlobalDisplFromBasic, return_value_policy<copy_const_reference>())

  .def("getCooNodos",&XC::CrdTransf::getCooNodos, return_value_policy<copy_const_reference>())
  .def("getCooPuntos",&XC::CrdTransf::getCooPuntos, return_value_policy<copy_const_reference>())
  .def("getCooPunto",&XC::CrdTransf::getCooPunto, return_value_policy<copy_const_reference>())

  ;

class_<XC::CrdTransf2d, bases<XC::CrdTransf >, boost::noncopyable >("CrdTransf2d", no_init)
  ;

class_<XC::SmallDispCrdTransf2d, bases<XC::CrdTransf2d >, boost::noncopyable >("SmallDispCrdTransf2d", no_init)
  ;

class_<XC::LinearCrdTransf2d, bases<XC::SmallDispCrdTransf2d >, boost::noncopyable >("LinearCrdTransf2d", no_init)
  ;

class_<XC::CrdTransf3d, bases<XC::CrdTransf >, boost::noncopyable >("CrdTransf3d", no_init)
  .add_property("xzVector", &XC::CrdTransf3d::get_xz_vector,&XC::CrdTransf3d::set_xz_vector)
  .add_property("getKVector", make_function(&XC::CrdTransf3d::getK, return_internal_reference<>()))
  .def("rotate",&XC::CrdTransf3d::gira,"rotates local system around x axis.")
  ;

class_<XC::SmallDispCrdTransf3d, bases<XC::CrdTransf3d >, boost::noncopyable >("SmallDispCrdTransf3d", no_init)
  ;

class_<XC::LinearCrdTransf3d, bases<XC::SmallDispCrdTransf3d >, boost::noncopyable >("LinearCrdTransf3d", no_init)
  ;

class_<XC::CorotCrdTransf2d, bases<XC::CrdTransf2d>, boost::noncopyable >("CorotCrdTransf2d", no_init);

class_<XC::CorotCrdTransf3d, bases<XC::CrdTransf3d>, boost::noncopyable >("CorotCrdTransf3d", no_init);

class_<XC::PDeltaCrdTransf2d, bases<XC::SmallDispCrdTransf2d>, boost::noncopyable >("PDeltaCrdTransf2d", no_init);

class_<XC::PDeltaCrdTransf3d, bases<XC::SmallDispCrdTransf3d>, boost::noncopyable >("PDeltaCrdTransf3d", no_init);


