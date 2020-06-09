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
  .def("getStress",&XC::Fiber::getStress,"Returns fiber strain.")
  .def("getForce",&XC::Fiber::getForce,"Returns fiber internal force.")
  .def("getMz",&XC::Fiber::getMz)
  .def("getMy",&XC::Fiber::getMy)
  ;

class_<XC::UniaxialFiber, bases<XC::Fiber>, boost::noncopyable >("UniaxialFiber", no_init)
  ;

class_<XC::UniaxialFiber2d, bases<XC::UniaxialFiber>, boost::noncopyable >("UniaxialFiber2d", no_init);

class_<XC::UniaxialFiber3d, bases<XC::UniaxialFiber>, boost::noncopyable >("UniaxialFiber3d", no_init);

typedef XC::FiberPtrDeque::fiber_ptrs_dq fiber_ptrs_dq;
class_<fiber_ptrs_dq, boost::noncopyable >("fiber_ptrs_dq", no_init)
  .def(vector_indexing_suite<fiber_ptrs_dq>())
  ;

XC::Matrix &(XC::FiberPtrDeque::*getHomogenizedSectionITensor)(const double &) const= &XC::FiberPtrDeque::getIHomogenizedSection;
XC::Matrix &(XC::FiberPtrDeque::*getHomogenizedSectionITensorRelToPoint)(const double &,const Pos2d &) const= &XC::FiberPtrDeque::getIHomogenizedSection;
double (XC::FiberPtrDeque::*getHomogenizedSectionIRelTo)(const double &,const Pos2d &,const XC::Vector &) const= &XC::FiberPtrDeque::getIHomogenizedSection;
double (XC::FiberPtrDeque::*getHomogenizedSectionIRelToLine)(const double &, const Line2d &) const= &XC::FiberPtrDeque::getIHomogenizedSection;
class_<XC::FiberPtrDeque, bases<CommandEntity,fiber_ptrs_dq>, boost::noncopyable >("FiberPtrDeque", no_init)
//.def("__iter__", boost::python::iterator<XC::FiberPtrDeque >())
  .def("insert",&XC::FiberPtrDeque::push_back,"inserts fiber pointer.")
  .def("empty",&XC::FiberPtrDeque::empty,"Return true if there are no fibers.")
  .def("clear",&XC::FiberPtrDeque::clear,"Removes all the fibers.")
  .def("getNumFibers",&XC::FiberPtrDeque::getNumFibers)
  .def("getCenterOfMassY",&XC::FiberPtrDeque::getCenterOfMassY,return_value_policy<copy_const_reference>())
  .def("getCenterOfMassZ",&XC::FiberPtrDeque::getCenterOfMassZ,return_value_policy<copy_const_reference>())
  .def("getCenterOfMass",&XC::FiberPtrDeque::getCenterOfMass)
  .def("getRelativeY",&XC::FiberPtrDeque::getRelativeY)
  .def("getRelativeZ",&XC::FiberPtrDeque::getRelativeZ)

//.def("getPositions",&XC::FiberPtrDeque::getPositions)

  .def("isSubjectedToBending",&XC::FiberPtrDeque::isSubjectedToBending)
  .def("getResultant",&XC::FiberPtrDeque::getResultant)
  .def("getMz",&XC::FiberPtrDeque::getMz)
  .def("getMy",&XC::FiberPtrDeque::getMy)
  .def("getExcentricidadMz",&XC::FiberPtrDeque::getExcentricidadMz)
  .def("getExcentricidadMy",&XC::FiberPtrDeque::getExcentricidadMy)
  .def("getMomentVector",&XC::FiberPtrDeque::getMomentVector)
  .def("getResultantPosition",&XC::FiberPtrDeque::getResultantPosition)
  .def("getNeutralAxis",&XC::FiberPtrDeque::getNeutralAxis)

  .def("getCompressionResultant",&XC::FiberPtrDeque::getCompressionResultant,"Return the resultant of the forces in the compressed fibers.")
  .def("getCompressedFibersMz",&XC::FiberPtrDeque::getCompressedFibersMz,"Return the bending moment around z of the forces in the compressed fibers.")
