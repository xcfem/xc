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

class_<XC::DamperBase, bases<XC::UniaxialMaterial>, boost::noncopyable >("DamperBase", no_init)
    .def("setElasticStiffness",&XC::DamperBase::setElasticStiffness, "Set the axial linear elastic stiffness.")
    .def("getElasticStiffness",&XC::DamperBase::getElasticStiffness, "Get the axial linear elastic stiffness.")
    .add_property("K",&XC::DamperBase::getElasticStiffness,&XC::DamperBase::setElasticStiffness, "Elastic stiffness of linear spring to model the axial flexibility of a viscous damper (e.g. combined stiffness of the supporting brace and internal damper portion") 

    .def("setDampingCoeff",&XC::DamperBase::setDampingCoeff, "Set the damping coefficient.")
    .def("getDampingCoeff",&XC::DamperBase::getDampingCoeff, "Get the damping coefficient.")
    .add_property("C",&XC::DamperBase::getDampingCoeff,&XC::DamperBase::getDampingCoeff, "Damping coefficient.")
      
    .def("setGapLength",&XC::DamperBase::setGapLength, "Set the gap length due to pin tolerance.")
    .def("getGapLength",&XC::DamperBase::getGapLength, "Get the gap length due to pin tolerance.")
    .add_property("LGap",&XC::DamperBase::getGapLength,&XC::DamperBase::setGapLength, "Gap length to simulate the gap length due to the pin tolerance.")
      
    .def("setNumericalAlgorithm",&XC::DamperBase::setNumericalAlgorithm, "Set the numerical algorithm  \n1 = Dormand-Prince54, \n2=6th order Adams-Bashforth-Moulton, \n3=modified Rosenbrock Triple)")
    .def("getNumericalAlgorithm",&XC::DamperBase::getNumericalAlgorithm, "Get the numerical algorithm  \n1 = Dormand-Prince54, \n2=6th order Adams-Bashforth-Moulton, \n3=modified Rosenbrock Triple)")
   .add_property("NM",&XC::DamperBase::getNumericalAlgorithm,&XC::DamperBase::setNumericalAlgorithm, "Employed adaptive numerical algorithm (default value NM = 1; 1 = Dormand-Prince54, 2=6th order Adams-Bashforth-Moulton, 3=modified Rosenbrock Triple.")
      
    .def("setRelativeTolerance",&XC::DamperBase::setRelativeTolerance, "Set the tolerance for absolute relative error control of the adaptive iterative algorithm.")    
    .def("getRelativeTolerance",&XC::DamperBase::getRelativeTolerance, "Get the tolerance for absolute relative error control of the adaptive iterative algorithm.")
     .add_property("RelTol",&XC::DamperBase::getRelativeTolerance,&XC::DamperBase::setRelativeTolerance, "Tolerance for absolute relative error control of the adaptive iterative algorithm (default value 10^-6).") 
      
    .def("setAbsoluteTolerance",&XC::DamperBase::setAbsoluteTolerance, "Set the tolerance for absolute error control of adaptive iterative algorithm.")
    .def("getAbsoluteTolerance",&XC::DamperBase::getAbsoluteTolerance, "Get the tolerance for absolute error control of adaptive iterative algorithm.")
    .add_property("AbsTol",&XC::DamperBase::getAbsoluteTolerance,&XC::DamperBase::setAbsoluteTolerance, "Tolerance for absolute error control of adaptive iterative algorithm (default value 10^-10).") 
      
    .def("setMaxIter",&XC::DamperBase::setMaxIter, "Set the maximum number of sub-step iterations within an integration step.")
    .def("getMaxIter",&XC::DamperBase::getMaxIter, "Get the maximum number of sub-step iterations within an integration step.")
    .add_property("MaxHalf",&XC::DamperBase::getMaxIter,&XC::DamperBase::setMaxIter, "Maximum number of sub-step iterations within an integration step (default value 15).")

    .def("setup", &XC::DamperBase::setup, "Initialize values.")
   ;

class_<XC::BilinearOilDamper, bases<XC::DamperBase> >("BilinearOilDamper")
    .def("setDamperReliefLoad",&XC::BilinearOilDamper::setDamperReliefLoad, "Get damper relief load.")
    .def("getDamperReliefLoad",&XC::BilinearOilDamper::getDamperReliefLoad, "Set damper relief load.")
    .add_property("",&XC::BilinearOilDamper::getDamperReliefLoad,&XC::BilinearOilDamper::setDamperReliefLoad, "Get/set the value of the damper relief load.")

    .def("setPostReliefViscousDamping",&XC::BilinearOilDamper::setPostReliefViscousDamping, "Set the value of the post-relief viscous damping coefficient ratio.")
    .def("getDamperPostReliefViscousDamping",&XC::BilinearOilDamper::getDamperPostReliefViscousDamping, "Get the value of the post-relief viscous damping coefficient ratio.")
    .add_property("p",&XC::BilinearOilDamper::getDamperPostReliefViscousDamping, &XC::BilinearOilDamper::setPostReliefViscousDamping,"Get/set the value of the post-relief viscous damping coefficient ratio.")

   ;

class_<XC::ViscousDamper, bases<XC::DamperBase> >("ViscousDamper")
    .def("setVelocityExponent",&XC::ViscousDamper::setVelocityExponent, "Set the velocity exponent.")
    .def("getVelocityExponent",&XC::ViscousDamper::getVelocityExponent, "Get the velocity exponent.")
    .add_property("Alpha",&XC::ViscousDamper::getVelocityExponent,&XC::ViscousDamper::setVelocityExponent, "Get/set the velocity exponent.")
   ;



