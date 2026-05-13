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

#include "truss/python_interface.tcc"

class_<XC::BeamColumn, bases<XC::Element1D>, boost::noncopyable >("BeamColumn", no_init)
  .def("getSection", make_function(&XC::BeamColumn::getSectionPtr, return_internal_reference<>() ), "getSection(i): return the i-th section of the element.")
  .add_property("extrapolationMatrix",make_function(&XC::BeamColumn::getExtrapolationMatrix,return_internal_reference<>() ),"Returns the element extrapolation matrix.")
  .def("getExtrapolatedValues", &XC::BeamColumn::getExtrapolatedValues,"Return the values at nodes from the values at the Gauss points.")
  ;

XC::ElasticSection2dPhysicalProperties &(XC::ProtoBeam2d::*getElasticSection2dPhysicalProp)(void)= &XC::ProtoBeam2d::getPhysicalProperties;
XC::CrossSectionProperties2d &(XC::ProtoBeam2d::*getCrossSectionProperties2d)(void)= &XC::ProtoBeam2d::getSectionProperties;
void (XC::ProtoBeam2d::*pbeam2d_set_material_ref)(const XC::Material &)= &XC::ProtoBeam2d::setMaterial;
void (XC::ProtoBeam2d::*pbeam2d_set_material_name)(const std::string &)= &XC::ProtoBeam2d::setMaterial;
class_<XC::ProtoBeam2d, bases<XC::BeamColumn>, boost::noncopyable >("ProtoBeam2d", no_init)
  .add_property("sectionProperties", make_function(getCrossSectionProperties2d, return_internal_reference<>()), &XC::ProtoBeam2d::setSectionProperties,"Access to section properties.")
  .add_property("physicalProperties", make_function(getElasticSection2dPhysicalProp, return_internal_reference<>() ),"Returns materials at integration points (gauss points).")
  .add_property("getPhysicalProperties", make_function(getElasticSection2dPhysicalProp, return_internal_reference<>() ),"TO DEPRECATE: use physicalProperties. Returns materials at integration points (gauss points).")
  .def("setSectionProperties",&XC::ProtoBeam2d::setSectionProperties,"Set cross section properties.")
  .def("setMaterial", pbeam2d_set_material_ref, "Assigns the given material to the element.")
  .def("setMaterial", pbeam2d_set_material_name, "Assigns the material with the given name to the element.")
  .def("getVDirStrongAxisLocalCoord",&XC::ProtoBeam2d::getVDirStrongAxisLocalCoord,"Returns the direction vector of element strong axis expressed in the local coordinate system.")
  .def("getVDirWeakAxisLocalCoord",&XC::ProtoBeam2d::getVDirWeakAxisLocalCoord,"Returns the direction vector of element weak axis expressed in the local coordinate system.")
  .def("getStrongAxisAngle",&XC::ProtoBeam2d::getStrongAxisAngle,"Returns the angle between element strong axis and local XZ plane.")
  .def("getWeakAxisAngle",&XC::ProtoBeam2d::getWeakAxisAngle,"Returns the angle between element weak axis and local XZ plane.")
   ;

XC::ElasticSection3dPhysicalProperties &(XC::ProtoBeam3d::*getElasticSection3dPhysicalProp)(void)= &XC::ProtoBeam3d::getPhysicalProperties;
XC::CrossSectionProperties3d &(XC::ProtoBeam3d::*getCrossSectionProperties3d)(void)= &XC::ProtoBeam3d::getSectionProperties;
void (XC::ProtoBeam3d::*pbeam3d_set_material_ref)(const XC::Material &)= &XC::ProtoBeam3d::setMaterial;
void (XC::ProtoBeam3d::*pbeam3d_set_material_name)(const std::string &)= &XC::ProtoBeam3d::setMaterial;
class_<XC::ProtoBeam3d, bases<XC::BeamColumn>, boost::noncopyable >("ProtoBeam3d", no_init)
  .add_property("sectionProperties", make_function(getCrossSectionProperties3d, return_internal_reference<>()), &XC::ProtoBeam3d::setSectionProperties,"Access to section properties.")
  .add_property("physicalProperties", make_function(getElasticSection3dPhysicalProp, return_internal_reference<>() ),"Returns materials at integration points (gauss points).")
  .add_property("getPhysicalProperties", make_function(getElasticSection3dPhysicalProp, return_internal_reference<>() ),"TO DEPRECATE: use physicalProperties. Returns materials at integration points (gauss points).")
  .def("setSectionProperties",&XC::ProtoBeam3d::setSectionProperties,"Set cross section properties.")
  .def("setMaterial", pbeam3d_set_material_ref, "Assigns the given material to the element.")
  .def("setMaterial", pbeam3d_set_material_name, "Assigns the material with the given name to the element.")
  .def("getVDirStrongAxisLocalCoord",&XC::ProtoBeam3d::getVDirStrongAxisLocalCoord,"Returns the direction vector of element strong axis expressed in the local coordinate system.")
  .def("getVDirWeakAxisLocalCoord",&XC::ProtoBeam3d::getVDirWeakAxisLocalCoord,"Returns the direction vector of element weak axis expressed in the local coordinate system.")
  .def("getStrongAxisAngle",&XC::ProtoBeam3d::getStrongAxisAngle,"Returns the angle between element strong axis and local XZ plane.")
  .def("getWeakAxisAngle",&XC::ProtoBeam3d::getWeakAxisAngle,"Returns the angle between element weak axis and local XZ plane.")
   ;

