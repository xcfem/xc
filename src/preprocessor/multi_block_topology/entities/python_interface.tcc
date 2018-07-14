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

XC::NodePtrArray3d &(XC::EntMdlr::*getTtzNodes)(void)= &XC::EntMdlr::getTtzNodes;
XC::ElemPtrArray3d &(XC::EntMdlr::*getTtzElements)(void)= &XC::EntMdlr::getTtzElements;
XC::Node *(XC::EntMdlr::*getNodeEntMdlr)(const size_t &i,const size_t &,const size_t &)= &XC::EntMdlr::getNode;
XC::Node *(XC::EntMdlr::*getNearestNodeEntMdlr)(const Pos3d &)= &XC::EntMdlr::getNearestNode;
XC::Element *(XC::EntMdlr::*getElementEntMdlr)(const size_t &i,const size_t &,const size_t &)= &XC::EntMdlr::getElement; 


XC::Element *(XC::EntMdlr::*getNearestElementEntMdlr)(const Pos3d &)= &XC::EntMdlr::getNearestElement;
class_<XC::EntMdlr, bases<XC::SetEstruct>, boost::noncopyable >("EntMdlr", no_init)
  .add_property("getIdx", &XC::EntMdlr::getIdx)
  .add_property("getNodeLayers", make_function( getTtzNodes, return_internal_reference<>() ))
  .add_property("getElementLayers", make_function( getTtzElements, return_internal_reference<>() ))
  .add_property("getVtkCellType", &XC::EntMdlr::getVtkCellType)
  .def("getNode",make_function(getNodeEntMdlr, return_internal_reference<>() ),"Returns (i,j,k) node.")
  .def("getNearestNode",make_function(getNearestNodeEntMdlr, return_internal_reference<>() ),"Returns nearest node.")
  .def("getElement",make_function(getElementEntMdlr, return_internal_reference<>() ),"Returns (i,j,k) node.")
  .def("getNearestElement",make_function(getNearestElementEntMdlr, return_internal_reference<>() ),"Returns nearest element.")
  .def("getSimpsonWeights", &XC::EntMdlr::getSimpsonWeights,"Returns weights for Simpson's rule integration.")
  .def("In", &XC::EntMdlr::In,"\n""In(geomObject,tolerance) \n""Return true if this object lies inside the geometric object.")
  .def("Out", &XC::EntMdlr::Out,"\n""Out(geomObject,tolerance) \n""Return true if this object lies outside the geometric object.")
   ;

XC::Node *(XC::Pnt::*getNodePnt)(void)= &XC::Pnt::getNode;
class_<XC::Pnt, XC::Pnt *, bases<XC::EntMdlr>, boost::noncopyable >("Pnt", no_init)
  .add_property("getPos", make_function( &XC::Pnt::GetPos, return_internal_reference<>() ),&XC::Pnt::setPos)
  .def("getVectorPos", &XC::Pnt::VectorPos,"Return the position vector of the point.")
  .add_property("hasNode",&XC::Pnt::hasNode,"True if the point has a node.")
  .add_property("getTagNode",&XC::Pnt::getTagNode,"Returns node's tag.")
  .def("getNode",make_function(getNodePnt, return_internal_reference<>() ),"Returns point's node.")
  .add_property("getNLines",&XC::Pnt::getNLines,"Returns number of lines that touch the point.")
   ;

#include "python_interface_lines.tcc"

class_<XC::Face, XC::Face *,bases<XC::CmbEdge>, boost::noncopyable >("Face", no_init)
  .add_property("nDivI", &XC::Face::NDivI, &XC::Face::SetNDivI)
  .add_property("nDivJ", &XC::Face::NDivJ, &XC::Face::SetNDivJ)
  .def("getContour",&XC::Face::getContour,"Return the face contour as a 3D polyline.")
   ;

class_<XC::QuadSurface, XC::QuadSurface *, bases<XC::Face>, boost::noncopyable >("QuadSurface", no_init)
  .def("defGridPoints",&XC::QuadSurface::defGridPoints)
  .def("setElemSizeI",&XC::QuadSurface::SetElemSizeI)
  .def("setElemSizeJ",&XC::QuadSurface::SetElemSizeJ)
  .def("setElemSizeIJ",&XC::QuadSurface::SetElemSizeIJ)
  .add_property("getIVector", &XC::QuadSurface::getIVector,"Return the local x vector.")
  .add_property("getJVector", &XC::QuadSurface::getJVector,"Return the local x vector.")
  .add_property("getKVector", &XC::QuadSurface::getKVector,"Return the local x vector.")
   ;


