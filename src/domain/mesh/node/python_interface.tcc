//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC is free software: you can redistribute it and/or modify 
//  it under the terms of the GNU General Public License as published by
//  por la Fundación para el Software Libre, ya sea la versió 3 
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

XC::Vector &(XC::Node::*getCooRef)(void)= &XC::Node::getCrds;
XC::Vector (XC::Node::*getDistributionFactor)(int) const= &XC::Node::getDistributionFactor;
XC::Vector (XC::Node::*getDistributionFactorForGdls)(int,const std::set<int> &) const= &XC::Node::getDistributionFactor;
double (XC::Node::*getModalParticipationFactor)(int) const= &XC::Node::getModalParticipationFactor;
double (XC::Node::*getModalParticipationFactorForGdls)(int,const std::set<int> &) const= &XC::Node::getModalParticipationFactor;
XC::Vector (XC::Node::*getModalParticipationFactors)(void) const= &XC::Node::getModalParticipationFactors;
XC::Vector (XC::Node::*getModalParticipationFactorsForGdls)(const boost::python::list &) const= &XC::Node::getModalParticipationFactorsForGdls;
XC::Vector (XC::Node::*getMaxModalDisplacement)(int,const double &) const= &XC::Node::getMaxModalDisplacement;
XC::Vector (XC::Node::*getMaxModalVelocity)(int,const double &) const= &XC::Node::getMaxModalVelocity;
XC::Vector (XC::Node::*getMaxModalAcceleration)(int,const double &) const= &XC::Node::getMaxModalAcceleration;
XC::Vector (XC::Node::*getMaxModalDisplacementForGdls)(int,const double &,const boost::python::list &) const= &XC::Node::getMaxModalDisplacementForGdls;
XC::Vector (XC::Node::*getMaxModalVelocityForGdls)(int,const double &,const boost::python::list &) const= &XC::Node::getMaxModalVelocityForGdls;
XC::Vector (XC::Node::*getMaxModalAccelerationForGdls)(int,const double &,const boost::python::list &) const= &XC::Node::getMaxModalAccelerationForGdls;

