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

#include "entities/python_interface.tcc"
#include "matrices/python_interface.tcc"
#include "trf/python_interface.tcc"

class_<XC::ReferenceFrame, bases<XC::EntMdlrBase>, boost::noncopyable >("ReferenceFrame", no_init);

class_<XC::CartesianReferenceFrame3d, bases<XC::ReferenceFrame>, boost::noncopyable >("CartesianReferenceFrame3d", no_init)
.add_property("org",&XC::CartesianReferenceFrame3d::getOrg,&XC::CartesianReferenceFrame3d::setOrg,"Origin of the 3D cartesian coordinate system")
  ;

class_<XC::ModelComponentContainerBase, bases<CommandEntity>, boost::noncopyable >("ModelComponentContainerBase", no_init)
  .def("setDefaultTag",&XC::ModelComponentContainerBase::setTag)
  ;

//class_<XC::ModelComponentContainer, bases<XC::ModelComponentContainerBase>, boost::noncopyable >("ModelComponentContainer", no_init);

typedef XC::ModelComponentContainer<XC::ReferenceFrame> MapMultiBlockTopologyReferenceFrame;
class_<MapMultiBlockTopologyReferenceFrame, bases<XC::ModelComponentContainerBase>, boost::noncopyable >("MapMultiBlockTopologyReferenceFrame", no_init);


XC::ReferenceFrame *(XC::ReferenceFrameMap::*newReferenceSystem)(const std::string &)= &XC::ReferenceFrameMap::New;
class_<XC::ReferenceFrameMap, bases<MapMultiBlockTopologyReferenceFrame>, boost::noncopyable >("ReferenceFrameMap", no_init)
.def("newReferenceSystem", make_function(newReferenceSystem,return_internal_reference<>() ))
  ;


XC::ReferenceFrameMap &(XC::MultiBlockTopology::*getReferenceSystems)(void)= &XC::MultiBlockTopology::getReferenceSystems;
XC::PntMap &(XC::MultiBlockTopology::*getPntMapRef)(void)= &XC::MultiBlockTopology::getPoints;
XC::LineMap &(XC::MultiBlockTopology::*getLineMapRef)(void)= &XC::MultiBlockTopology::getLines;
XC::SurfaceMap &(XC::MultiBlockTopology::*getSurfaceMapRef)(void)= &XC::MultiBlockTopology::getSurfaces;
XC::BodyMap &(XC::MultiBlockTopology::*getBodyMapRef)(void)= &XC::MultiBlockTopology::getBodies;
XC::Framework2d &(XC::MultiBlockTopology::*getRefToFramework2d)(void)= &XC::MultiBlockTopology::getFramework2d;
XC::Framework3d &(XC::MultiBlockTopology::*getRefToFramework3d)(void)= &XC::MultiBlockTopology::getFramework3d;
XC::UniformGridMap &(XC::MultiBlockTopology::*getUniformGridsRef)(void)= &XC::MultiBlockTopology::getUniformGrids;
XC::Edge *(XC::MultiBlockTopology::*getLineWithEndPoints)(const XC::PntMap::Indice &,const XC::PntMap::Indice &) = &XC::MultiBlockTopology::find_edge_by_endpoints;
class_<XC::MultiBlockTopology, bases<XC::PreprocessorContainer>, boost::noncopyable >("MultiBlockTopology", no_init)
  .add_property("getReferenceSystems", make_function( getReferenceSystems, return_internal_reference<>() ))
  .add_property("getPoints", make_function( getPntMapRef, return_internal_reference<>() ), "Return the point container.")
  .add_property("getLines", make_function( getLineMapRef, return_internal_reference<>() ), "Return the lines container.")
  .add_property("getSurfaces", make_function( getSurfaceMapRef, return_internal_reference<>() ),"Return the surfaces container.")
  .add_property("getBodies", make_function( getBodyMapRef, return_internal_reference<>() ),"Return the bodies container.")
  .add_property("get2DNets", make_function( getRefToFramework2d, return_internal_reference<>() ))
  .add_property("get3DNets", make_function( getRefToFramework3d, return_internal_reference<>() ))
  .add_property("getUniformGrids", make_function( getUniformGridsRef, return_internal_reference<>() ))
  .def("conciliaNDivs", &XC::MultiBlockTopology::conciliaNDivs)
  .def("getLineWithEndPoints",make_function( getLineWithEndPoints, return_internal_reference<>() ))
  .def("getAverageSize", &XC::MultiBlockTopology::getAverageSize, "Return the average size of the geometric entities.")
   ;


