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

XC::TritrizPtrNod &(XC::EntMdlr::*getTtzNodes)(void)= &XC::EntMdlr::getTtzNodes;
XC::TritrizPtrElem &(XC::EntMdlr::*getTtzElements)(void)= &XC::EntMdlr::getTtzElements;
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
  .def("getVectorPos", &XC::Pnt::VectorPos,"Returns the position vector of the point.")
  .add_property("hasNode",&XC::Pnt::hasNode,"True if the point has a node.")
  .add_property("getTagNode",&XC::Pnt::getTagNode,"Returns node's tag.")
  .def("getNode",make_function(getNodePnt, return_internal_reference<>() ),"Returns point's node.")
  .add_property("getNLines",&XC::Pnt::getNLines,"Returns number of lines that touch the point.")
   ;

#include "python_interface_lines.tcc"

class_<XC::Face, XC::Face *,bases<XC::CmbEdge>, boost::noncopyable >("Face", no_init)
  .add_property("nDivI", &XC::Face::NDivI, &XC::Face::SetNDivI)
  .add_property("nDivJ", &XC::Face::NDivJ, &XC::Face::SetNDivJ)
  .def("getContour",&XC::Face::getContour,"Returns the face contour as a 3D polyline.")
   ;

class_<XC::QuadSurface, XC::QuadSurface *, bases<XC::Face>, boost::noncopyable >("QuadSurface", no_init)
  .def("defGridPoints",&XC::QuadSurface::defGridPoints)
  .def("setElemSizeI",&XC::QuadSurface::SetElemSizeI)
  .def("setElemSizeJ",&XC::QuadSurface::SetElemSizeJ)
  .def("setElemSizeIJ",&XC::QuadSurface::SetElemSizeIJ)
  .add_property("getIVector", &XC::QuadSurface::getIVector,"Returns the local x vector.")
  .add_property("getJVector", &XC::QuadSurface::getJVector,"Returns the local x vector.")
  .add_property("getKVector", &XC::QuadSurface::getKVector,"Returns the local x vector.")
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


class_<XC::MapCadMemberBase, bases<EntCmd>, boost::noncopyable >("MapCadMemberBase", no_init)
   .add_property("defaultTag", &XC::MapCadMemberBase::getTag, &XC::MapCadMemberBase::setTag)
   ;

typedef XC::MapCadMember<XC::Pnt> map_cm_puntos;
class_<map_cm_puntos, bases<XC::MapCadMemberBase>, boost::noncopyable >("MapCMPuntos", no_init)
//.def("__iter__", boost::python::iterator<map_cm_puntos>())
  .add_property("size", &map_cm_puntos::size)
  .def("exists", &map_cm_puntos::exists)
  .def("get", make_function(&map_cm_puntos::get,return_internal_reference<>()))
  .def("getKeys", &map_cm_puntos::getKeys)
   ;

typedef XC::MapEnt<XC::Pnt> map_puntos;
XC::Pnt *(map_puntos::*getNearestPoint)(const Pos3d &)= &map_puntos::getNearest;
class_<map_puntos, bases<map_cm_puntos>, boost::noncopyable >("map_puntos", no_init)
  .def("getNearest", getNearestPoint, return_internal_reference<>(),"Returns the nearest point.")
   ;

XC::Pnt *(XC::MapPuntos::*newPoint)(void)= &XC::MapPuntos::New;
XC::Pnt *(XC::MapPuntos::*newPointFromPos3d)(const Pos3d &)= &XC::MapPuntos::New;
XC::Pnt *(XC::MapPuntos::*newPointIDPos3d)(const size_t &,const Pos3d &)= &XC::MapPuntos::New;
class_<XC::MapPuntos, bases<map_puntos>, boost::noncopyable >("MapPuntos", no_init)
  .def("newPnt", newPoint, return_internal_reference<>(),"Creates a point.")
  .def("newPntFromPos3d", newPointFromPos3d, return_internal_reference<>(),"Creates a point in the position.")
  .def("newPntIDPos3d", newPointIDPos3d, return_internal_reference<>(),"Creates a point with the ID and the the position provided.")
   .def("get", &XC::MapPuntos::get, return_internal_reference<>(),"Returns the i-th point.")
   ;

typedef XC::MapCadMember<XC::Edge> map_cm_edges;
class_<map_cm_edges, bases<XC::MapCadMemberBase>, boost::noncopyable >("MapCMEdges", no_init)
//.def("__iter__", boost::python::iterator<map_cm_edges>())
  .add_property("size", &map_cm_edges::size)
  .def("exists", &map_cm_edges::exists)
  .def("get", make_function(&map_cm_edges::get,return_internal_reference<>()))
  .def("getKeys", &map_cm_edges::getKeys)
   ;

