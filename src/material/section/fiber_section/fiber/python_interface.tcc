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

XC::UniaxialMaterial *(XC::Fiber::*getMaterialPtr)(void)= &XC::Fiber::getMaterial;
class_<XC::Fiber,XC::Fiber*, bases<XC::TaggedObject>, boost::noncopyable >("Fiber", no_init)
  .def("getMaterial",make_function(getMaterialPtr,return_internal_reference<>()))
  .def("getArea",&XC::Fiber::getArea)
  .def("setTrialFiberStrain",&XC::Fiber::setTrialFiberStrain)
	
  .def("commitState",&XC::Fiber::commitState)
  .def("revertToLastCommit",&XC::Fiber::revertToLastCommit)
  .def("revertToStart",&XC::Fiber::revertToStart)
    
  .def("getOrder",&XC::Fiber::getOrder)

  .def("isDead",&XC::Fiber::isDead)
  .def("isAlive",&XC::Fiber::isAlive)
  .def("kill",&XC::Fiber::kill)

  .def("alive",&XC::Fiber::alive)

  .def("getFiberLocation",&XC::Fiber::getFiberLocation)
  .def("getLocY",&XC::Fiber::getLocY)
  .def("getLocZ",&XC::Fiber::getLocZ)
  .def("getPos",&XC::Fiber::getPos)

  .def("getArea",&XC::Fiber::getArea,"Return the area of the fiber.")
  .def("getStrain",&XC::Fiber::getStrain,"Returns fiber strain.")
  .def("getForce",&XC::Fiber::getForce,"Returns fiber internal force.")
  .def("getMz",&XC::Fiber::getMz)
  .def("getMy",&XC::Fiber::getMy)
  ;

class_<XC::UniaxialFiber, bases<XC::Fiber>, boost::noncopyable >("UniaxialFiber", no_init)
  ;

class_<XC::UniaxialFiber2d, bases<XC::UniaxialFiber>, boost::noncopyable >("UniaxialFiber2d", no_init);

class_<XC::UniaxialFiber3d, bases<XC::UniaxialFiber>, boost::noncopyable >("UniaxialFiber3d", no_init);

typedef XC::FiberDeque::fiber_ptrs_dq fiber_ptrs_dq;
class_<fiber_ptrs_dq, boost::noncopyable >("fiber_ptrs_dq", no_init)
  .def(vector_indexing_suite<fiber_ptrs_dq>())
  ;

XC::Matrix &(XC::FiberDeque::*getHomogenizedSectionITensor)(const double &) const= &XC::FiberDeque::getIHomogenizedSection;
XC::Matrix &(XC::FiberDeque::*getHomogenizedSectionITensorRelToPoint)(const double &,const Pos2d &) const= &XC::FiberDeque::getIHomogenizedSection;
double (XC::FiberDeque::*getHomogenizedSectionIRelTo)(const double &,const Pos2d &,const XC::Vector &) const= &XC::FiberDeque::getIHomogenizedSection;
double (XC::FiberDeque::*getHomogenizedSectionIRelToLine)(const double &, const Recta2d &) const= &XC::FiberDeque::getIHomogenizedSection;
class_<XC::FiberDeque, bases<EntCmd,fiber_ptrs_dq>, boost::noncopyable >("FiberDeque", no_init)
  .def("__iter__", boost::python::iterator<XC::FiberDeque >())
  .def("insert",&XC::FiberDeque::push_back,"inserts fiber pointer.")
  .def("empty",&XC::FiberDeque::empty)
  .def("getNumFibers",&XC::FiberDeque::getNumFibers)
  .def("getCenterOfMassY",&XC::FiberDeque::getCenterOfMassY,return_value_policy<copy_const_reference>())
  .def("getCenterOfMassZ",&XC::FiberDeque::getCenterOfMassZ,return_value_policy<copy_const_reference>())
  .def("getCenterOfMass",&XC::FiberDeque::getCenterOfMass)
  .def("getRelativeY",&XC::FiberDeque::getRelativeY)
  .def("getRelativeZ",&XC::FiberDeque::getRelativeZ)

