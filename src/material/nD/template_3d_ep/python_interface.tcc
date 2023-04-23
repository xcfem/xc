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

// class_<XC::MatPoint3D, XC::MatPoint3D *, bases<CommandEntity>, boost::noncopyable >("MatPoint3D", no_init)
//   ;

class_<XC::state_vars>("ep_state_vars")
//.def(init<XC::stresstensor, XC::straintensor, XC::straintensor>(), (arg("Stressp"), arg("Strainp"), arg("ElasticStrainp")))
  .def(init<XC::stresstensor, XC::straintensor, XC::straintensor>())
  ;

class_<XC::trial_state_vars, bases<XC::state_vars> >("ep_trial_state_vars")
  .def(init<XC::stresstensor, XC::straintensor, XC::straintensor, XC::straintensor>())
  ;

class_<XC::EPState, XC::EPState *, bases<CommandEntity> >("EPState")
  .add_property("stress", make_function( &XC::EPState::getStress, return_internal_reference<>() ), &XC::EPState::setStress,"Get/set the trial stress.")
  .add_property("scalarVars", &XC::EPState::getScalarVarPy, &XC::EPState::setScalarVarPy,"Get/set the scalar vars.")
  .add_property("tensorVars", &XC::EPState::getTensorVarPy, &XC::EPState::setTensorVarPy,"Get/set the tensor vars.")
  ;

class_<XC::YieldSurface, XC::YieldSurface *, bases<CommandEntity>, boost::noncopyable >("YieldSurface", no_init)
  .def("f",&XC::YieldSurface::f, (arg("elastoPlasticState")), "Yield criterion evaluation function.")
  ;

class_<XC::CamClayYieldSurface, XC::CamClayYieldSurface *, bases<XC::YieldSurface> >("CamClayYieldSurface")
  .def(init<double>())
  .add_property("M", &XC::CamClayYieldSurface::getM, &XC::CamClayYieldSurface::setM,"Get/set the slope of critical state line.")
  ;

class_<XC::DruckerPragerYieldSurface, XC::DruckerPragerYieldSurface *, bases<XC::YieldSurface> >("DruckerPragerYieldSurface")
  ;

// class_<XC::DruckerPragerYieldSurface01, XC::DruckerPragerYieldSurface01 *, bases<XC::YieldSurface> >("DruckerPragerYieldSurface01")
//   .def(init<double>())
//   .add_property("Pc", &XC::DruckerPragerYieldSurface01::getPc, &XC::DruckerPragerYieldSurface01::setPc,"Get/set the Pc parameter.")
//   ;

class_<XC::ManzariDafaliasYieldSurface, XC::ManzariDafaliasYieldSurface *, bases<XC::YieldSurface> >("ManzariDafaliasYieldSurface")
  ;

class_<XC::RoundedMohrCoulomb01YieldSurface, XC::RoundedMohrCoulomb01YieldSurface *, bases<XC::YieldSurface> >("RoundedMohrCoulomb01YieldSurface")
  ;

class_<XC::TriFCYieldSurface, XC::TriFCYieldSurface *, bases<XC::YieldSurface> >("TriFCYieldSurface")
  .def(init<double, double, double, double>())
  .add_property("fcomp", &XC::TriFCYieldSurface::getFcomp, &XC::TriFCYieldSurface::setFcomp,"fcomp parameter.")
  .add_property("ftens", &XC::TriFCYieldSurface::getFtens, &XC::TriFCYieldSurface::setFtens,"ftens parameter.")
  .add_property("el", &XC::TriFCYieldSurface::getEl, &XC::TriFCYieldSurface::setEl,"el parameter.")
  .add_property("c", &XC::TriFCYieldSurface::getC, &XC::TriFCYieldSurface::setC,"c parameter.")
  ;

class_<XC::VonMisesYieldSurface, XC::VonMisesYieldSurface *, bases<XC::YieldSurface> >("VonMisesYieldSurface")
  ;

