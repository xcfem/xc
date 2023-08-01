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

class_<XC::SteelBase, bases<XC::UniaxialMaterial>, boost::noncopyable >("SteelBaseMaterial", no_init)
  .add_property("E", &XC::SteelBase::getInitialTangent, &XC::SteelBase::setInitialTangent,"Initial Young's modulus.")
  .add_property("fy", &XC::SteelBase::getFy, &XC::SteelBase::setFy,"Yield stress.")
  .add_property("b", &XC::SteelBase::getHardeningRatio, &XC::SteelBase::setHardeningRatio,"Hardening ratio.")
  .add_property("a1", &XC::SteelBase::getA1, &XC::SteelBase::setA1,"Get/set first coefficient for isotropic hardening in compression (a1).")
  .add_property("a2", &XC::SteelBase::getA2, &XC::SteelBase::setA2,"Get/set second coefficient for isotropic hardening in compression (a2).")
  .add_property("a3", &XC::SteelBase::getA3, &XC::SteelBase::setA3,"Get/set first coefficient for isotropic hardening in tension (a3).")
  .add_property("a4", &XC::SteelBase::getA4, &XC::SteelBase::setA4,"Get/set second coefficient for isotropic hardening in tension (a4).")
   ;

class_<XC::Steel02, bases<XC::SteelBase> >("Steel02")
  .add_property("initialStress", &XC::Steel02::getInitialStress, &XC::Steel02::setInitialStress,"Initial stress.")
  .add_property("R0", &XC::Steel02::getR0, &XC::Steel02::setR0,"Get/set the first parameter that controls the transition from elastic to plastic branches (R0).")
  .add_property("cR1", &XC::Steel02::getCR1, &XC::Steel02::setCR1,"Get/set the second parameter that controls the transition from elastic to plastic branches (cR1).")
  .add_property("cR2", &XC::Steel02::getCR2, &XC::Steel02::setCR2,"Get/set the third parameter that controls the transition from elastic to plastic branches (R0).")
  .def("setParams", &XC::Steel02::setParamsPy, "parameters to control the transition from elastic to plastic branches. params=[R0,cR1,cR2]. Recommended values: R0=between 10 and 20, cR1=0.925, cR2=0.15.")
   ;

class_<XC::SteelBase0103, bases<XC::SteelBase>, boost::noncopyable >("SteelBase0103", no_init);

class_<XC::Steel01 , bases<XC::SteelBase0103>, boost::noncopyable >("Steel01", no_init);

//class_<XC::Steel03 , bases<XC::SteelBase0103>, boost::noncopyable >("Steel03", no_init);


