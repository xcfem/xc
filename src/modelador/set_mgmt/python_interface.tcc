//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//python_interface.tcc

typedef XC::DqPtrs<XC::Node> dq_ptrs_node;
class_<dq_ptrs_node, boost::noncopyable >("dq_ptrs_node",no_init)
  .def("__iter__", range<return_internal_reference<> >(&dq_ptrs_node::indBegin, &dq_ptrs_node::indEnd))
  .add_property("size", &dq_ptrs_node::size)
  .def("__len__",&dq_ptrs_node::size)
  .def("getTags",make_function(&dq_ptrs_node::getTags, return_internal_reference<>() ),"Returns node identifiers.")
  .def("clear",&dq_ptrs_node::clear,"Removes all items.")
  ;

XC::Node *(XC::DqPtrsNode::*getNearestNodeDqPtrs)(const Pos3d &)= &XC::DqPtrsNode::getNearestNode;
class_<XC::DqPtrsNode, bases<dq_ptrs_node>, boost::noncopyable >("DqPtrsNode",no_init)
  .def("append", &XC::DqPtrsNode::push_back,"Appends node at last position.")
  .def("pushFront", &XC::DqPtrsNode::push_front,"Push node at first position.")
  .add_property("getNumLiveNodes", &XC::DqPtrsNode::getNumLiveNodes)
  .add_property("getNumDeadNodes", &XC::DqPtrsNode::getNumDeadNodes)
  .def("getNearestNode",make_function(getNearestNodeDqPtrs, return_internal_reference<>() ),"Returns nearest node.")
   ;

typedef XC::DqPtrs<XC::Element> dq_ptrs_element;
class_<dq_ptrs_element, boost::noncopyable >("dq_ptrs_element",no_init)
  .def("__iter__", range<return_internal_reference<> >(&dq_ptrs_element::indBegin, &dq_ptrs_element::indEnd))
  .add_property("size", &dq_ptrs_element::size)
  .def("__len__",&dq_ptrs_element::size)
  .def("getTags",make_function(&dq_ptrs_element::getTags, return_internal_reference<>() ),"Returns element identifiers.")
  .def("clear",&dq_ptrs_element::clear,"Removes all items.")
  ;

XC::Element *(XC::DqPtrsElem::*getNearestElementDqPtrs)(const Pos3d &)= &XC::DqPtrsElem::getNearestElement;
class_<XC::DqPtrsElem, bases<dq_ptrs_element>, boost::noncopyable >("DqPtrsElem",no_init)
  .def("append", &XC::DqPtrsElem::push_back,"Appends element at last position.")
  .def("pushFront", &XC::DqPtrsElem::push_front,"Push element at first position.")
  .add_property("getNumLiveElements", &XC::DqPtrsElem::getNumLiveElements)
  .add_property("getNumDeadElements", &XC::DqPtrsElem::getNumDeadElements)
  .def("getNearestElement",make_function(getNearestElementDqPtrs, return_internal_reference<>() ),"Returns nearest element.")
   ;

typedef XC::DqPtrs<XC::Constraint> dq_ptrs_constraint;
class_<dq_ptrs_constraint, boost::noncopyable >("dq_ptrs_constraint",no_init)
  .def("__iter__", range<return_internal_reference<> >(&dq_ptrs_constraint::indBegin, &dq_ptrs_constraint::indEnd))
  .add_property("size", &dq_ptrs_constraint::size)
  .def("__len__",&dq_ptrs_constraint::size)
  .def("getTags",make_function(&dq_ptrs_constraint::getTags, return_internal_reference<>() ),"Returns constraint identifiers.")
  .def("clear",&dq_ptrs_constraint::clear,"Removes all items.")
  ;

class_<XC::DqPtrsConstraint, bases<dq_ptrs_constraint>, boost::noncopyable >("DqPtrsConstraint",no_init)
  .def("append", &XC::DqPtrsConstraint::push_back,"Appends constraint at last position.")
  .def("pushFront", &XC::DqPtrsConstraint::push_front,"Push constraint at first position.")
   ;

XC::DqPtrsNode &(XC::SetMeshComp::*GetNodosRef)(void)= &XC::SetMeshComp::GetNodos;
XC::DqPtrsElem &(XC::SetMeshComp::*GetElementosRef)(void)= &XC::SetMeshComp::GetElementos;
XC::DqPtrsConstraint &(XC::SetMeshComp::*GetConstraintsRef)(void)= &XC::SetMeshComp::GetConstraints;
XC::Node *(XC::SetMeshComp::*getNearestNodeSetMeshComp)(const Pos3d &)= &XC::SetMeshComp::getNearestNode;
XC::Element *(XC::SetMeshComp::*getNearestElementSetMeshComp)(const Pos3d &)= &XC::SetMeshComp::getNearestElement;
void (XC::SetMeshComp::*transforms)(const XC::TrfGeom &)= &XC::SetMeshComp::Transforma;
class_<XC::SetMeshComp, bases<XC::SetBase>, boost::noncopyable >("SetMeshComp",no_init)
  .add_property("getNodes", make_function(GetNodosRef, return_internal_reference<>() ))
  .add_property("getElements", make_function(GetElementosRef, return_internal_reference<>() ))
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
  .def("clear",&XC::SetMeshComp::clear,"Removes all items.")
   ;

