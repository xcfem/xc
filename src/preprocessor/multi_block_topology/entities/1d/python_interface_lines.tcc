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
//python_interface_lines.tcc

XC::Pnt *(XC::Edge::*getP1)(void)= &XC::Edge::P1;
XC::Pnt *(XC::Edge::*getP2)(void)= &XC::Edge::P2;
XC::Node *(XC::Edge::*getFirstNodePtr)(void)= &XC::Edge::getFirstNode;
XC::Node *(XC::Edge::*getLastNodePtr)(void)= &XC::Edge::getLastNode;
XC::Pnt *(XC::Edge::*getEdgeVertexPtr)(const size_t &)= &XC::Edge::getVertex;
size_t (XC::Edge::*get_num_connected_surfaces_edge)(void) const= &XC::Edge::getNumConnectedSurfaces;
size_t (XC::Edge::*get_num_connected_surfaces_edge_set)(const XC::SetBase *) const= &XC::Edge::getNumConnectedSurfaces;
size_t (XC::Edge::*get_num_connected_edges_edge)(void) const= &XC::Edge::getNumConnectedEdges;
size_t (XC::Edge::*get_num_connected_edges_edge_set)(const XC::SetBase *) const= &XC::Edge::getNumConnectedEdges;
boost::python::list (XC::Edge::*get_connected_edges_edge_py)(void) const= &XC::Edge::getConnectedEdgesPy;
boost::python::list (XC::Edge::*get_connected_edges_edge_set_py)(const XC::SetBase *) const= &XC::Edge::getConnectedEdgesPy;
boost::python::list (XC::Edge::*get_connected_surfaces_edge_py)(void) const= &XC::Edge::getConnectedSurfacesPy;
boost::python::list (XC::Edge::*get_connected_surfaces_edge_set_py)(const XC::SetBase *) const= &XC::Edge::getConnectedSurfacesPy;
class_<XC::Edge, XC::Edge*, bases<XC::EntMdlr>, boost::noncopyable >("Edge","Base class for one-dimensional geometry objects." ,no_init)
  .add_property("nDiv", &XC::Edge::NDiv, &XC::Edge::setNDiv,"Number of divisions.")
  .def("setNDiv", &XC::Edge::setNDiv,"Set the number of divisions.")
  .def("setElemSize",&XC::Edge::SetElemSize,"Set the element size")
  .add_property("getNumVertices", &XC::Edge::getNumberOfVertices,"Return the number of vertices.")

  .add_property("numConnectedSurfaces", get_num_connected_surfaces_edge, "Return the number of connected surfaces.")
  .def("getNumConnectedSurfaces", get_num_connected_surfaces_edge_set, "Return the number of surfaces of the given set connected to this edge.")
  .add_property("numConnectedEdges", get_num_connected_edges_edge, "Returns number of edges connected to this one.")
  .def("getNumConnectedEdges", get_num_connected_edges_edge_set, "Return the number of edges of the given set connected to this edge.")
  .add_property("connectedEdges", get_connected_edges_edge_py,"Returns the edges connected to this one.")
  .def("getConnectedEdges", get_connected_edges_edge_set_py,"Returns the edges of the given set connected to this one.")
  .add_property("connectedSuraces", get_connected_surfaces_edge_py,"Returns the surfaces connected to this edge.")
  .def("getConnectedSurfaces", get_connected_surfaces_edge_set_py,"Returns the surfaces of the given set connected to this edge.")
  .add_property("getIdxVertices",&XC::Edge::getIndicesVertices,"Return the IDs of the vertices")

  .add_property("getVertices",&XC::Edge::getVerticesPy,"Return the vertices (or points) defining the line.")
  .add_property("points",&XC::Edge::getVerticesPy,"Return the points (or vertices) defining the line.")
  .add_property("segments",&XC::Edge::getSegmentsPy,"Return the segments")
  .add_property("firstNode",make_function(getFirstNodePtr, return_internal_reference<>()),"Return the first node of the edge")
  .add_property("lastNode",make_function(getLastNodePtr, return_internal_reference<>()),"Return the last node of the edge")
  .def("getVertex",make_function(getEdgeVertexPtr, return_internal_reference<>()),"Return i-th vertex.")
  .def("getLength", &XC::Edge::getLength,"Return the length of the edge.")
  .def("getElemSize", &XC::Edge::getElemSize,"Return the element size.")
  .def("getCentroid", &XC::Edge::getCentroid,"Return the centroid of the edge.")
  .def("getTang", &XC::Edge::getTang, return_internal_reference<>(),"Return a vector tangent to the edge at parameter 0<=s<=L.")
  .def("divide",&XC::Edge::divide,"Create points along the line.")
  .def("getP1",make_function(getP1, return_internal_reference<>()),"Return the start point of the edge")
  .def("getP2",make_function(getP2, return_internal_reference<>()),"Return the end point of the edge")
  .def("getKPoints",&XC::Edge::getKPoints,"Return the tags of the edge end points.")
  .def("getTagsNodes",&XC::Edge::getTagsNodesForward,"Return node tags in forward order.")
  .def("getTagsNodesReverse",&XC::Edge::getTagsNodesReverse,"Return node tags in reverse order.")
  .add_property("getIVector", &XC::Edge::getIVector,"Return the local x vector.")
  .add_property("getJVector", &XC::Edge::getJVector,"Return the local y vector.")
  .add_property("getKVector", &XC::Edge::getKVector,"Return the local z vector.")
  .def("getLocalAxes",&XC::Edge::getLocalAxes,"Return the surface local axes as matrix rows: [[x1,y1,z1],[x2,y2,z2],...·] .")
   ;

