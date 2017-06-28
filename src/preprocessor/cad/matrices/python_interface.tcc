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

//Point pointers arrays

typedef MatrizT<XC::Pnt *,std::vector<XC::Pnt *> > matriz_t_ptr_puntos;
matriz_t_ptr_puntos::reference (matriz_t_ptr_puntos::*getPnt)(size_t,size_t)= &matriz_t_ptr_puntos::at;
class_<matriz_t_ptr_puntos, bases<ProtoMatriz>, boost::noncopyable >("matriz_t_ptr_puntos", no_init)
  .def("getPoint", make_function( getPnt, return_internal_reference<>() ))  ;

typedef XC::MatrizPtrBase<XC::Pnt> matriz_ptr_puntos;
class_<matriz_ptr_puntos, bases<matriz_t_ptr_puntos,EntCmd>, boost::noncopyable >("matriz_ptr_puntos", no_init)
  .def("dim",&matriz_ptr_puntos::dim,"assings matrix dimensions.")
  .add_property("nRow",&matriz_ptr_puntos::getNumFilas,"returns number of rows.")
  .add_property("nCol",&matriz_ptr_puntos::getNumCols,"returns number of columns.")
  ;

XC::Pnt *(XC::MatrizPtrPnt::*getNearestPntMatrizPtrPnt)(const Pos3d &)= &XC::MatrizPtrPnt::getNearestPnt;
XC::Pnt *(XC::MatrizPtrPnt::*getPntWithTagMatrizPtrPnt)(const size_t &)= &XC::MatrizPtrPnt::buscaPunto;
class_<XC::MatrizPtrPnt, bases<matriz_ptr_puntos>, boost::noncopyable >("MatrizPtrPnt", no_init)
  .def("getNearestPnt",make_function(getNearestPntMatrizPtrPnt, return_internal_reference<>() ),"Returns nearest point.")
  .def("getPntWithTag",make_function(getPntWithTagMatrizPtrPnt, return_internal_reference<>() ),"Returns point by tag.")
  .def("setPnt", &XC::MatrizPtrPnt::setPnt)
  .def("getPnt", &XC::MatrizPtrPnt::getPnt, return_internal_reference<>())
  .def("getCentroid",&XC::MatrizPtrPnt::getCentroide)
  ;

typedef std::vector<XC::MatrizPtrPnt> vector_mp_puntos;
XC::MatrizPtrPnt &(vector_mp_puntos::*getCapaPuntos)(size_t )= &vector_mp_puntos::at;
class_<vector_mp_puntos, boost::noncopyable >("vector_mp_puntos", no_init)
  .def(vector_indexing_suite<vector_mp_puntos>() )
  .def("getLayer", getCapaPuntos, return_internal_reference<>(), "returns point layer." )
  ;

typedef XC::TritrizPtrBase<XC::MatrizPtrPnt> tritriz_puntos;
class_<tritriz_puntos, bases<vector_mp_puntos,EntCmd>, boost::noncopyable >("tritriz_puntos", no_init)
  .def("dim",&tritriz_puntos::dim,"assings dimensions.")
  .def("getAtIJK", &tritriz_puntos::getAtIJK, return_internal_reference<>(),"Returns reference to value at (i,j,k) position.")
  ;

XC::Pnt *(XC::TritrizPtrPnt::*getNearestPntTritrizPtrPnt)(const Pos3d &)= &XC::TritrizPtrPnt::getNearestPnt;
XC::Pnt *(XC::TritrizPtrPnt::*getPntWithTagTritrizPtrPnt)(const int &)= &XC::TritrizPtrPnt::buscaPunto;
class_<XC::TritrizPtrPnt, bases<tritriz_puntos>, boost::noncopyable >("TritrizPtrPnt", no_init)
  .def("getNearestPnt",make_function(getNearestPntTritrizPtrPnt, return_internal_reference<>() ),"Returns nearest point.")
  .def("getPntWithTag",make_function(getPntWithTagTritrizPtrPnt, return_internal_reference<>() ),"Returns point by tag.")
  .def("setPnt", &XC::TritrizPtrPnt::setPnt)
  .def("getPnt", &XC::TritrizPtrPnt::getPnt, return_internal_reference<>())
  .def("getCentroid",&XC::TritrizPtrPnt::getCentroide)
  ;

