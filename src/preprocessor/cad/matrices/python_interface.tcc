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
class_<XC::TritrizPtrPnt, bases<tritriz_puntos>, boost::noncopyable >("TrtrizPtrPnt", no_init)
  .def("getNearestPnt",make_function(getNearestPntTritrizPtrPnt, return_internal_reference<>() ),"Returns nearest point.")
  .def("getPntWithTag",make_function(getPntWithTagTritrizPtrPnt, return_internal_reference<>() ),"Returns point by tag.")
  .def("setPnt", &XC::TritrizPtrPnt::setPnt)
  .def("getPnt", &XC::TritrizPtrPnt::getPnt, return_internal_reference<>())
  .def("getCentroid",&XC::TritrizPtrPnt::getCentroide)
  ;

//Node pointers arrays

typedef MatrizT<XC::Node *,std::vector<XC::Node *> > matriz_t_ptr_nodos;
matriz_t_ptr_nodos::reference (matriz_t_ptr_nodos::*getNod)(size_t,size_t)= &matriz_t_ptr_nodos::at;
class_<matriz_t_ptr_nodos, bases<ProtoMatriz>, boost::noncopyable >("matriz_t_ptr_nodos", no_init)
  ;

typedef XC::MatrizPtrBase<XC::Node> matriz_ptr_nodos;
class_<matriz_ptr_nodos, bases<matriz_t_ptr_nodos,EntCmd>, boost::noncopyable >("matriz_ptr_nodos", no_init)
  .def("getTagsInteriorNodes",&matriz_ptr_nodos::getTagsInteriorObjs,"return interior node tags.")
  .add_property("nRow",&matriz_ptr_nodos::getNumFilas,"returns number of rows.")
  .add_property("nCol",&matriz_ptr_nodos::getNumCols,"returns number of columns.")
  .def("dim",&matriz_ptr_nodos::dim,"assings matrix dimensions.")
  ;

XC::Node *(XC::MatrizPtrNod::*getNearestNodMatrizPtrNod)(const Pos3d &)= &XC::MatrizPtrNod::getNearestNode;
XC::Node *(XC::MatrizPtrNod::*getNodWithTagMatrizPtrNod)(const int &)= &XC::MatrizPtrNod::buscaNodo;
class_<XC::MatrizPtrNod, bases<matriz_ptr_nodos>, boost::noncopyable >("MatrizPtrNod", no_init)
  .def("getNode", getNod, return_internal_reference<>())
  .def("getNearestNode",make_function(getNearestNodMatrizPtrNod, return_internal_reference<>() ),"Returns nearest node.")
  .def("getNodeWithTag",make_function(getNodWithTagMatrizPtrNod, return_internal_reference<>() ),"Returns node by tag.")
  ;


typedef std::vector<XC::MatrizPtrNod> vector_mp_nodos;
XC::MatrizPtrNod &(vector_mp_nodos::*getCapaNodos)(size_t )= &vector_mp_nodos::at;
class_<vector_mp_nodos, boost::noncopyable >("vector_mp_nodos", no_init)
  .def(vector_indexing_suite<vector_mp_nodos>() )
  .def("getLayer", getCapaNodos, return_internal_reference<>(), "returns node layer." )
  ;

typedef XC::TritrizPtrBase<XC::MatrizPtrNod> tritriz_nodos;
class_<tritriz_nodos, bases<vector_mp_nodos,EntCmd>, boost::noncopyable >("tritriz_nodos", no_init)
  .def("dim",&tritriz_nodos::dim,"assings dimensions.")
  .def("getAtIJK", &tritriz_nodos::getAtIJK, return_internal_reference<>(),"Returns reference to value at (i,j,k) position.")
  ;

XC::Node *(XC::TritrizPtrNod::*getNearestNodeTritrizPtrNod)(const Pos3d &)= &XC::TritrizPtrNod::getNearestNode;
XC::Node *(XC::TritrizPtrNod::*getNodeWithTagTritrizPtrNod)(const int &)= &XC::TritrizPtrNod::buscaNodo;
class_<XC::TritrizPtrNod, bases<tritriz_nodos>, boost::noncopyable >("TrtrizPtrNod", no_init)
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
XC::Element *(XC::MatrizPtrElem::*getElementWithTag)(const int &)= &XC::MatrizPtrElem::buscaElemento;
class_<XC::MatrizPtrElem, bases<matriz_ptr_elements>, boost::noncopyable >("MatrizPtrElem", no_init)
  .def("getNearestElement",make_function(getNearestElementMatrizPtrElem, return_internal_reference<>() ),"Returns nearest element.")
  .def("getElementWithTag",make_function(getElementWithTag, return_internal_reference<>() ),"Returns element by tag.")
  ;

typedef std::vector<XC::MatrizPtrElem> vector_mp_elementos;
XC::MatrizPtrElem &(vector_mp_elementos::*getCapaElementos)(size_t )= &vector_mp_elementos::at;
class_<vector_mp_elementos, boost::noncopyable >("vector_mp_elementos", no_init)
  .def(vector_indexing_suite<vector_mp_elementos>() )
  .def("getLayer", getCapaElementos, return_internal_reference<>(), "returns element layer.")
  ;

typedef XC::TritrizPtrBase<XC::MatrizPtrElem> tritriz_elementos;
class_<tritriz_elementos, bases<vector_mp_elementos,EntCmd>, boost::noncopyable >("tritriz_elementos", no_init)
  .def("getTagsElementsCapa",&tritriz_elementos::getTagsObjsCapa,"return layer elements tags.")
  .def("dim",&tritriz_elementos::dim,"assings dimensions.")
  .def("getAtIJK", &tritriz_elementos::getAtIJK, return_internal_reference<>(),"Returns reference to value at (i,j,k) position.")
  ;

XC::Element *(XC::TritrizPtrElem::*getNearestElementTritrizPtrElem)(const Pos3d &)= &XC::TritrizPtrElem::getNearestElement;
XC::Element *(XC::TritrizPtrElem::*getElementWithTagTritrizPtrElem)(const int &)= &XC::TritrizPtrElem::buscaElemento;
class_<XC::TritrizPtrElem, bases<tritriz_elementos>, boost::noncopyable >("TrtrizPtrElem", no_init)
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

class_<XC::MapEsquemas3d, bases<map_cm_t_puntos>, boost::noncopyable >("MapEsquemas2d", no_init)
  .def("new3DNet",make_function(&XC::MapEsquemas3d::makeNew, return_internal_reference<>() ),"Creates a 3D net.")
   ;