.def("getCompressedFibersMy",&XC::FiberPtrDeque::getCompressedFibersMy,"Return the bending moment around y of the forces in the compressed fibers.")
  .def("getCompressedFibersCentroid",&XC::FiberPtrDeque::getCompressedFibersCentroid,return_value_policy<copy_const_reference>(),"Return the centroid of the compressed fibers.")
  .def("getCentroidFibersWithStrainSmallerThan",&XC::FiberPtrDeque::getCentroidFibersWithStrainSmallerThan,return_value_policy<copy_const_reference>(), "Return the centroid of the fibers whose strain is smaller than the argument.")

  .def("getTensionResultant",&XC::FiberPtrDeque::getTensionResultant,"Return the resultant of the forces in the tensioned fibers.")
  .def("getTensionedFibersMz",&XC::FiberPtrDeque::getTensionedFibersMz,"Return the bending moment around z of the forces in the tensioned fibers.")
  .def("getTensionedFibersMy",&XC::FiberPtrDeque::getTensionedFibersMy,"Return the bending moment around y of the forces in the tensioned fibers.")
  .def("getTensionedFibersCentroid",&XC::FiberPtrDeque::getTensionedFibersCentroid,return_value_policy<copy_const_reference>(),"Return the centroid of the tensioned fibers.")
.def("getCentroidFibersWithStrainGreaterThan",&XC::FiberPtrDeque::getCentroidFibersWithStrainGreaterThan,return_value_policy<copy_const_reference>(), "Return the centroid of the fibers whose strain is greater than the argument.")

  .def("commitState",&XC::FiberPtrDeque::commitState)

  .def("getStrainMin",&XC::FiberPtrDeque::getStrainMin)
  .def("getStrainMax",&XC::FiberPtrDeque::getStrainMax)
  .def("getStrainMed",&XC::FiberPtrDeque::getStrainMed)
  .def("getStressMin",&XC::FiberPtrDeque::getStressMin)
  .def("getStressMax",&XC::FiberPtrDeque::getStressMax)
  .def("getStressMed",&XC::FiberPtrDeque::getStressMed)
  .def("getDeformationPlane",&XC::FiberPtrDeque::getDeformationPlane,"returns deformation plane.")
  .def("getDeformation",&XC::FiberPtrDeque::getDeformation,return_value_policy<copy_const_reference>(),"Returns generalized strain vector.")
  .def("isTensioned",&XC::FiberPtrDeque::isTensioned,"Return true if all the fibers are tensioned.")
  .def("isBent",&XC::FiberPtrDeque::isBent,"Returns true if some fibers are tensioned and other are compressed (bending with or without axial force).")
  .def("isCompressed",&XC::FiberPtrDeque::isCompressed,"Return true if all the fibers are compressed.")
  .def("getStrClaseEsfuerzo",&XC::FiberPtrDeque::getStrClaseEsfuerzo)
  .def("getNeutralAxisDepth",&XC::FiberPtrDeque::getNeutralAxisDepth)
  .def("getLeverArmVector",&XC::FiberPtrDeque::getLeverArmVector)
  .def("getLeverArmSegment",&XC::FiberPtrDeque::getLeverArmSegment)
  .def("getMechanicLeverArm",&XC::FiberPtrDeque::getMechanicLeverArm)
  .def("getBendingPlaneTrace",&XC::FiberPtrDeque::getBendingPlaneTrace)
  .def("getTensionedPlaneTrace",&XC::FiberPtrDeque::getTensionedPlaneTrace)
  .def("getCompressedPlaneTrace",&XC::FiberPtrDeque::getCompressedPlaneTrace)
  .def("computeFibersEffectiveConcreteArea",&XC::FiberPtrDeque::computeFibersEffectiveConcreteArea)
   //.def("getFiberEffectiveConcretAreaContour",&XC::FiberPtrDeque::getFiberEffectiveConcretAreaContour)
  .def("getFiberEffectiveConcreteArea",&XC::FiberPtrDeque::getFiberEffectiveConcreteArea)
  .def("getFibersEffectiveConcreteArea",&XC::FiberPtrDeque::getFibersEffectiveConcreteArea)
  .def("computeCovers",&XC::FiberPtrDeque::computeCovers)
  .def("computeSpacement",&XC::FiberPtrDeque::computeSpacement)
  .def("getFiberCover",&XC::FiberPtrDeque::getFiberCover,return_value_policy<copy_const_reference>())
  .def("getFiberSpacing",&XC::FiberPtrDeque::getFiberSpacing,return_value_policy<copy_const_reference>())
  .def("getEquivalentDiameterOfFiber",&XC::FiberPtrDeque::getEquivalentDiameterOfFiber)
  .def("getSigmaSRAtFiber",&XC::FiberPtrDeque::getSigmaSRAtFiber)
  .def("getAverageDistanceBetweenFibers",&XC::FiberPtrDeque::getAverageDistanceBetweenFibers)
  .def("updateCenterOfMass",&XC::FiberPtrDeque::updateCenterOfMass)

  .def("getYMin",&XC::FiberPtrDeque::GetYMin)
  .def("getZMin",&XC::FiberPtrDeque::GetZMin)
  .def("getYMax",&XC::FiberPtrDeque::GetYMax)
  .def("getZMax",&XC::FiberPtrDeque::GetZMax)
  .def("getPMax",&XC::FiberPtrDeque::GetPMax)
  .def("getPMin",&XC::FiberPtrDeque::GetPMin)