typedef XC::MapEnt<XC::Edge> map_edges;
class_<map_edges, bases<map_cm_edges>, boost::noncopyable >("map_edges", no_init)
   ;

class_<XC::MapLineas, bases<map_edges>, boost::noncopyable >("MapLineas", no_init)
   .def("newLine", &XC::MapLineas::newLine, return_internal_reference<>(),"Creates a line.")
   .def("newDividedLine", &XC::MapLineas::newDividedLine, return_internal_reference<>(),"Creates a line.")
   .def("newCircleArc", &XC::MapLineas::newCircleArc, return_internal_reference<>(),"Creates a circle arc.")
   .def("newLineSequence", &XC::MapLineas::newLineSequence, return_internal_reference<>(),"Creates a line sequence (polyline).")
   .def("get", &XC::MapLineas::get, return_internal_reference<>(),"Returns the i-th line.")
   ;

typedef XC::MapCadMember<XC::Face> map_cm_faces;
//map_cm_faces::reference (XC::ID::*getItemMapCMFaces)(const std::string &)= &map_cm_faces::operator[];
// class_<map_cm_faces::pair >("FacePair") IT DOESN'T WORK.
//   .def_readwrite("first", &map_cm_faces::pair::first)
//   .def_readwrite("second", &map_cm_faces::pair::second);
class_<map_cm_faces, bases<XC::MapCadMemberBase>, boost::noncopyable >("MapCMFaces", no_init)
  .add_property("size", &map_cm_faces::size)
  .def("exists", &map_cm_faces::exists)
  .def("get", make_function(&map_cm_faces::get,return_internal_reference<>()))
  .def("getKeys", &map_cm_faces::getKeys)
   ;

typedef XC::MapEnt<XC::Face> map_faces;
class_<map_faces, bases<map_cm_faces>, boost::noncopyable >("map_faces", no_init)
   ;

class_<XC::MapSurfaces, bases<map_faces>, boost::noncopyable >("MapSurfaces", no_init)
   .def("newQuadSurfacePts", &XC::MapSurfaces::newQuadSurfacePts, return_internal_reference<>(),"Creates a quadrilateral surface.")
   .def("newQuadSurfaceLines", &XC::MapSurfaces::newQuadSurfaceLines, return_internal_reference<>(),"Creates a quadrilateral surface.")
   .def("newQuadSurfaceGridPts", &XC::MapSurfaces::newQuadSurfaceGridPoints, return_internal_reference<>(),"Create a quadrilateral surface.")
   .def("get", &XC::MapSurfaces::get, return_internal_reference<>(),"Return the i-th face.")
   .def("conciliaNDivs", &XC::MapSurfaces::conciliaNDivs)
.def("checkNDivs",&XC::MapSurfaces::checkNDivs,"Check the number of divisions.")
   ;

typedef XC::MapCadMember<XC::Body> map_cm_bodies;
class_<map_cm_bodies, bases<XC::MapCadMemberBase>, boost::noncopyable >("MapCMBodies", no_init)
//.def("__iter__", boost::python::iterator<map_cm_bodies>())
  .add_property("size", &map_cm_bodies::size)
  .def("exists", &map_cm_bodies::exists)
  .def("get", make_function(&map_cm_bodies::get,return_internal_reference<>()))
  .def("getKeys", &map_cm_bodies::getKeys)
   ;

typedef XC::MapEnt<XC::Body> map_bodies;
class_<map_bodies, bases<map_cm_bodies>, boost::noncopyable >("map_bodies", no_init)
   ;

class_<XC::MapCuerpos, bases<map_bodies>, boost::noncopyable >("MapBodies", no_init)
   ;

typedef XC::MapCadMember<XC::Body> map_cm_ugrids;
class_<map_cm_ugrids, bases<XC::MapCadMemberBase>, boost::noncopyable >("MapCMUgrids", no_init)
//.def("__iter__", boost::python::iterator<map_cm_ugrids>())
  .add_property("size", &map_cm_ugrids::size)
  .def("exists", &map_cm_ugrids::exists)
  .def("get", make_function(&map_cm_ugrids::get,return_internal_reference<>()))
  .def("getKeys", &map_cm_ugrids::getKeys)
   ;

typedef XC::MapEnt<XC::UniformGrid> map_ugrids;
class_<map_ugrids, bases<map_cm_ugrids>, boost::noncopyable >("map_ugrids", no_init)
   ;

XC::UniformGrid *(XC::MapUniformGrids::*newUniformGrid)(void)= &XC::MapUniformGrids::Nueva;

class_<XC::MapUniformGrids, bases<map_ugrids>, boost::noncopyable >("MapUniformGrids", no_init)
   .def("newUniformGrid", newUniformGrid, return_internal_reference<>(),"Creates a uniform grid.")
   ;




