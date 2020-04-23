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
XC::Vector (XC::Node::*getDistributionFactorForDOFs)(int,const std::set<int> &) const= &XC::Node::getDistributionFactor;
double (XC::Node::*getModalParticipationFactor)(int) const= &XC::Node::getModalParticipationFactor;
double (XC::Node::*getModalParticipationFactorForDOFs)(int,const std::set<int> &) const= &XC::Node::getModalParticipationFactor;
XC::Vector (XC::Node::*getModalParticipationFactors)(void) const= &XC::Node::getModalParticipationFactors;
XC::Vector (XC::Node::*getModalParticipationFactorsForDOFs)(const boost::python::list &) const= &XC::Node::getModalParticipationFactorsForDOFs;
XC::Vector (XC::Node::*getMaxModalDisplacement)(int,const double &) const= &XC::Node::getMaxModalDisplacement;
XC::Vector (XC::Node::*getMaxModalVelocity)(int,const double &) const= &XC::Node::getMaxModalVelocity;
XC::Vector (XC::Node::*getMaxModalAcceleration)(int,const double &) const= &XC::Node::getMaxModalAcceleration;
XC::Vector (XC::Node::*getMaxModalDisplacementForDOFs)(int,const double &,const boost::python::list &) const= &XC::Node::getMaxModalDisplacementForDOFs;
XC::Vector (XC::Node::*getMaxModalVelocityForDOFs)(int,const double &,const boost::python::list &) const= &XC::Node::getMaxModalVelocityForDOFs;
XC::Vector (XC::Node::*getMaxModalAccelerationForDOFs)(int,const double &,const boost::python::list &) const= &XC::Node::getMaxModalAccelerationForDOFs;
void (XC::Node::*fixDOFs)(const XC::ID &, const XC::Vector &)= &XC::Node::fix;
bool (XC::Node::*In3D)(const GeomObj3d &,const double &,const double &) const= &XC::Node::In;
bool (XC::Node::*Out3D)(const GeomObj3d &,const double &,const double &) const= &XC::Node::Out;
class_<XC::Node, XC::Node *, bases<XC::MeshComponent>, boost::noncopyable >("Node", no_init)
  .add_property("getCoo", make_function( getCooRef, return_internal_reference<>() ),"Return node coordinates.")
