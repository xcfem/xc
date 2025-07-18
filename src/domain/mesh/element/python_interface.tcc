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

//Python exposition of the utilities.

//class_<XC::ElemPos, bases<KDTreePos>, boost::noncopyable >("ElemPos", no_init);

#include "utils/python_interface.tcc"

Pos2d (XC::Element::*getProjection2D)(const Pos2d &,bool) const= &XC::Element::getProjection;
Pos3d (XC::Element::*getProjection3D)(const Pos3d &,bool) const= &XC::Element::getProjection;
XC::NodePtrsWithIDs &(XC::Element::*getNodePtrsRef)(void)= &XC::Element::getNodePtrs;
const XC::Vector &(XC::Element::*getResistingForceRef)(void) const= &XC::Element::getResistingForce;
const XC::Vector &(XC::Element::*getNodeResistingForceINOD)(const size_t &iNod) const= &XC::Element::getNodeResistingForce;
const XC::Vector &(XC::Element::*getNodeResistingForceIncInertiaINOD)(const size_t &iNod) const= &XC::Element::getNodeResistingForceIncInertia;
const XC::Vector &(XC::Element::*getNodeResistingForcePTR)(const XC::Node *) const= &XC::Element::getNodeResistingForce;
const XC::Vector &(XC::Element::*getNodeResistingForceIncInertiaPTR)(const XC::Node *) const= &XC::Element::getNodeResistingForceIncInertia;
const XC::Vector &(XC::Element::*getNodeResistingForceTAG)(const int &) const= &XC::Element::getNodeResistingForceByTag;
const XC::Vector &(XC::Element::*getNodeResistingForceIncInertiaTAG)(const int &) const= &XC::Element::getNodeResistingForceIncInertiaByTag;
const XC::Matrix &(XC::Element::*getInitialStiffRef)(void) const= &XC::Element::getInitialStiff;
const XC::Matrix &(XC::Element::*getTangentStiffRef)(void) const= &XC::Element::getTangentStiff;
const XC::Matrix &(XC::Element::*getMassRef)(void) const= &XC::Element::getMass;
XC::Matrix (XC::Element::*getNodeMassRef)(const XC::Node *) const= &XC::Element::getMass;
XC::Matrix (XC::Element::*getTotalMassRef)(void) const= &XC::Element::getTotalMass;
const XC::Matrix &(XC::Element::*getDampRef)(void) const= &XC::Element::getDamp;
bool (XC::Element::*ElementIn3D)(const GeomObj3d &,const double &,const double &) const= &XC::Element::In;
bool (XC::Element::*ElementOut3D)(const GeomObj3d &,const double &,const double &) const= &XC::Element::Out;
bool (XC::Element::*ElementIn2D)(const GeomObj2d &,const double &,const double &) const= &XC::Element::In;
bool (XC::Element::*ElementOut2D)(const GeomObj2d &,const double &,const double &) const= &XC::Element::Out;
bool (XC::Element::*ElementCrosses3D)(const GeomObj3d &,const double &,const double &) const= &XC::Element::Crosses;
bool (XC::Element::*ElementCrosses2D)(const GeomObj2d &,const double &,const double &) const= &XC::Element::Crosses;
void (XC::Element::*setIdNodesRef)(const XC::ID &inodes)= &XC::Element::setIdNodes;
double (XC::Element::*getDistPos2d)(const Pos2d &,bool initialGeometry) const= &XC::Element::getDist;
double (XC::Element::*getDistPos3d)(const Pos3d &,bool initialGeometry) const= &XC::Element::getDist;
double (XC::Element::*getDist2Pos2d)(const Pos2d &,bool initialGeometry) const= &XC::Element::getDist2;
double (XC::Element::*getDist2Pos3d)(const Pos3d &,bool initialGeometry) const= &XC::Element::getDist2;
boost::python::list (XC::Element::*get_connected_elements_element_py)(void)= &XC::Element::getConnectedElementsPy;    
boost::python::list (XC::Element::*get_connected_elements_element_set_py)(const XC::SetBase *)= &XC::Element::getConnectedElementsPy; 
boost::python::list (XC::Element::*get_connected_elements_tags_element_py)(void) const= &XC::Element::getConnectedElementTags;    
boost::python::list (XC::Element::*get_connected_elements_tags_element_set_py)(const XC::SetBase *) const= &XC::Element::getConnectedElementTags; 
class_<XC::Element, XC::Element *,bases<XC::MeshComponent>, boost::noncopyable >("Element", no_init)
  .add_property("getNodes", make_function( getNodePtrsRef, return_internal_reference<>() ),"DEPRECATED; return the element nodes.")
  .add_property("nodes", make_function( getNodePtrsRef, return_internal_reference<>() ),"Return the element nodes.")
  .add_property("numNodes", &XC::Element::getNumExternalNodes, "Return the number of nodes.")
  .def("find",&XC::Element::find,"Return the index of the node in the element (-1 if not found).")
  .add_property("getIdxNodes",&XC::Element::getIdxNodes,"Return the node indices for its use in VTK arrays.")
  .def("setIdNodes", setIdNodesRef," setIdNodes(xc.ID([idNode0, idNode1,...]) set the element nodes.")
  .def("setIdNode", &XC::Element::setIdNode," setIdNode(i-th, idNode) set the element i-th node.")
  .def("replaceNode", &XC::Element::replaceNode," replaceNode(oldNode, newNode) replaces the old node by the new one.")
  .add_property("getDimension",&XC::Element::getDimension,"Return element's dimension (point: 0, line: 1, surface: 2 or volume: 3).")
  .def("commitState", &XC::Element::commitState,"Commits element state.")
  .def("revertToLastCommit", &XC::Element::revertToLastCommit,"Return to the last committed state.")
  .def("revertToStart", &XC::Element::revertToStart,"Return the element to its initial state.")
  .def("update", &XC::Element::update,"Updates the element state.")
  .def("getNumDOF", &XC::Element::getNumDOF,"Return the number of element DOFs.")
  .def("getResistingForce",make_function(getResistingForceRef, return_internal_reference<>() ),"Calculates element's resisting force.")
  .def("getNodeResistingForce", make_function(getNodeResistingForceINOD, return_internal_reference<>() ),"getNodeResistingForce(ith node): returns the generalized force of the element over the given node.")
  .def("getNodeResistingForceIncInertia", make_function(getNodeResistingForceIncInertiaINOD, return_internal_reference<>() ),"getNodeResistingForceIncInertia(ith node): returns the generalized force of the element over the given node including inertia effects.")
  .def("getNodeResistingForce", make_function(getNodeResistingForcePTR, return_internal_reference<>() ), "getNodeResistingForce(node): returns the generalized force of the element over the given node.")
  .def("getNodeResistingForceIncInertiaByTag", make_function(getNodeResistingForceIncInertiaPTR, return_internal_reference<>() ),"getNodeResistingForceIncInertia(node): returns the generalized force of the element over the given node including inertia effects.")
  .def("getNodeResistingForceByTag", make_function(getNodeResistingForceTAG, return_internal_reference<>() ),"getNodeResistingForce(nodeTag): returns the generalized force of the element over the given node.")
  .def("getNodeResistingForceIncInertiaByTag", make_function(getNodeResistingForceIncInertiaTAG, return_internal_reference<>() ),"getNodeResistingForceIncInertia(nodeTag): returns the generalized force of the element over the given node including inertia effects.")
  .def("getTangentStiff",make_function(getTangentStiffRef, return_internal_reference<>() ),"Return tangent stiffness matrix.")
  .def("getInitialStiff",make_function(getInitialStiffRef, return_internal_reference<>() ),"Return initial stiffness matrix.")
  .add_property("mass", make_function(getMassRef, return_internal_reference<>()) ,"Element mass matrix.")
  .add_property("totalMass", getTotalMassRef, "Returns the sum of the mass matrices corresponding to the nodes.")
  .def("getTotalMassComponent", &XC::Element::getTotalMassComponent,"Return the total mass matrix component for the DOF argument.")
  .def("nodeMass", getNodeMassRef, "Components of the element mass matrix corresponding to DOFs of the argument node.")
  .add_property("damp", make_function(getDampRef, return_internal_reference<>()) ,"Element damping matrix.")
  .def("setDeadSRF",XC::Element::setDeadSRF,"Assigns Stress Reduction Factor for element deactivation.")
  .add_property("getVtkCellType",&XC::Element::getVtkCellType,"Return cell type for Vtk graphics.")
  .def("getPosCentroid",&XC::Element::getCenterOfMassPosition,"getCenterOfMassPosition(initialCoordinates) return centroid's position. If initialCoordinates==True, return the initial position otherwise return the current position.")
  .def("getCooCentroid",&XC::Element::getCenterOfMassCoordinates,"getCenterOfMassCoordinates(initialCoordinates) return centroid's coordinates. If initialCoordinates==True, return the initial coordinates otherwise return the current coordinates.")
  .def("In", ElementIn3D,"\n""In(geomObject,factor,tolerance) \n""Return true if the current positions of all the nodes scaled by a factor: initialPos+factor*currentDisplacement lie inside the geometric object.")
  .def("Out", ElementOut3D,"\n""Out(geomObject,factor,tolerance) \n""Return true if current positions of all the nodes scaled by a factor: initialPos+factor*currentDisplacement lie outside the geometric object.")
  .def("In", ElementIn2D,"\n""In(geomObject,factor,tolerance) \n""Return true if the current positions of all the nodes scaled by a factor: initialPos+factor*currentDisplacement lie inside the geometric object.")
  .def("Out", ElementOut2D,"\n""Out(geomObject,factor,tolerance) \n""Return true if current positions of all the nodes scaled by a factor: initialPos+factor*currentDisplacement lie outside the geometric object.")
  .def("Crosses", ElementCrosses3D,"\n""Crosses(geomObject,factor,tolerance) \n""Return true if the current positions of all the nodes scaled by a factor: initialPos+factor*currentDisplacement crosses (has nodes at both sides of) the geometric object.")
  .def("Crosses", ElementCrosses2D,"\n""Crosses(geomObject,factor,tolerance) \n""Return true if the current positions of all the nodes scaled by a factor: initialPos+factor*currentDisplacement crosses (has nodes at both sides of) the geometric object.")
  .def("getLocalAxes",&XC::Element::getLocalAxes,"Return the element local axes as matrix rows: [[x1,y1,z1],[x2,y2,z2],...·] .")
  .def("getPoints",&XC::Element::getPoints,"Return a uniform grid of points over the element.")
  .def("getDist",getDistPos3d,"Return the distance from the point to this element.")
  .def("getDist2",getDist2Pos3d,"Return the squared distance from the point to this element.")
  .def("getDist",getDistPos2d,"Return the distance from the point to this element.")
  .def("getDist2",getDist2Pos2d,"Return the squared distance from the point to this element.")
  .def("getProjection", getProjection2D, "Return the projection of the argument on the element.")
  .def("getProjection", getProjection3D, "Return the projection of the argument on the element.")

  .def("resetTributaries",&XC::Element::resetTributaries)
  .def("dumpTributaries",&XC::Element::dumpTributaries)
  .def("computeTributaryLengths",&XC::Element::computeTributaryLengths)
  .def("getTributaryLength",&XC::Element::getTributaryLength)
  .def("getTributaryLengthByTag",&XC::Element::getTributaryLengthByTag)
  .def("computeTributaryAreas",&XC::Element::computeTributaryAreas)
  .def("getTributaryArea",&XC::Element::getTributaryArea)
  .def("getTributaryAreaByTag",&XC::Element::getTributaryAreaByTag)
  .def("computeTributaryVolumes",&XC::Element::computeTributaryVolumes)
  .def("getTributaryVolume",&XC::Element::getTributaryVolume)
  .def("getTributaryVolumeByTag",&XC::Element::getTributaryVolumeByTag)
  .def("getMaxCooNod",&XC::Element::MaxCooNod)
  .def("getMinCooNod",&XC::Element::MinCooNod)

  //.def("getBaseVector",&XC::Element::getBaseVector,"Return a vector in the direction of the i-th local axis.")
  .def("resetNodalCoordinates",&XC::Element::resetNodalCoordinates,"Initialize the element variables that depend on the position of the nodes (i.e. after a change in their coordinates to set an imperfection or a precamber).")
  .def("getBaseVector3d",&XC::Element::getBaseVector3d,"Return a 3D vector in the direction of the i-th local axis.")
  .def("getIVector3d",&XC::Element::getIVector3d,"Return a 3D vector in the direction of the local axis 1.")
  .def("getJVector3d",&XC::Element::getJVector3d,"Return a 3D vector in the direction of the local axis 2.")
  .def("getKVector3d",&XC::Element::getKVector3d,"Return a 3D vector in the direction of the local axis 3.")
  .def("getGaussModel",make_function(&XC::Element::getGaussModel, return_internal_reference<>() ),"Return the element Gauss quadrature.")
  .def("getCoordinateSystem",&XC::Element::getCooSys,"Return the element coordinate system.")
  .def("get2DCoordinateSystem",&XC::Element::getCooSys2d,"Return the element coordinate system in a two-dimensional space.")
  .def("getLength",&XC::Element::getLength, "getLength(initialGeometry): return element's length. If initialGeometry is True the returned length corresponds to its undeformed geometry.")
  .def("getArea", &XC::Element::getArea, "getArea(initialGeometry): return element's area. If initialGeometry is True the returned area corresponds to its undeformed geometry.")
  .def("getVolume", &XC::Element::getVolume, "getVolume(initialGeometry): return element's volume. If initialGeometry is True the returned volume corresponds to its undeformed geometry.")
  .def("getValuesAtNodes",&XC::Element::getValuesAtNodes,"getValuesAtNodes(string, silent): return the value of the argument at the element nodes. If silent==True don't complain about non-existent property.")
  .def("createInertiaLoad", &XC::Element::createInertiaLoad,"Create the inertia load for the given acceleration vector.")
  .def("copySetsFrom", &XC::Element::copySetsFrom,"Add this element to all the sets containing the given element.")

  .add_property("connectedElements", get_connected_elements_element_py, "Returns the elements connected to any of the nodes of this element.")
  .def("getConnectedElements", get_connected_elements_element_set_py, "Returns the elements from the given set that are connected to any of the nodes of this element.")
