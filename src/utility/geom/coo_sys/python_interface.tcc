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
class_<CooSys, bases<ProtoGeom>, boost::noncopyable >("CooSys", no_init)
  .add_property("numberOfAxis",&CooSys::numberOfAxis)
  .add_property("EsNormal",&CooSys::EsNormal)
  .add_property("EsOrtogonal",&CooSys::EsOrtogonal)
  .add_property("EsOrtonormal",&CooSys::EsOrtonormal)
  .add_property("EsDextrogiro",&CooSys::EsDextrogiro)
  ;

class_<Xd2dCooSys, bases<CooSys>, boost::noncopyable >("CooSysXd2d", no_init)
  .def("getAxisVDir",&Xd2dCooSys::getAxisVDir)
  .def("getAxisDir",&Xd2dCooSys::getAxisDir)
  ;

class_<Xd3dCooSys, bases<CooSys>, boost::noncopyable >("CooSysXd3d", no_init)
  .def("getAxisVDir",&Xd3dCooSys::getAxisVDir)
  .def("getAxisDir",&Xd3dCooSys::getAxisDir)
  ;

class_<Rect1d2dCooSys, bases<Xd2dCooSys>, boost::noncopyable >("CooSysRect1d2d", no_init)
  .def(init<const Pos2d &,const Pos2d &>())
  .def(init<const Vector2d &>())
  .def("getIVector",&Rect1d2dCooSys::getIVector, "Unary vector i.")
  .def("getGlobalCoordinates",&Rect1d2dCooSys::getGlobalCoordinates)
  .def("getLocalCoordinates",&Rect1d2dCooSys::getLocalCoordinates)
  ;

class_<Rect1d3dCooSys, bases<Xd3dCooSys> >("CooSysRect1d3d")
  .def(init<const Pos3d &,const Pos3d &>())
  .def(init<const Vector3d &>())
  .def("getIVector",&Rect1d3dCooSys::getIVector, "Unary vector i.")
  .def("getGlobalCoordinates",&Rect1d2dCooSys::getGlobalCoordinates)
  .def("getLocalCoordinates",&Rect1d3dCooSys::getLocalCoordinates)
  ;

class_<Rect2d2dCooSys, bases<Xd2dCooSys> >("CooSysRect2d2d")
  .def(init<const Pos2d &,const Pos2d &>())
  .def(init<const Vector2d &>())
  .def("getIVector",&Rect2d2dCooSys::getIVector, "Unary vector i.")
  .def("getJVector",&Rect2d2dCooSys::getJVector, "Unary vector j.")
  .def("getGlobalCoordinates",&Rect2d2dCooSys::getGlobalCoordinates)
  .def("getLocalCoordinates",&Rect2d2dCooSys::getLocalCoordinates)
  ;

class_<Rect2d3dCooSys, bases<Xd3dCooSys> >("CooSysRect2d3d")
  .def(init<const Pos3d &,const Pos3d &, const Pos3d &>())
  .def(init<const Vector3d &>())
  .def(init<const Vector3d &,const Vector3d &>())
  .def("getIVector",&Rect2d3dCooSys::getIVector, "Unary vector i.")
  .def("getJVector",&Rect2d3dCooSys::getJVector, "Unary vector j.")
  .def("getKVector",&Rect2d3dCooSys::getKVector, "Unary vector k.")
  .def("getGlobalCoordinates",&Rect2d3dCooSys::getGlobalCoordinates)
  .def("getLocalCoordinates",&Rect2d3dCooSys::getLocalCoordinates)
  ;

class_<Rect3d3dCooSys, bases<Xd3dCooSys> >("CooSysRect3d3d")
  .def(init<const Pos3d &,const Pos3d &>())
  .def(init<const Pos3d &,const Pos3d &, const Pos3d &>())
  .def(init<const Vector3d &,const Vector3d &>())
  .def(init<const Vector3d &,const Vector3d &,const Vector3d &>())
  .def("getIVector",&Rect3d3dCooSys::getIVector, "Unary vector i.")
  .def("getJVector",&Rect3d3dCooSys::getJVector, "Unary vector j.")
  .def("getKVector",&Rect3d3dCooSys::getKVector, "Unary vector k.")
  .def("getGlobalCoordinates",&Rect3d3dCooSys::getGlobalCoordinates)
  .def("getLocalCoordinates",&Rect3d3dCooSys::getLocalCoordinates)
  ;


