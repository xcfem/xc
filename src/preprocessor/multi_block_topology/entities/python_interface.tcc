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

#include "0d/python_interface.tcc"
#include "1d/python_interface_lines.tcc"
#include "2d/python_interface.tcc"
#include "3d/python_interface.tcc"

#include "containers/python_interface.tcc"


