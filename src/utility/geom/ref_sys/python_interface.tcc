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
typedef Ref<Rect1d2dCooSys> ref_Rect1d2dCooSys;
const ref_Rect1d2dCooSys::PGlobal &(ref_Rect1d2dCooSys::*getOrg1d2d)(void) const= &ref_Rect1d2dCooSys::Org;
const Rect1d2dCooSys &(ref_Rect1d2dCooSys::*getCoordTransformation1d2d)(void) const= &ref_Rect1d2dCooSys::Trf;
class_<ref_Rect1d2dCooSys, bases<ProtoGeom>, boost::noncopyable >("ref_CooSysRect1d2d", no_init)
  .add_property("Org", make_function(getOrg1d2d,return_internal_reference<>()), &ref_Rect1d2dCooSys::setOrg,"Origin of the reference frame.")
  .add_property("Trf", make_function(getCoordTransformation1d2d,return_internal_reference<>()), &ref_Rect1d2dCooSys::setTrf,"Coordinate system of the reference frame.")
  .def("getAxisVDir",&ref_Rect1d2dCooSys::getAxisVDir)
  .def("getPosGlobal",&ref_Rect1d2dCooSys::GetPosGlobal, "Return the corresponding global position.")
.def("getCooGlobales",&ref_Rect1d2dCooSys::GetCooGlobales,"Return global coordinates of the argument.")
  .def("getPosLocal",&ref_Rect1d2dCooSys::GetPosLocal, "Return the corresponding local position.")
  .def("getCooLocales",&ref_Rect1d2dCooSys::GetCooLocales,"Return local coordinates of the argument.")
  .def("getOrg",make_function(getOrg1d2d,return_internal_reference<>()))
  .def("getCoordTransformation",make_function(getCoordTransformation1d2d,return_internal_reference<>()))
  ;

class_<Ref1d2d , bases<ref_Rect1d2dCooSys> >("Ref1d2d")
  .def(init<const Pos2d &>())
  .def(init<const Pos2d &,const Rect1d2dCooSys &>())
  .def(init<const Pos2d &,const Vector2d &>())
  .def(init<const Pos2d &,const Dir2d &>())
  .def(init<const Pos2d &,const Pos2d &>())
  .def("getIVector",&Ref1d2d::getIVector, "Return unary vector i")
  ;

typedef Ref<Rect1d3dCooSys> ref_Rect1d3dCooSys;
const ref_Rect1d3dCooSys::PGlobal &(ref_Rect1d3dCooSys::*getOrg1d3d)(void) const= &ref_Rect1d3dCooSys::Org;
const Rect1d3dCooSys &(ref_Rect1d3dCooSys::*getCoordTransformation1d3d)(void) const= &ref_Rect1d3dCooSys::Trf;
class_<ref_Rect1d3dCooSys, bases<ProtoGeom>, boost::noncopyable >("ref_CooSysRect1d3d", no_init)
  .add_property("Org", make_function(getOrg1d3d,return_internal_reference<>()), &ref_Rect1d3dCooSys::setOrg,"Origin of the reference frame.")
  .add_property("Trf", make_function(getCoordTransformation1d3d,return_internal_reference<>()), &ref_Rect1d3dCooSys::setTrf,"Coordinate system of the reference frame.")
  .def("getAxisVDir",&ref_Rect1d3dCooSys::getAxisVDir)
  .def("getPosGlobal",&ref_Rect1d3dCooSys::GetPosGlobal)
  .def("getCooGlobales",&ref_Rect1d3dCooSys::GetCooGlobales,"Return global coordinates of the argument.")
  .def("getPosLocal",&ref_Rect1d3dCooSys::GetPosLocal)
.def("getCooLocales",&ref_Rect1d3dCooSys::GetCooLocales, "Return local coordinates of the argument.")
  .def("getOrg",make_function(getOrg1d3d,return_internal_reference<>()))
  .def("getCoordTransformation",make_function(getCoordTransformation1d3d,return_internal_reference<>()))
  ;

