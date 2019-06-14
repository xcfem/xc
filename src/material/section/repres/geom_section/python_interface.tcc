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

#include "region/python_interface.tcc"
#include "reinfBar/python_interface.tcc"
#include "reinfLayer/python_interface.tcc"

class_<XC::EntGeomSection, bases<NamedEntity>, boost::noncopyable >("EntGeomSection", no_init);

class_<XC::SectionReferenceFrame, bases<XC::EntGeomSection>, boost::noncopyable >("SectionReferenceFrame", no_init);

XC::Spot *(XC::Axis::*getSpot1)(void)= &XC::Axis::P1;
XC::Spot *(XC::Axis::*getSpot2)(void)= &XC::Axis::P2;
class_<XC::Axis, bases<XC::EntGeomSection>, boost::noncopyable >("Axis", no_init)
  .add_property("p1",make_function(getSpot1,return_internal_reference<>()))
  .add_property("p2",make_function(getSpot2,return_internal_reference<>()))
  .add_property("nDiv",&XC::Axis::NDiv,&XC::Axis::SetNDiv)
  .add_property("numVertices",&XC::Axis::getNumberOfVertices)
  ;

const Pos2d &(XC::Spot::*getPosSpot)(void) const= &XC::Spot::GetPos;
class_<XC::Spot, bases<XC::EntGeomSection>, boost::noncopyable >("Spot", no_init)
  .add_property("pos",make_function(getPosSpot,return_internal_reference<>()),&XC::Spot::setPos)
  .add_property("numLines",&XC::Spot::nLines)
  ;

class_<XC::Segment, bases<XC::Axis>, boost::noncopyable >("Segment", no_init)
  .def("getLength",&XC::Segment::getLength)
  ;

XC::RegionContainer &(XC::GeomSection::*getRegionContainer)(void)= &XC::GeomSection::getRegions;
XC::ListReinfLayer &(XC::GeomSection::*getReinfLayerContainer)(void)= &XC::GeomSection::getReinforcementLayers;
class_<XC::GeomSection, XC::GeomSection *, bases<XC::SectionMassProperties>, boost::noncopyable >("GeomSection", no_init)
  .add_property("getRegions",make_function(getRegionContainer,return_internal_reference<>()))
  .add_property("getReinfLayers",make_function(getReinfLayerContainer,return_internal_reference<>()))
  .def("getRegionsContour",&XC::GeomSection::getRegionsContour)
  .def("getCompressedZoneContour",&XC::GeomSection::getCompressedZoneContour)
.def("getCover",&XC::GeomSection::getCover,"return the concrete cover of the reinforcement.")
  .add_property("tagSpot",&XC::GeomSection::getTagSpot,&XC::GeomSection::setTagSpot)
  .def("newSpot",make_function(&XC::GeomSection::newSpot,return_internal_reference<>()))
  .def("distSpots",&XC::GeomSection::DistSpots)
  .add_property("tagLine",&XC::GeomSection::getAxisTag,&XC::GeomSection::setAxisTag)
  .def("newSegment",make_function(&XC::GeomSection::newSegment,return_internal_reference<>()))
  ;


class_<XC::SectionCartesianReferenceFrame, bases<XC::SectionReferenceFrame>, boost::noncopyable >("SectionCartesianReferenceFrame", no_init);

