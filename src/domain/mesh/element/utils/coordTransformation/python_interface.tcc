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

//Coordinate transformations

const XC::Vector &(XC::CrdTransf::*getVectorGlobalFromLocal)(const XC::Vector &) const= &XC::CrdTransf::getVectorGlobalCoordFromLocal;
const XC::Vector &(XC::CrdTransf::*getVectorLocalFromGlobal)(const XC::Vector &) const= &XC::CrdTransf::getVectorLocalCoordFromGlobal;
class_<XC::CrdTransf, XC::CrdTransf*, bases<XC::TaggedObject,XC::MovableObject >, boost::noncopyable >("CrdTransf", no_init)
  .def("getName",&XC::CrdTransf::getName,"Returns the name of the coordinate transformation.")
  .add_property("name",&XC::CrdTransf::getName,"Returns the name of the coordinate transformation.")
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

  .def("getCooNodes",&XC::CrdTransf::getCooNodes, return_value_policy<copy_const_reference>(),"Return the coordinates of the nodes as rows of the returned matrix.")
  .def("getCooPoints",&XC::CrdTransf::getCooPoints, return_value_policy<copy_const_reference>(),"Return points distributed between the nodes as a matrix with the coordinates as rows.")
  .def("getCooPoint",&XC::CrdTransf::getCooPoint, return_value_policy<copy_const_reference>(),"Return the point that correspond to the relative coordinate 0<=xrel<=1.")

  ;

class_<XC::CrdTransf2d, XC::CrdTransf2d*, bases<XC::CrdTransf >, boost::noncopyable >("CrdTransf2d", no_init)
  ;

class_<XC::SmallDispCrdTransf2d, XC::SmallDispCrdTransf2d*, bases<XC::CrdTransf2d >, boost::noncopyable >("SmallDispCrdTransf2d", no_init)
  ;

class_<XC::LinearCrdTransf2d, XC::LinearCrdTransf2d*, bases<XC::SmallDispCrdTransf2d >, boost::noncopyable >("LinearCrdTransf2d", no_init)
  ;

class_<XC::CrdTransf3d, bases<XC::CrdTransf >, boost::noncopyable >("CrdTransf3d", no_init)
  .add_property("xzVector", &XC::CrdTransf3d::get_xz_vector,&XC::CrdTransf3d::set_xz_vector)
  .add_property("getKVector", make_function(&XC::CrdTransf3d::getK, return_internal_reference<>()))
  .def("rotate",&XC::CrdTransf3d::gira,"rotates local system around x axis.")
  ;

class_<XC::SmallDispCrdTransf3d, XC::SmallDispCrdTransf3d*, bases<XC::CrdTransf3d >, boost::noncopyable >("SmallDispCrdTransf3d", no_init)
  ;

class_<XC::LinearCrdTransf3d, XC::LinearCrdTransf3d*, bases<XC::SmallDispCrdTransf3d >, boost::noncopyable >("LinearCrdTransf3d", no_init)
  ;

class_<XC::CorotCrdTransf2d, XC::CorotCrdTransf2d *, bases<XC::CrdTransf2d>, boost::noncopyable >("CorotCrdTransf2d", no_init);

class_<XC::CorotCrdTransf3d, XC::CorotCrdTransf3d*, bases<XC::CrdTransf3d>, boost::noncopyable >("CorotCrdTransf3d", no_init);

class_<XC::PDeltaCrdTransf2d, XC::PDeltaCrdTransf2d*, bases<XC::SmallDispCrdTransf2d>, boost::noncopyable >("PDeltaCrdTransf2d", no_init);

class_<XC::PDeltaCrdTransf3d, XC::PDeltaCrdTransf3d*, bases<XC::SmallDispCrdTransf3d>, boost::noncopyable >("PDeltaCrdTransf3d", no_init);

const XC::Vector &(XC::ShellCrdTransf3dBase::*getVGlobalFromLocal)(const XC::Vector &) const= &XC::ShellCrdTransf3dBase::getVectorGlobalCoordFromLocal;
const XC::Vector &(XC::ShellCrdTransf3dBase::*getVLocalFromGlobal)(const XC::Vector &) const= &XC::ShellCrdTransf3dBase::getVectorLocalCoordFromGlobal;
class_<XC::ShellCrdTransf3dBase, bases<CommandEntity>, boost::noncopyable >("ShellCrdTransf3dBase", no_init)
  .add_property("getTrfMatrix", &XC::ShellCrdTransf3dBase::getTrfMatrix,"Returns the transformation matrix.")
  .add_property("getLocalAxes", &XC::ShellCrdTransf3dBase::getTrfMatrix,"Returns transformation local axes as matrix rows: [[x1,y1,z1],[x2,y2,z2],...·] .")
  .add_property("getG1Vector", make_function(&XC::ShellCrdTransf3dBase::G1, return_internal_reference<>() ))
  .add_property("getG2Vector", make_function(&XC::ShellCrdTransf3dBase::G2, return_internal_reference<>() ))
  .add_property("getG3Vector", make_function(&XC::ShellCrdTransf3dBase::G3, return_internal_reference<>() ))
  .add_property("getVPosCentroide", make_function(&XC::ShellCrdTransf3dBase::getVPosCentroide, return_internal_reference<>() ))
  .add_property("getPlane", &XC::ShellCrdTransf3dBase::getPlane)
  .def("update", &XC::ShellCrdTransf3dBase::update)
  .def("getBasicTrialDisp", &XC::ShellCrdTransf3dBase::getBasicTrialDisp)
  .def("getBasicTrialVel", &XC::ShellCrdTransf3dBase::getBasicTrialVel)
  .def("getBasicTrialAccel", &XC::ShellCrdTransf3dBase::getBasicTrialAccel)
  .def("getProj", &XC::ShellCrdTransf3dBase::getProj)
  .add_property("getLocalReference", &XC::ShellCrdTransf3dBase::getLocalReference)
  .def("getLocalCoordinates", &XC::ShellCrdTransf3dBase::getLocalCoordinates,"Returns local coordinates of a point.")
  .def("getNaturalCoordinates", &XC::ShellCrdTransf3dBase::getNaturalCoordinates,"Returns local coordinates of a point.")
  .def("getVectorGlobalCoordFromLocal", getVGlobalFromLocal, return_value_policy<copy_const_reference>(),"Returns global coordinates of a vector.")
  .def("getVectorLocalCoordFromGlobal", getVLocalFromGlobal,  return_value_policy<copy_const_reference>(),"Returns local coordinates of a vector.")
   ;

class_<XC::ShellLinearCrdTransf3d, bases<XC::ShellCrdTransf3dBase>, boost::noncopyable >("ShellLinearCrdTransf3d", no_init)
   ;
