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

class_<XC::PseudoTimeTracker, bases<XC::MovableObject>, boost::noncopyable >("PseudoTimeTracker", no_init)
  .add_property("getCurrentTime", make_function(&XC::PseudoTimeTracker::getCurrentTime, return_value_policy<copy_const_reference>() ),"return current pseudo-time.")
  .add_property("getCommittedTime", make_function(&XC::PseudoTimeTracker::getCommittedTime, return_value_policy<copy_const_reference>() ),"return committed pseudo-time.")
  .add_property("getDt", make_function(&XC::PseudoTimeTracker::getDt, return_value_policy<copy_const_reference>() ),"return difference between committed and current time.")
  .add_property("getEigenValueTimeSet", make_function(&XC::PseudoTimeTracker::getEigenValueTimeSet, return_value_policy<copy_const_reference>() ),"return eigenvalue time set.")
  
  ;

XC::Mesh &(XC::Domain::*getMeshRef)(void)= &XC::Domain::getMesh;
XC::Preprocessor *(XC::Domain::*getPreprocessor)(void)= &XC::Domain::getPreprocessor;
XC::ConstrContainer &(XC::Domain::*getConstraintsRef)(void)= &XC::Domain::getConstraints;
class_<XC::Domain, bases<XC::ObjWithRecorders>, boost::noncopyable >("Domain", no_init)
  .add_property("getPreprocessor", make_function( getPreprocessor, return_internal_reference<>() ),"returns preprocessor.")
  .add_property("getMesh", make_function( getMeshRef, return_internal_reference<>() ),"returns finite element mesh.")
  .add_property("getConstraints", make_function( getConstraintsRef, return_internal_reference<>() ),"returns mesh constraints.")
  .add_property("getTimeTracker", make_function( &XC::Domain::getTimeTracker, return_internal_reference<>() ),"returns the pseudo-time tracker of the domain.")
  .add_property("currentCombinationName", &XC::Domain::getCurrentCombinationName,"returns current combination/load case name.")
  .def("setDeadSRF",XC::Domain::setDeadSRF,"Assigns Stress Reduction Factor for element deactivation.")
  .def("commit",&XC::Domain::commit)
  .def("revertToLastCommit",&XC::Domain::revertToLastCommit)
  .def("revertToStart",&XC::Domain::revertToStart)  
  .def("setLoadConstant",&XC::Domain::setLoadConstant,"sets currents load patterns as constant in time.")  
  .def("setTime",&XC::Domain::setTime,"sets the time on the time tracker.")  
  .def("setRayleighDampingFactors",&XC::Domain::setRayleighDampingFactors,"sets the Rayleigh damping factors.")  
  .def("calculateNodalReactions",&XC::Domain::calculateNodalReactions,"triggers nodal reaction calculation.")  
  .def("checkNodalReactions",&XC::Domain::checkNodalReactions,"checkNodalReactions(tolerance): check that reactions at nodes correspond to constrained degrees of freedom.")
  .def("removeAllLoadPatterns",&XC::Domain::removeAllLoadPatterns,"removeAllLoadPatterns(): remove all load patterns from domain.")
  .add_property("numModes",&XC::Domain::getNumModes,"numModes: return the number of computed eigenmodes.")
  .def("setEigenvalues",&XC::Domain::setEigenvalues,"setEigenvalues(xcVector): set the eigenvalues.")
  .def("getEigenvalue", &XC::Domain::getEigenvalue, return_value_policy<copy_const_reference>(),"getEigenvalue(i): return the i-th eigenvalue.")
  .def("getAngularFrequency",&XC::Domain::getAngularFrequency,"getAngularFrequency(mode): return the natural frequency corresponding to the mode.")
  .def("getPeriod",&XC::Domain::getPeriod,"getPeriod(mode): return the period T corresponding to the mode.")
  .def("getFrequency",&XC::Domain::getFrequency,"getFrequency(mode): return the frequency f corresponding to the mode.")
  .def("getEigenvalues", make_function( &XC::Domain::getEigenvalues, return_internal_reference<>() ),"getEigenvalues(): return the computed eigenvalues.")
  .def("getAngularFrequencies",&XC::Domain::getAngularFrequencies,"getAngularFrequencies(): return the computed natural frequencies.")
  .def("getPeriods",&XC::Domain::getPeriods,"getPeriods(): return the computed periods.")
  .def("getFrequencies",&XC::Domain::getFrequencies,"getFrequencies(): return the computed frequencies.")
  .def("setModalParticipationFactors",&XC::Domain::setModalParticipationFactors,"setModalParticipationFactors(): set the modal participation factors.")
  .def("getModalParticipationFactor",&XC::Domain::getModalParticipationFactor, return_value_policy<copy_const_reference>(),"getModalParticipationFactor(mode): get the modal participation factor corresponding to the mode.")
  .def("getModalParticipationFactors", make_function( &XC::Domain::getModalParticipationFactors, return_internal_reference<>() ),"getModalParticipationFactors(): return the computed modal participation factors.")
  .def("getEffectiveModalMass",&XC::Domain::getEffectiveModalMass,"getEffectiveModalMass(mode): return the effective modal mass corresponding to the mode.")
  .def("getEffectiveModalMasses",&XC::Domain::getEffectiveModalMasses,"getEffectiveModalMasses(): return the computed effective modal masses.")
  .def("getTotalEffectiveModalMass",&XC::Domain::getTotalEffectiveModalMass,"getTotalEffectiveModalMass(): return the total mass.")
  .add_property("totalMass", &XC::Domain::getTotalMass, "Return the total mass matrix.")
  .def("getTotalMassComponent", &XC::Domain::getTotalMassComponent,"Return the total mass matrix component for the DOF argument.")
  ;