//.def("Bnd",&XC::FiberPtrDeque::Bnd)
  .def("getArea",&XC::FiberPtrDeque::getArea,"getArea(factor) return the area of the fibers multiplied by the factor.")
  .def("getAreaHomogenizedSection",&XC::FiberPtrDeque::getAreaHomogenizedSection)
  .def("getCenterOfMassHomogenizedSection",make_function(&XC::FiberPtrDeque::getCenterOfMassHomogenizedSection,return_internal_reference<>()))
  .def("getIz",&XC::FiberPtrDeque::getIz)
  .def("getIy",&XC::FiberPtrDeque::getIy)
  .def("getPyz",&XC::FiberPtrDeque::getPyz)
  .def("getI1",&XC::FiberPtrDeque::getI1)
  .def("getI2",&XC::FiberPtrDeque::getI2)
  .def("getTh1",&XC::FiberPtrDeque::getTh1)
  .def("getAxis1",&XC::FiberPtrDeque::getAxis1)
  .def("getAxis2",&XC::FiberPtrDeque::getAxis2)
  .def("getIyHomogenizedSection",&XC::FiberPtrDeque::getIyHomogenizedSection)
  .def("getIzHomogenizedSection",&XC::FiberPtrDeque::getIzHomogenizedSection)
  .def("getPyzHomogenizedSection",&XC::FiberPtrDeque::getPyzHomogenizedSection)
  .def("getHomogenizedSectionITensor",make_function(getHomogenizedSectionITensor,return_internal_reference<>()))
  .def("getHomogenizedSectionITensorRelToPoint",make_function(getHomogenizedSectionITensorRelToPoint,return_internal_reference<>()))
  .def("getHomogenizedSectionIRelTo",getHomogenizedSectionIRelTo)
  .def("getHomogenizedSectionIRelToLine",getHomogenizedSectionIRelToLine)
  .def("getSzPos",&XC::FiberPtrDeque::getSzPos)
  .def("getSzNeg",&XC::FiberPtrDeque::getSzNeg)
  .def("getSyPos",&XC::FiberPtrDeque::getSyPos)
  .def("getSyNeg",&XC::FiberPtrDeque::getSyNeg)
  .def("getSPosHomogenizedSection",&XC::FiberPtrDeque::getSPosHomogenizedSection)
  .def("getSNegHomogenizedSection",&XC::FiberPtrDeque::getSNegHomogenizedSection)

  .def("getIpolar",&XC::FiberPtrDeque::getIpolar)
  .def("getiz",&XC::FiberPtrDeque::getiz)
  .def("getiy",&XC::FiberPtrDeque::getiy)
  .def("getTagNearestFiber",&XC::FiberPtrDeque::nearest_fiber)
   ;

class_<XC::FiberSet, bases<XC::FiberPtrDeque>, boost::noncopyable >("FiberSet", no_init)
  .def(self += self)
  .def(self + self)
  .def(self - self)
  .def(self * self)
  ;

class_<XC::FiberContainer , bases<XC::FiberPtrDeque>, boost::noncopyable >("FiberContainer", no_init)
//.def("insert",&XC::FiberContainer::insert,"insert fiber.")
  ;

typedef std::map<std::string,XC::FiberSet> map_fiber_sets;
class_<map_fiber_sets, boost::noncopyable >("map_fiber_sets", no_init)
  .def(boost::python::map_indexing_suite<map_fiber_sets >())
  ;

class_<XC::FiberSets , bases<map_fiber_sets>, boost::noncopyable >("FiberSets", no_init)
  .def("create",make_function(&XC::FiberSets::create_fiber_set, return_internal_reference<>()),"creates set.")
  ;