#include "elasticBeamColumn/python_interface.tcc"

void (XC::BeamColumnWithSectionFD::*bcwsfd_set_material_ref)(const XC::Material &)= &XC::BeamColumnWithSectionFD::setMaterial;
void (XC::BeamColumnWithSectionFD::*bcwsfd_set_material_name)(const std::string &)= &XC::BeamColumnWithSectionFD::setMaterial;
class_<XC::BeamColumnWithSectionFD, bases<XC::BeamColumn>, boost::noncopyable >("BeamColumnWithSectionFD", no_init)
  .def("getNumSections",&XC::BeamColumnWithSectionFD::getNumSections)
  .def("getSections",make_function(&XC::BeamColumnWithSectionFD::getSections, return_internal_reference<>() ),"Returns element's sections.")
  .def("setMaterial", bcwsfd_set_material_ref, "Assigns the given material to the element.")
  .def("setMaterial", bcwsfd_set_material_name, "Assigns the material with the given name to the element.")
  .add_property("rho", &XC::BeamColumnWithSectionFD::getRho,&XC::BeamColumnWithSectionFD::setRho, "Get/set the element density per unit length.")
  .def("setLinearRho", &XC::BeamColumnWithSectionFD::setRho, "Set the element density per unit length.")
  .def("getLinearRho", &XC::BeamColumnWithSectionFD::getRho, "Get the element density per unit length.")
  ;

class_<XC::BeamColumnWithSectionFDTrf2d, bases<XC::BeamColumnWithSectionFD>, boost::noncopyable >("BeamColumnWithSectionFDTrf2d", no_init)
  .def("setCoordTransf", &XC::BeamColumnWithSectionFDTrf2d::setCoordTransf, "Assigns the coordinate transformation.")
   ;

XC::Vector (XC::BeamColumnWithSectionFDTrf3d::*get_vdir_strong_axis_local_coord_section)(const size_t &) const= &XC::BeamColumnWithSectionFDTrf3d::getVDirStrongAxisLocalCoord;
XC::Vector (XC::BeamColumnWithSectionFDTrf3d::*get_vdir_strong_axis_local_coord_average)(void) const= &XC::BeamColumnWithSectionFDTrf3d::getVDirStrongAxisLocalCoord;
XC::Vector (XC::BeamColumnWithSectionFDTrf3d::*get_vdir_weak_axis_local_coord_section)(const size_t &) const= &XC::BeamColumnWithSectionFDTrf3d::getVDirWeakAxisLocalCoord;
XC::Vector (XC::BeamColumnWithSectionFDTrf3d::*get_vdir_weak_axis_local_coord_average)(const size_t &) const= &XC::BeamColumnWithSectionFDTrf3d::getVDirWeakAxisLocalCoord;

double (XC::BeamColumnWithSectionFDTrf3d::*get_strong_axis_angle_section)(const size_t &i) const= &XC::BeamColumnWithSectionFDTrf3d::getStrongAxisAngle;
double (XC::BeamColumnWithSectionFDTrf3d::*get_strong_axis_angle_average)(void) const= &XC::BeamColumnWithSectionFDTrf3d::getStrongAxisAngle;
double (XC::BeamColumnWithSectionFDTrf3d::*get_weak_axis_angle_section)(const size_t &i) const= &XC::BeamColumnWithSectionFDTrf3d::getWeakAxisAngle;
double (XC::BeamColumnWithSectionFDTrf3d::*get_weak_axis_angle_average)(void) const= &XC::BeamColumnWithSectionFDTrf3d::getWeakAxisAngle;

