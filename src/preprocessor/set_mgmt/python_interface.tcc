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

typedef XC::DqPtrs<XC::Node> dq_ptrs_node;
class_<dq_ptrs_node, bases<CommandEntity>, boost::noncopyable >("dq_ptrs_node",no_init)
  .def("__iter__", range<return_internal_reference<> >(&dq_ptrs_node::indBegin, &dq_ptrs_node::indEnd))
  .add_property("size", &dq_ptrs_node::size, "Returns list size.")
  .def("__len__",&dq_ptrs_node::size, "Returns list size.")
  .def("at",make_function(&dq_ptrs_node::get, return_internal_reference<>() ), "Access specified node with bounds checking.")
  .def("__getitem__",make_function(&dq_ptrs_node::get, return_internal_reference<>() ), "Access specified node with bounds checking.")
  .def("getTags",make_function(&dq_ptrs_node::getTags, return_internal_reference<>() ),"Returns node identifiers.")
  .def("findTag",make_function(&dq_ptrs_node::findTag, return_internal_reference<>() ),"Returns the node identified by the tag argument.")
  .def("clear",&dq_ptrs_node::clear,"Removes all items.")
  ;

XC::Node *(XC::DqPtrsNode::*getNearestNodeDqPtrs)(const Pos3d &)= &XC::DqPtrsNode::getNearest;
class_<XC::DqPtrsNode, bases<dq_ptrs_node> >("DqPtrsNode",no_init)
  .def("append", &XC::DqPtrsNode::push_back,"Appends node at the end of the list.")
  .def("pushFront", &XC::DqPtrsNode::push_front,"Push node at the beginning of the list.")
  .add_property("getNumLiveNodes", &XC::DqPtrsNode::getNumLiveNodes)
  .add_property("getNumDeadNodes", &XC::DqPtrsNode::getNumDeadNodes)
  .def("getNearestNode",make_function(getNearestNodeDqPtrs, return_internal_reference<>() ),"Returns nearest node.")
  .def("pickNodesInside",&XC::DqPtrsNode::pickNodesInside,"pickNodesInside(geomObj,tol) return the nodes inside the geometric object.")
  .def("getBnd", &XC::DqPtrsNode::Bnd, "Returns nodes boundary.")
  .def("getCentroid", &XC::DqPtrsNode::getCentroid, "Returns nodes centroid.")
  .def(self += self)
  .def(self + self)
  .def(self - self)
  .def(self * self)
  .def("createInertiaLoads", &XC::DqPtrsNode::createInertiaLoads,"Create the inertia load for the given acceleration vector.")
  .add_property("totalMass", &XC::DqPtrsNode::getTotalMass, "Return the total mass matrix.")
  .def("getTotalMassComponent", &XC::DqPtrsNode::getTotalMassComponent,"Return the total mass matrix component for the DOF argument.")
  ;

typedef XC::DqPtrs<XC::Element> dq_ptrs_element;
class_<dq_ptrs_element, bases<CommandEntity>, boost::noncopyable >("dq_ptrs_element",no_init)
  .def("__iter__", range<return_internal_reference<> >(&dq_ptrs_element::indBegin, &dq_ptrs_element::indEnd))
  .add_property("size", &dq_ptrs_element::size, "Returns list size.")
  .def("__len__",&dq_ptrs_element::size, "Returns list size.")
  .def("at",make_function(&dq_ptrs_element::get, return_internal_reference<>() ), "Access specified element with bounds checking.")
  .def("__getitem__",make_function(&dq_ptrs_element::get, return_internal_reference<>() ), "Access specified element with bounds checking.")
  .def("getTags",make_function(&dq_ptrs_element::getTags, return_internal_reference<>() ),"Returns element identifiers.")
  .def("findTag",make_function(&dq_ptrs_element::findTag, return_internal_reference<>() ),"Returns the element identified by the tag argument.")
  .def("clear",&dq_ptrs_element::clear,"Removes all items.")
  ;

