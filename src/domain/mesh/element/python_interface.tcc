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

class_<XC::Information, boost::noncopyable >("Information", no_init);


//class_<XC::ElemPos, bases<KDTreePos>, boost::noncopyable >("ElemPos", no_init);

class_<XC::ParticlePos3d>("ParticlePos3d")
  .def(init<Pos2d>())
  .def(init<Pos3d>())
  .add_property("r", make_function( &XC::ParticlePos3d::r_coordinate, return_value_policy<return_by_value>()), &XC::ParticlePos3d::set_r_coordinate, "Return r coordinate.")
  .add_property("s", make_function( &XC::ParticlePos3d::s_coordinate, return_value_policy<return_by_value>()), &XC::ParticlePos3d::set_s_coordinate, "Return s coordinate.")
  .add_property("t", make_function( &XC::ParticlePos3d::t_coordinate, return_value_policy<return_by_value>()), &XC::ParticlePos3d::set_t_coordinate, "Return t coordinate.")
  .def(self_ns::str(self_ns::self))
  ;

class_<XC::RayleighDampingFactors, bases<EntCmd>, boost::noncopyable >("RayleighDampingFactors", no_init);

#include "utils/fvectors/python_interface.tcc"
#include "utils/physical_properties/python_interface.tcc"

typedef XC::NodePtrs::vector_ptr_nodes vector_ptr_nodes;
class_<vector_ptr_nodes, boost::noncopyable>("vector_ptr_nodes")
  .def(vector_indexing_suite<vector_ptr_nodes>() )
  ;

XC::Node *(XC::NodePtrs::*getNearestNodePtr)(const Pos3d &,bool initialGeometry)= &XC::NodePtrs::getNearestNode;
class_<XC::NodePtrs, bases<EntCmd,vector_ptr_nodes>, boost::noncopyable >("NodePtrs", no_init)
  .def("getNearestNode",make_function(getNearestNodePtr, return_internal_reference<>() ),"Return nearest node.")
  ;

class_<XC::NodePtrsWithIDs, bases<XC::NodePtrs>, boost::noncopyable >("NodePtrsWithIDs", no_init)
  .add_property("getExternalNodes",make_function(&XC::NodePtrsWithIDs::getExternalNodes, return_internal_reference<>() ),"Return tags of external nodes.")
  ;

XC::NodePtrsWithIDs &(XC::Element::*getNodePtrsRef)(void)= &XC::Element::getNodePtrs;
const XC::Vector &(XC::Element::*getResistingForceRef)(void) const= &XC::Element::getResistingForce;
const XC::Matrix &(XC::Element::*getInitialStiffRef)(void) const= &XC::Element::getInitialStiff;
const XC::Matrix &(XC::Element::*getTangentStiffRef)(void) const= &XC::Element::getTangentStiff;
class_<XC::Element, XC::Element *,bases<XC::MeshComponent>, boost::noncopyable >("Element", no_init)
  .add_property("getNodes", make_function( getNodePtrsRef, return_internal_reference<>() ))
  .add_property("getIdxNodes",&XC::Element::getIdxNodes,"Return the node indices for its use in VTK arrays.")
  .add_property("getDimension",&XC::Element::getDimension,"Return element's dimension (point: 0, line: 1, surface: 2 or volume: 3).")
  .def("commitState", &XC::Element::commitState,"Commits element state.")
  .def("revertToLastCommit", &XC::Element::revertToLastCommit,"Return to the last commited state.")
  .def("revertToStart", &XC::Element::revertToStart,"Return the element to its initial state.")
  .def("getNumDOF", &XC::Element::getNumDOF,"Return the number of element DOFs.")
  .def("getResistingForce",make_function(getResistingForceRef, return_internal_reference<>() ),"Calculates element's resisting force.")
  .def("getTangentStiff",make_function(getTangentStiffRef, return_internal_reference<>() ),"Return tangent stiffness matrix.")
  .def("getInitialStiff",make_function(getInitialStiffRef, return_internal_reference<>() ),"Return initial stiffness matrix.")
  .def("setDeadSRF",XC::Element::setDeadSRF,"Assigns Stress Reduction Factor for element deactivation.")
  .add_property("getVtkCellType",&XC::Element::getVtkCellType,"Return cell type for Vtk graphics.")
  .def("getMEDCellType",&XC::Element::getVtkCellType,"Return cell type for MED file writing.")
  .def("getPosCentroid",&XC::Element::getPosCdg,"Return centroid's position.")
  .def("getCooCentroid",&XC::Element::getCooCdg,"Return centroid's coordinates.")
  .def("getLocalAxes",&XC::Element::getLocalAxes,"Return element local axes as matrix rows: [[x1,y1,z1],[x2,y2,z2],...·] .")
  .def("getPoints",&XC::Element::getPuntos,"Return a uniform grid of points over the element.")
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
  .def("getCoordinateSystem",&XC::Element::getSisCoo,"Return the element coordinate system.")
   ;