//.def("getPositions",&XC::FiberDeque::getPositions)

  .def("isSubjectedToBending",&XC::FiberDeque::isSubjectedToBending)
  .def("getResultant",&XC::FiberDeque::getResultant)
  .def("getMz",&XC::FiberDeque::getMz)
  .def("getMy",&XC::FiberDeque::getMy)
  .def("getExcentricidadMz",&XC::FiberDeque::getExcentricidadMz)
  .def("getExcentricidadMy",&XC::FiberDeque::getExcentricidadMy)
  .def("getMomentVector",&XC::FiberDeque::getMomentVector)
  .def("getResultantPosition",&XC::FiberDeque::getResultantPosition)
  .def("getNeutralAxis",&XC::FiberDeque::getNeutralAxis)

  .def("getCompressionResultant",&XC::FiberDeque::getCompressionResultant,"Return the resultant of the forces in the compressed fibers.")
  .def("getCompressedFibersMz",&XC::FiberDeque::getCompressedFibersMz,"Return the bending moment around z of the forces in the compressed fibers.")
.def("getCompressedFibersMy",&XC::FiberDeque::getCompressedFibersMy,"Return the bending moment around y of the forces in the compressed fibers.")
  .def("getCompressedFibersCentroid",&XC::FiberDeque::getCompressedFibersCentroid,return_value_policy<copy_const_reference>(),"Return the centroid of the compressed fibers.")
.def("getCentroidFibersWithStrainSmallerThan",&XC::FiberDeque::getCentroidFibersWithStrainSmallerThan,return_value_policy<copy_const_reference>(), "Return the centroid of the fibers whose strain is smaller than the argument.")

  .def("getTensionResultant",&XC::FiberDeque::getTensionResultant,"Return the resultant of the forces in the tensioned fibers.")
  .def("getTensionedFibersMz",&XC::FiberDeque::getTensionedFibersMz,"Return the bending moment around z of the forces in the tensioned fibers.")
  .def("getTensionedFibersMy",&XC::FiberDeque::getTensionedFibersMy,"Return the bending moment around y of the forces in the tensioned fibers.")
  .def("getTensionedFibersCentroid",&XC::FiberDeque::getTensionedFibersCentroid,return_value_policy<copy_const_reference>(),"Return the centroid of the tensioned fibers.")
.def("getCentroidFibersWithStrainGreaterThan",&XC::FiberDeque::getCentroidFibersWithStrainGreaterThan,return_value_policy<copy_const_reference>(), "Return the centroid of the fibers whose strain is greater than the argument.")

  .def("commitState",&XC::FiberDeque::commitState)

  .def("getStrainMin",&XC::FiberDeque::getStrainMin)
  .def("getStrainMax",&XC::FiberDeque::getStrainMax)
  .def("getStrainMed",&XC::FiberDeque::getStrainMed)
  .def("getStressMin",&XC::FiberDeque::getStressMin)
  .def("getStressMax",&XC::FiberDeque::getStressMax)
  .def("getStressMed",&XC::FiberDeque::getStressMed)
  .def("getDeformationPlane",&XC::FiberDeque::getDeformationPlane,"returns deformation plane.")
  .def("getDeformation",&XC::FiberDeque::getDeformation,return_value_policy<copy_const_reference>(),"Returns generalized strain vector.")
  .def("isTensioned",&XC::FiberDeque::isTensioned,"Return true if all the fibers are tensioned.")
  .def("isBent",&XC::FiberDeque::isBent,"Returns true if some fibers are tensioned and other are compressed (bending with or without axial force).")
  .def("enCompresion",&XC::FiberDeque::enCompresion)
  .def("getStrClaseEsfuerzo",&XC::FiberDeque::getStrClaseEsfuerzo)
  .def("getNeutralAxisDepth",&XC::FiberDeque::getNeutralAxisDepth)
  .def("getVectorBrazoMecanico",&XC::FiberDeque::getVectorBrazoMecanico)
  .def("getLeverArmSegment",&XC::FiberDeque::getLeverArmSegment)
  .def("getMechanicLeverArm",&XC::FiberDeque::getMechanicLeverArm)
  .def("getBendingPlaneTrace",&XC::FiberDeque::getBendingPlaneTrace)
  .def("getTensionedPlaneTrace",&XC::FiberDeque::getTensionedPlaneTrace)
  .def("getCompressedPlaneTrace",&XC::FiberDeque::getCompressedPlaneTrace)
  .def("computeFibersEffectiveConcreteArea",&XC::FiberDeque::computeFibersEffectiveConcreteArea)
   //.def("getFiberEffectiveConcretAreaContour",&XC::FiberDeque::getFiberEffectiveConcretAreaContour)
  .def("getFiberEffectiveConcreteArea",&XC::FiberDeque::getFiberEffectiveConcreteArea)
  .def("getFibersEffectiveConcreteArea",&XC::FiberDeque::getFibersEffectiveConcreteArea)
  .def("computeCovers",&XC::FiberDeque::computeCovers)
  .def("computeSpacement",&XC::FiberDeque::computeSpacement)
  .def("getFiberCover",&XC::FiberDeque::getFiberCover,return_value_policy<copy_const_reference>())
  .def("getFiberSpacing",&XC::FiberDeque::getFiberSpacing,return_value_policy<copy_const_reference>())
  .def("getEquivalentDiameterOfFiber",&XC::FiberDeque::getEquivalentDiameterOfFiber)
  .def("getSigmaSRAtFiber",&XC::FiberDeque::getSigmaSRAtFiber)
  .def("getAverageDistanceBetweenFibers",&XC::FiberDeque::getAverageDistanceBetweenFibers)
  .def("updateCenterOfMass",&XC::FiberDeque::updateCenterOfMass)

  .def("getYMin",&XC::FiberDeque::GetYMin)
  .def("getZMin",&XC::FiberDeque::GetZMin)
  .def("getYMax",&XC::FiberDeque::GetYMax)
  .def("getZMax",&XC::FiberDeque::GetZMax)
  .def("getPMax",&XC::FiberDeque::GetPMax)
  .def("getPMin",&XC::FiberDeque::GetPMin)
