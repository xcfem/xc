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

#include "truss/python_interface.tcc"

XC::ElasticSection2dPhysicalProperties &(XC::ProtoBeam2d::*getElasticSection2dPhysicalProp)(void)= &XC::ProtoBeam2d::getPhysicalProperties;
XC::CrossSectionProperties2d &(XC::ProtoBeam2d::*getCrossSectionProperties2d)(void)= &XC::ProtoBeam2d::getSectionProperties;
class_<XC::ProtoBeam2d, bases<XC::Element1D>, boost::noncopyable >("ProtoBeam2d", no_init)
  .add_property("sectionProperties", make_function(getCrossSectionProperties2d, return_internal_reference<>()), &XC::ProtoBeam2d::setSectionProperties,"Access to section properties.")
  .add_property("getPhysicalProperties", make_function(getElasticSection2dPhysicalProp, return_internal_reference<>() ),"Returns materials at integration points (gauss points).")
  .def("setSectionProperties",&XC::ProtoBeam2d::setSectionProperties,"Set cross section properties.")
  .def("getVDirStrongAxisLocalCoord",&XC::ProtoBeam2d::getVDirStrongAxisLocalCoord,"Returns the direction vector of element strong axis expressed in the local coordinate system.")
  .def("getVDirWeakAxisLocalCoord",&XC::ProtoBeam2d::getVDirWeakAxisLocalCoord,"Returns the direction vector of element weak axis expressed in the local coordinate system.")
  .def("getStrongAxisAngle",&XC::ProtoBeam2d::getStrongAxisAngle,"Returns the angle between element strong axis and local XZ plane.")
  .def("getWeakAxisAngle",&XC::ProtoBeam2d::getWeakAxisAngle,"Returns the angle between element weak axis and local XZ plane.")
   ;

XC::ElasticSection3dPhysicalProperties &(XC::ProtoBeam3d::*getElasticSection3dPhysicalProp)(void)= &XC::ProtoBeam3d::getPhysicalProperties;
XC::CrossSectionProperties3d &(XC::ProtoBeam3d::*getCrossSectionProperties3d)(void)= &XC::ProtoBeam3d::getSectionProperties;
class_<XC::ProtoBeam3d, bases<XC::Element1D>, boost::noncopyable >("ProtoBeam3d", no_init)
  .add_property("sectionProperties", make_function(getCrossSectionProperties3d, return_internal_reference<>()), &XC::ProtoBeam3d::setSectionProperties,"Access to section properties.")
  .add_property("getPhysicalProperties", make_function(getElasticSection3dPhysicalProp, return_internal_reference<>() ),"Returns materials at integration points (gauss points).")
  .def("setSectionProperties",&XC::ProtoBeam3d::setSectionProperties,"Set cross section properties.")
  .def("getVDirStrongAxisLocalCoord",&XC::ProtoBeam3d::getVDirStrongAxisLocalCoord,"Returns the direction vector of element strong axis expressed in the local coordinate system.")
  .def("getVDirWeakAxisLocalCoord",&XC::ProtoBeam3d::getVDirWeakAxisLocalCoord,"Returns the direction vector of element weak axis expressed in the local coordinate system.")
  .def("getStrongAxisAngle",&XC::ProtoBeam3d::getStrongAxisAngle,"Returns the angle between element strong axis and local XZ plane.")
  .def("getWeakAxisAngle",&XC::ProtoBeam3d::getWeakAxisAngle,"Returns the angle between element weak axis and local XZ plane.")
   ;

#include "elasticBeamColumn/python_interface.tcc"

class_<XC::BeamColumnWithSectionFD, bases<XC::Element1D>, boost::noncopyable >("BeamColumnWithSectionFD", no_init)
  .def("getNumSections",&XC::BeamColumnWithSectionFD::getNumSections)
  .def("getSections",make_function(&XC::BeamColumnWithSectionFD::getSections, return_internal_reference<>() ),"Returns element's sections.")
   ;

class_<XC::BeamColumnWithSectionFDTrf2d, bases<XC::BeamColumnWithSectionFD>, boost::noncopyable >("BeamColumnWithSectionFDTrf2d", no_init)
   ;

class_<XC::BeamColumnWithSectionFDTrf3d, bases<XC::BeamColumnWithSectionFD>, boost::noncopyable >("BeamColumnWithSectionFDTrf3d", no_init)
  .def("getVDirStrongAxisLocalCoord",&XC::BeamColumnWithSectionFDTrf3d::getVDirStrongAxisLocalCoord)
  .def("getVDirWeakAxisLocalCoord",&XC::BeamColumnWithSectionFDTrf3d::getVDirWeakAxisLocalCoord)
  .def("getStrongAxisAngle",&XC::BeamColumnWithSectionFDTrf3d::getStrongAxisAngle)
  .def("getWeakAxisAngle",&XC::BeamColumnWithSectionFDTrf3d::getWeakAxisAngle)
  .def("getVDirStrongAxisGlobalCoord",make_function(&XC::BeamColumnWithSectionFDTrf3d::getVDirStrongAxisGlobalCoord, return_value_policy<copy_const_reference>()))
  .def("getVDirWeakAxisGlobalCoord",make_function(&XC::BeamColumnWithSectionFDTrf3d::getVDirWeakAxisGlobalCoord, return_value_policy<copy_const_reference>()))
   ;
#include "beamWithHinges/python_interface.tcc"

class_<XC::NLForceBeamColumn2dBase, bases<XC::BeamColumnWithSectionFDTrf2d>, boost::noncopyable >("NLForceBeamColumn2dBase", no_init)
  .add_property("rho", &XC::NLForceBeamColumn2dBase::getRho,&XC::NLForceBeamColumn2dBase::setRho)
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
  .add_property("rho", &XC::NLForceBeamColumn3dBase::getRho,&XC::NLForceBeamColumn3dBase::setRho)
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
