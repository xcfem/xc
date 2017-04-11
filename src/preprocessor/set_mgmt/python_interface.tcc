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

typedef XC::DqPtrs<XC::Node> dq_ptrs_node;
class_<dq_ptrs_node, bases<EntCmd>, boost::noncopyable >("dq_ptrs_node",no_init)
  .def("__iter__", range<return_internal_reference<> >(&dq_ptrs_node::indBegin, &dq_ptrs_node::indEnd))
  .add_property("size", &dq_ptrs_node::size, "Returns list size.")
  .def("__len__",&dq_ptrs_node::size, "Returns list size.")
  .def("at",make_function(&dq_ptrs_node::get, return_internal_reference<>() ), "Access specified node with bounds checking.")
  .def("getTags",make_function(&dq_ptrs_node::getTags, return_internal_reference<>() ),"Returns node identifiers.")
  .def("clear",&dq_ptrs_node::clear,"Removes all items.")
  ;

XC::Node *(XC::DqPtrsNode::*getNearestNodeDqPtrs)(const Pos3d &)= &XC::DqPtrsNode::getNearestNode;
class_<XC::DqPtrsNode, bases<dq_ptrs_node>, boost::noncopyable >("DqPtrsNode",no_init)
  .def("append", &XC::DqPtrsNode::push_back,"Appends node at the end of the list.")
  .def("pushFront", &XC::DqPtrsNode::push_front,"Push node at the beginning of the list.")
  .add_property("getNumLiveNodes", &XC::DqPtrsNode::getNumLiveNodes)
  .add_property("getNumDeadNodes", &XC::DqPtrsNode::getNumDeadNodes)
  .def("getNearestNode",make_function(getNearestNodeDqPtrs, return_internal_reference<>() ),"Returns nearest node.")
   ;

typedef XC::DqPtrs<XC::Element> dq_ptrs_element;
class_<dq_ptrs_element, bases<EntCmd>, boost::noncopyable >("dq_ptrs_element",no_init)
  .def("__iter__", range<return_internal_reference<> >(&dq_ptrs_element::indBegin, &dq_ptrs_element::indEnd))
  .add_property("size", &dq_ptrs_element::size, "Returns list size.")
  .def("__len__",&dq_ptrs_element::size, "Returns list size.")
  .def("at",make_function(&dq_ptrs_element::get, return_internal_reference<>() ), "Access specified element with bounds checking.")
  .def("getTags",make_function(&dq_ptrs_element::getTags, return_internal_reference<>() ),"Returns element identifiers.")
  .def("clear",&dq_ptrs_element::clear,"Removes all items.")
  ;

XC::Element *(XC::DqPtrsElem::*getNearestElementDqPtrs)(const Pos3d &)= &XC::DqPtrsElem::getNearestElement;
class_<XC::DqPtrsElem, bases<dq_ptrs_element>, boost::noncopyable >("DqPtrsElem",no_init)
  .def("append", &XC::DqPtrsElem::push_back,"Appends element at the end of the list.")
  .def("pushFront", &XC::DqPtrsElem::push_front,"Push element at the beginning of the list.")
  .add_property("getNumLiveElements", &XC::DqPtrsElem::getNumLiveElements)
  .add_property("getNumDeadElements", &XC::DqPtrsElem::getNumDeadElements)
  .def("getNearestElement",make_function(getNearestElementDqPtrs, return_internal_reference<>() ),"Returns nearest element.")
  .def("getContours",&XC::DqPtrsElem::getContours,"Returns contour(s) from the element set in the form of closed 3D polylines.")
   ;

typedef XC::DqPtrs<XC::Constraint> dq_ptrs_constraint;
class_<dq_ptrs_constraint, bases<EntCmd>, boost::noncopyable >("dq_ptrs_constraint",no_init)
  .def("__iter__", range<return_internal_reference<> >(&dq_ptrs_constraint::indBegin, &dq_ptrs_constraint::indEnd))
  .add_property("size", &dq_ptrs_constraint::size, "Returns list size.")
  .def("__len__",&dq_ptrs_constraint::size, "Returns list size.")
  .def("at",make_function(&dq_ptrs_constraint::get, return_internal_reference<>() ), "Access specified constraint with bounds checking.")
  .def("getTags",make_function(&dq_ptrs_constraint::getTags, return_internal_reference<>() ),"Returns constraint identifiers.")
  .def("clear",&dq_ptrs_constraint::clear,"Removes all items.")
  ;

class_<XC::DqPtrsConstraint, bases<dq_ptrs_constraint>, boost::noncopyable >("DqPtrsConstraint",no_init)
  .def("append", &XC::DqPtrsConstraint::push_back,"Appends constraint at the end of the list.")
  .def("pushFront", &XC::DqPtrsConstraint::push_front,"Push constraint at the beginning of the list.")
   ;

