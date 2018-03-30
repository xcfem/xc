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

class_<XC::Translation, bases<XC::TrfGeom>, boost::noncopyable >("Translation", no_init)
  .def("setVector",&XC::Translation::setVector)
  ;

class_<XC::Scaling, bases<XC::TrfGeom>, boost::noncopyable >("Scaling", no_init)
  .def("setScaleFactor",&XC::Scaling::setScaleFactor)
  ;

class_<XC::Reflection, bases<XC::TrfGeom>, boost::noncopyable >("Reflection", no_init)
  .def("setPlane",&XC::Reflection::setPlane)
  ;

class_<XC::Rotation, bases<XC::TrfGeom>, boost::noncopyable >("Rotation", no_init)
  .def("setAxisAng",&XC::Rotation::setAxisAng)
  ;

typedef XC::ModelComponentContainer<XC::TrfGeom> MapMultiBlockTopologyTrfGeom;
class_<MapMultiBlockTopologyTrfGeom, bases<XC::ModelComponentContainerBase>, boost::noncopyable >("MapMultiBlockTopologyTraGeom", no_init);

class_<XC::MapTrfGeom, bases<MapMultiBlockTopologyTrfGeom>, boost::noncopyable >("MapTrfGeom", no_init)
.def("newTransformation",&XC::MapTrfGeom::newTransformation, return_internal_reference<>(),"Defines a new transformation.")
  ;




