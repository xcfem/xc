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
  .add_property("nDivI", &XC::Block::NDivI, &XC::Block::setNDivI, "Number or divisions in I direction.")
  .add_property("nDivJ", &XC::Block::NDivJ, &XC::Block::setNDivJ, "Number or divisions in J direction.")
  .add_property("nDivK", &XC::Block::NDivK, &XC::Block::setNDivK, "Number or divisions in K direction.")
  ;

class_<XC::UniformGrid, XC::UniformGrid *, bases<XC::EntMdlr>, boost::noncopyable >("UniformGrid", no_init)
  .add_property("org", make_function(&XC::UniformGrid::getOrg, return_internal_reference<>() ), &XC::UniformGrid::setOrg)
  .add_property("Lx", &XC::UniformGrid::getLx, &XC::UniformGrid::setLx, "Length in x direction.")
  .add_property("Ly", &XC::UniformGrid::getLy, &XC::UniformGrid::setLy, "Length in y direction.")
  .add_property("Lz", &XC::UniformGrid::getLz, &XC::UniformGrid::setLz, "Length in z direction.")
  .add_property("nDivX", &XC::UniformGrid::getNDivX, &XC::UniformGrid::setNDivX, "Number or divisions in x direction.")
  .add_property("nDivY", &XC::UniformGrid::getNDivY, &XC::UniformGrid::setNDivY, "Number or divisions in y direction.")
  .add_property("nDivZ", &XC::UniformGrid::getNDivZ, &XC::UniformGrid::setNDivZ, "Number or divisions in z direction.")
  ;