class_<XC::PotentialSurface, XC::PotentialSurface *, bases<CommandEntity>, boost::noncopyable >("PotentialSurface", no_init)
  .def("dQods",&XC::PotentialSurface::dQods,"dQ/dsigma_ij = 3*( S_ij )")
  .def("d2Qods2",&XC::PotentialSurface::d2Qods2,"d2Qods2 = d[ 3*(S_ij - alpha_ij) ] /dsigma_ij")
  .def("d2Qodsds1",&XC::PotentialSurface::d2Qodsds1,"The d(dQ/dsigma_ij)/ds1")
  .def("d2Qodsds2",&XC::PotentialSurface::d2Qodsds2,"The d(dQ/dsigma_ij)/ds2")
  .def("d2Qodsds3",&XC::PotentialSurface::d2Qodsds3,"The d(dQ/dsigma_ij)/ds3")
  .def("d2Qodsds4",&XC::PotentialSurface::d2Qodsds4,"The d(dQ/dsigma_ij)/ds4")
  .def("d2Qodsdt1",&XC::PotentialSurface::d2Qodsdt1,"The d(dQ/dsigma_ij)/dt1_mn")
  .def("d2Qodsdt2",&XC::PotentialSurface::d2Qodsdt2,"The d(dQ/dsigma_ij)/dt2_mn")
  .def("d2Qodsdt3",&XC::PotentialSurface::d2Qodsdt3,"The d(dQ/dsigma_ij)/dt3_mn")
  .def("d2Qodsdt4",&XC::PotentialSurface::d2Qodsdt4,"The d(dQ/dsigma_ij)/dt4_mn")
  ;

class_<XC::CamClayPotentialSurface, XC::CamClayPotentialSurface *, bases<XC::PotentialSurface> >("CamClayPotentialSurface")
  .def(init<double>())
  .add_property("M", &XC::CamClayPotentialSurface::getM, &XC::CamClayPotentialSurface::setM,"Get/set the slope of critical state line.")
  ;

class_<XC::DruckerPragerPotentialSurface, XC::DruckerPragerPotentialSurface *, bases<XC::PotentialSurface> >("DruckerPragerPotentialSurface")
  .def(init<double>())
  .add_property("alpha2", &XC::DruckerPragerPotentialSurface::getalfa2, &XC::DruckerPragerPotentialSurface::setAlfa2,"Potential surface orientation angle.")
  ;

class_<XC::ManzariDafaliasPotentialSurface, XC::ManzariDafaliasPotentialSurface *, bases<XC::PotentialSurface> >("ManzariDafaliasPotentialSurface")
  ;
class_<XC::RoundedMohrCoulomb01PotentialSurface, XC::RoundedMohrCoulomb01PotentialSurface *, bases<XC::PotentialSurface> >("RoundedMohrCoulomb01PotentialSurface")
  ;
class_<XC::VonMisesPotentialSurface, XC::VonMisesPotentialSurface *, bases<XC::PotentialSurface> >("VonMisesPotentialSurface")
  ;
// Scalar evolution laws.
class_<XC::EvolutionLaw_S, XC::EvolutionLaw_S *, bases<CommandEntity> >("EvolutionLaw_S", no_init)
  .def("h_s",&XC::EvolutionLaw_S::h_s, (arg("elastoPlasticState"), arg("PotentialSurface")), "Hardening function.")
  ;
class_<XC::EvolutionLaw_L_Eeq, XC::EvolutionLaw_L_Eeq *, bases<XC::EvolutionLaw_S> >("EvolutionLaw_L_Eeq")
  .def(init<double>())
  .add_property("a", &XC::EvolutionLaw_L_Eeq::geta, &XC::EvolutionLaw_L_Eeq::seta, "Coefficient to define the linear hardening rule.")
  ;
class_<XC::EvolutionLaw_NL_Ep, XC::EvolutionLaw_NL_Ep *, bases<XC::EvolutionLaw_S> >("EvolutionLaw_NL_Ep")
  .def(init<double, double, double>())
  .add_property("eo", &XC::EvolutionLaw_NL_Ep::geteo, &XC::EvolutionLaw_NL_Ep::seteo,"Get/set the void ratio.")
  .add_property("lmbda", &XC::EvolutionLaw_NL_Ep::getlambda, &XC::EvolutionLaw_NL_Ep::setlambda,"Get/set lambda.")
  .add_property("kappa", &XC::EvolutionLaw_NL_Ep::getkappa, &XC::EvolutionLaw_NL_Ep::setkappa,"Get/set kappa.")
  ;

