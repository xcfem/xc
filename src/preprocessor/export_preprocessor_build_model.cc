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
//export_preprocessor_build_model.cxx

#include "python_interface.h"

void export_preprocessor_build_model(void)
  {
    using namespace boost::python;
    docstring_options doc_options;

XC::Preprocessor *(XC::EntMdlrBase::*getPreprocessorRef)(void)= &XC::EntMdlrBase::getPreprocessor;
class_<XC::EntMdlrBase, bases<NamedEntity>, boost::noncopyable >("EntMdlrBase", no_init)
  .add_property("tag", &XC::EntMdlrBase::getTag, "Return the object identifier.")
  .add_property("getPreprocessor", make_function( getPreprocessorRef, return_internal_reference<>() ),"Return the preprocessor that built this object.")
  .def("__eq__", &XC::EntMdlrBase::operator==)
  //.def("__ne__", &XC::EntMdlrBase::operator!=)
   ;

bool (XC::SetBase::*isNodeIn)(const XC::Node *) const= &XC::SetBase::In;
bool (XC::SetBase::*isElementIn)(const XC::Element *) const= &XC::SetBase::In;
bool (XC::SetBase::*isPntIn)(const XC::Pnt *) const= &XC::SetBase::In;
bool (XC::SetBase::*isEdgeIn)(const XC::Edge *) const= &XC::SetBase::In;
bool (XC::SetBase::*isFaceIn)(const XC::Face *) const= &XC::SetBase::In;
bool (XC::SetBase::*isBodyIn)(const XC::Body *) const= &XC::SetBase::In;
bool (XC::SetBase::*isUniformGridIn)(const XC::UniformGrid *) const= &XC::SetBase::In;
class_<XC::SetBase, bases<XC::EntMdlrBase>, boost::noncopyable >("SetBase", no_init)
  .add_property("name", make_function( &XC::SetBase::getName, return_value_policy<copy_const_reference>()), &XC::SetBase::setName,"returns object name.")
  .def("genMesh", &XC::SetBase::genMesh,"Triggers mesh generation.")
  .def("getNodeTags",&XC::SetBase::getNodeTags,"return set of node tags.")
  .def("getElementTags",&XC::SetBase::getElementTags,"return set of node tags.")
  .def("resetTributaries",&XC::SetBase::resetTributaries)
  .def("computeTributaryLengths",&XC::SetBase::computeTributaryLengths)
  .def("computeTributaryAreas",&XC::SetBase::computeTributaryAreas)
  .def("computeTributaryVolumes",&XC::SetBase::computeTributaryVolumes)
  .def("isNodeIn",isNodeIn,"True if set contains node.")
  .def("isElementIn",isElementIn,"True if set contains the element argument.")
  .def("isPntIn",isPntIn,"True if sets contains the point argument.")
  .def("isEdgeIn",isEdgeIn,"True if set contains the edge argument.")
  .def("isFaceIn",isFaceIn,"True if set contains the face argument.")
  .def("isBodyIn",isBodyIn,"True if set contains the body argument.")
  .def("isUniformGridIn",isUniformGridIn,"True if set contains the uniform grid argument.")
  .add_property("getNumNodes",&XC::SetBase::getNumberOfNodes,"Return the number or nodes.")
  .add_property("getNumElements",&XC::SetBase::getNumberOfElements,"Return the number or elements.")
  .add_property("color", make_function(&XC::SetBase::getColor, return_internal_reference<>() ),&XC::SetBase::setColor,"stores a color for the objects of the set.")
  .def("setColorComponents",make_function(&XC::SetBase::setColorComponents, return_internal_reference<>() ),"set the components of the color for the objects of the set.")
  .def("createInertiaLoads", &XC::SetBase::createInertiaLoads, "Create the inertia load for the given acceleration vector.")
  ;

class_<XC::SetEstruct, bases<XC::SetBase>, boost::noncopyable >("SetEstruct", no_init)
  .add_property("getNumNodeLayers", &XC::SetEstruct::getNumNodeLayers,"Return the number of node layers.")
  .add_property("getNumNodeRows", &XC::SetEstruct::getNumNodeRows,"Return the number of node rows .")
  .add_property("getNumNodeColumns", &XC::SetEstruct::getNumNodeColumns,"Return the number of node columns.")
  .add_property("getNumElementLayers", &XC::SetEstruct::getNumElementLayers,"Return the number of element layers.")
  .add_property("getNumElementRows", &XC::SetEstruct::getNumElementRows,"Return the number of element rows.")
  .add_property("getNumElementColumns", &XC::SetEstruct::getNumElementColumns,"Return the number of element columns.")
  .add_property("getNumNodes", &XC::SetEstruct::getNumberOfNodes,"Return the number or nodes.")
  .add_property("getNumElements", &XC::SetEstruct::getNumberOfElements,"Return the number or elements.")
  .add_property("isIRow", &XC::SetEstruct::isIRow)
  .add_property("isJRow", &XC::SetEstruct::isJRow)
  .add_property("isKRow", &XC::SetEstruct::isKRow)
  .add_property("getDimension", &XC::SetEstruct::Dimension)
  .add_property("getStrType", &XC::SetEstruct::getStrType,"Return a string describing the set type i_row, j_row, k_row,...")
  .add_property("getNodes", &XC::SetEstruct::getNodes, "Return entity nodes. DEPRECATED use nodes.")
  .add_property("getElements", &XC::SetEstruct::getElements, "Return entity elements. DEPRECATED use elements.")
  .add_property("nodes", &XC::SetEstruct::getNodes, "Return entity nodes.")
  .add_property("elements", &XC::SetEstruct::getElements, "Return entity elements.")
  .def("getNodeI", &XC::SetEstruct::getNodeI, return_internal_reference<>(), "Return node in position (i)." )
  .def("getNodeIJ", &XC::SetEstruct::getNodeIJ, return_internal_reference<>(), "Return node in position (i,j)." )
  .def("getNodeIJK", &XC::SetEstruct::getNodeIJK, return_internal_reference<>(), "Return node in position (i,j,k).")
  .def("getElementI", &XC::SetEstruct::getElementI, return_internal_reference<>(), "Return element in position (i)." )
  .def("getElementIJ", &XC::SetEstruct::getElementIJ, return_internal_reference<>(), "Return element in position (i,j)." )
  .def("getElementIJK", &XC::SetEstruct::getElementIJK, return_internal_reference<>(), "Return element in position (i,j,k)." )
   ;

#include "preprocessor/multi_block_topology/python_interface.tcc"

  }

