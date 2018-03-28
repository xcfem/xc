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

#include "entities/python_interface.tcc"
#include "matrices/python_interface.tcc"
#include "trf/python_interface.tcc"

class_<XC::ReferenceFrame, bases<XC::EntMdlrBase>, boost::noncopyable >("ReferenceFrame", no_init);

class_<XC::CartesianReferenceFrame3d, bases<XC::ReferenceFrame>, boost::noncopyable >("CartesianReferenceFrame3d", no_init)
.add_property("org",&XC::CartesianReferenceFrame3d::getOrg,&XC::CartesianReferenceFrame3d::setOrg,"Origin of the 3D cartesian coordinate system")
  ;

class_<XC::MapCadMemberBase, bases<EntCmd>, boost::noncopyable >("MapCadMemberBase", no_init)
  .def("setDefaultTag",&XC::MapCadMemberBase::setTag)
  ;

//class_<XC::MapCadMember, bases<XC::MapCadMemberBase>, boost::noncopyable >("MapCadMember", no_init);

typedef XC::MapCadMember<XC::ReferenceFrame> MapCadReferenceFrame;
class_<MapCadReferenceFrame, bases<XC::MapCadMemberBase>, boost::noncopyable >("MapCadReferenceFrame", no_init);


XC::ReferenceFrame *(XC::ReferenceFrameMap::*newReferenceSystem)(const std::string &)= &XC::ReferenceFrameMap::New;
class_<XC::ReferenceFrameMap, bases<MapCadReferenceFrame>, boost::noncopyable >("ReferenceFrameMap", no_init)
.def("newReferenceSystem", make_function(newReferenceSystem,return_internal_reference<>() ))
  ;


XC::ReferenceFrameMap &(XC::Cad::*getReferenceSystems)(void)= &XC::Cad::getReferenceSystems;
XC::MapTrfGeom &(XC::Cad::*getGeometricTransformations)(void)= &XC::Cad::getTransformacionesGeometricas;
XC::PntMap &(XC::Cad::*getPntMapRef)(void)= &XC::Cad::getPuntos;
XC::MapLineas &(XC::Cad::*getMapLineasRef)(void)= &XC::Cad::getLineas;
XC::MapSurfaces &(XC::Cad::*getMapSurfacesRef)(void)= &XC::Cad::getSurfaces;
XC::MapEsquemas2d &(XC::Cad::*getEsquemas2dRef)(void)= &XC::Cad::getEsquemas2d;
XC::MapEsquemas3d &(XC::Cad::*getEsquemas3dRef)(void)= &XC::Cad::getEsquemas3d;
XC::MapUniformGrids &(XC::Cad::*getUniformGridsRef)(void)= &XC::Cad::getUniformGrids;
XC::Edge *(XC::Cad::*getLineWithEndPoints)(const XC::PntMap::Indice &,const XC::PntMap::Indice &) = &XC::Cad::busca_edge_extremos;
class_<XC::Cad, bases<XC::PreprocessorContainer>, boost::noncopyable >("Cad", no_init)
  .add_property("getReferenceSystems", make_function( getReferenceSystems, return_internal_reference<>() ))
  .add_property("getGeometricTransformations", make_function( getGeometricTransformations, return_internal_reference<>() ))
  .add_property("getPoints", make_function( getPntMapRef, return_internal_reference<>() ))
  .add_property("getLines", make_function( getMapLineasRef, return_internal_reference<>() ))
  .add_property("getSurfaces", make_function( getMapSurfacesRef, return_internal_reference<>() ))
  .add_property("get2DNets", make_function( getEsquemas2dRef, return_internal_reference<>() ))
  .add_property("get3DNets", make_function( getEsquemas3dRef, return_internal_reference<>() ))
  .add_property("getUniformGrids", make_function( getUniformGridsRef, return_internal_reference<>() ))
  .def("conciliaNDivs", &XC::Cad::conciliaNDivs)
  .def("getLineWithEndPoints",make_function( getLineWithEndPoints, return_internal_reference<>() ))
   ;


