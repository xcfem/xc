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

#include "frictionModel/python_interface.tcc"

class_<XC::FrictionElementBase, bases<XC::Element0D>, boost::noncopyable >("FrictionElementBase", no_init)
    // parameters
    .def("setLocalXDirection", &XC::FrictionElementBase::setLocalXDirection, "Set local x direction vector.")
    .def("getLocalXDirection", make_function(&XC::FrictionElementBase::getLocalXDirection, return_internal_reference<>() ), "Get local x direction vector.")
    .def("setLocalYDirection", &XC::FrictionElementBase::setLocalYDirection, "Set local y direction vector.")
    .def("getLocalYDirection", make_function(&XC::FrictionElementBase::getLocalYDirection, return_internal_reference<>() ), "Get local y direction vector.")
    .def("setBearingElementMass", &XC::FrictionElementBase::setBearingElementMass, "Set the mass of the bearing element.")
    .def("getBearingElementMass", make_function(&XC::FrictionElementBase::getBearingElementMass, return_internal_reference<>() ), "Get the mass of the bearing element.")
    .def("setMaxIter", &XC::FrictionElementBase::setMaxIter, "Set the maximum number of iterations.")
    .def("getMaxIter", &XC::FrictionElementBase::getMaxIter, "Get the maximum number of iterations.")
    .def("setTol", &XC::FrictionElementBase::setTol, "Set the tolerance for convergence criterion.")
    .def("getTol", make_function(&XC::FrictionElementBase::getTol, return_internal_reference<>() ), "Get the tolerance for convergence criterion.")
    .def("setLength", &XC::FrictionElementBase::setLength, "Set the element length.")
    .def("getLength", make_function(&XC::FrictionElementBase::getLength, return_internal_reference<>() ), "Get the element length.")
    
    
    // state variables
    .def("setDisplacementsInBasicSystem", &XC::FrictionElementBase::setDisplacementsInBasicSystem, "Set displacements in basic system.")
    .def("getDisplacementsInBasicSystem", make_function(&XC::FrictionElementBase::getDisplacementsInBasicSystem, return_internal_reference<>() ), "Get displacements in basic system.")
    .def("setForcesInBasicSystem", &XC::FrictionElementBase::setForcesInBasicSystem, "Set forces in basic system.")
    .def("getForcesInBasicSystem", make_function(&XC::FrictionElementBase::getForcesInBasicSystem, return_internal_reference<>() ), "Get forcesin basic system.")
    .def("setStiffnessInBasicSystem", &XC::FrictionElementBase::setStiffnessInBasicSystem, "Set stiffnes in basic system.")
    .def("getStiffnessInBasicSystem", make_function(&XC::FrictionElementBase::getStiffnessInBasicSystem, return_internal_reference<>() ), "Get stiffness in basic system.")
    .def("setDisplacementsInLocalSystem", &XC::FrictionElementBase::setDisplacementsInLocalSystem, "Set displacements in local system.")
    .def("getDisplacementsInLocalSystem", make_function(&XC::FrictionElementBase::getDisplacementsInLocalSystem, return_internal_reference<>() ), "Get displacements in local system.")
    .def("setTransformationGlobalToLocalSystem", &XC::FrictionElementBase::setTransformationGlobalToLocalSystem, "Set transformation matrix from global to local system.")
    .def("getTransformationGlobalToLocalSystem", make_function(&XC::FrictionElementBase::getTransformationGlobalToLocalSystem, return_internal_reference<>() ), "Get transformation matrix from global to local system.")
    .def("setTransformationLocalToBasicSystem", &XC::FrictionElementBase::setTransformationLocalToBasicSystem, "Set transformation matrix from local to basic system.")
    .def("getTransformationLocalToBasicSystem", make_function(&XC::FrictionElementBase::getTransformationLocalToBasicSystem, return_internal_reference<>() ), "Get transformation matrix from local to basic system.")
  ;

class_<XC::SimpleBearingBase, bases<XC::FrictionElementBase>, boost::noncopyable >("SimpleBearingBase", no_init)
    .def("setInitialStiffnessOfHystereticComponent", &XC::SimpleBearingBase::setInitialStiffnessOfHystereticComponent, "Set initial stiffness of hysteretic component.")
    .def("getInitialStiffnessOfHystereticComponent", make_function(&XC::SimpleBearingBase::getInitialStiffnessOfHystereticComponent, return_internal_reference<>() ), "Get initial stiffness of hysteretic component.")
    .def("setShearDistanceFromNodeIAsFractionOfLength", &XC::SimpleBearingBase::setShearDistanceFromNodeIAsFractionOfLength, "Set shear distance from node I as fraction of length.")
    .def("getShearDistanceFromNodeIAsFractionOfLength", make_function(&XC::SimpleBearingBase::getShearDistanceFromNodeIAsFractionOfLength, return_internal_reference<>() ), "Get shear distance from node I as fraction of length.")
    .def("setAddRayleighDampingFlag", &XC::SimpleBearingBase::setAddRayleighDampingFlag, "Set flag to add Rayleigh damping.")
    .def("getAddRayleighDampingFlag", &XC::SimpleBearingBase::getAddRayleighDampingFlag, "Get flag to add Rayleigh damping.")
    .def("setElementIsOnP0Flag", &XC::SimpleBearingBase::setElementIsOnP0Flag, "Set flag to indicate if the element is on P0.")
    .def("getElementIsOnP0Flag", &XC::SimpleBearingBase::getElementIsOnP0Flag, "Get flag to indicate if the element is on P0.")
  ;

class_<XC::FlatSliderSimple2d, bases<XC::SimpleBearingBase>, boost::noncopyable >("FlatSliderSimple2d", no_init)
  ;

class_<XC::FlatSliderSimple3d, bases<XC::SimpleBearingBase>, boost::noncopyable >("FlatSliderSimple3d", no_init)
  .def("setStiffnessFactorWhenUplift", &XC::FlatSliderSimple3d::setStiffnessFactorWhenUplift, "Set the stiffness factor when uplift is encountered.")
  .def("getStiffnessFactorWhenUplift", make_function(&XC::FlatSliderSimple3d::getStiffnessFactorWhenUplift, return_internal_reference<>() ), "Get the stiffness factor when uplift is encountered.")
  ;

class_<XC::MultiFP2d, bases<XC::Element0D>, boost::noncopyable >("MultiFP2d", no_init)
  ;

class_<XC::SingleFPSimple2d , bases<XC::FrictionElementBase>, boost::noncopyable >("SingleFPSimple2d", no_init);

class_<XC::SingleFPSimple3d , bases<XC::FrictionElementBase>, boost::noncopyable >("SingleFPSimple3d", no_init);

class_<XC::RJWatsonEQS2d , bases<XC::SimpleBearingBase>, boost::noncopyable >("RJWatsonEQS2d", no_init);

class_<XC::RJWatsonEQS3d , bases<XC::SimpleBearingBase>, boost::noncopyable >("RJWatsonEQS3d", no_init);

class_<XC::TripleFrictionPendulum, bases<XC::Element0D>, boost::noncopyable >("TripleFrictionPendulum", no_init)
  ;

//class_<XC::TripleFrictionPendulumX, bases<XC::Element0D>, boost::noncopyable >("TripleFrictionPendulumX", no_init)
  ;
