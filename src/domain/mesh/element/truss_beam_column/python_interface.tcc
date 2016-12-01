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

#include "truss/python_interface.tcc"

class_<XC::ProtoBeam2d, bases<XC::Element1D>, boost::noncopyable >("ProtoBeam2d", no_init)
  .add_property("sectionProperties", &XC::ProtoBeam2d::getSectionProperties, &XC::ProtoBeam2d::setSectionProperties)
  .def("getVDirEjeFuerteLocales",&XC::ProtoBeam2d::getVDirEjeFuerteLocales,"Returns the direction vector of element strong axis expressed in the local coordinate system.")
  .def("getVDirEjeDebilLocales",&XC::ProtoBeam2d::getVDirEjeDebilLocales,"Returns the direction vector of element weak axis expressed in the local coordinate system.")
.def("getAnguloEjeFuerte",&XC::ProtoBeam2d::getAnguloEjeFuerte,"Returns the angle between element strong axis and local XZ plane.")
  .def("getAnguloEjeDebil",&XC::ProtoBeam2d::getAnguloEjeDebil,"Returns the angle between element weak axis and local XZ plane.")
   ;

class_<XC::ProtoBeam3d, bases<XC::Element1D>, boost::noncopyable >("ProtoBeam3d", no_init)
  .add_property("sectionProperties", &XC::ProtoBeam3d::getSectionProperties, &XC::ProtoBeam3d::setSectionProperties)
  .def("getVDirEjeFuerteLocales",&XC::ProtoBeam3d::getVDirEjeFuerteLocales,"Returns the direction vector of element strong axis expressed in the local coordinate system.")
  .def("getVDirEjeDebilLocales",&XC::ProtoBeam3d::getVDirEjeDebilLocales,"Returns the direction vector of element weak axis expressed in the local coordinate system.")
.def("getAnguloEjeFuerte",&XC::ProtoBeam3d::getAnguloEjeFuerte,"Returns the angle between element strong axis and local XZ plane.")
  .def("getAnguloEjeDebil",&XC::ProtoBeam3d::getAnguloEjeDebil,"Returns the angle between element weak axis and local XZ plane.")
   ;

#include "elasticBeamColumn/python_interface.tcc"

class_<XC::BeamColumnWithSectionFD, bases<XC::Element1D>, boost::noncopyable >("BeamColumnWithSectionFD", no_init)
  .def("getNumSections",&XC::BeamColumnWithSectionFD::getNumSections)
  .def("getSections",make_function(&XC::BeamColumnWithSectionFD::getSections, return_internal_reference<>() ),"Returns element's sections.")
   ;

class_<XC::BeamColumnWithSectionFDTrf2d, bases<XC::BeamColumnWithSectionFD>, boost::noncopyable >("BeamColumnWithSectionFDTrf2d", no_init)
   ;

class_<XC::BeamColumnWithSectionFDTrf3d, bases<XC::BeamColumnWithSectionFD>, boost::noncopyable >("BeamColumnWithSectionFDTrf3d", no_init)
  .def("getVDirEjeFuerteLocales",&XC::BeamColumnWithSectionFDTrf3d::getVDirEjeFuerteLocales)
  .def("getVDirEjeDebilLocales",&XC::BeamColumnWithSectionFDTrf3d::getVDirEjeDebilLocales)
  .def("getAnguloEjeFuerte",&XC::BeamColumnWithSectionFDTrf3d::getAnguloEjeFuerte)
  .def("getAnguloEjeDebil",&XC::BeamColumnWithSectionFDTrf3d::getAnguloEjeDebil)
  .def("getVDirEjeFuerteGlobales",make_function(&XC::BeamColumnWithSectionFDTrf3d::getVDirEjeFuerteGlobales, return_value_policy<copy_const_reference>()))
  .def("getVDirEjeDebilGlobales",make_function(&XC::BeamColumnWithSectionFDTrf3d::getVDirEjeDebilGlobales, return_value_policy<copy_const_reference>()))
   ;
#include "beamWithHinges/python_interface.tcc"

class_<XC::NLForceBeamColumn2dBase, bases<XC::BeamColumnWithSectionFDTrf2d>, boost::noncopyable >("NLForceBeamColumn2dBase", no_init)
  .add_property("rho", &XC::NLForceBeamColumn2dBase::getRho,&XC::NLForceBeamColumn2dBase::setRho)
  .add_property("getV", &XC::NLForceBeamColumn2dBase::getV, "Mean shear force.")
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
  .add_property("getN", &XC::NLForceBeamColumn3dBase::getN,"Mean internal axial force.")
  .add_property("getAMz1", &XC::NLForceBeamColumn3dBase::getAMz1,"Z bending moment which acts over the back end of the element.")
  .add_property("getAMz2", &XC::NLForceBeamColumn3dBase::getAMz2,"Z bending moment which acts over the front end of the element.")
  .add_property("getMz1", &XC::NLForceBeamColumn3dBase::getMz1,"Internal Z bending moment at the back end of the element.")
  .add_property("getMz2", &XC::NLForceBeamColumn3dBase::getMz2,"Internal Z bending moment at the front end of the element.")
  .add_property("getVy", &XC::NLForceBeamColumn3dBase::getVy, "Internal mean Y shear force.")
  .add_property("getAVy1", &XC::NLForceBeamColumn3dBase::getAVy1, "Y shear force wich acts at the back end of the element.")
  .add_property("getAVy2", &XC::NLForceBeamColumn3dBase::getAVy2, "Y shear force wich acts at the front end of the element.")
  .add_property("getVy1", &XC::NLForceBeamColumn3dBase::getVy1, "Internal Y shear force at the back end of the element.")
  .add_property("getVy2", &XC::NLForceBeamColumn3dBase::getVy2, "Internal Y shear force at the front end of the element.")
  .add_property("getVz", &XC::NLForceBeamColumn3dBase::getVz, "Internal mean Z shear force.")
  .add_property("getAVz1", &XC::NLForceBeamColumn3dBase::getAVz1, "Z shear force wich acts at the back end of the element.")
  .add_property("getAVz2", &XC::NLForceBeamColumn3dBase::getAVz2, "Z shear force wich acts at the back end of the element.")
  .add_property("getVz1", &XC::NLForceBeamColumn3dBase::getVz1, "Internal Z shear force at the back end of the element.")
  .add_property("getVz2", &XC::NLForceBeamColumn3dBase::getVz2, "Internal Z shear force at the front end of the element.")
  .add_property("getMy1", &XC::NLForceBeamColumn3dBase::getMy1,"Internal Y bending moment at the back end of the element.")
  .add_property("getMy2", &XC::NLForceBeamColumn3dBase::getMy2,"Internal Y bending moment at the front end of the element.")
  .add_property("getT", &XC::NLForceBeamColumn3dBase::getT,"Internal mean torque at the element.")
  .add_property("getT1", &XC::NLForceBeamColumn3dBase::getT1,"Internal torque at the back end of the element.")
  .add_property("getT2", &XC::NLForceBeamColumn3dBase::getT2,"Internal torque at the front end of the element.")
   ;

#include "forceBeamColumn/python_interface.tcc"