class_<XC::ElementIter, boost::noncopyable >("ElementIter", no_init)
  .def("next", &XC::ElementIter::operator(), return_internal_reference<>(),"Return next node.")
   ;


class_<XC::ElementBase<2>, bases<XC::Element >, boost::noncopyable >("ElementBase2N", no_init);

class_<XC::ElementBase<3>, bases<XC::Element >, boost::noncopyable >("ElementBase3N", no_init);

class_<XC::ElementBase<4>, bases<XC::Element >, boost::noncopyable >("ElementBase4N", no_init);

class_<XC::ElementBase<8>, bases<XC::Element >, boost::noncopyable >("ElementBase8N", no_init);

class_<XC::ElementBase<20>, bases<XC::Element >, boost::noncopyable >("ElementBase20N", no_init);

class_<XC::ElementBase<27>, bases<XC::Element >, boost::noncopyable >("ElementBase27N", no_init);

class_<XC::Element0D , bases<XC::ElementBase<2>>, boost::noncopyable >("Element0D", no_init)
  .def("setupVectors",&XC::Element0D::setUpVectors)
  .add_property("getNumDOF",&XC::Element0D::getNumDOF,"Return element number of DOFs (6 or 12).")
  .add_property("getDimension",&XC::Element0D::getDimension,"Return element dimension.")
  .add_property("getIVector",make_function(&XC::Element0D::getX, return_internal_reference<>() ),"Return i vector.")
  .add_property("getJVector",make_function(&XC::Element0D::getY, return_internal_reference<>() ),"Return j vector.")
  .add_property("getKVector",make_function(&XC::Element0D::getZ, return_internal_reference<>() ),"Return k vector.")
  .add_property("getSisCoo",&XC::Element0D::getSisCoo,"Return element coordinate system.")
  .add_property("getTransformation",make_function(&XC::Element0D::getTransformation, return_internal_reference<>() ),"Return element transformation.")
  ;


XC::CrdTransf *(XC::Element1D::*getCrdTransf)(void)= &XC::Element1D::getCoordTransf;
class_<XC::Element1D, bases<XC::ElementBase<2> >, boost::noncopyable >("Element1D", no_init)
    .add_property("getCoordTransf", make_function( getCrdTransf, return_internal_reference<>() ))
    .def("getLineSegment", &XC::Element1D::getSegmento)
    .def("getMEDCellType", &XC::Element1D::getMEDCellType,"Return the cell type that corresponds to the element in a MED file.")
    .def("vector2dUniformLoadGlobal", &XC::Element1D::vector2dUniformLoadGlobal,"Element's load.")
    .def("vector2dUniformLoadLocal", &XC::Element1D::vector2dUniformLoadLocal,"Element's load.")
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
    .def("getCooPuntos",make_function(&XC::Element1D::getCooPuntos, return_internal_reference<>()) ,"Points over element's length.")
       ;




#include "truss_beam_column/python_interface.tcc"
#include "utils/coordTransformation/python_interface.tcc"


#include "plane/python_interface.tcc"
//#include "frictionBearing/python_interface.tcc"
#include "zeroLength/python_interface.tcc"