//.def("Bnd",&XC::FiberDeque::Bnd)
  .def("getArea",&XC::FiberDeque::getArea,"getArea(factor) return the area of the fibers multiplied by the factor.")
  .def("getAreaHomogenizedSection",&XC::FiberDeque::getAreaHomogenizedSection)
  .def("getCenterOfMassHomogenizedSection",make_function(&XC::FiberDeque::getCenterOfMassHomogenizedSection,return_internal_reference<>()))
  .def("getIz",&XC::FiberDeque::getIz)
  .def("getIy",&XC::FiberDeque::getIy)
  .def("getPyz",&XC::FiberDeque::getPyz)
  .def("getI1",&XC::FiberDeque::getI1)
  .def("getI2",&XC::FiberDeque::getI2)
  .def("getTh1",&XC::FiberDeque::getTh1)
  .def("getAxis1",&XC::FiberDeque::getAxis1)
  .def("getAxis2",&XC::FiberDeque::getAxis2)
  .def("getIyHomogenizedSection",&XC::FiberDeque::getIyHomogenizedSection)
  .def("getIzHomogenizedSection",&XC::FiberDeque::getIzHomogenizedSection)
  .def("getPyzHomogenizedSection",&XC::FiberDeque::getPyzHomogenizedSection)
  .def("getHomogenizedSectionITensor",make_function(getHomogenizedSectionITensor,return_internal_reference<>()))
  .def("getHomogenizedSectionITensorRelToPoint",make_function(getHomogenizedSectionITensorRelToPoint,return_internal_reference<>()))
  .def("getHomogenizedSectionIRelTo",getHomogenizedSectionIRelTo)
  .def("getHomogenizedSectionIRelToLine",getHomogenizedSectionIRelToLine)
  .def("getSzPos",&XC::FiberDeque::getSzPos)
  .def("getSzNeg",&XC::FiberDeque::getSzNeg)
  .def("getSyPos",&XC::FiberDeque::getSyPos)
  .def("getSyNeg",&XC::FiberDeque::getSyNeg)
  .def("getSPosHomogenizedSection",&XC::FiberDeque::getSPosHomogenizedSection)
  .def("getSNegHomogenizedSection",&XC::FiberDeque::getSNegHomogenizedSection)

  .def("getIpolar",&XC::FiberDeque::getIpolar)
  .def("getiz",&XC::FiberDeque::getiz)
  .def("getiy",&XC::FiberDeque::getiy)
  .def("getTagNearestFiber",&XC::FiberDeque::nearest_fiber)
   ;

class_<XC::FiberContainer , bases<XC::FiberDeque>, boost::noncopyable >("FiberContainer", no_init);

typedef std::map<std::string,XC::FiberDeque> map_fiber_sets;
class_<map_fiber_sets, boost::noncopyable >("MapFiberSets", no_init)
  .def(boost::python::map_indexing_suite<map_fiber_sets >())
  ;

class_<XC::FiberSets , bases<map_fiber_sets>, boost::noncopyable >("FiberSets", no_init)
  .def("create",make_function(&XC::FiberSets::create_fiber_set, return_internal_reference<>()),"creates set.")
  ;