XC::Element *(XC::DqPtrsElem::*getNearestElementDqPtrs)(const Pos3d &)= &XC::DqPtrsElem::getNearest;
class_<XC::DqPtrsElem, bases<dq_ptrs_element> >("DqPtrsElem",no_init)
  .def("append", &XC::DqPtrsElem::push_back,"Appends element at the end of the list.")
  .def("pushFront", &XC::DqPtrsElem::push_front,"Push element at the beginning of the list.")
  .add_property("getNumLiveElements", &XC::DqPtrsElem::getNumLiveElements)
  .add_property("getNumDeadElements", &XC::DqPtrsElem::getNumDeadElements)
  .def("getNearestElement",make_function(getNearestElementDqPtrs, return_internal_reference<>() ),"Returns nearest element.")
  .def("getBnd", &XC::DqPtrsElem::Bnd, "Returns elements boundary.")
  .def("getContours",&XC::DqPtrsElem::getContours,"Returns contour(s) from the element set in the form of closed 3D polylines.")
  .def("pickElemsInside",&XC::DqPtrsElem::pickElemsInside,"pickElemsInside(geomObj,tol) return the elements inside the geometric object.") 
  .def("pickElemsOfType",&XC::DqPtrsElem::pickElemsOfType,"pickElemsOfType(typeName) return the elements whose type contains the string.")
  .def("pickElemsOfDimension",&XC::DqPtrsElem::pickElemsOfDimension,"pickElemsOfDimension(dim) return the elements whose dimension equals the argument.")
  .def("getTypes",&XC::DqPtrsElem::getTypesPy,"getElementTypes() return a list with the element types in the container.")
  .def("getMaterials",&XC::DqPtrsElem::getMaterialNamesPy,"getElementMaterials() return a list with the names of the element materials in the container.")
  .def("pickElemsOfMaterial",&XC::DqPtrsElem::pickElemsOfMaterial,"pickElemsOfMaterial(materialName) return the elements that have that material.")
  // Loads on elements.
  .def("createInertiaLoads", &XC::DqPtrsElem::createInertiaLoads,"Create the inertia load for the given acceleration vector.")
  .def("vector2dPointLoadGlobal", &XC::DqPtrsElem::vector2dPointLoadGlobal,"Element's load.")
  .def("vector2dPointLoadLocal", &XC::DqPtrsElem::vector2dPointLoadLocal,"Element's load.")
  .def("vector3dPointLoadGlobal", &XC::DqPtrsElem::vector3dPointLoadGlobal,"Element's load.")
  .def("vector3dPointLoadLocal", &XC::DqPtrsElem::vector3dPointLoadLocal,"Element's load.")

  .add_property("totalMass", &XC::DqPtrsElem::getTotalMass, "Return the total mass matrix.")
  .def("getTotalMassComponent", &XC::DqPtrsElem::getTotalMassComponent,"Return the total mass matrix component for the DOF argument.")
  .def("getAverageSize", &XC::DqPtrsElem::getAverageSize,"Get the average size of the elements (elements of dimension zero are ignored).")
  .def(self += self)
  .def(self + self)
  .def(self - self)
  .def(self * self)
   ;

typedef XC::DqPtrs<XC::Constraint> dq_ptrs_constraint;
class_<dq_ptrs_constraint, bases<CommandEntity>, boost::noncopyable >("dq_ptrs_constraint",no_init)
  .def("__iter__", range<return_internal_reference<> >(&dq_ptrs_constraint::indBegin, &dq_ptrs_constraint::indEnd))
  .add_property("size", &dq_ptrs_constraint::size, "Returns list size.")
  .def("__len__",&dq_ptrs_constraint::size, "Returns list size.")
  .def("at",make_function(&dq_ptrs_constraint::get, return_internal_reference<>() ), "Access specified constraint with bounds checking.")
  .def("__getitem__",make_function(&dq_ptrs_constraint::get, return_internal_reference<>() ), "Access specified constraint with bounds checking.")
  .def("getTags",make_function(&dq_ptrs_constraint::getTags, return_internal_reference<>() ),"Returns constraint identifiers.")
  .def("clear",&dq_ptrs_constraint::clear,"Removes all items.")
  ;

