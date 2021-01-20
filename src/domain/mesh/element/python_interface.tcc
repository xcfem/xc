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

//Python exposition of the utilities.

//class_<XC::ElemPos, bases<KDTreePos>, boost::noncopyable >("ElemPos", no_init);

#include "utils/fvectors/python_interface.tcc"
#include "utils/physical_properties/python_interface.tcc"
#include "domain/mesh/element/utils/python_interface.tcc"

Pos2d (XC::Element::*getProjection2D)(const Pos2d &,bool) const= &XC::Element::getProjection;
Pos3d (XC::Element::*getProjection3D)(const Pos3d &,bool) const= &XC::Element::getProjection;
XC::NodePtrsWithIDs &(XC::Element::*getNodePtrsRef)(void)= &XC::Element::getNodePtrs;
const XC::Vector &(XC::Element::*getResistingForceRef)(void) const= &XC::Element::getResistingForce;
const XC::Matrix &(XC::Element::*getInitialStiffRef)(void) const= &XC::Element::getInitialStiff;
const XC::Matrix &(XC::Element::*getTangentStiffRef)(void) const= &XC::Element::getTangentStiff;
bool (XC::Element::*ElementIn3D)(const GeomObj3d &,const double &,const double &) const= &XC::Element::In;
bool (XC::Element::*ElementOut3D)(const GeomObj3d &,const double &,const double &) const= &XC::Element::Out;
void (XC::Element::*setIdNodesRef)(const XC::ID &inodes)= &XC::Element::setIdNodes;
double (XC::Element::*getDistPos2d)(const Pos2d &,bool initialGeometry) const= &XC::Element::getDist;
double (XC::Element::*getDistPos3d)(const Pos3d &,bool initialGeometry) const= &XC::Element::getDist;
double (XC::Element::*getDist2Pos2d)(const Pos2d &,bool initialGeometry) const= &XC::Element::getDist2;
double (XC::Element::*getDist2Pos3d)(const Pos3d &,bool initialGeometry) const= &XC::Element::getDist2;
class_<XC::Element, XC::Element *,bases<XC::MeshComponent>, boost::noncopyable >("Element", no_init)
  .add_property("getNodes", make_function( getNodePtrsRef, return_internal_reference<>() ))
  .add_property("getIdxNodes",&XC::Element::getIdxNodes,"Return the node indices for its use in VTK arrays.")
  .def("setIdNodes", setIdNodesRef," setIdNodes(xc.ID([idNode0, idNode1,...]) set the element nodes.")
  .def("setIdNode", &XC::Element::setIdNode," setIdNode(i-th, idNode) set the element i-th node.")
  .add_property("getDimension",&XC::Element::getDimension,"Return element's dimension (point: 0, line: 1, surface: 2 or volume: 3).")
  .def("commitState", &XC::Element::commitState,"Commits element state.")
  .def("revertToLastCommit", &XC::Element::revertToLastCommit,"Return to the last committed state.")
  .def("revertToStart", &XC::Element::revertToStart,"Return the element to its initial state.")
  .def("update", &XC::Element::update,"Updates the element state.")
  .def("getNumDOF", &XC::Element::getNumDOF,"Return the number of element DOFs.")
  .def("getResistingForce",make_function(getResistingForceRef, return_internal_reference<>() ),"Calculates element's resisting force.")
  .def("getTangentStiff",make_function(getTangentStiffRef, return_internal_reference<>() ),"Return tangent stiffness matrix.")
  .def("getInitialStiff",make_function(getInitialStiffRef, return_internal_reference<>() ),"Return initial stiffness matrix.")
  .def("setDeadSRF",XC::Element::setDeadSRF,"Assigns Stress Reduction Factor for element deactivation.")
  .add_property("getVtkCellType",&XC::Element::getVtkCellType,"Return cell type for Vtk graphics.")
  .def("getPosCentroid",&XC::Element::getCenterOfMassPosition,"Return centroid's position.")
  .def("getCooCentroid",&XC::Element::getCenterOfMassCoordinates,"Return centroid's coordinates.")
  .def("In", ElementIn3D,"\n""In(geomObject,factor,tolerance) \n""Return true if the current positions of all the nodes scaled by a factor: initialPos+factor*currentDisplacement lie inside the geometric object.")
  .def("Out", ElementOut3D,"\n""Out(geomObject,factor,tolerance) \n""Return true if current positions of all the nodes scaled by a factor: initialPos+factor*currentDisplacement lie outside the geometric object.")
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
  .def("getBaseVector3d",&XC::Element::getBaseVector3d,"Return a 3D vector in the direction of the i-th local axis.")
  .def("getIVector3d",&XC::Element::getIVector3d,"Return a 3D vector in the direction of the local axis 1.")
  .def("getJVector3d",&XC::Element::getJVector3d,"Return a 3D vector in the direction of the local axis 2.")
  .def("getKVector3d",&XC::Element::getKVector3d,"Return a 3D vector in the direction of the local axis 3.")
  .def("getCoordinateSystem",&XC::Element::getCooSys,"Return the element coordinate system.")
  .def("getValuesAtNodes",&XC::Element::getValuesAtNodes,"getValuesAtNodes(string, silent): return the value of the argument at the element nodes. If silent==True don't complain about non-existent property.")
  .def("createInertiaLoad", &XC::Element::createInertiaLoad,"Create the inertia load for the given acceleration vector.")
   ;

class_<XC::ElementIter, boost::noncopyable >("ElementIter", no_init)
  .def("next", &XC::ElementIter::operator(), return_internal_reference<>(),"Return next node.")
   ;

class_<XC::ElementBase<2>, bases<XC::Element >, boost::noncopyable >("ElementBase2N","Base class for 2 node elements.", no_init);

class_<XC::ElementBase<3>, bases<XC::Element >, boost::noncopyable >("ElementBase3N","Base class for 3 node elements.", no_init);

class_<XC::ElementBase<4>, bases<XC::Element >, boost::noncopyable >("ElementBase4N","Base class for 4 node elements.", no_init);

class_<XC::ElementBase<8>, bases<XC::Element >, boost::noncopyable >("ElementBase8N","Base class for 8 node elements.", no_init);

class_<XC::ElementBase<9>, bases<XC::Element >, boost::noncopyable >("ElementBase9N","Base class for 9 node elements.", no_init);

class_<XC::ElementBase<20>, bases<XC::Element >, boost::noncopyable >("ElementBase20N","Base class for 20 node elements.", no_init);

class_<XC::ElementBase<27>, bases<XC::Element >, boost::noncopyable >("ElementBase27N","Base class for 27 node elements.", no_init);

class_<XC::Element0D , bases<XC::ElementBase<2>>, boost::noncopyable >("Element0D", no_init)
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
class_<XC::Element1D, bases<XC::ElementBase<2> >, boost::noncopyable >("Element1D", no_init)
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
    .def("getLength",&XC::Element1D::getLength, "Returns element length.")
       ;




#include "truss_beam_column/python_interface.tcc"
#include "utils/coordTransformation/python_interface.tcc"


#include "plane/python_interface.tcc"
#include "volumetric/python_interface.tcc"
//#include "frictionBearing/python_interface.tcc"
#include "zeroLength/python_interface.tcc"