.add_property("getNumberDOF", &XC::Node::getNumberDOF,"Return the number of DOFs of the node.")
  .add_property("mass",make_function(&XC::Node::getMass, return_internal_reference<>()) ,&XC::Node::setMass,"Node mass.")
  .add_property("get3dCoo", &XC::Node::getCrds3d,"Return 3D coordinates of the node.")
  .def("getPos2d", &XC::Node::getPosition2d,"getPosition2d(v), returns the 2D position obtained by adding the vector to the position of node.")
  .def("getPos3d", &XC::Node::getPosition3d,"getPosition3d(v), returns the 3D position obtained by adding the vector to the position of node..")
  .add_property("getInitialPos2d", &XC::Node::getInitialPosition2d,"Returns 2D initial position of node.")
  .add_property("getInitialPos3d", &XC::Node::getInitialPosition3d,"Returns 3D initial position of node.")
  .def("getCurrentPos2d", &XC::Node::getCurrentPosition2d,"Returns 2D current position of node scaled by a factor: initialPos+factor*currentDisplacement.")
  .def("getCurrentPos3d", &XC::Node::getCurrentPosition3d,"\n""getCurrentPos3d(factor) \n""Return 3D current position of node scaled by a factor: initialPos+factor*currentDisplacement.")
  .def("In", In3D,"\n""In(geomObject,factor,tolerance) \n""Return true if current position of node scaled by a factor: initialPos+factor*currentDisplacement lies inside the geometric object.")
  .def("Out", Out3D,"\n""Out(geomObject,factor,tolerance) \n""Return true if current position of node scaled by a factor: initialPos+factor*currentDisplacement lies outside the geometric object.")
  //Call Policies: here we don't use return_internal_reference because
  // if we do so we obtain always last calculated value and that makes impossible
  // to put history results in a Python list for example. That's why we use
  // copy_const_reference instead. 
  .add_property("getReaction", make_function( &XC::Node::getReaction, return_value_policy<copy_const_reference>() ), "Return the reaction at this node.")
  .add_property("getReactionForce3d", &XC::Node::getReactionForce3d, "Return the reaction force at this node as a 3D vector.")
  .add_property("getReactionMoment3d", &XC::Node::getReactionMoment3d, "Return the reaction moment at this node as a 3D vector.")
  .def("checkReactionForce", &XC::Node::checkReactionForce, "checkReactionForce(tolerance): return true if reactions correspond to constrained degrees of freedom, false otherwise.")
  .def("setReaction", &XC::Node::setReaction, "Set the reaction at this node (not an standard procedure).")
  .add_property("getDisp", make_function( &XC::Node::getDisp, return_value_policy<copy_const_reference>() ),"Return the displacement vector.")
  .add_property("getDispXYZ", &XC::Node::getDispXYZ, "Return the translational components of the displacement.")
  .add_property("getRotXYZ", &XC::Node::getRotXYZ, "Return the rotational components of the displacement.")
  .add_property("getVel", make_function( &XC::Node::getVel, return_value_policy<copy_const_reference>() ),"Return the velocity vector.")
  .add_property("getVelXYZ", &XC::Node::getVelXYZ,"Return the translational components of the velocity vector.")
  .add_property("getOmegaXYZ", &XC::Node::getOmegaXYZ, "Return the rotational components of the velocity vector.")
  .add_property("getAccel", make_function( &XC::Node::getAccel, return_value_policy<copy_const_reference>() ), "Return the acceleration vector.")
  .add_property("getAccelXYZ", &XC::Node::getAccelXYZ, "Return the translational components of the displacement vector.")
  .add_property("getAlphaXYZ", &XC::Node::getAlphaXYZ)
  .add_property("isAlive",&XC::Node::isAlive,"True if node is active.")
  .add_property("isDead",&XC::Node::isDead,"True if node is dead.")
  .add_property("isFrozen",&XC::Node::isFrozen,"True if node is frozen.")
  .add_property("isFree",&XC::Node::isFree,"True if node can move freely.")
  .def("setTrialdDispComponent",&XC::Node::setTrialDispComponent,"Set value of trial displacement i-component.")
  .def("setTrialDisp",&XC::Node::setTrialDisp,"Set trial displacement vector.")
  .def("setTrialVel",&XC::Node::setTrialVel,"Set trial velocity vector.")
  .def("setTrialAccel",&XC::Node::setTrialAccel,"Set trial acceleration vector.")
  .def("setNumColR",&XC::Node::setNumColR)
  .def("fix",fixDOFs,"Set displacement constraints")
  .def("addTributary",&XC::Node::addTributary)
  .def("resetTributary",&XC::Node::resetTributary)
  .def("getTributary",make_function(&XC::Node::getTributary,return_value_policy<copy_const_reference>()))
  .def("getTributaryArea",make_function(&XC::Node::getTributary,return_value_policy<copy_const_reference>()))
  .def("getTributaryLength",make_function(&XC::Node::getTributary,return_value_policy<copy_const_reference>()))
  .def("getTributaryVolume",make_function(&XC::Node::getTributary,return_value_policy<copy_const_reference>()))
  .def("getResistingForce",make_function(&XC::Node::getResistingForce,return_value_policy<copy_const_reference>()))
  .def("getResistingSlidingVectorsSystem3d",&XC::Node::getResistingSlidingVectorsSystem3d)

  .def("setNumEigenvectors",&XC::Node::setNumEigenvectors,"set the number of eigenvectors for the node.")
  .def("setEigenvector",&XC::Node::setEigenvector,"setEigenvector(mode, vector) set the igenvector for i-th mode.")
  .add_property("getNumModes",&XC::Node::getNumModes,"return the number of modes for the node.")
  .def("getEigenvector",&XC::Node::getEigenvector,"getEigenvector(i) returns the eigenvector that corresponds to i-th mode.")
  .def("getNormalizedEigenvector",&XC::Node::getNormalizedEigenvector,"getNormalizedEigenvector(i) returns the eigenvector that corresponds to i-th mode.")
  .add_property("getEigenvectors",make_function(&XC::Node::getEigenvectors,return_internal_reference<>()),"Returns all the eigenvectors for the node.")
  .add_property("getNormalizedEigenvectors",&XC::Node::getNormalizedEigenvectors,"Returns all the normalized eigenvectors for the node.")
  .def("getEigenPos2d", &XC::Node::getEigenPosition2d,"getEigenPos2d(factor, mode) returns 2D modal position of node scaled by a factor: initialPos+factor*getNormalizedEigenvector(mode).")
  .def("getEigenPos3d", &XC::Node::getEigenPosition3d,"\n""getEigenPos3d(factor, mode) \n""Return 3D modal position of node scaled by a factor: initialPos+factor*getNormalizedEigenvector(mode).")
  .def("getEigenvectorDisp3dComponents", &XC::Node::getEigenvectorDisp3dComponents, "getEigenvectorDisp3dComponents(i) return the displacement components of the eigenvector corresponding to i-th mode at this node as a 3D vector.")
  .def("getEigenvectorRot3dComponents", &XC::Node::getEigenvectorRot3dComponents, "getEigenvectorRot3dComponents(i) return the displacement components of the eigenvector corresponding to i-th mode at this node as a 3D vector.")
  .def("getNEigenvectorDisp3dComponents", &XC::Node::getEigenvectorDisp3dComponents, "getEigenvectorDisp3dComponents(i) return the displacement components of the eigenvector corresponding to i-th mode at this node as a 3D vector.")
  .def("getEigenvectorRot3dComponents", &XC::Node::getEigenvectorRot3dComponents, "getEigenvectorRot3dComponents(i) return the displacement components of the eigenvector corresponding to i-th mode at this node as a 3D vector.")
    
  .def("getAngularFrequency",&XC::Node::getAngularFrequency,"Return the angular frequency corresponding to the i-th mode.")
  .add_property("getAngularFrequencies",&XC::Node::getAngularFrequencies,"Returns the angular frequencies for all the modes.")

  .def("getModalParticipationFactor",getModalParticipationFactor,"Return the modal participation factor corresponding to the i-th mode.")
  .add_property("getModalParticipationFactors",getModalParticipationFactors,"Return the modal participation factors for all the models.")
  .def("getModalParticipationFactorForDOFs",getModalParticipationFactorForDOFs,"getModalParticipationFactorForDOFs(i,dofs): return the 'dofs' components of the modal participation factors corresponding to the i-th model.")
  .def("getModalParticipationFactorsForDOFs",getModalParticipationFactorsForDOFs,"getModalParticipationFactorsForDOFs(dofs): return the 'dofs' components of all the modal participation factors.")

  .def("getDistributionFactor",getDistributionFactor,"Returns the distribution factor corresponding to the i-th mode.")
  .def("getDistributionFactorForDOFs",getDistributionFactorForDOFs,"getDistributionFactorForDOFs(i,dofs): returns the 'dofs' components of the distribution factor corresponding to the i-th mode.")
  .add_property("getDistributionFactors",&XC::Node::getDistributionFactors,"Returns the distribution factor corresponding to all the computed modes.")

  .def("getEffectiveModalMass",&XC::Node::getEffectiveModalMass,"Returns the effective modal mass corresponding to the i-th mode.")
  .add_property("getEffectiveModalMasses",&XC::Node::getEffectiveModalMasses,"Returns the effective modal masses for all the computed modes.")
  .def("getEquivalentStaticLoad",&XC::Node::getEquivalentStaticLoad,"getEquivalentStaticLoad(mode,modeAccel): return the equivalent static load for the mode being passed as parameter and the acceleration corresponding to that mode.")

  .def("newLoad",make_function(&XC::Node::newLoad, return_internal_reference<>() ),"Create a new load on the node and put it on the current load pattern.")

  .def("getNumberOfConnectedConstraints",&XC::Node::getNumberOfConnectedConstraints,"Returns the number of constraints that affect the node.")
  .def("getNumberOfConnectedElements",&XC::Node::getNumberOfConnectedElements,"Returns the number of elements connected to the node.")

  .def("getMaxModalDisplacement",getMaxModalDisplacement)
  .def("getMaxModalVelocity",getMaxModalVelocity)
  .def("getMaxModalAcceleration",getMaxModalAcceleration)
  .def("getMaxModalDisplacementForDOFs",getMaxModalDisplacementForDOFs)
  .def("getMaxModalVelocityForDOFs",getMaxModalVelocityForDOFs)
  .def("getMaxModalAccelerationForDOFs",getMaxModalAccelerationForDOFs)
   ;

class_<XC::NodeIter, boost::noncopyable >("NodeIter", no_init)
  .def("next", &XC::NodeIter::operator(), return_internal_reference<>(),"Returns next node.")
   ;