class_<XC::DqPtrsConstraint, bases<dq_ptrs_constraint>, boost::noncopyable >("DqPtrsConstraint",no_init)
  .def("append", &XC::DqPtrsConstraint::push_back,"Appends constraint at the end of the list.")
  .def("pushFront", &XC::DqPtrsConstraint::push_front,"Push constraint at the beginning of the list.")
   .def(self += self)
   .def(self + self)
   .def(self - self)
   .def(self * self)
   ;

// XC::SetBase exposed in export_preprocessor_build_model.cc

XC::DqPtrsNode &(XC::SetMeshComp::*getNodesRef)(void)= &XC::SetMeshComp::getNodes;
XC::DqPtrsElem &(XC::SetMeshComp::*getElementsRef)(void)= &XC::SetMeshComp::getElements;
XC::DqPtrsConstraint &(XC::SetMeshComp::*GetConstraintsRef)(void)= &XC::SetMeshComp::GetConstraints;
XC::Node *(XC::SetMeshComp::*getNearestNodeSetMeshComp)(const Pos3d &)= &XC::SetMeshComp::getNearestNode;
XC::Element *(XC::SetMeshComp::*getNearestElementSetMeshComp)(const Pos3d &)= &XC::SetMeshComp::getNearestElement;
void (XC::SetMeshComp::*transforms)(const XC::TrfGeom &)= &XC::SetMeshComp::Transform;
void (XC::SetMeshComp::*extend_nodes)(const XC::DqPtrsNode &)= &XC::SetMeshComp::extend;
void (XC::SetMeshComp::*extend_elements)(const XC::DqPtrsElem &)= &XC::SetMeshComp::extend;
void (XC::SetMeshComp::*extend_constraints)(const XC::SetMeshComp::lst_ptr_constraints &)= &XC::SetMeshComp::extend;
void (XC::SetMeshComp::*extend_mesh_comp)(const XC::SetMeshComp &)= &XC::SetMeshComp::extend;
class_<XC::SetMeshComp, XC::SetMeshComp *, bases<XC::SetBase>>("SetMeshComp",no_init)
  .add_property("name", &XC::SetMeshComp::getStrName, &XC::SetMeshComp::newName,"get object name.")
  .def("rename",&XC::SetMeshComp::rename,"Change the name of the set.")
  .add_property("getNodes", make_function(getNodesRef, return_internal_reference<>() ),"return the nodes of the set. DEPRECATED use nodes.")
  .add_property("getElements", make_function(getElementsRef, return_internal_reference<>() ),"return the elements of the set. DEPRECATED use elements.")
  .add_property("getConstraints", make_function(GetConstraintsRef, return_internal_reference<>() ),"return the constraints of the set.")
  .add_property("nodes", make_function(getNodesRef, return_internal_reference<>() ),&XC::SetMeshComp::setNodes,"nodes of the set.")
  .add_property("elements", make_function(getElementsRef, return_internal_reference<>() ),&XC::SetMeshComp::setElements,"elements of the set.")
  .add_property("constraints", make_function(GetConstraintsRef, return_internal_reference<>() ),&XC::SetMeshComp::setConstraints,"constraints of the set.")
  .def("getNearestNode",make_function(getNearestNodeSetMeshComp, return_internal_reference<>() ),"Returns nearest node.")
  .def("getNearestElement",make_function(getNearestElementSetMeshComp, return_internal_reference<>() ),"Returns nearest element.")
  .def("killElements",&XC::SetMeshComp::kill_elements,"Deactivates set's elements.")
  .def("aliveElements",&XC::SetMeshComp::alive_elements,"Activates set's elements.")
  .def("getNumDeadElements",&XC::SetMeshComp::getNumDeadElements,"Number of inactive elements.")
  .def("getNumLiveElements",&XC::SetMeshComp::getNumLiveElements,"Number of active elements.")
  .def("getNumDeadNodes",&XC::SetMeshComp::getNumDeadNodes,"Number of inactive nodes.")
  .def("getNumLiveNodes",&XC::SetMeshComp::getNumLiveNodes,"Number of active nodes.")
  .def("transforms",transforms,"Apply transformation to set members.")

  .def("getResistingSlidingVectorsSystem3d",&XC::SetMeshComp::getResistingSlidingVectorsSystem3d, "Return the resultant of the forces over the nodes near to the plane, of the elements behind the plane.")

  .def("getTangentStiffness",&XC::SetMeshComp::getTangentStiff,"getTangentStiffness(node) return the contribution of the elements to the tangent stiffness of the node argument.")
  .def("getInitialStiffness",&XC::Node::getInitialStiff,"getInitialStiffness(elementSet) return the contribution of the elements to the initial stiffness of the node argument.")

  .def("appendFromGeomEntity", &XC::SetMeshComp::appendFromGeomEntity,"Extend this set with the nodes and elements of the geometric entity being passed as parameter.")
  .def("clear",&XC::SetMeshComp::clear,"Removes all items.")
  .def("empty",&XC::SetMeshComp::empty,"Return true if the set is empty.")
  .def("pickNodesInside",&XC::SetMeshComp::pickNodesInside,"pickNodesInside(newSetName, geomObj, tol) return a set with the nodes inside the geometric object.") 
  .def("pickElemsInside",&XC::SetMeshComp::pickElemsInside,"pickElemsInside(newSetName, geomObj, tol) return a set with the elements inside the geometric object.") 
  .def("getElementTypes",&XC::SetMeshComp::getElementTypesPy,"getElementTypes() return a list with the element types in the container.")
  .def("pickElemsOfType",&XC::SetMeshComp::pickElemsOfType,"pickElemsOfType(typeName) return the elements whose type contains the string argument.")
  .def("getElementMaterials",&XC::SetMeshComp::getElementMaterialNamesPy,"getElementMaterials() return a list with the names of the element materials in the container.")
  .def("pickElemsOfMaterial",&XC::SetMeshComp::pickElemsOfMaterial,"pickElemsOfMaterial(materialName) return the elements that have that material.")
  .def("getBnd", &XC::SetMeshComp::Bnd, "Returns set boundary.")
  .def("fillUpwards", &XC::SetMeshComp::fillUpwards,"add entities upwards.")
  .def("fillDownwards", &XC::SetMeshComp::fillDownwards,"add entities downwards.")
  .def(self += self)
  .def(self -= self)
  .def(self *= self)
  .def("extend", extend_nodes, "Appends the nodes in the argument to the set.")
  .def("extend", extend_elements, "Appends the elements in the argument to the set.")
  .def("extend", extend_constraints, "Appends the constraints in the argument to the set.")
  .def("extend", extend_mesh_comp, "Appends the argument components to the set.")
  .def("createInertiaLoads", &XC::SetMeshComp::createInertiaLoads,"Create the inertia load for the given acceleration vector.")
  .add_property("totalMass", &XC::SetMeshComp::getTotalMass, "Return the total mass matrix.")
  .def("getTotalMassComponent", &XC::SetMeshComp::getTotalMassComponent,"Return the total mass matrix component for the DOF argument.")
  // The following operators return copies of the object stored in MapSet
  // and then the operations in those objects are lost. For now we'll
  // leave them aside.
  // .def(self + self)
  // .def(self - self)
  // .def(self * self)
  ;