class_<Ref1d3d , bases<ref_Rect1d3dCooSys> >("Ref1d3d")
  .def(init<const Pos3d &>())
  .def(init<const Pos3d &,const Rect1d3dCooSys &>())
  .def(init<const Pos3d &,const Vector3d &>())
  .def(init<const Pos3d &,const Dir3d &>())
  .def(init<const Pos3d &,const Pos3d &>())
  .def("getIVector",&Ref1d3d::getIVector, "Return unary vector i")
  ;

typedef Ref<Rect2d2dCooSys> ref_Rect2d2dCooSys;
const ref_Rect2d2dCooSys::PGlobal &(ref_Rect2d2dCooSys::*getOrg2d2d)(void) const= &ref_Rect2d2dCooSys::Org;
const Rect2d2dCooSys &(ref_Rect2d2dCooSys::*getCoordTransformation2d2d)(void) const= &ref_Rect2d2dCooSys::Trf;
class_<ref_Rect2d2dCooSys, bases<ProtoGeom>, boost::noncopyable >("ref_CooSysRect2d2d", no_init)
  .add_property("Org", make_function(getOrg2d2d,return_internal_reference<>()), &ref_Rect2d2dCooSys::setOrg,"Origin of the reference frame.")
  .add_property("Trf", make_function(getCoordTransformation2d2d,return_internal_reference<>()), &ref_Rect2d2dCooSys::setTrf,"Coordinate system of the reference frame.")
  .def("getAxisVDir",&ref_Rect2d2dCooSys::getAxisVDir)
  .def("getPosGlobal",&ref_Rect2d2dCooSys::GetPosGlobal)
  .def("getCooGlobales",&ref_Rect2d2dCooSys::GetCooGlobales,"Return global coordinates of the argument.")
  .def("getPosLocal",&ref_Rect2d2dCooSys::GetPosLocal)
  .def("getCooLocales",&ref_Rect2d2dCooSys::GetCooLocales, "Return local coordinates of the argument.")
  .def("getOrg",make_function(getOrg2d2d,return_internal_reference<>()))
  .def("getCoordTransformation",make_function(getCoordTransformation2d2d,return_internal_reference<>()))
  ;

class_<Ref2d2d , bases<ref_Rect2d2dCooSys> >("Ref2d2d")
  .def(init<const Pos2d &>())
  .def(init<const Pos2d &,const Rect2d2dCooSys &>())
  .def(init<const Pos2d &,const Vector2d &>())
  .def(init<const Pos2d &,const Dir2d &>())
  .def(init<const Pos2d &,const Pos2d &>())
  .def("getIVector",&Ref2d2d::getIVector, "Return unary vector i")
  .def("getJVector",&Ref2d2d::getJVector, "Return unary vector j")
  ;

typedef Ref<Rect2d3dCooSys> ref_Rect2d3dCooSys;
const ref_Rect2d3dCooSys::PGlobal &(ref_Rect2d3dCooSys::*getOrg2d3d)(void) const= &ref_Rect2d3dCooSys::Org;
const Rect2d3dCooSys &(ref_Rect2d3dCooSys::*getCoordTransformation2d3d)(void) const= &ref_Rect2d3dCooSys::Trf;
class_<ref_Rect2d3dCooSys, bases<ProtoGeom>, boost::noncopyable >("ref_CooSysRect2d3d", no_init)
  .add_property("Org", make_function(getOrg2d3d,return_internal_reference<>()), &ref_Rect2d3dCooSys::setOrg,"Origin of the reference frame.")
  .add_property("Trf", make_function(getCoordTransformation2d3d,return_internal_reference<>()), &ref_Rect2d3dCooSys::setTrf,"Coordinate system of the reference frame.")
  .def("getAxisVDir",&ref_Rect2d3dCooSys::getAxisVDir)
  .def("getPosGlobal",&ref_Rect2d3dCooSys::GetPosGlobal)
  .def("getCooGlobales",&ref_Rect2d3dCooSys::GetCooGlobales,"Return global coordinates of the argument.")
  .def("getPosLocal",&ref_Rect2d3dCooSys::GetPosLocal)
  .def("getCooLocales",&ref_Rect2d3dCooSys::GetCooLocales, "Return local coordinates of the argument.")
  .def("getOrg",make_function(getOrg2d3d,return_internal_reference<>()))
  .def("getCoordTransformation",make_function(getCoordTransformation2d3d,return_internal_reference<>()))
  ;

