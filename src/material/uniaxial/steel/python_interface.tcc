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

class_<XC::SteelBase, bases<XC::UniaxialMaterial>, boost::noncopyable >("SteelBaseMaterial", no_init)
  .add_property("E", &XC::SteelBase::getInitialTangent, &XC::SteelBase::setInitialTangent,"Initial Young's modulus.")
  .add_property("fy", &XC::SteelBase::getFy, &XC::SteelBase::setFy,"Yield stress.")
  .add_property("b", &XC::SteelBase::getHardeningRatio, &XC::SteelBase::setHardeningRatio,"Hardening ratio.")
   ;

class_<XC::Steel02, bases<XC::SteelBase> >("Steel02")
  .add_property("initialStress", &XC::Steel02::getInitialStress, &XC::Steel02::setInitialStress,"Initial stress.")
   ;

class_<XC::SteelBase0103, bases<XC::SteelBase>, boost::noncopyable >("SteelBase0103", no_init);

class_<XC::Steel01 , bases<XC::SteelBase0103>, boost::noncopyable >("Steel01", no_init);

//class_<XC::Steel03 , bases<XC::SteelBase0103>, boost::noncopyable >("Steel03", no_init);


