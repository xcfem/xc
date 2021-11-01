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

typedef TMatrix<XC::Pnt *,std::vector<XC::Pnt *> > t_matrix_point_ptr;
t_matrix_point_ptr::reference (t_matrix_point_ptr::*getPnt)(size_t,size_t)= &t_matrix_point_ptr::at;
class_<t_matrix_point_ptr, bases<ProtoMatrix>, boost::noncopyable >("t_matrix_point_ptr", no_init)
  .def("getPoint", make_function( getPnt, return_internal_reference<>() ))  ;

typedef XC::PtrArrayBase<XC::Pnt> point_ptr_matrix;
class_<point_ptr_matrix, bases<t_matrix_point_ptr,CommandEntity>, boost::noncopyable >("point_ptr_matrix", no_init)
  .def("dim",&point_ptr_matrix::dim,"assigns matrix dimensions.")
  .add_property("nRow",&point_ptr_matrix::getNumberOfRows,"returns number of rows.")
  .add_property("nCol",&point_ptr_matrix::getNumberOfColumns,"returns number of columns.")
  ;

XC::Pnt *(XC::PntPtrArray::*getNearestPntPntPtrArray)(const Pos3d &)= &XC::PntPtrArray::getNearestPnt;
XC::Pnt *(XC::PntPtrArray::*getPntWithTagPntPtrArray)(const size_t &)= &XC::PntPtrArray::findPoint;
class_<XC::PntPtrArray, bases<point_ptr_matrix>, boost::noncopyable >("PntPtrArray", no_init)
  .def("getNearestPnt",make_function(getNearestPntPntPtrArray, return_internal_reference<>() ),"Returns nearest point.")
  .def("getPntWithTag",make_function(getPntWithTagPntPtrArray, return_internal_reference<>() ),"Returns point by tag.")
  .def("setPnt", &XC::PntPtrArray::setPnt)
  .def("getPnt", &XC::PntPtrArray::getPnt, return_internal_reference<>())
  .def("getCentroid",&XC::PntPtrArray::getCentroid)
  ;

typedef std::vector<XC::PntPtrArray> vector_mp_points;
XC::PntPtrArray &(vector_mp_points::*getPointLayer)(size_t )= &vector_mp_points::at;
class_<vector_mp_points, boost::noncopyable >("vector_mp_points", no_init)
  .def(vector_indexing_suite<vector_mp_points>() )
  .def("getLayer", getPointLayer, return_internal_reference<>(), "returns point layer." )
  ;

typedef XC::PtrArray3dBase<XC::PntPtrArray> array3d_points;
class_<array3d_points, bases<vector_mp_points,CommandEntity>, boost::noncopyable >("array3d_points", no_init)
  .def("dim",&array3d_points::dim,"assigns dimensions.")
  .def("getAtIJK", &array3d_points::getAtIJK, return_internal_reference<>(),"Returns reference to value at (i,j,k) position.")
  ;

XC::Pnt *(XC::PntPtrArray3d::*getNearestPntPntPtrArray3d)(const Pos3d &)= &XC::PntPtrArray3d::getNearestPnt;
XC::Pnt *(XC::PntPtrArray3d::*getPntWithTagPntPtrArray3d)(const int &)= &XC::PntPtrArray3d::findPoint;
class_<XC::PntPtrArray3d, bases<array3d_points>, boost::noncopyable >("PntPtrArray3d", no_init)
  .def("getNearestPnt",make_function(getNearestPntPntPtrArray3d, return_internal_reference<>() ),"Returns nearest point.")
  .def("getPntWithTag",make_function(getPntWithTagPntPtrArray3d, return_internal_reference<>() ),"Returns point by tag.")
  .def("setPnt", &XC::PntPtrArray3d::setPnt)
  .def("getPnt", &XC::PntPtrArray3d::getPnt, return_internal_reference<>())
  .def("getCentroid",&XC::PntPtrArray3d::getCentroid)
  ;

//Node pointers arrays

typedef TMatrix<XC::Node *,std::vector<XC::Node *> > t_matrix_ptr_nodes;
t_matrix_ptr_nodes::reference (t_matrix_ptr_nodes::*getNod)(size_t,size_t)= &t_matrix_ptr_nodes::at;
class_<t_matrix_ptr_nodes, bases<ProtoMatrix>, boost::noncopyable >("t_matrix_ptr_nodes", no_init)
  ;