XC::DqPtrsNode &(XC::SetMeshComp::*GetNodosRef)(void)= &XC::SetMeshComp::GetNodos;
XC::DqPtrsElem &(XC::SetMeshComp::*getElementsRef)(void)= &XC::SetMeshComp::getElements;
XC::DqPtrsConstraint &(XC::SetMeshComp::*GetConstraintsRef)(void)= &XC::SetMeshComp::GetConstraints;
XC::Node *(XC::SetMeshComp::*getNearestNodeSetMeshComp)(const Pos3d &)= &XC::SetMeshComp::getNearestNode;
XC::Element *(XC::SetMeshComp::*getNearestElementSetMeshComp)(const Pos3d &)= &XC::SetMeshComp::getNearestElement;
void (XC::SetMeshComp::*transforms)(const XC::TrfGeom &)= &XC::SetMeshComp::Transforma;
class_<XC::SetMeshComp, bases<XC::SetBase>, boost::noncopyable >("SetMeshComp",no_init)
  .add_property("getNodes", make_function(GetNodosRef, return_internal_reference<>() ))
  .add_property("getElements", make_function(getElementsRef, return_internal_reference<>() ))
  .add_property("getConstraints", make_function(GetConstraintsRef, return_internal_reference<>() ))
  .def("getNearestNode",make_function(getNearestNodeSetMeshComp, return_internal_reference<>() ),"Returns nearest node.")
  .def("getNearestElement",make_function(getNearestElementSetMeshComp, return_internal_reference<>() ),"Returns nearest element.")
  .def("killElements",&XC::SetMeshComp::kill_elements,"Deactivates set's elements.")
  .def("aliveElements",&XC::SetMeshComp::alive_elements,"Activates set's elements.")
  .def("getNumDeadElements",&XC::SetMeshComp::getNumDeadElements,"Number of inactive elements.")
  .def("getNumLiveElements",&XC::SetMeshComp::getNumLiveElements,"Number of active elements.")
  .def("getNumDeadNodes",&XC::SetMeshComp::getNumDeadNodes,"Number of inactive nodes.")
  .def("getNumLiveNodes",&XC::SetMeshComp::getNumLiveNodes,"Number of active nodes.")
  .def("transforms",transforms,"Apply transformation to set members.")
  .def("getResistingSVD3d",&XC::SetMeshComp::getResistingSVD3d)
  .def("appendFromGeomEntity", &XC::SetMeshComp::appendFromGeomEntity,"Extend this set with the nodes and elements of the geometric entity being passed as parameter.")
  .def("clear",&XC::SetMeshComp::clear,"Removes all items.")
   ;

typedef XC::DqPtrs<XC::Pnt> dq_ptrs_pnt;
class_<dq_ptrs_pnt, bases<EntCmd>, boost::noncopyable >("dq_ptrs_pnt",no_init)
  .def("__iter__", range<return_internal_reference<> >(&dq_ptrs_pnt::indBegin, &dq_ptrs_pnt::indEnd))
  .def("at",make_function(&dq_ptrs_pnt::get, return_internal_reference<>() ), "Access specified point with bounds checking.")
  .def("clear",&dq_ptrs_pnt::clear,"Removes all items.")
   ;

class_<XC::Set::lst_ptr_points, bases<dq_ptrs_pnt>, boost::noncopyable >("lstPnts",no_init)
  .def("append", &XC::Set::lst_ptr_points::push_back,"Appends a point at the end of the list.")
  .def("pushFront", &XC::Set::lst_ptr_points::push_front,"Push point at the beginning of the list.")
  .add_property("size", &XC::Set::lst_ptr_points::size, "Returns list size.")
  .def("__len__",&XC::Set::lst_ptr_points::size, "Returns list size.")
   ;

typedef XC::DqPtrs<XC::Edge> dq_ptrs_lineas;
class_<dq_ptrs_lineas, bases<EntCmd>, boost::noncopyable >("dq_ptrs_lineas",no_init)
  .def("__iter__", range<return_internal_reference<> >(&dq_ptrs_lineas::indBegin, &dq_ptrs_lineas::indEnd))
  .def("at",make_function(&dq_ptrs_lineas::get, return_internal_reference<>() ), "Access specified line with bounds checking.")
  .def("clear",&dq_ptrs_lineas::clear,"Removes all items.")
   ;

class_<XC::Set::lst_ptr_lineas, bases<dq_ptrs_lineas>, boost::noncopyable >("lstLines",no_init)
  .def("append", &XC::Set::lst_ptr_lineas::push_back,"Appends line at the end of the list.")
  .def("pushFront", &XC::Set::lst_ptr_lineas::push_front,"Push line at the beginning of the list.")
  .add_property("size", &XC::Set::lst_ptr_lineas::size, "Returns list size.")
  .def("__len__",&XC::Set::lst_ptr_lineas::size, "Returns list size.")
   ;

