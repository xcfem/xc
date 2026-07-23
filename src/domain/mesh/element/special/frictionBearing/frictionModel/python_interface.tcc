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

//class_<XC::FrictionResponse, bases<XC::Response>, boost::noncopyable >("FrictionResponse", no_init);

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

class_<XC::VelPressureDep, bases<XC::VelDependent>, boost::noncopyable >("VelPressureDep", no_init)
  .def("setNominalContactArea", &XC::VelPressureDep::setNominalContacArea, "Set value for nominal contact area.")
  .def("getNominalContactArea", &XC::VelPressureDep::getNominalContacArea, "Get value for nominal contact area.")
  .add_property("A", &XC::VelPressureDep::getNominalContacArea, &XC::VelPressureDep::setNominalContacArea, "nominal contact area.")

  .def("setDeltaMuPressureParameter", &XC::VelPressureDep::setDeltaMuPressureParameter, "Set DeltaMu pressure parameter.")
  .def("getDeltaMuPressureParameter", &XC::VelPressureDep::getDeltaMuPressureParameter, "Get DeltaMu pressure parameter.")
  .add_property("deltaMu", &XC::VelPressureDep::getDeltaMuPressureParameter, &XC::VelPressureDep::setDeltaMuPressureParameter, "deltaMu pressure parameter.")

  .def("setAlphaPressureParameter", &XC::VelPressureDep::setAlphaPressureParameter, "Set value for alpha pressure parameter.")
  .def("getAlphaPressureParameter", &XC::VelPressureDep::getAlphaPressureParameter, "Get value for alpha pressure parameter.")
  .add_property("alpha", &XC::VelPressureDep::getAlphaPressureParameter, &XC::VelPressureDep::setAlphaPressureParameter, "alpha pressure parameter.")

  // read-only, since this looks like a derived/output quantity.
  .add_property("DmuDn", &XC::VelPressureDep::getDmuDn, "derivative of friction coefficient with respect to normal force.")
  ;

class_<XC::VelNormalFrcDep, bases<XC::Coulomb>, boost::noncopyable >("VelNormalFrcDep", no_init)
  .add_property("aSlow", &XC::VelNormalFrcDep::getASlow, &XC::VelNormalFrcDep::setASlow, "constant for slow friction coefficient.")
  .add_property("nSlow", &XC::VelNormalFrcDep::getNSlow, &XC::VelNormalFrcDep::setNSlow, "normal force exponent for slow friction coefficient (nSlow <= 0)")
  .add_property("aFast", &XC::VelNormalFrcDep::getAFast, &XC::VelNormalFrcDep::setAFast, "constant for fast friction coefficient.")
  .add_property("nFast", &XC::VelNormalFrcDep::getNFast, &XC::VelNormalFrcDep::setNFast, "normal force exponent for fast friction coefficient (nFast <= 0)")
  .add_property("alpha0", &XC::VelNormalFrcDep::getAlpha0, &XC::VelNormalFrcDep::setAlpha0, "constant rate parameter.")
  .add_property("alpha1", &XC::VelNormalFrcDep::getAlpha1, &XC::VelNormalFrcDep::setAlpha1, "linear rate parameter.")
  .add_property("alpha2", &XC::VelNormalFrcDep::getAlpha2, &XC::VelNormalFrcDep::setAlpha2, "quadratic rate parameter.")
  .add_property("maxMuFact", &XC::VelNormalFrcDep::getMaxMuFact, &XC::VelNormalFrcDep::setMaxMuFact, "factor for determining the maximum friction coefficients.")

  // read-only, since these look like derived/output quantities
  .add_property("DmuDn", &XC::VelNormalFrcDep::getDmuDn, "derivative of friction coefficient with respect normal force.")
  .add_property("DmuDvel", &XC::VelNormalFrcDep::getDmuDvel, "derivative of friction coefficient with respect to to velocity.")
  ;

class_<XC::VelDepMultiLinear, bases<XC::Coulomb>, boost::noncopyable >("VelDepMultiLinear", no_init)
  .add_property("velocityFrictionPoints", &XC::VelDepMultiLinear::getVelocityFrictionPoints, &XC::VelDepMultiLinear::setVelocityFrictionPoints, "Get/set the list of tuples (velocity, COF) that define the bearing behavior.")
  .add_property("trialID", &XC::VelDepMultiLinear::getTrialID, &XC::VelDepMultiLinear::setTrialID, "trial ID into velocity, friction arrays.")
  .add_property("trialIDmin", &XC::VelDepMultiLinear::getTrialIDmin, &XC::VelDepMultiLinear::setTrialIDmin, "minimum of trial ID.")
  .add_property("trialIDmax", &XC::VelDepMultiLinear::getTrialIDmax, &XC::VelDepMultiLinear::setTrialIDmax, "maximum of trial ID.")
  ;