typedef XC::DqPtrs<XC::Pnt> dq_ptrs_pnt;
class_<dq_ptrs_pnt, boost::noncopyable >("dq_ptrs_pnt",no_init)
  .def("__iter__", range<return_internal_reference<> >(&dq_ptrs_pnt::indBegin, &dq_ptrs_pnt::indEnd))
  .def("clear",&dq_ptrs_pnt::clear,"Removes all items.")
   ;

class_<XC::Set::lst_ptr_puntos, bases<dq_ptrs_pnt>, boost::noncopyable >("lstPnts",no_init)
  .def("append", &XC::Set::lst_ptr_puntos::push_back,"Appends node at last position.")
  .def("pushFront", &XC::Set::lst_ptr_puntos::push_front,"Push node at first position.")
  .add_property("size", &XC::Set::lst_ptr_puntos::size)
  .def("__len__",&XC::Set::lst_ptr_puntos::size)
   ;

typedef XC::DqPtrs<XC::Edge> dq_ptrs_lineas;
class_<dq_ptrs_lineas, boost::noncopyable >("dq_ptrs_lineas",no_init)
  .def("__iter__", range<return_internal_reference<> >(&dq_ptrs_lineas::indBegin, &dq_ptrs_lineas::indEnd))
  .def("clear",&dq_ptrs_lineas::clear,"Removes all items.")
   ;

class_<XC::Set::lst_ptr_lineas, bases<dq_ptrs_lineas>, boost::noncopyable >("lstLines",no_init)
  .def("append", &XC::Set::lst_ptr_lineas::push_back,"Appends node at last position.")
  .def("pushFront", &XC::Set::lst_ptr_lineas::push_front,"Push node at first position.")
  .add_property("size", &XC::Set::lst_ptr_lineas::size)
  .def("__len__",&XC::Set::lst_ptr_lineas::size)
   ;

typedef XC::DqPtrs<XC::Face> dq_ptrs_superficies;
class_<dq_ptrs_superficies, boost::noncopyable >("dq_ptrs_superficies",no_init)
  .def("__iter__", range<return_internal_reference<> >(&dq_ptrs_superficies::indBegin, &dq_ptrs_superficies::indEnd))
  .def("clear",&dq_ptrs_superficies::clear,"Removes all items.")
   ;

class_<XC::Set::lst_ptr_superficies, bases<dq_ptrs_superficies>, boost::noncopyable >("lstSurfaces",no_init)
  .def("append", &XC::Set::lst_ptr_superficies::push_back,"Appends node at last position.")
  .def("pushFront", &XC::Set::lst_ptr_superficies::push_front,"Push node at first position.")
  .add_property("size", &XC::Set::lst_ptr_superficies::size)
  .def("__len__",&XC::Set::lst_ptr_superficies::size)
   ;

typedef XC::DqPtrs<XC::Body> dq_ptrs_cuerpos;
class_<dq_ptrs_cuerpos, boost::noncopyable >("dq_ptrs_cuerpos",no_init)
  .def("__iter__", range<return_internal_reference<> >(&dq_ptrs_cuerpos::indBegin, &dq_ptrs_cuerpos::indEnd))
  .def("clear",&dq_ptrs_cuerpos::clear,"Removes all items.")
   ;

class_<XC::Set::lst_ptr_cuerpos, bases<dq_ptrs_cuerpos>, boost::noncopyable >("lstBodies",no_init)
  .def("append", &XC::Set::lst_ptr_cuerpos::push_back,"Appends node at last position.")
  .def("pushFront", &XC::Set::lst_ptr_cuerpos::push_front,"Push node at first position.")
  .add_property("size", &XC::Set::lst_ptr_cuerpos::size)
  .def("__len__",&XC::Set::lst_ptr_cuerpos::size)
   ;


XC::Set::lst_ptr_puntos &(XC::Set::*GetPuntos)(void)= &XC::Set::GetPuntos;
XC::Set::lst_ptr_lineas &(XC::Set::*GetLineas)(void)= &XC::Set::GetLineas;
XC::Set::lst_ptr_superficies &(XC::Set::*GetSuperficies)(void)= &XC::Set::GetSuperficies;
XC::Set::lst_ptr_cuerpos &(XC::Set::*GetCuerpos)(void)= &XC::Set::GetCuerpos;
class_<XC::Set, bases<XC::SetMeshComp> >("Set")
  .add_property("getPoints", make_function(GetPuntos, return_internal_reference<>() ))
  .add_property("getLines", make_function(GetLineas, return_internal_reference<>() ))
  .add_property("getSurfaces", make_function(GetSuperficies, return_internal_reference<>() ))
  .add_property("getBodies", make_function(GetCuerpos, return_internal_reference<>() ))
  .def("append", &XC::Set::agrega_listas,"Append components to this set.")
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



