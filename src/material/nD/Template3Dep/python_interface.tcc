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

// class_<XC::MatPoint3D, XC::MatPoint3D *, bases<CommandEntity>, boost::noncopyable >("MatPoint3D", no_init)
//   ;

class_<XC::YieldSurface, XC::YieldSurface *, bases<CommandEntity>, boost::noncopyable >("YieldSurface", no_init)
  ;

class_<XC::CamClayYieldSurface, XC::CamClayYieldSurface *, bases<XC::YieldSurface> >("CamClayYieldSurface")
  ;
class_<XC::DruckerPragerYieldSurface, XC::DruckerPragerYieldSurface *, bases<XC::YieldSurface> >("DruckerPragerYieldSurface")
  ;
class_<XC::ManzariDafaliasYieldSurface, XC::ManzariDafaliasYieldSurface *, bases<XC::YieldSurface> >("ManzariDafaliasYieldSurface")
  ;
class_<XC::RoundedMohrCoulomb01YieldSurface, XC::RoundedMohrCoulomb01YieldSurface *, bases<XC::YieldSurface> >("RoundedMohrCoulomb01YieldSurface")
  ;
class_<XC::TriFCYieldSurface, XC::TriFCYieldSurface *, bases<XC::YieldSurface> >("TriFCYieldSurface")
  ;
class_<XC::VonMisesYieldSurface, XC::VonMisesYieldSurface *, bases<XC::YieldSurface> >("VonMisesYieldSurface")
  ;

class_<XC::PotentialSurface, XC::PotentialSurface *, bases<CommandEntity>, boost::noncopyable >("PotentialSurface", no_init)
  ;

class_<XC::CamClayPotentialSurface, XC::CamClayPotentialSurface *, bases<XC::PotentialSurface> >("CamClayPotentialSurface")
  ;
class_<XC::DruckerPragerPotentialSurface, XC::DruckerPragerPotentialSurface *, bases<XC::PotentialSurface> >("DruckerPragerPotentialSurface")
  ;
class_<XC::ManzariDafaliasPotentialSurface, XC::ManzariDafaliasPotentialSurface *, bases<XC::PotentialSurface> >("ManzariDafaliasPotentialSurface")
  ;
class_<XC::RoundedMohrCoulomb01PotentialSurface, XC::RoundedMohrCoulomb01PotentialSurface *, bases<XC::PotentialSurface> >("RoundedMohrCoulomb01PotentialSurface")
  ;
class_<XC::VonMisesPotentialSurface, XC::VonMisesPotentialSurface *, bases<XC::PotentialSurface> >("VonMisesPotentialSurface")
  ;

class_<XC::EvolutionLaw_S, XC::EvolutionLaw_S *, bases<CommandEntity>, boost::noncopyable >("EvolutionLaw_S", no_init)
  ;
class_<XC::EvolutionLaw_L_Eeq, XC::EvolutionLaw_L_Eeq *, bases<XC::EvolutionLaw_S>, boost::noncopyable >("EvolutionLaw_L_Eeq", no_init)
  ;
class_<XC::EvolutionLaw_NL_Ep, XC::EvolutionLaw_NL_Ep *, bases<XC::EvolutionLaw_S>, boost::noncopyable >("EvolutionLaw_NL_Ep", no_init)
  ;
class_<XC::EvolutionLaw_NL_Eeq, XC::EvolutionLaw_NL_Eeq *, bases<XC::EvolutionLaw_S>, boost::noncopyable >("EvolutionLaw_NL_Eeq", no_init)
  ;
class_<XC::ManzariDafaliasEvolutionLaw, XC::ManzariDafaliasEvolutionLaw *, bases<XC::EvolutionLaw_S> >("ManzariDafaliasEvolutionLaw")
  ;

class_<XC::EvolutionLaw_T, XC::EvolutionLaw_T *, bases<CommandEntity>, boost::noncopyable >("EvolutionLaw_T", no_init)
  ;
class_<XC::EvolutionLaw_L_Eij, XC::EvolutionLaw_L_Eij *, bases<XC::EvolutionLaw_T>, boost::noncopyable >("EvolutionLaw_L_Eij", no_init)
  ;
class_<XC::EvolutionLaw_NL_Eij, XC::EvolutionLaw_NL_Eij *, bases<XC::EvolutionLaw_T>, boost::noncopyable >("EvolutionLaw_NL_Eij", no_init)
  ;
class_<XC::EvolutionLaw_NL_EijMD, XC::EvolutionLaw_NL_EijMD *, bases<XC::EvolutionLaw_T>, boost::noncopyable >("EvolutionLaw_NL_EijMD", no_init)
  ;

class_<XC::EPState, XC::EPState *, bases<CommandEntity>, boost::noncopyable >("EPState", no_init)
  ;

class_<XC::Template3Dep, XC::Template3Dep *, bases<XC::NDMaterial>, boost::noncopyable >("Template3Dep", no_init)
  ;