typedef XC::PtrArrayBase<XC::Node> ptr_nodes_matrix;
class_<ptr_nodes_matrix, bases<t_matrix_ptr_nodes,CommandEntity>, boost::noncopyable >("ptr_nodes_matrix", no_init)
  .def("getTagsInteriorNodes",&ptr_nodes_matrix::getTagsInteriorObjs,"return interior node tags.")
  .add_property("nRow",&ptr_nodes_matrix::getNumberOfRows,"returns number of rows.")
  .add_property("nCol",&ptr_nodes_matrix::getNumberOfColumns,"returns number of columns.")
  .def("dim",&ptr_nodes_matrix::dim,"assigns matrix dimensions.")
  ;

XC::Node *(XC::NodePtrArray::*getNearestNodNodePtrArray)(const Pos3d &)= &XC::NodePtrArray::getNearestNode;
XC::Node *(XC::NodePtrArray::*getNodWithTagNodePtrArray)(const int &)= &XC::NodePtrArray::findNode;
class_<XC::NodePtrArray, bases<ptr_nodes_matrix>, boost::noncopyable >("NodePtrArray", no_init)
  .add_property("nodes",&XC::NodePtrArray::getPyNodeList,"returns the nodes of the container in a python list.")
  .def("getNode", getNod, return_internal_reference<>(),"getNode(i,j) returns the node correponding to the indexes.")
  .def("getNearestNode",make_function(getNearestNodNodePtrArray, return_internal_reference<>() ),"Returns nearest node.")
  .def("getNodeWithTag",make_function(getNodWithTagNodePtrArray, return_internal_reference<>() ),"Returns node by tag.")
  ;


typedef std::vector<XC::NodePtrArray> vector_mp_nodes;
XC::NodePtrArray &(vector_mp_nodes::*getNodeLayer)(size_t )= &vector_mp_nodes::at;
class_<vector_mp_nodes, boost::noncopyable >("vector_mp_nodes", no_init)
  .def(vector_indexing_suite<vector_mp_nodes>() )
  .def("getLayer", getNodeLayer, return_internal_reference<>(), "returns node layer." )
  ;

typedef XC::PtrArray3dBase<XC::NodePtrArray> array3d_nodes;
class_<array3d_nodes, bases<vector_mp_nodes,CommandEntity>, boost::noncopyable >("array3d_nodes", no_init)
  .def("dim",&array3d_nodes::dim,"assigns dimensions.")
  .def("getAtIJK", &array3d_nodes::getAtIJK, return_internal_reference<>(),"Returns reference to value at (i,j,k) position.")
  ;

XC::Node *(XC::NodePtrArray3d::*getNearestNodeNodePtrArray3d)(const Pos3d &)= &XC::NodePtrArray3d::getNearestNode;
XC::Node *(XC::NodePtrArray3d::*getNodeWithTagNodePtrArray3d)(const int &)= &XC::NodePtrArray3d::findNode;
class_<XC::NodePtrArray3d, bases<array3d_nodes>, boost::noncopyable >("NodePtrArray3d", no_init)
  .add_property("nodes",&XC::NodePtrArray3d::getPyNodeList,"returns the nodes of the container in a python list.")
  .def("getNearestNode",make_function(getNearestNodeNodePtrArray3d, return_internal_reference<>() ),"Returns nearest node.")
  .def("getNodeWithTag",make_function(getNodeWithTagNodePtrArray3d, return_internal_reference<>() ),"Returns node by tag.")
  ;

//Element pointers arrays

typedef TMatrix<XC::Element *,std::vector<XC::Element> > t_matrix_ptr_elements;
t_matrix_ptr_elements::reference (t_matrix_ptr_elements::*getElem)(size_t,size_t)= &t_matrix_ptr_elements::at;
class_<t_matrix_ptr_elements, bases<ProtoMatrix>, boost::noncopyable >("t_matrix_ptr_elements", no_init)
  .add_property("getElement", make_function( getElem, return_internal_reference<>() ))  ;

