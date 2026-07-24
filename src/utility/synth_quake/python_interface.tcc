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

class_<XC::SDOFResponse>("SDOFResponse")
  .def(init<double, double, double, double, double>())
  .def("setMass", &XC::SDOFResponse::setMass, "Set the mass.")
  .def("getMass", &XC::SDOFResponse::getMass, "Get the mass.")
  .def("setDampingRatio", &XC::SDOFResponse::setDampingRatio, "Set the damping ratio.")
  .def("getDampingRatio", &XC::SDOFResponse::getDampingRatio, "Get the damping ratio.")
  .def("setStiffness", &XC::SDOFResponse::setStiffness, "Set the stiffness.")
  .def("getStiffness", &XC::SDOFResponse::getStiffness, "Get the stiffness.")
  .def("setYieldingStrength", &XC::SDOFResponse::setYieldingStrength, "Set the yield strength.")
  .def("getYieldingStrength", &XC::SDOFResponse::getYieldingStrength, "Get the yield strength.")
  .def("setStrainHardeningRatio", &XC::SDOFResponse::setStrainHardeningRatio, "Set the strain-hardening ratio.")
  .def("getStrainHardeningRatio", &XC::SDOFResponse::getStrainHardeningRatio, "Set the strain-hardening ratio.")
  .def("getResponse", &XC::SDOFResponse::getResponse, "Compute the SDOF response to the given excitation.")
  ;

