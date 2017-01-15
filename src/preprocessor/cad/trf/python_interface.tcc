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

Pos3d (XC::TrfGeom::*getTrfPos)(const Pos3d &) const= &XC::TrfGeom::Transforma;
Vector3d (XC::TrfGeom::*getTrfVector)(const Vector3d &) const= &XC::TrfGeom::Transforma;
class_<XC::TrfGeom, bases<XC::EntMdlrBase>, boost::noncopyable >("TrfGeom", no_init)
  .def("getTrfPos",getTrfPos)
  .def("getTrfVector",getTrfVector)
  ;

class_<XC::Traslacion, bases<XC::TrfGeom>, boost::noncopyable >("Traslacion", no_init)
  .def("setVector",&XC::Traslacion::setVector)
  ;

class_<XC::Escalado, bases<XC::TrfGeom>, boost::noncopyable >("Escalado", no_init)
  .def("setScaleFactor",&XC::Escalado::setScaleFactor)
  ;

class_<XC::Reflexion, bases<XC::TrfGeom>, boost::noncopyable >("Reflexion", no_init)
  .def("setPlane",&XC::Reflexion::setPlane)
  ;

class_<XC::Rotacion, bases<XC::TrfGeom>, boost::noncopyable >("Rotacion", no_init)
  .def("setAxisAng",&XC::Rotacion::setAxisAng)
  ;

typedef XC::MapCadMember<XC::TrfGeom> MapCadTrfGeom;
class_<MapCadTrfGeom, bases<XC::MapCadMemberBase>, boost::noncopyable >("MapCadTraGeom", no_init);

class_<XC::MapTrfGeom, bases<MapCadTrfGeom>, boost::noncopyable >("MapTrfGeom", no_init)
.def("newTransformation",&XC::MapTrfGeom::newTransformation, return_internal_reference<>(),"Defines a new transformation.")
  ;




