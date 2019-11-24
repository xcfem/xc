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
  .def("getPosCentroid", &XC::EntMdlr::getCentroid,"Returns the centroid of the object.")
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

#include "1d/python_interface_lines.tcc"

class_<XC::Face, XC::Face *,bases<XC::CmbEdge>, boost::noncopyable >("Face", no_init)
  .add_property("nDivI", &XC::Face::NDivI, &XC::Face::setNDivI)
  .add_property("nDivJ", &XC::Face::NDivJ, &XC::Face::setNDivJ)
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
  .def("getLocalAxes",&XC::QuadSurface::getLocalAxes,"Return the surface local axes as matrix rows: [[x1,y1,z1],[x2,y2,z2],...·] .")
  .def("getPolygon",&XC::QuadSurface::getPolygon,"Return the contour as a 3D polygon.")
   ;


XC::Face *(XC::Body::BodyFace::*getSurfacePtr)(void)= &XC::Body::BodyFace::Surface;
class_<XC::Body::BodyFace, XC::Body::BodyFace*, bases<CommandEntity>, boost::noncopyable >("BodyFace","body faced." ,no_init)
  .def("getSurface",make_function(getSurfacePtr,return_internal_reference<>()),"Return the surface corresponding to the body face.")
   ;

const XC::Pnt *(XC::Body::*getVertexPtr)(const size_t &) const= &XC::Body::getVertex;
class_<XC::Body, XC::Body *, bases<XC::EntMdlr>, boost::noncopyable >("Body","Six-faced body." ,no_init)
  .add_property("getIdxVertices",&XC::Edge::getIndicesVertices)
  .def("getVertex",make_function(getVertexPtr,return_internal_reference<>()),"Return the vertex corresponding to the index argument.")
   ;

const XC::Body::BodyFace *(XC::Block::*getFacePtr)(const size_t &) const= &XC::Block::getFace;
class_<XC::Block, XC::Block *, bases<XC::Body>, boost::noncopyable >("Block", "Six-faced solid.",no_init)
  .def("getFace",make_function(getFacePtr,return_internal_reference<>()),"Return the face corresponding to the indes argument.")
  .add_property("nDivI", &XC::Block::NDivI, &XC::Block::setNDivI)
  .add_property("nDivJ", &XC::Block::NDivJ, &XC::Block::setNDivJ)
  .add_property("nDivK", &XC::Block::NDivK, &XC::Block::setNDivK)
  ;

class_<XC::UniformGrid, XC::UniformGrid *, bases<XC::EntMdlr>, boost::noncopyable >("UniformGrid", no_init)
  .add_property("org", make_function(&XC::UniformGrid::getOrg, return_internal_reference<>() ), &XC::UniformGrid::setOrg)
  .add_property("Lx", &XC::UniformGrid::getLx, &XC::UniformGrid::setLx)
  .add_property("Ly", &XC::UniformGrid::getLy, &XC::UniformGrid::setLy)
  .add_property("Lz", &XC::UniformGrid::getLz, &XC::UniformGrid::setLz)
  .add_property("nDivX", &XC::UniformGrid::getNDivX, &XC::UniformGrid::setNDivX)
  .add_property("nDivY", &XC::UniformGrid::getNDivY, &XC::UniformGrid::setNDivY)
  .add_property("nDivZ", &XC::UniformGrid::getNDivZ, &XC::UniformGrid::setNDivZ)
  ;


class_<XC::ModelComponentContainerBase, bases<CommandEntity>, boost::noncopyable >("ModelComponentContainerBase", no_init)
   .add_property("defaultTag", &XC::ModelComponentContainerBase::getTag, &XC::ModelComponentContainerBase::setTag)
   ;

#include "containers/python_interface.tcc"