const XC::Vector &(XC::BeamColumnWithSectionFDTrf3d::*get_vdir_strong_axis_global_coord_section)(const size_t &, bool) const= &XC::BeamColumnWithSectionFDTrf3d::getVDirStrongAxisGlobalCoord;
const XC::Vector &(XC::BeamColumnWithSectionFDTrf3d::*get_vdir_strong_axis_global_coord_average)(bool) const= &XC::BeamColumnWithSectionFDTrf3d::getVDirStrongAxisGlobalCoord;
const XC::Vector &(XC::BeamColumnWithSectionFDTrf3d::*get_vdir_weak_axis_global_coord_section)(const size_t &, bool) const= &XC::BeamColumnWithSectionFDTrf3d::getVDirWeakAxisGlobalCoord;
const XC::Vector &(XC::BeamColumnWithSectionFDTrf3d::*get_vdir_weak_axis_global_coord_average)(bool) const= &XC::BeamColumnWithSectionFDTrf3d::getVDirWeakAxisGlobalCoord;

class_<XC::BeamColumnWithSectionFDTrf3d, bases<XC::BeamColumnWithSectionFD>, boost::noncopyable >("BeamColumnWithSectionFDTrf3d", no_init)
  .def("setCoordTransf", &XC::BeamColumnWithSectionFDTrf3d::setCoordTransf, "Assigns the coordinate transformation.")
  .def("getVDirStrongAxisLocalCoord",get_vdir_strong_axis_local_coord_section,"Returns i-th cross section strong axis direction vector expressed in local coordinates.")
  .def("getVDirStrongAxisLocalCoord",get_vdir_strong_axis_local_coord_average,"Returns the average cross section strong axis direction vector expressed in local coordinates.")
  .def("getVDirWeakAxisLocalCoord",get_vdir_weak_axis_local_coord_section,"Returns i-th cross section weak axis direction vector expressed in local coordinates.")
  .def("getVDirWeakAxisLocalCoord",get_vdir_weak_axis_local_coord_average,"Returns the average cross section weak axis direction vector expressed in local coordinates.")

  .def("getStrongAxisAngle", get_strong_axis_angle_section, "Returns the angle between i-th cross section strong axis and the local XZ plane.")
  .def("getStrongAxisAngle", get_strong_axis_angle_average, "Returns the angle between the average cross section strong axis and the local XZ plane.")
  .def("getWeakAxisAngle", get_weak_axis_angle_section, "Returns the angle between i-th cross section weak axis and the local XZ plane.")
  .def("getWeakAxisAngle", get_weak_axis_angle_average, "Returns the angle between the average cross section weak axis and the local XZ plane.")

  .def("getVDirStrongAxisGlobalCoord", make_function(get_vdir_strong_axis_global_coord_section, return_value_policy<copy_const_reference>()), "Returns i-th cross section strong axis direction vector expressed in global coordinates.")
  .def("getVDirStrongAxisGlobalCoord", make_function(get_vdir_strong_axis_global_coord_average, return_value_policy<copy_const_reference>()), "Returns the average cross section strong axis direction vector expressed in global coordinates.")
  .def("getVDirWeakAxisGlobalCoord", make_function(get_vdir_weak_axis_global_coord_section, return_value_policy<copy_const_reference>()), "Returns i-th cross section weak axis direction vector expressed in global coordinates.")
  .def("getVDirWeakAxisGlobalCoord", make_function(get_vdir_weak_axis_global_coord_average, return_value_policy<copy_const_reference>()), "Returns the average cross section weak axis direction vector expressed in global coordinates.")
   ;
#include "beamWithHinges/python_interface.tcc"