class_<Ref2d3d , bases<ref_Rect2d3dCooSys> >("Ref2d3d")
  .def(init<const Pos3d &>())
  .def(init<const Pos3d &,const Rect2d3dCooSys &>())
  .def(init<const Pos3d &,const Pos3d &,const Pos3d &>())
  .def(init<const Pos3d &,const Vector3d &>())
  .def(init<const Pos3d &,const Vector3d &,const Vector3d &>())
  .def("getIVector",&Ref2d3d::getIVector, "Return unary vector i")
  .def("getJVector",&Ref2d3d::getJVector, "Return unary vector j")
  ;

typedef Ref<Rect3d3dCooSys> ref_Rect3d3dCooSys;
const ref_Rect3d3dCooSys::PGlobal &(ref_Rect3d3dCooSys::*getOrg3d3d)(void) const= &ref_Rect3d3dCooSys::Org;
const Rect3d3dCooSys &(ref_Rect3d3dCooSys::*getCoordTransformation3d3d)(void) const= &ref_Rect3d3dCooSys::Trf;
class_<ref_Rect3d3dCooSys, bases<ProtoGeom>, boost::noncopyable >("ref_CooSysRect3d3d", no_init)
  .add_property("Org", make_function(getOrg3d3d,return_internal_reference<>()), &ref_Rect3d3dCooSys::setOrg,"Origin of the reference frame.")
  .add_property("Trf", make_function(getCoordTransformation3d3d,return_internal_reference<>()), &ref_Rect3d3dCooSys::setTrf,"Coordinate system of the reference frame.")
  .def("getAxisVDir",&ref_Rect3d3dCooSys::getAxisVDir)
  .def("getPosGlobal",&ref_Rect3d3dCooSys::GetPosGlobal)
  .def("getCooGlobales",&ref_Rect3d3dCooSys::GetCooGlobales,"Return global coordinates of the argument.")
  .def("getPosLocal",&ref_Rect3d3dCooSys::GetPosLocal)
  .def("getCooLocales",&ref_Rect3d3dCooSys::GetCooLocales, "Return local coordinates of the argument.")
  .def("getOrg",make_function(getOrg3d3d,return_internal_reference<>()))
  .def("getCoordTransformation",make_function(getCoordTransformation3d3d,return_internal_reference<>()))
  ;

class_<Ref3d3d , bases<ref_Rect3d3dCooSys> >("Ref3d3d")
  .def(init<const Pos3d &>())
  .def(init<const Pos3d &,const Rect3d3dCooSys &>())
  .def(init<const Pos3d &,const Pos3d &>())
  .def(init<const Pos3d &,const Pos3d &,const Pos3d &>())
  .def(init<const Pos3d &,const Vector3d &,const Vector3d &>())
  .def(init<const Pos3d &,const Vector3d &,const Vector3d &,const Vector3d &>())
  .def(init<const Line3d &,const Pos3d &>())
  .def("getIVector",&Ref3d3d::getIVector, "Return unary vector i.")
  .def("getJVector",&Ref3d3d::getJVector, "Return unary vector j.")
  .def("getKVector",&Ref3d3d::getKVector, "Return unary vector k.")
 ;

class_<PrincipalAxesOfInertia2D>("PrincipalAxesOfInertia2D",init<const Pos2d &,const GEOM_FT &,const GEOM_FT &,const GEOM_FT &>())
  .add_property("I1",  make_function( &PrincipalAxesOfInertia2D::I1,  return_value_policy<copy_const_reference>() ),"first principal moment of inertia.")
  .add_property("I2",  make_function( &PrincipalAxesOfInertia2D::I2,  return_value_policy<copy_const_reference>() ),"second principal moment of inertia.")
  .def("getAxis1VDir", &PrincipalAxesOfInertia2D::getAxis1VDir,"returns the direction vector of the first principal moment of inertia")
 ;