typedef XC::DqPtrs<XC::Pnt> dq_ptrs_pnt;
class_<dq_ptrs_pnt, bases<CommandEntity>, boost::noncopyable >("dq_ptrs_pnt",no_init)
  .def("__iter__", range<return_internal_reference<> >(&dq_ptrs_pnt::indBegin, &dq_ptrs_pnt::indEnd))
  .def("at",make_function(&dq_ptrs_pnt::get, return_internal_reference<>() ), "Access specified point with bounds checking.")
  .def("__getitem__",make_function(&dq_ptrs_pnt::get, return_internal_reference<>() ), "Access specified point with bounds checking.")
  .def("findTag",make_function(&dq_ptrs_pnt::findTag, return_internal_reference<>() ),"Returns the point identified by the tag argument.")
  .def("clear",&dq_ptrs_pnt::clear,"Removes all items.")
   ;

XC::Pnt *(XC::SetEntities::lst_ptr_points::*getNearestPnt)(const Pos3d &)= &XC::SetEntities::lst_ptr_points::getNearest;
class_<XC::SetEntities::lst_ptr_points, bases<dq_ptrs_pnt>>("lstPnts",no_init)
  .def("append", &XC::SetEntities::lst_ptr_points::push_back,"Append a point at the end of the list.")
  .def("pushFront", &XC::SetEntities::lst_ptr_points::push_front,"Push point at the beginning of the list.")
  .add_property("size", &XC::SetEntities::lst_ptr_points::size, "Return list size.")
  .def("__len__",&XC::SetEntities::lst_ptr_points::size, "Return list size.")
  .def("pickPointsInside",&XC::SetEntities::lst_ptr_points::pickEntitiesInside,"pickPointsInside(geomObj,tol) return the nodes inside the geometric object.") 
  .def("getBnd", &XC::SetEntities::lst_ptr_points::Bnd, "Return points boundary.")
  .def("getNearest",make_function(getNearestPnt, return_internal_reference<>() ),"Return the nearest point to the position argument.")
  .def("findTag",make_function(&XC::SetEntities::lst_ptr_points::findTag, return_internal_reference<>() ),"Returns the point identified by the tag argument.")
  .def("getCentroid", &XC::SetEntities::lst_ptr_points::getCentroid, "Returns the centroid of the points.")
  .def("conciliaNDivs", &XC::SetEntities::conciliaNDivs)
   ;

