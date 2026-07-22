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

class_<XC::FrictionResponse, bases<XC::Response>, boost::noncopyable >("FrictionResponse", no_init);

class_<XC::FrictionModel , bases<XC::TaggedObject>, boost::noncopyable >("FrictionModel", no_init)
  .def("getTrialN", &XC::FrictionModel::getTrialN, "Get trial normal contact force.")
  .def("setTrialN", &XC::FrictionModel::setTrialN, "Set trial normal contact force.")
  .add_property("trialN", &XC::FrictionModel::getTrialN, &XC::FrictionModel::setTrialN, "Get/set trial normal contact force.")
  .def("getTrialVel", &XC::FrictionModel::getTrialVel, "Get trial velocity.")
  .def("setTrialVel", &XC::FrictionModel::setTrialVel, "Set trial velocity.")
  .add_property("trialVel", &XC::FrictionModel::getTrialVel, &XC::FrictionModel::setTrialVel, "Get/set trial velocity.")
  ;

class_<XC::Coulomb, bases<XC::FrictionModel>, boost::noncopyable >("Coulomb", no_init)
  .def("getMu", &XC::Coulomb::getMu, "Get friction coefficient.")
  .def("setMu", &XC::Coulomb::setMu, "Set friction coefficient.")
  .add_property("mu", &XC::Coulomb::getMu, &XC::Coulomb::setMu, "Get/set friction coefficient.")
  ;

class_<XC::VelDependent, bases<XC::Coulomb>, boost::noncopyable >("VelDependent", no_init)
  .def("getMuSlow", &XC::VelDependent::getMuSlow, "Get friction coefficient at slow velocity.")
  .def("setMuSlow", &XC::VelDependent::setMuSlow, "Set friction coefficient at slow velocity.")
  .add_property("muSlow", &XC::VelDependent::getMuSlow, &XC::VelDependent::setMuSlow, "Get/set friction coefficient at slow velocity.")

  .def("getMuFast", &XC::VelDependent::getMuFast, "Get friction coefficient at fast velocity.")
  .def("setMuFast", &XC::VelDependent::setMuFast, "Set friction coefficient at fast velocity.")
  .add_property("muFast", &XC::VelDependent::getMuFast, &XC::VelDependent::setMuFast, "Get/set friction coefficient at fast velocity.")

  .def("getTransRate", &XC::VelDependent::getTransRate, "Get transition rate from low to high velocity.")
  .def("setTransRate", &XC::VelDependent::setTransRate, "Set transition rate from low to high velocity.")
  .add_property("transRate", &XC::VelDependent::getTransRate, &XC::VelDependent::setTransRate, "Get/set transition rate from low to high velocity.")

  .def("getDmuDvel", &XC::VelDependent::getDmuDvel, "Get derivative of coeficent of friction with respect to to velocity.")
  .def("setDmuDvel", &XC::VelDependent::setDmuDvel, "Set derivative of COF wrt to velocity.")
  .add_property("DmuDvel", &XC::VelDependent::getDmuDvel, &XC::VelDependent::setDmuDvel, "Get/set derivative of COF wrt to velocity.")
  ;

class_<XC::VelPressureDep, bases<XC::VelDependent>, boost::noncopyable >("VelPressureDep", no_init);

class_<XC::VelNormalFrcDep, bases<XC::Coulomb>, boost::noncopyable >("VelNormalFrcDep", no_init);

class_<XC::VelDepMultiLinear, bases<XC::Coulomb>, boost::noncopyable >("VelDepMultiLinear", no_init);