const XC::Pnt *(XC::LineBase::*getFirstVertex)(void) const= &XC::LineBase::P1;
const XC::Pnt *(XC::LineBase::*getLastVertex)(void) const= &XC::LineBase::P2;
double (XC::LineBase::*getLambdaPt)(const Pos3d &) const= &XC::LineBase::getLambda;
class_<XC::LineBase, bases<XC::Edge>, boost::noncopyable >("LineBase", no_init)
  .add_property("firstVertex", make_function(getFirstVertex, return_internal_reference<>() ),"Return the first vertex.")
  .add_property("lastVertex", make_function(getLastVertex, return_internal_reference<>() ),"Return the last vertex.")
  .def("getLambda",getLambdaPt,"Returns the parameter of the point in the line (distance to the line's first point measured over the line).")
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
class_<XC::CmbEdge::Side, bases<CommandEntity> >("Side", no_init)
  .add_property("name", make_function(&XC::CmbEdge::Side::getName,return_value_policy<copy_const_reference>()), "return object name.")
  .add_property("isDirect", &XC::CmbEdge::Side::isDirect)
  .add_property("getEdge", make_function(getEdge, return_internal_reference<>()))
  .def("getLength", &XC::CmbEdge::Side::getLength,"Return edge's length.")
  .def("getCentroid", &XC::CmbEdge::Side::getCentroid,"Return edge's centroid.")
  .def("getTang", &XC::CmbEdge::Side::getTang, return_internal_reference<>(),"Return a vector tangent to the edge.")
  .def("genMesh", &XC::CmbEdge::Side::genMesh, "Generate mesh.")
  ;

typedef std::deque<XC::CmbEdge::Side> dq_sides;
class_<dq_sides, boost::noncopyable >("DqEdges", no_init)
  .def("__iter__", boost::python::iterator<dq_sides>())
  .add_property("size", &dq_sides::size)
   ;

std::deque<XC::CmbEdge::Side> &(XC::CmbEdge::*getSidesCmbEdge)(void)= &XC::CmbEdge::getSides;
class_<XC::CmbEdge, bases<XC::Edge>, boost::noncopyable >("CmbEdge","Compound line",no_init)
  .add_property("getNumVertices", &XC::CmbEdge::getNumberOfVertices,"Return the number of vertices.")
  .add_property("getNumEdges", &XC::CmbEdge::getNumberOfEdges,"Return the number of edges.")
  .add_property("getSides", make_function( getSidesCmbEdge, return_internal_reference<>()),"Return the sides of the compound edge.")
  .add_property("getEdges", &XC::CmbEdge::getEdgesPy,"Return the edges of the compound edge.")
  .add_property("edges", &XC::CmbEdge::getEdgesPy,"Return the edges of the compound edge.")
  .def("hasEdge", &XC::CmbEdge::hasEdge, "Return true if the edge makes part of this compound edge.")
  .def("addLines",&XC::CmbEdge::addLines, return_internal_reference<>(),"Add lines to the sequence.")
  .def("addPoints",&XC::CmbEdge::addPoints, return_internal_reference<>(),"Add points to the sequence.")
  .def("getAvgElemSize", &XC::CmbEdge::getAvgElemSize,"Return the average element size.")
  .def("getMaxElemSize", &XC::CmbEdge::getMaxElemSize,"Return the maximum element size.")
  .def("getMinElemSize", &XC::CmbEdge::getMinElemSize,"Return the minimum element size.")
  .def("getReversed",&XC::CmbEdge::getReversed,"Return the reversed face.")
  .def("reverse",&XC::CmbEdge::reverse,"Reverse the vertex sequence.")
   ;