typedef XC::DqPtrs<XC::Edge> dq_line_ptrs;
class_<dq_line_ptrs, bases<CommandEntity>, boost::noncopyable >("dq_line_ptrs",no_init)
//.def(vector_indexing_suite<dq_line_ptrs>())  Doesn't work with pointer containers.
  .def("__iter__", range<return_internal_reference<> >(&dq_line_ptrs::indBegin, &dq_line_ptrs::indEnd))
  .add_property("size", &dq_line_ptrs::size, "Return container size.")
  .def("__len__",&dq_line_ptrs::size, "Return container size.")
  .def("at",make_function(&dq_line_ptrs::get, return_internal_reference<>() ), "Access specified line with bounds checking.")
  .def("__getitem__",make_function(&dq_line_ptrs::get, return_internal_reference<>() ), "Access specified line with bounds checking.")
  .def("findTag",make_function(&dq_line_ptrs::findTag, return_internal_reference<>() ),"Returns the edge identified by the tag argument.")
  .def("clear",&dq_line_ptrs::clear,"Removes all items.")
   ;

class_<XC::SetEntities::lst_line_pointers, bases<dq_line_ptrs>>("lstLines",no_init)
  .def("append", &XC::SetEntities::lst_line_pointers::push_back,"Appends line at the end of the list.")
  .def("pushFront", &XC::SetEntities::lst_line_pointers::push_front,"Push line at the beginning of the list.")
  .def("pickLinesInside",&XC::SetEntities::lst_line_pointers::pickEntitiesInside,"pickLinesInside(geomObj,tol) return the nodes inside the geometric object.") 
  .def("getBnd", &XC::SetEntities::lst_line_pointers::Bnd, "Returns lines boundary.")
  .def("findTag",make_function(&XC::SetEntities::lst_line_pointers::findTag, return_internal_reference<>() ),"Returns the line identified by the tag argument.")
   ;