.add_property("connectedElementTags", get_connected_elements_tags_element_py, "Returns the tags of the elements connected to any of the nodes of this element.")
  .def("getConnectedElementTags", get_connected_elements_tags_element_set_py, "Returns the tags of the elements from the given set that are connected to any of the nodes of this element.")
   ;

XC::Element *(XC::ElementIter::*element_iter_parenthesis_op)(void)= &XC::ElementIter::operator();
class_<XC::ElementIter, boost::noncopyable >("ElementIter", no_init)
  .def("next", element_iter_parenthesis_op, return_internal_reference<>(),"Return next node.")
   ;

class_<ElementBase2N, bases<XC::Element >, boost::noncopyable >("ElementBase2N","Base class for 2 node elements.", no_init)
  ;

class_<ElementBase3N, bases<XC::Element >, boost::noncopyable >("ElementBase3N","Base class for 3 node elements.", no_init);

class_<ElementBase4N, bases<XC::Element >, boost::noncopyable >("ElementBase4N","Base class for 4 node elements.", no_init);

class_<ElementBase8N, bases<XC::Element >, boost::noncopyable >("ElementBase8N","Base class for 8 node elements.", no_init);

class_<ElementBase9N, bases<XC::Element >, boost::noncopyable >("ElementBase9N","Base class for 9 node elements.", no_init);