typedef XC::PtrArrayBase<XC::Element *> ptr_elements_matrix;
class_<ptr_elements_matrix, bases<t_matrix_ptr_elements,CommandEntity>, boost::noncopyable >("ptr_elements_matrix", no_init)
  .add_property("nRow",&ptr_elements_matrix::getNumberOfRows,"returns number of rows.")
  .add_property("nCol",&ptr_elements_matrix::getNumberOfColumns,"returns number of columns.")
  .def("dim",&ptr_elements_matrix::dim,"assigns matrix dimensions.")
  ;

XC::Element *(XC::ElemPtrArray::*getNearestElementElemPtrArray)(const Pos3d &)= &XC::ElemPtrArray::getNearestElement;
XC::Element *(XC::ElemPtrArray::*getElementWithTag)(const int &)= &XC::ElemPtrArray::findElement;
class_<XC::ElemPtrArray, bases<ptr_elements_matrix>, boost::noncopyable >("ElemPtrArray", no_init)
  .add_property("elements",&XC::ElemPtrArray::getPyElementList,"returns the elements of the container in a python list.")
  .def("getNearestElement",make_function(getNearestElementElemPtrArray, return_internal_reference<>() ),"Returns nearest element.")
  .def("getElementWithTag",make_function(getElementWithTag, return_internal_reference<>() ),"Returns element by tag.")
  ;

typedef std::vector<XC::ElemPtrArray> vector_mp_elements;
XC::ElemPtrArray &(vector_mp_elements::*getElementLayer)(size_t )= &vector_mp_elements::at;
class_<vector_mp_elements, boost::noncopyable >("vector_mp_elements", no_init)
  .def(vector_indexing_suite<vector_mp_elements>() )
  .def("getLayer", getElementLayer, return_internal_reference<>(), "returns element layer.")
  ;

typedef XC::PtrArray3dBase<XC::ElemPtrArray> array3d_elements;
class_<array3d_elements, bases<vector_mp_elements,CommandEntity>, boost::noncopyable >("array3d_elements", no_init)
  .def("getLayerElementsTags",&array3d_elements::getLayerObjectsTags,"return layer elements tags.")
  .def("dim",&array3d_elements::dim,"assigns dimensions.")
  .def("getAtIJK", &array3d_elements::getAtIJK, return_internal_reference<>(),"Returns reference to value at (i,j,k) position.")
  ;

XC::Element *(XC::ElemPtrArray3d::*getNearestElementElemPtrArray3d)(const Pos3d &)= &XC::ElemPtrArray3d::getNearestElement;
XC::Element *(XC::ElemPtrArray3d::*getElementWithTagElemPtrArray3d)(const int &)= &XC::ElemPtrArray3d::findElement;
class_<XC::ElemPtrArray3d, bases<array3d_elements>, boost::noncopyable >("ElemPtrArray3d", no_init)
  .add_property("elements",&XC::ElemPtrArray3d::getPyElementList,"returns the elements of the container in a python list.")
  .def("getNearestElement",make_function(getNearestElementElemPtrArray3d, return_internal_reference<>() ),"Returns nearest element.")
  .def("getElementWithTag",make_function(getElementWithTagElemPtrArray3d, return_internal_reference<>() ),"Returns element by tag.")
  ;


//Frameworks 

typedef XC::ModelComponentContainer<XC::PntPtrArray> map_cm_m_points;
class_<map_cm_m_points, bases<XC::ModelComponentContainerBase>, boost::noncopyable >("map_cm_m_points", no_init);

class_<XC::Framework2d, bases<map_cm_m_points>, boost::noncopyable >("Framework2d", no_init)
  .def("new2DNet",make_function(&XC::Framework2d::makeNew, return_internal_reference<>() ),"Creates a 2D net.")
   ;

typedef XC::ModelComponentContainer<XC::PntPtrArray3d> map_cm_t_points;
class_<map_cm_t_points, bases<XC::ModelComponentContainerBase>, boost::noncopyable >("map_cm_t_points", no_init);

class_<XC::Framework3d, bases<map_cm_t_points>, boost::noncopyable >("Framework3d", no_init)
  .def("new3DNet",make_function(&XC::Framework3d::makeNew, return_internal_reference<>() ),"Creates a 3D net.")
   ;