typedef XC::DqPtrs<XC::Face> dq_ptrs_surfaces;
class_<dq_ptrs_surfaces, bases<CommandEntity>, boost::noncopyable >("dq_ptrs_surfaces",no_init)
  .def("__iter__", range<return_internal_reference<> >(&dq_ptrs_surfaces::indBegin, &dq_ptrs_surfaces::indEnd))
  .def("at",make_function(&dq_ptrs_surfaces::get, return_internal_reference<>() ), "Access specified surface with bounds checking.")
  .def("__getitem__",make_function(&dq_ptrs_surfaces::get, return_internal_reference<>() ), "Access specified surface with bounds checking.")
  .def("findTag",make_function(&dq_ptrs_surfaces::findTag, return_internal_reference<>() ),"Returns the surface identified by the tag argument.")
  .def("clear",&dq_ptrs_surfaces::clear,"Removes all items.")
   ;

class_<XC::SetEntities::lst_surface_ptrs, bases<dq_ptrs_surfaces> >("lstSurfaces",no_init)
  .def("append", &XC::SetEntities::lst_surface_ptrs::push_back,"Appends surface at the end of the list.")
  .def("pushFront", &XC::SetEntities::lst_surface_ptrs::push_front,"Push surface at the beginning of the list.")
  .add_property("size", &XC::SetEntities::lst_surface_ptrs::size, "Returns list size.")
  .def("__len__",&XC::SetEntities::lst_surface_ptrs::size, "Returns list size.")
  .def("pickSurfacesInside",&XC::SetEntities::lst_surface_ptrs::pickEntitiesInside,"pickSurfacesInside(geomObj,tol) return the nodes inside the geometric object.") 
  .def("getBnd", &XC::SetEntities::lst_surface_ptrs::Bnd, "Returns surfaces boundary.")
  .def("findTag",make_function(&XC::SetEntities::lst_surface_ptrs::findTag, return_internal_reference<>() ),"Returns the surface identified by the tag argument.")
   ;

typedef XC::DqPtrs<XC::Body> dq_body_ptrs;
class_<dq_body_ptrs, bases<CommandEntity>, boost::noncopyable >("dq_body_ptrs",no_init)
  .def("__iter__", range<return_internal_reference<> >(&dq_body_ptrs::indBegin, &dq_body_ptrs::indEnd))
  .def("at",make_function(&dq_body_ptrs::get, return_internal_reference<>() ), "Access specified body with bounds checking.")
  .def("__getitem__",make_function(&dq_body_ptrs::get, return_internal_reference<>() ), "Access specified body with bounds checking.")
  .def("findTag",make_function(&dq_body_ptrs::findTag, return_internal_reference<>() ),"Returns the body identified by the tag argument.")
  .def("clear",&dq_body_ptrs::clear,"Removes all items.")
   ;

class_<XC::SetEntities::lst_body_pointers, bases<dq_body_ptrs> >("lstBodies",no_init)
  .def("append", &XC::SetEntities::lst_body_pointers::push_back,"Appends body at the end of the list.")
  .def("pushFront", &XC::SetEntities::lst_body_pointers::push_front,"Push body at the beginning of the list.")
  .add_property("size", &XC::SetEntities::lst_body_pointers::size, "Returns list size.")
  .def("__len__",&XC::SetEntities::lst_body_pointers::size, "Returns list size.")
  .def("pickBodiesInside",&XC::SetEntities::lst_body_pointers::pickEntitiesInside,"pickBodiesInside(geomObj,tol) return the nodes inside the geometric object.") 
  .def("getBnd", &XC::SetEntities::lst_body_pointers::Bnd, "Returns bodies boundary.")
  .def("findTag",make_function(&XC::SetEntities::lst_body_pointers::findTag, return_internal_reference<>() ),"Returns the body identified by the tag argument.")
   ;

