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

 //class_<XC::InternalParamsA, bases<XC::MovableObject>, boost::noncopyable >("InternalParamsA", no_init);

 //class_<XC::InternalParamsIn, bases<XC::InternalParamsA>, boost::noncopyable >("InternalParamsIn", no_init);

 //class_<XC::InternalParamsLR, bases<XC::InternalParamsA>, boost::noncopyable >("InternalParamsLR", no_init);

 //class_<XC::InternalParamsLRIn, bases<XC::InternalParamsLR>, boost::noncopyable >("InternalParamsLRIn", no_init);

class_<XC::PYBase, bases<XC::UniaxialMaterial>, boost::noncopyable >("PYBase", no_init)
  .add_property("soilType", &XC::PYBase::getSoilType,  &XC::PYBase::setSoilType,"soil type (1: soft clay, 2: sand).")
  .add_property("ultimateCapacity", &XC::PYBase::getUltimateCapacity,  &XC::PYBase::setUltimateCapacity,"ultimate capacity of the p-y material (force).")
  .add_property("y50", &XC::PYBase::getY50,  &XC::PYBase::setY50,"displacement at which 50% of pult is mobilized in monotonic loading.")
  .add_property("dashPot", &XC::PYBase::getDashPot,  &XC::PYBase::setDashPot,"viscous damping term (dashpot) on the far-field (elastic) component of the displacement rate (velocity).")
  .add_property("initialTangent", &XC::PYBase::getInitialTangent,  &XC::PYBase::setInitialTangent,"initial stiffness.")
  ;

class_<XC::PQyzBase, bases<XC::PYBase>, boost::noncopyable >("PQyzBase", no_init);

class_<XC::TzSimple1, bases<XC::PYBase>, boost::noncopyable >("TzSimple1", no_init);

class_<XC::TzLiq1 , bases<XC::TzSimple1>, boost::noncopyable >("TzLiq1", no_init);

class_<XC::PySimple1, bases<XC::PQyzBase>, boost::noncopyable >("PySimple1", no_init)
  .add_property("dragResistanceFactor", &XC::PySimple1::getDragResistanceFactor,  &XC::PySimple1::setDragResistanceFactor,"variable that sets the drag resistance within a fully-mobilized gap as Cd*getUltimateCapacity().")
  .def("initialize",&XC::PySimple1::initialize,"Initialize material variables and history variables.")
  ;

class_<XC::QzSimple1 , bases<XC::PQyzBase>, boost::noncopyable >("QzSimple1", no_init)
  .add_property("suction", &XC::QzSimple1::get_suction,  &XC::QzSimple1::set_suction,"Get/set the suction parameter (Uplift resistance is equal to suction*qult). The value of suction must be 0.0 to 0.1.")
  ;

class_<XC::PyLiq1 , bases<XC::PySimple1>, boost::noncopyable >("PyLiq1", no_init);

//Generators.
class_<XC::Simple1GenBase, bases<CommandEntity>, boost::noncopyable >("Simple1GenBase", no_init);
class_<XC::PySimple1Gen, bases<XC::Simple1GenBase>, boost::noncopyable >("PySimple1Gen", no_init);
class_<XC::TzSimple1Gen, bases<XC::Simple1GenBase>, boost::noncopyable >("TzSimple1Gen", no_init);

