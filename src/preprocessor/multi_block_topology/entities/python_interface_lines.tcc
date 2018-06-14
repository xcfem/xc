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
class_<XC::Edge, bases<XC::EntMdlr>, boost::noncopyable >("Edge","Base class for one-dimensional geometry objects." ,no_init)
  .add_property("nDiv", &XC::Edge::NDiv, &XC::Edge::SetNDiv,"Number of divisions.")
  .def("setElemSize",&XC::Edge::SetElemSize,"Set the element size")
  .add_property("getNumVertices", &XC::Edge::getNumberOfVertices,"Return the number of vertices.")
  .add_property("getIdxVertices",&XC::Edge::getIndicesVertices,"Return the IDs of the vertices")
  .add_property("firstNode",make_function(getFirstNodePtr, return_internal_reference<>()),"Return the first node of the edge")
  .add_property("lastNode",make_function(getLastNodePtr, return_internal_reference<>()),"Return the last node of the edge")
  .def("getLong", &XC::Edge::getLength,"Return the length of the edge.")
  .def("getCentroid", &XC::Edge::getCentroid,"Return the centroid of the edge.")
  .def("getTang", &XC::Edge::getTang, return_internal_reference<>(),"Return a vector tangent to the edge.")
  .def("divide",&XC::Edge::divide,"Create points along the line.")
  .def("getKPoints",&XC::Edge::getKPoints,"Return the end points of the edge.")
  .def("getTagsNodes",&XC::Edge::getTagsNodesForward,"Return node tags in forward order.")
  .def("getTagsNodesReverse",&XC::Edge::getTagsNodesReverse,"Return node tags in reverse order.")
   ;

class_<XC::LineBase, bases<XC::Edge>, boost::noncopyable >("LineBase", no_init)
  .def("getLambda",&XC::LineBase::getLambda,"Returns the parameter of the point in the line (distance to the line's first point measured over the line).")
   ;

class_<XC::Line, bases<XC::LineBase>, boost::noncopyable >("Line", no_init)
  .def("splitAtPoint", make_function(&XC::Line::splitAtPoint, return_internal_reference<>()),"Breaks the line at the point.")
  .def("splitAtLambda", make_function(&XC::Line::splitAtLambda, return_internal_reference<>()),"Breaks the line at the point defined by the paramenter (0.0->L).")
  .def("splitAtNaturalCoordinate", make_function(&XC::Line::splitAtCooNatural, return_internal_reference<>()),"Breaks the line at the point defined by the natural coordinate (0.0->1.0).")
   ;

class_<XC::DividedLine, bases<XC::Line>, boost::noncopyable >("DividedLine", "Line segment between two points.",no_init)
  .def("setLongs",&XC::DividedLine::setLongs,"Asigns length for each division.")
  ;

class_<XC::CircularArc, bases<XC::LineBase>, boost::noncopyable >("CircleArc", "Circumference arc.",no_init)
  .def("getAngle", &XC::CircularArc::getIncludedAngle,"Return the angle subtended by the arc.")
  .def("getTheta1",&XC::CircularArc::getTheta1,"Return the start angle.")
  .def("getTheta2",&XC::CircularArc::getTheta2,"Return the end angle.")
  .def("getCentro",&XC::CircularArc::getCentro,"Return the center of the circumference.")
  .def("getPInic",&XC::CircularArc::getPInic,"Return start point of the arc.")
  .def("getPFin",&XC::CircularArc::getPFin,"Return end point of the arc.")
  .def("getPMed",&XC::CircularArc::getPMed,"Return the midpoint of the arc.")
  .def("getRadius",&XC::CircularArc::getRadius,"Return the radius of the circumference.")
   ;

XC::Edge *(XC::CmbEdge::Side::*getEdge)(void)= &XC::CmbEdge::Side::getEdge;
class_<XC::CmbEdge::Side,bases<EntCmd> >("Side", no_init)
  .add_property("isDirect", &XC::CmbEdge::Side::isDirect)
  .add_property("getEdge", make_function(getEdge, return_internal_reference<>()))
  .def("getLong", &XC::CmbEdge::Side::getLength,"Return edge's length.")
  .def("getCentroid", &XC::CmbEdge::Side::getCentroid,"Return edge's centroid.")
  .def("getTang", &XC::CmbEdge::Side::getTang, return_internal_reference<>(),"Return a vector tangent to the edge.")
  ;

typedef std::deque<XC::CmbEdge::Side> dq_lados;
class_<dq_lados, boost::noncopyable >("DqEdges", no_init)
  .def("__iter__", boost::python::iterator<dq_lados>())
  .add_property("size", &dq_lados::size)
   ;


class_<XC::CmbEdge, bases<XC::Edge>, boost::noncopyable >("CmbEdge","Compound line",no_init)
  .add_property("getNumVertices", &XC::CmbEdge::getNumberOfVertices,"Return the number of vertices.")
  .add_property("getNumEdges", &XC::CmbEdge::getNumberOfEdges,"Return the number of edges.")
  .add_property("getEdges", make_function( &XC::CmbEdge::getSides, return_internal_reference<>()))
  .def("addLines",&XC::CmbEdge::addLines, return_internal_reference<>(),"Add lines to the sequence.")
  .def("addPoints",&XC::CmbEdge::addPoints, return_internal_reference<>(),"Add points to the sequence.")
   ;