class_<ElementBase20N, bases<XC::Element >, boost::noncopyable >("ElementBase20N","Base class for 20 node elements.", no_init);

class_<ElementBase27N, bases<XC::Element >, boost::noncopyable >("ElementBase27N","Base class for 27 node elements.", no_init);

class_<XC::Element0D , bases<ElementBase2N >, boost::noncopyable >("Element0D", no_init)
  .def("setupVectors",&XC::Element0D::setUpVectors,"setupVectors(x,yp); set element orientation; x: axis of the element, yp: vector that lies on the xy plane of the element.")
  .add_property("getNumDOF",&XC::Element0D::getNumDOF,"Return element number of DOFs (6 or 12).")
  .add_property("getDimension",&XC::Element0D::getDimension,"Return element dimension.")
  .add_property("getIVector",make_function(&XC::Element0D::getX, return_internal_reference<>() ),"Return i vector.")
  .add_property("getJVector",make_function(&XC::Element0D::getY, return_internal_reference<>() ),"Return j vector.")
  .add_property("getKVector",make_function(&XC::Element0D::getZ, return_internal_reference<>() ),"Return k vector.")
  .add_property("getCooSys",&XC::Element0D::getCooSys,"Return element coordinate system.")
  .add_property("getTransformation",make_function(&XC::Element0D::getTransformation, return_internal_reference<>() ),"Return element transformation.")
  ;