class_<XC::Body, XC::Body *, bases<XC::EntMdlr>, boost::noncopyable >("Body","Six-faced body." ,no_init)
  .add_property("getIdxVertices",&XC::Edge::getIndicesVertices)
   ;

class_<XC::Block, XC::Block *, bases<XC::Body>, boost::noncopyable >("Block", "Six-faced solid.",no_init);

class_<XC::UniformGrid, XC::UniformGrid *, bases<XC::EntMdlr>, boost::noncopyable >("UniformGrid", no_init)
  .add_property("org", make_function(&XC::UniformGrid::getOrg, return_internal_reference<>() ), &XC::UniformGrid::setOrg)
  .add_property("Lx", &XC::UniformGrid::getLx, &XC::UniformGrid::setLx)
  .add_property("Ly", &XC::UniformGrid::getLy, &XC::UniformGrid::setLy)
  .add_property("Lz", &XC::UniformGrid::getLz, &XC::UniformGrid::setLz)
  .add_property("nDivX", &XC::UniformGrid::getNDivX, &XC::UniformGrid::setNDivX)
  .add_property("nDivY", &XC::UniformGrid::getNDivY, &XC::UniformGrid::setNDivY)
  .add_property("nDivZ", &XC::UniformGrid::getNDivZ, &XC::UniformGrid::setNDivZ)
  ;


class_<XC::ModelComponentContainerBase, bases<EntCmd>, boost::noncopyable >("ModelComponentContainerBase", no_init)
   .add_property("defaultTag", &XC::ModelComponentContainerBase::getTag, &XC::ModelComponentContainerBase::setTag)
   ;

typedef XC::ModelComponentContainer<XC::Pnt> cm_point_map;
class_<cm_point_map, bases<XC::ModelComponentContainerBase>, boost::noncopyable >("CMPointMap", no_init)
//.def("__iter__", boost::python::iterator<cm_point_map>())
  .add_property("size", &cm_point_map::size)
  .def("exists", &cm_point_map::exists)
  .def("get", make_function(&cm_point_map::get,return_internal_reference<>()))
  .def("getKeys", &cm_point_map::getKeys)
   ;

typedef XC::EntityMap<XC::Pnt> point_map;
XC::Pnt *(point_map::*getNearestPoint)(const Pos3d &)= &point_map::getNearest;
class_<point_map, bases<cm_point_map>, boost::noncopyable >("point_map", no_init)
  .def("getNearest", getNearestPoint, return_internal_reference<>(),"Return the nearest point.")
   ;

XC::Pnt *(XC::PntMap::*newPoint)(void)= &XC::PntMap::New;
XC::Pnt *(XC::PntMap::*newPointFromPos3d)(const Pos3d &)= &XC::PntMap::New;
XC::Pnt *(XC::PntMap::*newPointIDPos3d)(const size_t &,const Pos3d &)= &XC::PntMap::New;
class_<XC::PntMap, bases<point_map>, boost::noncopyable >("PntMap", no_init)
  .def("newPnt", newPoint, return_internal_reference<>(),"Creates a point.")
  .def("newPntFromPos3d", newPointFromPos3d, return_internal_reference<>(),"Creates a point in the position.")
  .def("newPntIDPos3d", newPointIDPos3d, return_internal_reference<>(),"Creates a point with the ID and the the position provided.")
   .def("get", &XC::PntMap::get, return_internal_reference<>(),"Return the i-th point.")
   ;

typedef XC::ModelComponentContainer<XC::Edge> map_cm_edges;
class_<map_cm_edges, bases<XC::ModelComponentContainerBase>, boost::noncopyable >("MapCMEdges", no_init)
//.def("__iter__", boost::python::iterator<map_cm_edges>())
  .add_property("size", &map_cm_edges::size)
  .def("exists", &map_cm_edges::exists)
  .def("get", make_function(&map_cm_edges::get,return_internal_reference<>()))
  .def("getKeys", &map_cm_edges::getKeys)
   ;

typedef XC::EntityMap<XC::Edge> map_edges;
class_<map_edges, bases<map_cm_edges>, boost::noncopyable >("map_edges", no_init)
   ;

