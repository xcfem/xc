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
//python_interface_lines.tcc

XC::Node *(XC::Edge::*getFirstNodePtr)(void)= &XC::Edge::getFirstNode;
XC::Node *(XC::Edge::*getLastNodePtr)(void)= &XC::Edge::getLastNode;
class_<XC::Edge, XC::Edge*, bases<XC::EntMdlr>, boost::noncopyable >("Edge","Base class for one-dimensional geometry objects." ,no_init)
  .add_property("nDiv", &XC::Edge::NDiv, &XC::Edge::setNDiv,"Number of divisions.")
  .def("setNDiv", &XC::Edge::setNDiv,"Set the number of divisions.")
  .def("setElemSize",&XC::Edge::SetElemSize,"Set the element size")
  .add_property("getNumVertices", &XC::Edge::getNumberOfVertices,"Return the number of vertices.")
  .add_property("getNumConnectedSurfaces", &XC::Edge::getNumConnectedSurfaces,"Return the number of connected surfaces.")
  .add_property("getIdxVertices",&XC::Edge::getIndicesVertices,"Return the IDs of the vertices")
  .add_property("firstNode",make_function(getFirstNodePtr, return_internal_reference<>()),"Return the first node of the edge")
  .add_property("lastNode",make_function(getLastNodePtr, return_internal_reference<>()),"Return the last node of the edge")
  .def("getLength", &XC::Edge::getLength,"Return the length of the edge.")
  .def("getCentroid", &XC::Edge::getCentroid,"Return the centroid of the edge.")
  .def("getTang", &XC::Edge::getTang, return_internal_reference<>(),"Return a vector tangent to the edge at parameter 0<=s<=L.")
  .def("divide",&XC::Edge::divide,"Create points along the line.")
  .def("getKPoints",&XC::Edge::getKPoints,"Return the end points of the edge.")
  .def("getTagsNodes",&XC::Edge::getTagsNodesForward,"Return node tags in forward order.")
  .def("getTagsNodesReverse",&XC::Edge::getTagsNodesReverse,"Return node tags in reverse order.")
  .add_property("getIVector", &XC::Edge::getIVector,"Return the local x vector.")
  .add_property("getJVector", &XC::Edge::getJVector,"Return the local y vector.")
  .add_property("getKVector", &XC::Edge::getKVector,"Return the local z vector.")
  .def("getLocalAxes",&XC::Edge::getLocalAxes,"Return the surface local axes as matrix rows: [[x1,y1,z1],[x2,y2,z2],...·] .")
   ;

const XC::Pnt *(XC::LineBase::*getFirstVertex)(void) const= &XC::LineBase::P1;
const XC::Pnt *(XC::LineBase::*getLastVertex)(void) const= &XC::LineBase::P2;
class_<XC::LineBase, bases<XC::Edge>, boost::noncopyable >("LineBase", no_init)
  .add_property("firstVertex", make_function(getFirstVertex, return_internal_reference<>() ),"Return the first vertex.")
  .add_property("lastVertex", make_function(getLastVertex, return_internal_reference<>() ),"Return the last vertex.")
  .def("getLambda",&XC::LineBase::getLambda,"Returns the parameter of the point in the line (distance to the line's first point measured over the line).")
   ;

class_<XC::Line, bases<XC::LineBase>, boost::noncopyable >("Line", no_init)
  .def("splitAtPoint", make_function(&XC::Line::splitAtPoint, return_internal_reference<>()),"Breaks the line at the point.")
  .def("splitAtLambda", make_function(&XC::Line::splitAtLambda, return_internal_reference<>()),"Breaks the line at the point defined by the paramenter (0.0->L).")
  .def("splitAtNaturalCoordinate", make_function(&XC::Line::splitAtNaturalCoord, return_internal_reference<>()),"Breaks the line at the point defined by the natural coordinate (0.0->1.0).")
   ;

class_<XC::DividedLine, bases<XC::Line>, boost::noncopyable >("DividedLine", "Line segment between two points.",no_init)
  .def("setLengths",&XC::DividedLine::setLengths,"Asigns length for each division.")
  ;

class_<XC::CircularArc, bases<XC::LineBase>, boost::noncopyable >("CircleArc", "Circumference arc.",no_init)
  .def("getAngle", &XC::CircularArc::getIncludedAngle,"Return the angle subtended by the arc.")
  .def("getTheta1",&XC::CircularArc::getTheta1,"Return the start angle.")
  .def("getTheta2",&XC::CircularArc::getTheta2,"Return the end angle.")
  .def("getCenter",&XC::CircularArc::getCenter,"Return the center of the circumference.")
  .def("getPInic",&XC::CircularArc::getPInic,"Return start point of the arc.")
  .def("getPFin",&XC::CircularArc::getPFin,"Return end point of the arc.")
  .def("getPMed",&XC::CircularArc::getPMed,"Return the midpoint of the arc.")
  .def("getRadius",&XC::CircularArc::getRadius,"Return the radius of the circumference.")
   ;

XC::Edge *(XC::CmbEdge::Side::*getEdge)(void)= &XC::CmbEdge::Side::getEdge;
class_<XC::CmbEdge::Side,bases<CommandEntity> >("Side", no_init)
  .add_property("isDirect", &XC::CmbEdge::Side::isDirect)
  .add_property("getEdge", make_function(getEdge, return_internal_reference<>()))
  .def("getLength", &XC::CmbEdge::Side::getLength,"Return edge's length.")
  .def("getCentroid", &XC::CmbEdge::Side::getCentroid,"Return edge's centroid.")
  .def("getTang", &XC::CmbEdge::Side::getTang, return_internal_reference<>(),"Return a vector tangent to the edge.")
  ;

typedef std::deque<XC::CmbEdge::Side> dq_sides;
class_<dq_sides, boost::noncopyable >("DqEdges", no_init)
  .def("__iter__", boost::python::iterator<dq_sides>())
  .add_property("size", &dq_sides::size)
   ;


class_<XC::CmbEdge, bases<XC::Edge>, boost::noncopyable >("CmbEdge","Compound line",no_init)
  .add_property("getNumVertices", &XC::CmbEdge::getNumberOfVertices,"Return the number of vertices.")
  .add_property("getNumEdges", &XC::CmbEdge::getNumberOfEdges,"Return the number of edges.")
  .add_property("getSides", make_function( &XC::CmbEdge::getSides, return_internal_reference<>()))
  .def("addLines",&XC::CmbEdge::addLines, return_internal_reference<>(),"Add lines to the sequence.")
  .def("addPoints",&XC::CmbEdge::addPoints, return_internal_reference<>(),"Add points to the sequence.")
  .def("getReversed",&XC::CmbEdge::getReversed,"Return the reversed face.")
  .def("reverse",&XC::CmbEdge::reverse,"Reverse the vertex sequence.")
   ;