class_<XC::NLForceBeamColumn2dBase, bases<XC::BeamColumnWithSectionFDTrf2d>, boost::noncopyable >("NLForceBeamColumn2dBase", no_init)
  .add_property("tol", &XC::NLForceBeamColumn2dBase::getTol,&XC::NLForceBeamColumn2dBase::setTol, "Get/set the tolerance for relative energy norm.")
  .def("getV", &XC::NLForceBeamColumn2dBase::getV, "Mean shear force.")
  .add_property("getV1", &XC::NLForceBeamColumn2dBase::getV1, "Internal shear force at back end.")
  .add_property("getV2", &XC::NLForceBeamColumn2dBase::getV2, "Internal shear force at front end.")
  .add_property("getVy1", &XC::NLForceBeamColumn2dBase::getV1, "Internal shear force at back end.")
  .add_property("getVy2", &XC::NLForceBeamColumn2dBase::getV2, "Internal shear force at front end.") 
  .add_property("getN1", &XC::NLForceBeamColumn2dBase::getN1, "Internal axial force at front end.")
  .add_property("getN2", &XC::NLForceBeamColumn2dBase::getN2, "Internal axial force at back end.")
  .add_property("getM1", &XC::NLForceBeamColumn2dBase::getM1, "Internal bending moment at back end.")
  .add_property("getM2", &XC::NLForceBeamColumn2dBase::getM2, "Internal bending moment at front end.")
  .add_property("getMz1", &XC::NLForceBeamColumn2dBase::getM1, "Internal bending moment at back end.")
  .add_property("getMz2", &XC::NLForceBeamColumn2dBase::getM2, "Internal bending moment at front end.")
   ;

class_<XC::NLForceBeamColumn3dBase, bases<XC::BeamColumnWithSectionFDTrf3d>, boost::noncopyable >("NLForceBeamColumn3dBase", no_init)
  .add_property("rho", &XC::NLForceBeamColumn3dBase::getRho,&XC::NLForceBeamColumn3dBase::setRho, "Get/set the element density.")
  .add_property("tol", &XC::NLForceBeamColumn2dBase::getTol,&XC::NLForceBeamColumn2dBase::setTol, "Get/set the tolerance for relative energy norm.")
  .add_property("getAN2", &XC::NLForceBeamColumn3dBase::getAN2,"Axial force which acts over the front end of the element.")
  .add_property("getN1", &XC::NLForceBeamColumn3dBase::getN1,"Internal axial force in the back end of the element.")
  .add_property("getN2", &XC::NLForceBeamColumn3dBase::getN2,"Internal axial force in the front end of the element.")
  .def("getN", &XC::NLForceBeamColumn3dBase::getN,"Average internal axial force.")
  .add_property("getAMz1", &XC::NLForceBeamColumn3dBase::getAMz1,"Z bending moment which acts over the back end of the element.")
  .add_property("getAMz2", &XC::NLForceBeamColumn3dBase::getAMz2,"Z bending moment which acts over the front end of the element.")
  .add_property("getMz1", &XC::NLForceBeamColumn3dBase::getMz1,"Internal Z bending moment at the back end of the element.")
  .add_property("getMz2", &XC::NLForceBeamColumn3dBase::getMz2,"Internal Z bending moment at the front end of the element.")
  .def("getVy", &XC::NLForceBeamColumn3dBase::getVy, "Internal mean Y shear force.")
  .add_property("getAVy1", &XC::NLForceBeamColumn3dBase::getAVy1, "Y shear force which acts at the back end of the element.")
  .add_property("getAVy2", &XC::NLForceBeamColumn3dBase::getAVy2, "Y shear force which acts at the front end of the element.")
  .add_property("getVy1", &XC::NLForceBeamColumn3dBase::getVy1, "Internal Y shear force at the back end of the element.")
  .add_property("getVy2", &XC::NLForceBeamColumn3dBase::getVy2, "Internal Y shear force at the front end of the element.")
  .def("getVz", &XC::NLForceBeamColumn3dBase::getVz, "Internal mean Z shear force.")
  .add_property("getAVz1", &XC::NLForceBeamColumn3dBase::getAVz1, "Z shear force which acts at the back end of the element.")
  .add_property("getAVz2", &XC::NLForceBeamColumn3dBase::getAVz2, "Z shear force which acts at the back end of the element.")
  .add_property("getVz1", &XC::NLForceBeamColumn3dBase::getVz1, "Internal Z shear force at the back end of the element.")
  .add_property("getVz2", &XC::NLForceBeamColumn3dBase::getVz2, "Internal Z shear force at the front end of the element.")
  .add_property("getMy1", &XC::NLForceBeamColumn3dBase::getMy1,"Internal Y bending moment at the back end of the element.")
  .add_property("getMy2", &XC::NLForceBeamColumn3dBase::getMy2,"Internal Y bending moment at the front end of the element.")
  .def("getT", &XC::NLForceBeamColumn3dBase::getT,"Internal mean torque at the element.")
  .add_property("getT1", &XC::NLForceBeamColumn3dBase::getT1,"Internal torque at the back end of the element.")
  .add_property("getT2", &XC::NLForceBeamColumn3dBase::getT2,"Internal torque at the front end of the element.")
   ;

#include "forceBeamColumn/python_interface.tcc"