class_<XC::EvolutionLaw_NL_Eeq, XC::EvolutionLaw_NL_Eeq *, bases<XC::EvolutionLaw_S> >("EvolutionLaw_NL_Eeq")
  ;
class_<XC::ManzariDafaliasEvolutionLaw, XC::ManzariDafaliasEvolutionLaw *, bases<XC::EvolutionLaw_S> >("ManzariDafaliasEvolutionLaw")
  ;

// Tensorial evolution laws.
class_<XC::EvolutionLaw_T, XC::EvolutionLaw_T *, bases<CommandEntity> >("EvolutionLaw_T", no_init)
  ;
class_<XC::EvolutionLaw_L_Eij, XC::EvolutionLaw_L_Eij *, bases<XC::EvolutionLaw_T>, boost::noncopyable >("EvolutionLaw_L_Eij")
  ;
class_<XC::EvolutionLaw_NL_Eij, XC::EvolutionLaw_NL_Eij *, bases<XC::EvolutionLaw_T>, boost::noncopyable >("EvolutionLaw_NL_Eij")
  ;
class_<XC::EvolutionLaw_NL_EijMD, XC::EvolutionLaw_NL_EijMD *, bases<XC::EvolutionLaw_T>, boost::noncopyable >("EvolutionLaw_NL_EijMD")
  ;

class_<XC::Template3Dep, XC::Template3Dep *, bases<XC::NDMaterial>, boost::noncopyable >("Template3Dep", no_init)
  .add_property("elasticMaterial", make_function( &XC::Template3Dep::getElMat, return_internal_reference<>() ), &XC::Template3Dep::setElMat,"Get/set the elastic material.")
  .add_property("yieldSurface", make_function( &XC::Template3Dep::getYS, return_internal_reference<>() ), &XC::Template3Dep::setYS,"Get/set the yield surface.")
  .add_property("potentialSurface", make_function( &XC::Template3Dep::getPS, return_internal_reference<>() ), &XC::Template3Dep::setPS,"Get/set the potential surface.")
  .add_property("elasticPlasticState", make_function( &XC::Template3Dep::getEPS, return_internal_reference<>() ), &XC::Template3Dep::setEPS,"Get/set the elastic-plastic state.")
  .add_property("scalarEvolutionLaw1", make_function( &XC::Template3Dep::getELS1, return_internal_reference<>() ), &XC::Template3Dep::setELS1,"Get/set the first scalar evolution law.")
  .add_property("scalarEvolutionLaw2", make_function( &XC::Template3Dep::getELS2, return_internal_reference<>() ), &XC::Template3Dep::setELS2,"Get/set the second scalar evolution law.")
  .add_property("scalarEvolutionLaw3", make_function( &XC::Template3Dep::getELS3, return_internal_reference<>() ), &XC::Template3Dep::setELS3,"Get/set the third scalar evolution law.")
  .add_property("scalarEvolutionLaw4", make_function( &XC::Template3Dep::getELS4, return_internal_reference<>() ), &XC::Template3Dep::setELS4,"Get/set the fourth scalar evolution law.")
  .add_property("tensorialEvolutionLaw1", make_function( &XC::Template3Dep::getELT1, return_internal_reference<>() ), &XC::Template3Dep::setELT1,"Get/set the first tensorial evolution law.")
  .add_property("tensorialEvolutionLaw2", make_function( &XC::Template3Dep::getELT2, return_internal_reference<>() ), &XC::Template3Dep::setELT2,"Get/set the second tensorial evolution law.")
  .add_property("tensorialEvolutionLaw3", make_function( &XC::Template3Dep::getELT3, return_internal_reference<>() ), &XC::Template3Dep::setELT3,"Get/set the third tensorial evolution law.")
  .add_property("tensorialEvolutionLaw4", make_function( &XC::Template3Dep::getELT4, return_internal_reference<>() ), &XC::Template3Dep::setELT4,"Get/set the fourth tensorial evolution law.")
  ;

