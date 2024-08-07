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
class_<XC::SoilMaterialBase, bases<XC::NDMaterial>, boost::noncopyable >("SoilMaterialBase", no_init)
  .add_property("stage", &XC::SoilMaterialBase::getMaterialStage,  &XC::SoilMaterialBase::updateMaterialStage,"variable that sets the stage of the material; 0:elastic, 1:plastic.")
  .def("updateMaterialStage", &XC::SoilMaterialBase::updateMaterialStage, "Update the stage of the material; 0:elastic, 1:plastic.")
  ;

class_<XC::FluidSolidPorousMaterial, bases<XC::SoilMaterialBase>, boost::noncopyable >("FluidSolidPorousMaterial", no_init);

class_<XC::PressureMultiYieldBase, bases<XC::SoilMaterialBase>, boost::noncopyable >("PressureMultiYieldBase", no_init)
  .add_property("frictionAngle", &XC::PressureMultiYieldBase::getFrictionAngle, &XC::PressureMultiYieldBase::setFrictionAngle,"internal friction angle of the soil.")
  .add_property("cohesion", &XC::PressureMultiYieldBase::getCohesion, &XC::PressureMultiYieldBase::setCohesion,"soil cohesion.")
  .add_property("pressDependCoeff", &XC::PressureMultiYieldBase::getPressDependCoeff, &XC::PressureMultiYieldBase::setPressDependCoeff,"pressure dependency coefficient.")
  .add_property("peakShearStrain", &XC::PressureMultiYieldBase::getPeakShearStrain, &XC::PressureMultiYieldBase::setPeakShearStrain, "peak shear strain.")
  .add_property("refPressure", &XC::PressureMultiYieldBase::getRefPressure, &XC::PressureMultiYieldBase::setRefPressure, "reference pressure.")
  .add_property("numberOfSurfaces", &XC::PressureMultiYieldBase::getNumberOfSurfaces, &XC::PressureMultiYieldBase::setNumberOfSurfaces, "number of yield surfaces.")
 ;

class_<XC::PressureDependMultiYieldBase, bases<XC::PressureMultiYieldBase>, boost::noncopyable >("PressureDependMultiYieldBase", no_init);

class_<XC::PressureDependMultiYield, bases<XC::PressureDependMultiYieldBase>, boost::noncopyable >("PressureDependMultiYield", no_init);

class_<XC::PressureDependMultiYield02, bases<XC::PressureDependMultiYieldBase>, boost::noncopyable >("PressureDependMultiYield02", no_init)
  .def("setup",&XC::PressureDependMultiYield02::setupPy)
  ;


class_<XC::PressureIndependMultiYield, bases<XC::PressureMultiYieldBase>, boost::noncopyable >("PressureIndependMultiYield", no_init)
  .def("setup",&XC::PressureIndependMultiYield::setupPy)
  .add_property("refShearModulus", &XC::PressureIndependMultiYield::getRefShearModulus, &XC::PressureIndependMultiYield::setRefShearModulus,"Reference low-strain shear modulus, specified at a reference mean effective confining pressure refPress.")
  .add_property("refBulkModulus", &XC::PressureIndependMultiYield::getRefBulkModulus, &XC::PressureIndependMultiYield::setRefBulkModulus,"Reference bulk modulus, specified at a reference mean effective confining pressure refPress.")
  ;

class_<XC::T2Vector, bases<XC::MovableObject>, boost::noncopyable >("T2Vector", no_init);

#include "cyclicSoil/python_interface.tcc"
