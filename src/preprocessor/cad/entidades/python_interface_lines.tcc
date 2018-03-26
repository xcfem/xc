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
  .add_property("getNumVertices", &XC::Edge::NumVertices,"Return the number of vertices.")
  .add_property("getIdxVertices",&XC::Edge::getIndicesVertices,"Return the IDs of the vertices")
  .add_property("firstNode",make_function(getFirstNodePtr, return_internal_reference<>()),"Return the first node of the edge")
  .add_property("lastNode",make_function(getLastNodePtr, return_internal_reference<>()),"Return the last node of the edge")
  .def("getLong", &XC::Edge::getLongitud,"Return the length of the edge.")
  .def("getCentroid", &XC::Edge::getCentroid,"Return the centroid of the edge.")
  .def("getTang", &XC::Edge::getTang, return_internal_reference<>(),"Return a vector tangent to the edge.")
  .def("divide",&XC::Edge::divide,"Create points along the line.")
  .def("getKPoints",&XC::Edge::getKPoints,"Return the end points of the edge.")
  .def("getTagsNodes",&XC::Edge::getTagsNodesDir,"Return node tags in forward order.")
  .def("getTagsNodesReverse",&XC::Edge::getTagsNodesInv,"Return node tags in reverse order.")
   ;

class_<XC::LineBase, bases<XC::Edge>, boost::noncopyable >("LineBase", no_init)
  .def("getLambda",&XC::LineBase::getLambda,"Returns the parameter of the point in the line (distance to the line's first point measured over the line).")
   ;

class_<XC::Linea, bases<XC::LineBase>, boost::noncopyable >("Line", no_init)
  .def("splitAtPoint", make_function(&XC::Linea::splitAtPoint, return_internal_reference<>()),"Breaks the line at the point.")
  .def("splitAtLambda", make_function(&XC::Linea::splitAtLambda, return_internal_reference<>()),"Breaks the line at the point defined by the paramenter (0.0->L).")
  .def("splitAtNaturalCoordinate", make_function(&XC::Linea::splitAtCooNatural, return_internal_reference<>()),"Breaks the line at the point defined by the natural coordinate (0.0->1.0).")
   ;

class_<XC::DividedLine, bases<XC::Linea>, boost::noncopyable >("DividedLine", "Line segment between two points.",no_init)
  .def("setLongs",&XC::DividedLine::setLongs,"Asigns length for each division.")
  ;

class_<XC::ArcoCircunf, bases<XC::LineBase>, boost::noncopyable >("CircleArc", "Circumference arc.",no_init)
  .def("getAngle", &XC::ArcoCircunf::getAnguloComprendido,"Return the angle subtended by the arc.")
  .def("getTheta1",&XC::ArcoCircunf::getTheta1,"Return the start angle.")
  .def("getTheta2",&XC::ArcoCircunf::getTheta2,"Return the end angle.")
  .def("getCentro",&XC::ArcoCircunf::getCentro,"Return the center of the circumference.")
  .def("getPInic",&XC::ArcoCircunf::getPInic,"Return start point of the arc.")
  .def("getPFin",&XC::ArcoCircunf::getPFin,"Return end point of the arc.")
  .def("getPMed",&XC::ArcoCircunf::getPMed,"Return the midpoint of the arc.")
  .def("getRadio",&XC::ArcoCircunf::getRadio,"Return the radius of the circumference.")
   ;

XC::Edge *(XC::CmbEdge::Lado::*getEdge)(void)= &XC::CmbEdge::Lado::getEdge;
class_<XC::CmbEdge::Lado,bases<EntCmd> >("Lado", no_init)
  .add_property("isDirect", &XC::CmbEdge::Lado::esDirecto)
  .add_property("getEdge", make_function(getEdge, return_internal_reference<>()))
  .def("getLong", &XC::CmbEdge::Lado::getLongitud,"Return edge's length.")
  .def("getCentroid", &XC::CmbEdge::Lado::getCentroid,"Return edge's centroid.")
  .def("getTang", &XC::CmbEdge::Lado::getTang, return_internal_reference<>(),"Return a vector tangent to the edge.")
  ;

typedef std::deque<XC::CmbEdge::Lado> dq_lados;
class_<dq_lados, boost::noncopyable >("DqEdges", no_init)
  .def("__iter__", boost::python::iterator<dq_lados>())
  .add_property("size", &dq_lados::size)
   ;


class_<XC::CmbEdge, bases<XC::Edge>, boost::noncopyable >("CmbEdge","Compound line",no_init)
  .add_property("getNumVertices", &XC::CmbEdge::NumVertices,"Return the number of vertices.")
  .add_property("getNumEdges", &XC::CmbEdge::NumEdges,"Return the number of edges.")
  .add_property("getEdges", make_function( &XC::CmbEdge::getLados, return_internal_reference<>()))
  .def("addLines",&XC::CmbEdge::addLines, return_internal_reference<>(),"Add lines to the sequence.")
  .def("addPoints",&XC::CmbEdge::addPoints, return_internal_reference<>(),"Add points to the sequence.")
   ;
