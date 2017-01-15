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