void (XC::Node::*fixGdls)(const XC::ID &, const XC::Vector &)= &XC::Node::fix;
class_<XC::Node, XC::Node *, bases<XC::MeshComponent>, boost::noncopyable >("Node", no_init)
  .add_property("getCoo", make_function( getCooRef, return_internal_reference<>() ))
  .add_property("mass",make_function(&XC::Node::getMass, return_internal_reference<>()) ,&XC::Node::setMass)
  .add_property("get3dCoo", &XC::Node::getCrds3d)
  .add_property("getInitialPos2d", &XC::Node::getInitialPosition2d,"Returns 2D initial position of node.")
  .add_property("getInitialPos3d", &XC::Node::getInitialPosition3d,"Returns 3D initial position of node.")
  .def("getCurrentPos2d", &XC::Node::getCurrentPosition2d,"Returns 2D current position of node scaled by a factor: initialPos+factor*currentDisplacement.")
  .def("getCurrentPos3d", &XC::Node::getCurrentPosition3d,"Returns 3D current position of node scaled by a factor: initialPos+factor*currentDisplacement.")
  .add_property("getReaction", make_function( &XC::Node::getReaction, return_internal_reference<>() ))
  .add_property("getDisp", make_function( &XC::Node::getDisp, return_internal_reference<>() ))
  .add_property("getDispXYZ", &XC::Node::getDispXYZ)
  .add_property("getRotXYZ", &XC::Node::getRotXYZ)
  .add_property("getVel", make_function( &XC::Node::getVel, return_internal_reference<>() ))
  .add_property("getVelXYZ", &XC::Node::getVelXYZ)
  .add_property("getOmegaXYZ", &XC::Node::getOmegaXYZ)
  .add_property("getAccel", make_function( &XC::Node::getAccel, return_internal_reference<>() ))
  .add_property("getAccelXYZ", &XC::Node::getAccelXYZ)
  .add_property("getAlphaXYZ", &XC::Node::getAlphaXYZ)
  .add_property("isAlive",&XC::Node::isAlive)
  .add_property("isDead",&XC::Node::isAlive)
  .add_property("isFrozen",&XC::Node::isAlive)
  .add_property("isFree",&XC::Node::isAlive)
  .def("setTrialdDispComponent",&XC::Node::setTrialDispComponent,"Set value of trial displacement i-component.")
  .def("setTrialDisp",&XC::Node::setTrialDisp,"Set trial displacement vector.")
  .def("setTrialVel",&XC::Node::setTrialVel,"Set trial velocity vector.")
  .def("setTrialAccel",&XC::Node::setTrialAccel,"Set trial acceleration vector.")
  .def("setNumColR",&XC::Node::setNumColR)
  .def("fix",fixGdls,"Set displacement constraints")
  .def("addTributary",&XC::Node::addTributary)
  .def("resetTributary",&XC::Node::resetTributary)
  .def("getTributary",make_function(&XC::Node::getTributary,return_value_policy<copy_const_reference>()))
  .def("getTributaryArea",make_function(&XC::Node::getTributary,return_value_policy<copy_const_reference>()))
  .def("getTributaryLength",make_function(&XC::Node::getTributary,return_value_policy<copy_const_reference>()))
  .def("getTributaryVolume",make_function(&XC::Node::getTributary,return_value_policy<copy_const_reference>()))
  .def("getResistingForce",make_function(&XC::Node::getResistingForce,return_value_policy<copy_const_reference>()))
  .def("getResistingSVD3d",&XC::Node::getResistingSVD3d)

  .def("setNumEigenvectors",&XC::Node::setNumEigenvectors)
  .def("setEigenvector",&XC::Node::setEigenvector)
  .add_property("getNumModes",&XC::Node::getNumModes)
  .def("getEigenvector",&XC::Node::getEigenvector)
  .def("getNormalizedEigenvector",&XC::Node::getNormalizedEigenvector)
  .add_property("getEigenvectors",make_function(&XC::Node::getEigenvectors,return_internal_reference<>()))
  .add_property("getNormalizedEigenvectors",&XC::Node::getNormalizedEigenvectors)
    
  .def("getAngularFrequency",&XC::Node::getAngularFrequency)
  .add_property("getAngularFrequencies",&XC::Node::getAngularFrequencies)

  .def("getModalParticipationFactor",getModalParticipationFactor)
  .add_property("getModalParticipationFactors",getModalParticipationFactors)
  .def("getModalParticipationFactorForGdls",getModalParticipationFactorForGdls)
  .def("getModalParticipationFactorsForGdls",getModalParticipationFactorsForGdls)

  .def("getDistributionFactor",getDistributionFactor)
  .def("getDistributionFactorForGdls",getDistributionFactorForGdls)
  .add_property("getDistributionFactors",&XC::Node::getDistributionFactors)

  .def("getEffectiveModalMass",&XC::Node::getEffectiveModalMass)
  .add_property("getEffectiveModalMasses",&XC::Node::getEffectiveModalMasses)

  .def("getEquivalentStaticLoad",&XC::Node::getEquivalentStaticLoad)

  .def("getMaxModalDisplacement",getMaxModalDisplacement)
  .def("getMaxModalVelocity",getMaxModalVelocity)
  .def("getMaxModalAcceleration",getMaxModalAcceleration)
  .def("getMaxModalDisplacementForGdls",getMaxModalDisplacementForGdls)
  .def("getMaxModalVelocityForGdls",getMaxModalVelocityForGdls)
  .def("getMaxModalAccelerationForGdls",getMaxModalAccelerationForGdls)
   ;

class_<XC::NodeIter, boost::noncopyable >("NodeIter", no_init)
  .def("next", &XC::NodeIter::operator(), return_internal_reference<>(),"Returns next node.")
   ;