void (XC::SetEntities::*fillDownwardsMesh)(XC::SetMeshComp &)= &XC::SetEntities::fillDownwards;
XC::Pnt *(XC::SetEntities::*getNearestPoint)(const Pos3d &)= &XC::SetEntities::getNearestPoint;
class_<XC::SetEntities, bases<XC::PreprocessorContainer> >("SetEntities",no_init)
  .add_property("useGmsh", &XC::SetEntities::getUseGmsh, &XC::SetEntities::setUseGmsh, "Get/set the useGmsh member value.")
  .def("getBnd", &XC::SetEntities::Bnd, "return entities boundary.")
  .def("fillUpwards", &XC::SetEntities::fillUpwards,"add entities upwards.")
  .def("fillDownwards", fillDownwardsMesh,"add entities downwards.")
  .def("splitLinesAtIntersections",&XC::SetEntities::splitLinesAtIntersections,"divide the lines of the set at intersection points.")
  .def("getAverageSize",&XC::SetEntities::getAverageSize,"Return the average length of the entities.")
  .def("getNearestPoint",make_function(getNearestPoint, return_internal_reference<>() ),"Return the nearest point to the position argument.")
  .def("getMaxElementSize",&XC::SetEntities::getMaxElementSize,"Return the maximum size of the elements according to the number of divisions of the lines.")
  .def("getMinElementSize",&XC::SetEntities::getMinElementSize,"Return the minimum size of the elements according to the number of divisions of the lines.")
  .def("getAverageElementSize",&XC::SetEntities::getAverageElementSize,"Return the average size of the elements according to the number of divisions of the lines.")
  ;

