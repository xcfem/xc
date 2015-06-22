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
   ;

class_<XC::ProtoBeam3d, bases<XC::Element1D>, boost::noncopyable >("ProtoBeam3d", no_init)
  .add_property("sectionProperties", &XC::ProtoBeam3d::getSectionProperties, &XC::ProtoBeam3d::setSectionProperties)
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

class_<XC::NLForceBeamColumn2dBase, bases<XC::BeamColumnWithSectionFDTrf2d>, boost::noncopyable >("NLForceBeamColumn2dBase", no_init)
  .add_property("rho", &XC::NLForceBeamColumn2dBase::getRho,&XC::NLForceBeamColumn2dBase::setRho)
   ;

class_<XC::NLForceBeamColumn3dBase, bases<XC::BeamColumnWithSectionFDTrf3d>, boost::noncopyable >("NLForceBeamColumn3dBase", no_init)
  .add_property("rho", &XC::NLForceBeamColumn3dBase::getRho,&XC::NLForceBeamColumn3dBase::setRho)
   ;

#include "forceBeamColumn/python_interface.tcc"