typedef XC::DqPtrs<XC::Face> dq_ptrs_surfaces;
class_<dq_ptrs_surfaces, bases<EntCmd>, boost::noncopyable >("dq_ptrs_surfaces",no_init)
  .def("__iter__", range<return_internal_reference<> >(&dq_ptrs_surfaces::indBegin, &dq_ptrs_surfaces::indEnd))
  .def("at",make_function(&dq_ptrs_surfaces::get, return_internal_reference<>() ), "Access specified surface with bounds checking.")
  .def("clear",&dq_ptrs_surfaces::clear,"Removes all items.")
   ;

class_<XC::Set::lst_surface_ptrs, bases<dq_ptrs_surfaces>, boost::noncopyable >("lstSurfaces",no_init)
  .def("append", &XC::Set::lst_surface_ptrs::push_back,"Appends surface at the end of the list.")
  .def("pushFront", &XC::Set::lst_surface_ptrs::push_front,"Push surface at the beginning of the list.")
  .add_property("size", &XC::Set::lst_surface_ptrs::size, "Returns list size.")
  .def("__len__",&XC::Set::lst_surface_ptrs::size, "Returns list size.")
   ;

typedef XC::DqPtrs<XC::Body> dq_ptrs_cuerpos;
class_<dq_ptrs_cuerpos, bases<EntCmd>, boost::noncopyable >("dq_ptrs_cuerpos",no_init)
  .def("__iter__", range<return_internal_reference<> >(&dq_ptrs_cuerpos::indBegin, &dq_ptrs_cuerpos::indEnd))
  .def("at",make_function(&dq_ptrs_cuerpos::get, return_internal_reference<>() ), "Access specified body with bounds checking.")
  .def("clear",&dq_ptrs_cuerpos::clear,"Removes all items.")
   ;

class_<XC::Set::lst_ptr_cuerpos, bases<dq_ptrs_cuerpos>, boost::noncopyable >("lstBodies",no_init)
  .def("append", &XC::Set::lst_ptr_cuerpos::push_back,"Appends body at the end of the list.")
  .def("pushFront", &XC::Set::lst_ptr_cuerpos::push_front,"Push body at the beginning of the list.")
  .add_property("size", &XC::Set::lst_ptr_cuerpos::size, "Returns list size.")
  .def("__len__",&XC::Set::lst_ptr_cuerpos::size, "Returns list size.")
   ;


XC::Set::lst_ptr_points &(XC::Set::*GetPuntos)(void)= &XC::Set::GetPuntos;
XC::Set::lst_ptr_lineas &(XC::Set::*GetLineas)(void)= &XC::Set::GetLineas;
XC::Set::lst_surface_ptrs &(XC::Set::*getSurfaces)(void)= &XC::Set::getSurfaces;
XC::Set::lst_ptr_cuerpos &(XC::Set::*GetCuerpos)(void)= &XC::Set::GetCuerpos;
class_<XC::Set, bases<XC::SetMeshComp> >("Set")
  .add_property("getPoints", make_function(GetPuntos, return_internal_reference<>() ))
  .add_property("getLines", make_function(GetLineas, return_internal_reference<>() ))
  .add_property("getSurfaces", make_function(getSurfaces, return_internal_reference<>() ))
  .add_property("getBodies", make_function(GetCuerpos, return_internal_reference<>() ))
  .def("append", &XC::Set::extend_lists,"DEPRECATED use extend; extend the components whith those of the argument.")
  .def("extend", &XC::Set::extend_lists,"Extend the components whith those of the argument.")
  .def("fillUpwards", &XC::Set::CompletaHaciaArriba,"add entities upwards.")
  .def("fillDownwards", &XC::Set::CompletaHaciaAbajo,"add entities downwards.")
  .def("numerate", &XC::Set::numera,"Numerate entities (VTK).")
  .def("clear",&XC::Set::clear,"Removes all items.")
   ;

typedef XC::SetFila<XC::TritrizPtrNod::var_ref_fila_i,XC::TritrizPtrElem::var_ref_fila_i> set_fila_i;
class_<set_fila_i, bases<XC::SetEstruct>, boost::noncopyable >("set_fila_i", no_init);
class_<XC::SetFilaI, bases<set_fila_i>, boost::noncopyable >("SetFilaI", no_init);

typedef XC::SetFila<XC::TritrizPtrNod::var_ref_fila_j,XC::TritrizPtrElem::var_ref_fila_j> set_fila_j;
class_<set_fila_j, bases<XC::SetEstruct>, boost::noncopyable >("set_fila_j", no_init);
class_<XC::SetFilaJ, bases<set_fila_j>, boost::noncopyable >("SetFilaJ", no_init);

typedef XC::SetFila<XC::TritrizPtrNod::var_ref_fila_k,XC::TritrizPtrElem::var_ref_fila_k> set_fila_k;
class_<set_fila_k, bases<XC::SetEstruct>, boost::noncopyable >("set_fila_k", no_init);
class_<XC::SetFilaK, bases<set_fila_k>, boost::noncopyable >("SetFilaK", no_init);