XC::SetEntities &(XC::Set::*getEntities)(void)= &XC::Set::getEntities;
XC::SetMeshComp &(XC::Set::*getMeshComponents)(void)= &XC::Set::getMeshComp;
XC::SetEntities::lst_ptr_points &(XC::Set::*getPoints)(void)= &XC::Set::getPoints;
XC::SetEntities::lst_line_pointers &(XC::Set::*getLines)(void)= &XC::Set::getLines;
XC::SetEntities::lst_surface_ptrs &(XC::Set::*getSurfaces)(void)= &XC::Set::getSurfaces;
XC::SetEntities::lst_body_pointers &(XC::Set::*getBodies)(void)= &XC::Set::getBodies;
void (XC::Set::*extend_points)(const XC::SetEntities::lst_ptr_points &)= &XC::Set::extend;
void (XC::Set::*extend_lines)(const XC::SetEntities::lst_line_pointers &)= &XC::Set::extend;
void (XC::Set::*extend_surfaces)(const XC::SetEntities::lst_surface_ptrs &)= &XC::Set::extend;
void (XC::Set::*extend_bodies)(const XC::SetEntities::lst_body_pointers &)= &XC::Set::extend;
void (XC::Set::*extend_ugrids)(const XC::SetEntities::lst_ptr_uniform_grids &)= &XC::Set::extend;
void (XC::Set::*extend_set)(const XC::Set &)= &XC::Set::extend;
class_<XC::Set, XC::Set *,bases<XC::SetMeshComp> >("Set")
  .add_property("description", make_function( &XC::Set::getDescription, return_value_policy<copy_const_reference>() ), &XC::Set::setDescription,"Description (string) of the set.")
  .add_property("useGmsh", &XC::Set::getUseGmsh, &XC::Set::setUseGmsh, "Get/set the useGmsh member value.")
  .add_property("getEntities", make_function(getEntities, return_internal_reference<>() ),"return the entities (points, lines, surfaces,...) of the set.")
  .add_property("getMeshComponents", make_function(getMeshComponents, return_internal_reference<>() ),"return the mesh components (nodes, elements,...) of the set.")
  .add_property("getPoints", make_function(getPoints, return_internal_reference<>() ),"return the points of the set.")
  .add_property("getLines", make_function(getLines, return_internal_reference<>() ),"return the lines of the set.")
  .add_property("getSurfaces", make_function(getSurfaces, return_internal_reference<>() ),"return the surfaces of the set.")
  .add_property("getBodies", make_function(getBodies, return_internal_reference<>() ),"return the bodies of the set.")
  .add_property("points", make_function(getPoints, return_internal_reference<>() ),&XC::Set::setPoints,"points of the set.")
  .add_property("lines", make_function(getLines, return_internal_reference<>() ),&XC::Set::setLines,"lines of the set.")
  .add_property("surfaces", make_function(getSurfaces, return_internal_reference<>() ),&XC::Set::setSurfaces,"surfaces of the set.")
  .add_property("bodies", make_function(getBodies, return_internal_reference<>() ),&XC::Set::setBodies,"bodies of the set.")
  .def("getEntitiesSet", &XC::Set::getEntitiesSet, "Return the entities (points, lines, surfaces,...) on a set.")
  .def("getMeshComponentsSet", &XC::Set::getMeshComponentsSet, "Return the mesh components (nodes, elements,...) on a set.")
  .def("fillUpwards", &XC::Set::fillUpwards,"add entities upwards.")
  .def("fillDownwards", &XC::Set::fillDownwards,"add entities downwards.")
  .def("conciliaNDivs", &XC::Set::conciliaNDivs)
  .def("numerate", &XC::Set::numera,"Numerate entities (VTK).")
  .def("clear",&XC::Set::clear,"Removes all items.")
  .def("empty",&XC::Set::empty,"Return true if the set is empty.")
  .def("getBnd", &XC::Set::Bnd, "Returns set boundary.")
  .def("extend", extend_nodes, "Appends the nodes in the argument to the set.")
  .def("extend", extend_elements, "Appends the elements in the argument to the set.")
  .def("extend", extend_constraints, "Appends the constraints in the argument to the set.")
  .def("extend", extend_points, "Appends the points in the argument to the set.")
  .def("extend", extend_lines, "Appends the lines in the argument to the set.")
  .def("extend", extend_surfaces, "Appends the surfaces in the argument to the set.")
  .def("extend", extend_bodies, "Appends the bodies in the argument to the set.")
  .def("extend", extend_ugrids, "Appends the uniform grids in the argument to the set.")
  .def("extend", extend_set, "Appends the argument components to the set.")
  .def(self += self)
  .def(self -= self)
  .def(self *= self)
  // The following operators return copies of the object stored in MapSet
  // and then the operations in those objects are lost. For now we'll
  // leave them aside.
  // .def(self + self)
  // .def(self - self)
  // .def(self * self)
   ;

typedef XC::RowSet<XC::NodePtrArray3d::var_ref_i_row,XC::ElemPtrArray3d::var_ref_i_row> set_i_row;
class_<set_i_row, bases<XC::SetEstruct>, boost::noncopyable >("set_i_row", no_init);
class_<XC::IRowSet, bases<set_i_row>, boost::noncopyable >("IRowSet", no_init);

typedef XC::RowSet<XC::NodePtrArray3d::var_ref_j_row,XC::ElemPtrArray3d::var_ref_j_row> set_j_row;
class_<set_j_row, bases<XC::SetEstruct>, boost::noncopyable >("set_j_row", no_init);
class_<XC::JRowSet, bases<set_j_row>, boost::noncopyable >("JRowSet", no_init);

typedef XC::RowSet<XC::NodePtrArray3d::var_ref_k_row,XC::ElemPtrArray3d::var_ref_k_row> set_k_row;
class_<set_k_row, bases<XC::SetEstruct>, boost::noncopyable >("set_k_row", no_init);
class_<XC::KRowSet, bases<set_k_row>, boost::noncopyable >("KRowSet", no_init);

// XC::MapSetBase and XC::MapSet exposed in export_preprocessor_main.cc