//Node pointers arrays

typedef MatrizT<XC::Node *,std::vector<XC::Node *> > matriz_t_ptr_nodes;
matriz_t_ptr_nodes::reference (matriz_t_ptr_nodes::*getNod)(size_t,size_t)= &matriz_t_ptr_nodes::at;
class_<matriz_t_ptr_nodes, bases<ProtoMatriz>, boost::noncopyable >("matriz_t_ptr_nodes", no_init)
  ;

typedef XC::MatrizPtrBase<XC::Node> matriz_ptr_nodes;
class_<matriz_ptr_nodes, bases<matriz_t_ptr_nodes,EntCmd>, boost::noncopyable >("matriz_ptr_nodes", no_init)
  .def("getTagsInteriorNodes",&matriz_ptr_nodes::getTagsInteriorObjs,"return interior node tags.")
  .add_property("nRow",&matriz_ptr_nodes::getNumFilas,"returns number of rows.")
  .add_property("nCol",&matriz_ptr_nodes::getNumCols,"returns number of columns.")
  .def("dim",&matriz_ptr_nodes::dim,"assings matrix dimensions.")
  ;

XC::Node *(XC::MatrizPtrNod::*getNearestNodMatrizPtrNod)(const Pos3d &)= &XC::MatrizPtrNod::getNearestNode;
XC::Node *(XC::MatrizPtrNod::*getNodWithTagMatrizPtrNod)(const int &)= &XC::MatrizPtrNod::buscaNodo;
class_<XC::MatrizPtrNod, bases<matriz_ptr_nodes>, boost::noncopyable >("MatrizPtrNod", no_init)
  .def("getNode", getNod, return_internal_reference<>())
  .def("getNearestNode",make_function(getNearestNodMatrizPtrNod, return_internal_reference<>() ),"Returns nearest node.")
  .def("getNodeWithTag",make_function(getNodWithTagMatrizPtrNod, return_internal_reference<>() ),"Returns node by tag.")
  ;


typedef std::vector<XC::MatrizPtrNod> vector_mp_nodes;
XC::MatrizPtrNod &(vector_mp_nodes::*getCapaNodos)(size_t )= &vector_mp_nodes::at;
class_<vector_mp_nodes, boost::noncopyable >("vector_mp_nodes", no_init)
  .def(vector_indexing_suite<vector_mp_nodes>() )
  .def("getLayer", getCapaNodos, return_internal_reference<>(), "returns node layer." )
  ;

typedef XC::TritrizPtrBase<XC::MatrizPtrNod> tritriz_nodes;
class_<tritriz_nodes, bases<vector_mp_nodes,EntCmd>, boost::noncopyable >("tritriz_nodes", no_init)
  .def("dim",&tritriz_nodes::dim,"assings dimensions.")
  .def("getAtIJK", &tritriz_nodes::getAtIJK, return_internal_reference<>(),"Returns reference to value at (i,j,k) position.")
  ;

XC::Node *(XC::TritrizPtrNod::*getNearestNodeTritrizPtrNod)(const Pos3d &)= &XC::TritrizPtrNod::getNearestNode;
XC::Node *(XC::TritrizPtrNod::*getNodeWithTagTritrizPtrNod)(const int &)= &XC::TritrizPtrNod::buscaNodo;
class_<XC::TritrizPtrNod, bases<tritriz_nodes>, boost::noncopyable >("TritrizPtrNod", no_init)
  .def("getNearestNode",make_function(getNearestNodeTritrizPtrNod, return_internal_reference<>() ),"Returns nearest node.")
  .def("getNodeWithTag",make_function(getNodeWithTagTritrizPtrNod, return_internal_reference<>() ),"Returns node by tag.")
  ;

//Element pointers arrays

typedef MatrizT<XC::Element *,std::vector<XC::Element> > matriz_t_ptr_elements;
matriz_t_ptr_elements::reference (matriz_t_ptr_elements::*getElem)(size_t,size_t)= &matriz_t_ptr_elements::at;
class_<matriz_t_ptr_elements, bases<ProtoMatriz>, boost::noncopyable >("matriz_t_ptr_elements", no_init)
  .add_property("getElement", make_function( getElem, return_internal_reference<>() ))  ;