class_<XC::LineMap, bases<map_edges>, boost::noncopyable >("LineMap", no_init)
   .def("newLine", &XC::LineMap::newLine, return_internal_reference<>(),"Creates a line.")
   .def("newDividedLine", &XC::LineMap::newDividedLine, return_internal_reference<>(),"Creates a line.")
   .def("newCircleArc", &XC::LineMap::newCircleArc, return_internal_reference<>(),"Creates a circle arc.")
   .def("newLineSequence", &XC::LineMap::newLineSequence, return_internal_reference<>(),"Creates a line sequence (polyline).")
   .def("get", &XC::LineMap::get, return_internal_reference<>(),"Return the i-th line.")
   ;

typedef XC::ModelComponentContainer<XC::Face> map_cm_faces;
//map_cm_faces::reference (XC::ID::*getItemMapCMFaces)(const std::string &)= &map_cm_faces::operator[];
// class_<map_cm_faces::pair >("FacePair") IT DOESN'T WORK.
//   .def_readwrite("first", &map_cm_faces::pair::first)
//   .def_readwrite("second", &map_cm_faces::pair::second);
class_<map_cm_faces, bases<XC::ModelComponentContainerBase>, boost::noncopyable >("MapCMFaces", no_init)
  .add_property("size", &map_cm_faces::size)
  .def("exists", &map_cm_faces::exists)
  .def("get", make_function(&map_cm_faces::get,return_internal_reference<>()))
  .def("getKeys", &map_cm_faces::getKeys)
   ;

typedef XC::EntityMap<XC::Face> map_faces;
class_<map_faces, bases<map_cm_faces>, boost::noncopyable >("map_faces", no_init)
   ;

class_<XC::SurfaceMap, bases<map_faces>, boost::noncopyable >("SurfaceMap", no_init)
   .def("newQuadSurfacePts", &XC::SurfaceMap::newQuadSurfacePts, return_internal_reference<>(),"Creates a quadrilateral surface.")
   .def("newQuadSurfaceLines", &XC::SurfaceMap::newQuadSurfaceLines, return_internal_reference<>(),"Creates a quadrilateral surface.")
   .def("newQuadSurfaceGridPts", &XC::SurfaceMap::newQuadSurfaceGridPoints, return_internal_reference<>(),"Create a quadrilateral surface.")
   .def("get", &XC::SurfaceMap::get, return_internal_reference<>(),"Return the i-th face.")
   .def("conciliaNDivs", &XC::SurfaceMap::conciliaNDivs)
   .def("checkNDivs",&XC::SurfaceMap::checkNDivs,"Check the number of divisions.")
   ;

typedef XC::ModelComponentContainer<XC::Body> map_cm_bodies;
class_<map_cm_bodies, bases<XC::ModelComponentContainerBase>, boost::noncopyable >("MapCMBodies", no_init)
//.def("__iter__", boost::python::iterator<map_cm_bodies>())
  .add_property("size", &map_cm_bodies::size)
  .def("exists", &map_cm_bodies::exists)
  .def("get", make_function(&map_cm_bodies::get,return_internal_reference<>()))
  .def("getKeys", &map_cm_bodies::getKeys)
   ;

typedef XC::EntityMap<XC::Body> map_bodies;
class_<map_bodies, bases<map_cm_bodies>, boost::noncopyable >("map_bodies", no_init)
   ;

class_<XC::BodyMap, bases<map_bodies>, boost::noncopyable >("MapBodies", no_init)
   ;

typedef XC::ModelComponentContainer<XC::Body> map_cm_ugrids;
class_<map_cm_ugrids, bases<XC::ModelComponentContainerBase>, boost::noncopyable >("MapCMUgrids", no_init)
//.def("__iter__", boost::python::iterator<map_cm_ugrids>())
  .add_property("size", &map_cm_ugrids::size)
  .def("exists", &map_cm_ugrids::exists)
  .def("get", make_function(&map_cm_ugrids::get,return_internal_reference<>()))
  .def("getKeys", &map_cm_ugrids::getKeys)
   ;

typedef XC::EntityMap<XC::UniformGrid> map_ugrids;
class_<map_ugrids, bases<map_cm_ugrids>, boost::noncopyable >("map_ugrids", no_init)
   ;

XC::UniformGrid *(XC::UniformGridMap::*newUniformGrid)(void)= &XC::UniformGridMap::Nueva;

class_<XC::UniformGridMap, bases<map_ugrids>, boost::noncopyable >("UniformGridMap", no_init)
   .def("newUniformGrid", newUniformGrid, return_internal_reference<>(),"Creates a uniform grid.")
   ;




