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

class_<XC::DamageModel,XC::DamageModel *, bases<XC::MovableObject,XC::TaggedObject>, boost::noncopyable >("DamageModel", no_init)
  .def("setTrial", &XC::DamageModel::setTrial,"Set trial vector.")
  .def("getDamage", &XC::DamageModel::getDamage,"Returns damage value.")
  .def("getPosDamage", &XC::DamageModel::getPosDamage,"Returns positive damage value.")
  .def("getNegDamage", &XC::DamageModel::getNegDamage,"Returns negative damage value.")
  .def("commitState", &XC::DamageModel::commitState,"Commits model state.")
  .def("revertToLastCommit", &XC::DamageModel::revertToLastCommit,"Returns the model to the last committed state.")
  .def("revertToStart", &XC::DamageModel::revertToStart,"Returns the model to its initial state.")
   ;

class_<XC::HystereticEnergy, bases<XC::DamageModel>, boost::noncopyable >("HystereticEnergy", no_init);

class_<XC::Kratzig, bases<XC::DamageModel>, boost::noncopyable >("Kratzig", no_init);

class_<XC::Mehanny, bases<XC::DamageModel>, boost::noncopyable >("Mehanny", no_init);

class_<XC::NormalizedPeak, bases<XC::DamageModel>, boost::noncopyable >("NormalizedPeak", no_init);

class_<XC::ParkAng, bases<XC::DamageModel>, boost::noncopyable >("ParkAng", no_init);