typedef XC::MatrizPtrBase<XC::Element *> matriz_ptr_elements;
class_<matriz_ptr_elements, bases<matriz_t_ptr_elements,EntCmd>, boost::noncopyable >("matriz_ptr_elements", no_init)
  .add_property("nRow",&matriz_ptr_elements::getNumFilas,"returns number of rows.")
  .add_property("nCol",&matriz_ptr_elements::getNumCols,"returns number of columns.")
  .def("dim",&matriz_ptr_elements::dim,"assings matrix dimensions.")
  ;

XC::Element *(XC::MatrizPtrElem::*getNearestElementMatrizPtrElem)(const Pos3d &)= &XC::MatrizPtrElem::getNearestElement;
XC::Element *(XC::MatrizPtrElem::*getElementWithTag)(const int &)= &XC::MatrizPtrElem::findElement;
class_<XC::MatrizPtrElem, bases<matriz_ptr_elements>, boost::noncopyable >("MatrizPtrElem", no_init)
  .def("getNearestElement",make_function(getNearestElementMatrizPtrElem, return_internal_reference<>() ),"Returns nearest element.")
  .def("getElementWithTag",make_function(getElementWithTag, return_internal_reference<>() ),"Returns element by tag.")
  ;

typedef std::vector<XC::MatrizPtrElem> vector_mp_elements;
XC::MatrizPtrElem &(vector_mp_elements::*getElementLayer)(size_t )= &vector_mp_elements::at;
class_<vector_mp_elements, boost::noncopyable >("vector_mp_elements", no_init)
  .def(vector_indexing_suite<vector_mp_elements>() )
  .def("getLayer", getElementLayer, return_internal_reference<>(), "returns element layer.")
  ;

typedef XC::TritrizPtrBase<XC::MatrizPtrElem> tritriz_elements;
class_<tritriz_elements, bases<vector_mp_elements,EntCmd>, boost::noncopyable >("tritriz_elements", no_init)
  .def("getTagsElementsCapa",&tritriz_elements::getTagsObjsCapa,"return layer elements tags.")
  .def("dim",&tritriz_elements::dim,"assings dimensions.")
  .def("getAtIJK", &tritriz_elements::getAtIJK, return_internal_reference<>(),"Returns reference to value at (i,j,k) position.")
  ;

XC::Element *(XC::TritrizPtrElem::*getNearestElementTritrizPtrElem)(const Pos3d &)= &XC::TritrizPtrElem::getNearestElement;
XC::Element *(XC::TritrizPtrElem::*getElementWithTagTritrizPtrElem)(const int &)= &XC::TritrizPtrElem::findElement;
class_<XC::TritrizPtrElem, bases<tritriz_elements>, boost::noncopyable >("TritrizPtrElem", no_init)
  .def("getNearestElement",make_function(getNearestElementTritrizPtrElem, return_internal_reference<>() ),"Returns nearest element.")
  .def("getElementWithTag",make_function(getElementWithTagTritrizPtrElem, return_internal_reference<>() ),"Returns element by tag.")
  ;


//schemes 

typedef XC::MapCadMember<XC::MatrizPtrPnt> map_cm_m_puntos;
class_<map_cm_m_puntos, bases<XC::MapCadMemberBase>, boost::noncopyable >("map_cm_m_puntos", no_init);

class_<XC::MapEsquemas2d, bases<map_cm_m_puntos>, boost::noncopyable >("MapEsquemas2d", no_init)
  .def("new2DNet",make_function(&XC::MapEsquemas2d::makeNew, return_internal_reference<>() ),"Creates a 2D net.")
   ;

typedef XC::MapCadMember<XC::TritrizPtrPnt> map_cm_t_puntos;
class_<map_cm_t_puntos, bases<XC::MapCadMemberBase>, boost::noncopyable >("map_cm_t_puntos", no_init);

class_<XC::MapEsquemas3d, bases<map_cm_t_puntos>, boost::noncopyable >("MapEsquemas3d", no_init)
  .def("new3DNet",make_function(&XC::MapEsquemas3d::makeNew, return_internal_reference<>() ),"Creates a 3D net.")
   ;