XC::CrdTransf *(XC::Element1D::*getCrdTransf)(void)= &XC::Element1D::getCoordTransf;
class_<XC::Element1D, bases<ElementBase2N >, boost::noncopyable >("Element1D", no_init)
    .add_property("getCoordTransf", make_function( getCrdTransf, return_internal_reference<>() ))
    .def("getLineSegment", &XC::Element1D::getLineSegment)
    .def("vector2dUniformLoadGlobal", &XC::Element1D::vector2dUniformLoadGlobal,"Element's load.")
    .def("vector2dUniformLoadLocal", &XC::Element1D::vector2dUniformLoadLocal,"Element's load.")
    .def("vector2dPartialUniformLoadGlobal", &XC::Element1D::vector2dPartialUniformLoadGlobal,"Element's load.")
    .def("vector2dPartialUniformLoadLocal", &XC::Element1D::vector2dPartialUniformLoadLocal,"Element's load.")
    .def("vector2dPointByRelDistLoadGlobal", &XC::Element1D::vector2dPointByRelDistLoadGlobal,"Element's load.")
    .def("vector2dPointByRelDistLoadLocal", &XC::Element1D::vector2dPointByRelDistLoadLocal,"Element's load.")
    .def("vector2dPointLoadGlobal", &XC::Element1D::vector2dPointLoadGlobal,"Element's load.")
    .def("vector2dPointLoadLocal", &XC::Element1D::vector2dPointLoadLocal,"Element's load.")
    .def("vector3dUniformLoadGlobal", &XC::Element1D::vector3dUniformLoadGlobal,"Element's load.")
    .def("vector3dUniformLoadLocal", &XC::Element1D::vector3dUniformLoadLocal,"Element's load.")
    .def("vector3dPointByRelDistLoadGlobal", &XC::Element1D::vector3dPointByRelDistLoadGlobal,"Element's load.")
    .def("vector3dPointByRelDistLoadLocal", &XC::Element1D::vector3dPointByRelDistLoadLocal,"Element's load.")
    .def("vector3dPointLoadGlobal", &XC::Element1D::vector3dPointLoadGlobal,"Element's load.")
    .def("vector3dPointLoadLocal", &XC::Element1D::vector3dPointLoadLocal,"Element's load.")
    .def("strainLoad", &XC::Element1D::strainLoad,"Element's load.")
    .def("getCooPoints",make_function(&XC::Element1D::getCooPoints, return_internal_reference<>()) ,"Returns points over element length.")
  ;


#include "truss_beam_column/python_interface.tcc"
#include "utils/coordTransformation/python_interface.tcc"


#include "plane/python_interface.tcc"
#include "volumetric/python_interface.tcc"
//#include "frictionBearing/python_interface.tcc"
#include "zero_length/python_interface.tcc"
#include "uw_elements/python_interface.tcc"
