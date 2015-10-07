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

#include "fiber/python_interface.tcc"

XC::Fiber *(XC::FiberSectionBase::*addFiberAdHoc)(const std::string &,const double &,const XC::Vector &)= &XC::FiberSectionBase::addFiber; 
class_<XC::FiberSectionBase, bases<XC::SeccionBarraPrismatica>, boost::noncopyable >("FiberSectionBase", no_init)
  .def("addFiber",make_function(addFiberAdHoc,return_internal_reference<>()),"Adds a fiber to the section.")
  .def("getFibers",make_function(&XC::FiberSectionBase::getFibers,return_internal_reference<>()))
  .def("getFiberSets",make_function(&XC::FiberSectionBase::getFiberSets,return_internal_reference<>()))
  .def("setInitialSectionDeformation",&XC::FiberSectionBase::setInitialSectionDeformation)
  .def("setTrialSectionDeformation",&XC::FiberSectionBase::setTrialSectionDeformation)
  .def("getArea",&XC::FiberSectionBase::getArea)
  .def("getInitialSectionDeformation",make_function(&XC::FiberSectionBase::getInitialSectionDeformation,return_internal_reference<>()))
  .def("getSectionDeformation",make_function(&XC::FiberSectionBase::getSectionDeformation,return_internal_reference<>()))
  .def("getSectionDeformationByName",&XC::FiberSectionBase::getSectionDeformationByName)
  .def("getFiberSectionRepr",make_function(&XC::FiberSectionBase::getFiberSectionRepr,return_internal_reference<>()))
  .def("setupFibers",&XC::FiberSectionBase::setupFibers)
  .def("getContornoRegiones",&XC::FiberSectionBase::getContornoRegiones)
//.def("getCantoMecanicoZonaComprimida",&XC::FiberSectionBase::getCantoMecanicoZonaComprimida)
//.def("getCantoMecanicoZonaComprimida",&XC::FiberSectionBase::getCantoMecanicoZonaComprimida)
//.def("getCantoMecanicoZonaTraccionada",&XC::FiberSectionBase::getCantoMecanicoZonaTraccionada)
//.def("getCantoMecanicoZonaTraccionada",&XC::FiberSectionBase::getCantoMecanicoZonaTraccionada)
  .def("getInternalForcesAxis",&XC::FiberSectionBase::getEjeEsfuerzos)
  .def("getCantoMecanico",&XC::FiberSectionBase::getCantoMecanico)
  .def("getNeutralAxisDepth",&XC::FiberSectionBase::getNeutralAxisDepth)
  .def("getDistFibraNeutra",&XC::FiberSectionBase::getDistFibraNeutra)
  .def("getVectorBrazoMecanico",&XC::FiberSectionBase::getVectorBrazoMecanico)
  .def("getVectorCantoUtil",&XC::FiberSectionBase::getVectorCantoUtil)
  .def("getSegmentoBrazoMecanico",&XC::FiberSectionBase::getSegmentoBrazoMecanico)
  .def("getSegmentoCantoUtil",&XC::FiberSectionBase::getSegmentoCantoUtil)
  .def("getBrazoMecanico",&XC::FiberSectionBase::getBrazoMecanico)
  .def("getCantoUtil",&XC::FiberSectionBase::getCantoUtil)
  .def("getTrazaPlanoFlexion",&XC::FiberSectionBase::getTrazaPlanoFlexion)
  .def("getTrazaPlanoTraccion",&XC::FiberSectionBase::getTrazaPlanoTraccion)
  .def("getTrazaPlanoCompresion",&XC::FiberSectionBase::getTrazaPlanoCompresion)
  .def("getAnchoMecanico",&XC::FiberSectionBase::getAnchoMecanico)
  .def("getAnchoBielaComprimida",&XC::FiberSectionBase::getAnchoBielaComprimida)
  .def("getRecubrimiento",&XC::FiberSectionBase::getRecubrimiento)
  .def("getRectaLimiteAcEficaz",&XC::FiberSectionBase::getRectaLimiteAcEficaz)
  .def("getAcEficazBruta",&XC::FiberSectionBase::getAcEficazBruta)
  .def("getAcEficazNeta",&XC::FiberSectionBase::getAcEficazNeta)
  .def("calcAcEficazFibras",&XC::FiberSectionBase::calcAcEficazFibras)
  .def("getContornoAcEficazBruta",&XC::FiberSectionBase::getContornoAcEficazBruta)
  .def("getHomogenizedI",&XC::FiberSectionBase::getHomogenizedI,"Moment of inertia relative to bending axis.")
  .def("getSPosHomogeneizada",&XC::FiberSectionBase::getSPosHomogeneizada,"Static moment relative to bending axis of area that rests over this axis.")
  .def("calcRecubrimientos",&XC::FiberSectionBase::calcRecubrimientos)
  .def("calcSeparaciones",&XC::FiberSectionBase::calcSeparaciones)
  .def("getStrClaseEsfuerzo",&XC::FiberSectionBase::getStrClaseEsfuerzo)
  ;

class_<XC::FiberSection2d, bases<XC::FiberSectionBase>, boost::noncopyable >("FiberSection2d", no_init);

class_<XC::FiberSection3dBase , bases<XC::FiberSectionBase>, boost::noncopyable >("FiberSection3dBase", no_init);

class_<XC::FiberSection3d, bases<XC::FiberSection3dBase>, boost::noncopyable >("FiberSection3d", no_init);

class_<XC::FiberSectionGJ, bases<XC::FiberSection3dBase>, boost::noncopyable >("FiberSectionGJ", no_init);

class_<XC::FiberSectionShear3d , bases<XC::FiberSection3d>, boost::noncopyable >("FiberSectionShear3d", no_init)
  .def("setRespVyByName",&XC::FiberSectionShear3d::setRespVyByName)
  .def("getRespVy",make_function(&XC::FiberSectionShear3d::getRespVy,return_internal_reference<>()),"Return shear y response.")
  .def("setRespVzByName",&XC::FiberSectionShear3d::setRespVzByName)
  .def("getRespVz",make_function(&XC::FiberSectionShear3d::getRespVz,return_internal_reference<>()),"Return shear z response.")
  .def("setRespTByName",&XC::FiberSectionShear3d::setRespTByName)
  .def("getRespT",make_function(&XC::FiberSectionShear3d::getRespT,return_internal_reference<>()),"Return torsion response.")
  .def("setRespVyVzTByName",&XC::FiberSectionShear3d::setRespVyVzTByName)
  ;
