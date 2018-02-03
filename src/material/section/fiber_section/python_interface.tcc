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
class_<XC::FiberSectionBase, bases<XC::PrismaticBarCrossSection>, boost::noncopyable >("FiberSectionBase", no_init)
  .def("addFiber",make_function(addFiberAdHoc,return_internal_reference<>()),"Adds a fiber to the section.")
.def("getFibers",make_function(&XC::FiberSectionBase::getFibers,return_internal_reference<>()),"Return a fiber container with the fibers in the section.")
.def("getFiberSets",make_function(&XC::FiberSectionBase::getFiberSets,return_internal_reference<>()),"Return the fiber sets in the fiber section.")
.def("setInitialSectionDeformation",&XC::FiberSectionBase::setInitialSectionDeformation,"Set generalized initial strains values in the section from the components of the vector passed as parameter")
  .def("setTrialSectionDeformation",&XC::FiberSectionBase::setTrialSectionDeformation,"Set generalized trial strains values in the section from the components of the vector passed as parameter")
.def("getArea",&XC::FiberSectionBase::getArea,"Return the area of the fiber section")
.def("getInitialSectionDeformation",make_function(&XC::FiberSectionBase::getInitialSectionDeformation,return_internal_reference<>()),"Return a vector with the components of the generalized initial strains in the section")
.def("getSectionDeformation",make_function(&XC::FiberSectionBase::getSectionDeformation,return_internal_reference<>()),"Return a vector with the components of the material's trial generalized strain.")
  .def("getSectionDeformationByName",&XC::FiberSectionBase::getSectionDeformationByName)
.def("getFiberSectionRepr",make_function(&XC::FiberSectionBase::getFiberSectionRepr,return_internal_reference<>()),"Return the fiber section representation.")
  .def("setupFibers",&XC::FiberSectionBase::setupFibers)
.def("getRegionsContour",&XC::FiberSectionBase::getRegionsContour,"Return the polygon that contours the fiber section.")
//.def("getCompressedZoneLeverArm",&XC::FiberSectionBase::getCompressedZoneLeverArm)
//.def("getCompressedZoneLeverArm",&XC::FiberSectionBase::getCompressedZoneLeverArm)
//.def("getTensionedZoneLeverArm",&XC::FiberSectionBase::getTensionedZoneLeverArm)
//.def("getTensionedZoneLeverArm",&XC::FiberSectionBase::getTensionedZoneLeverArm)
  .def("getInternalForcesAxis",&XC::FiberSectionBase::getEjeEsfuerzos)
.def("getLeverArm",&XC::FiberSectionBase::getLeverArm,"Return the length of the segment intersection of the bending plane with the fiber section")
.def("getNeutralAxisDepth",&XC::FiberSectionBase::getNeutralAxisDepth,"Return the distance from the most compressed fiber in the section to the neutral axis")
.def("getNeutralAxisDist",&XC::FiberSectionBase::getNeutralAxisDist,"Return the distance from the point of coordinates (y,z) to the neutral axis. Syntax: getNeutralAxisDist(y,z)")
  .def("getVectorBrazoMecanico",&XC::FiberSectionBase::getVectorBrazoMecanico,"Return a vector from the centroid of tensions to the centroid of compressions.")
  .def("getVectorCantoUtil",&XC::FiberSectionBase::getVectorCantoUtil,"Return a vector from the centroid of the area in tension to the most compressed fiber.")
  .def("getSegmentoBrazoMecanico",&XC::FiberSectionBase::getSegmentoBrazoMecanico)
  .def("getSegmentoCantoUtil",&XC::FiberSectionBase::getSegmentoCantoUtil)
.def("getMechanicLeverArm",&XC::FiberSectionBase::getMechanicLeverArm,"Return the distance between the centroid of the compressed fibers and the centroid of the tensioned fibers.")
  .def("getEffectiveDepth",&XC::FiberSectionBase::getEffectiveDepth)
.def("getTrazaPlanoFlexion",&XC::FiberSectionBase::getTrazaPlanoFlexion,"Return the straight line intersection of the bending plane with the plane that contains the fiber section")
.def("getTrazaPlanoTraccion",&XC::FiberSectionBase::getTrazaPlanoTraccion,"Return the straight line intersection of a plane perpendicular to the bending plane through the centroid of the tensioned fibers with the plane that contains the fiber section")
  .def("getTrazaPlanoCompresion",&XC::FiberSectionBase::getTrazaPlanoCompresion,"Return the straight line intersection of a plane perpendicular to the bending plane through the centroid of the compressed fibers with the plane that contains the fiber section")
.def("getAnchoMecanico",&XC::FiberSectionBase::getAnchoMecanico,"Return the width of the section involved in shear checking")
.def("getCompressedStrutWidth",&XC::FiberSectionBase::getCompressedStrutWidth,"Return the compressed strut width involved in the shear analysis")
.def("getCover",&XC::FiberSectionBase::getCover,"Return the concrete cover for the 2D position as parameter.")
  .def("getEffectiveConcreteAreaLimitLine",&XC::FiberSectionBase::getEffectiveConcreteAreaLimitLine,"Return the line that limits the effective concrete area, calculated in accordance with article 49.2.4 of EHE-08 (hatched area in figure 49.2.4b). Syntax: getEffectiveConcreteAreaLimitLine(hEfMax), where hEfMax is an upper limit for the value of the effective height involved in the calculation of the effective area")
  .def("getGrossEffectiveConcreteArea",&XC::FiberSectionBase::getGrossEffectiveConcreteArea)
  .def("getNetEffectiveConcreteArea",&XC::FiberSectionBase::getNetEffectiveConcreteArea)
  .def("computeFibersEffectiveConcreteArea",&XC::FiberSectionBase::computeFibersEffectiveConcreteArea)
  .def("getGrossEffectiveConcreteAreaContour",&XC::FiberSectionBase::getGrossEffectiveConcreteAreaContour)
  .def("getHomogenizedI",&XC::FiberSectionBase::getHomogenizedI,"Moment of inertia relative to bending axis.")
  .def("getSPosHomogeneizada",&XC::FiberSectionBase::getSPosHomogeneizada,"Static moment relative to bending axis of area that rests over this axis.")
  .def("computeCovers",&XC::FiberSectionBase::computeCovers,"Return the concrete cover of the set of reinforcement fibers whose name is given as parameter. Syntax: computeCovers(nameSetArmaduras)")
.def("computeSpacement",&XC::FiberSectionBase::computeSpacement,"Return the spacing between bars in the set of reinforcement fibers whose name is given as parameter. Syntax: computeSpacement(nameSetArmaduras)")
  .def("getStrClaseEsfuerzo",&XC::FiberSectionBase::getStrClaseEsfuerzo,"Return the type of load acting at the cross-section('flexion_compuesta',...). Syntax: getStrClaseEsfuerzo(tolerance)")
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
