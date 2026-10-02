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

// class_<XC::ResponseQuantities, bases<XC::MovableObject>, boost::noncopyable >("ResponseQuantities", no_init);

class_<XC::CentralDifferenceBase , bases<XC::TransientIntegrator>, boost::noncopyable >("CentralDifferenceBase", no_init);
class_<XC::CentralDifferenceAlternative , bases<XC::CentralDifferenceBase>, boost::noncopyable >("CentralDifferenceAlternative", no_init);
class_<XC::CentralDifferenceNoDamping, bases<XC::CentralDifferenceBase>, boost::noncopyable >("CentralDifferenceNoDamping", no_init);

void (XC::DampingFactorsIntegrator::*set_rayleigh_damping_factors)(const XC::RayleighDampingFactors &)= &XC::DampingFactorsIntegrator::setRayleighDampingFactors;
const XC::RayleighDampingFactors &(XC::DampingFactorsIntegrator::*get_rayleigh_damping_factors)(void) const= &XC::DampingFactorsIntegrator::getRayleighDampingFactors;
class_<XC::DampingFactorsIntegrator, bases<XC::TransientIntegrator>, boost::noncopyable >("DampingFactorsIntegrator", no_init)
  .def("setRayleighDampingFactors", set_rayleigh_damping_factors, "Set the Rayleigh damping factors.")
  .def("getRayleighDampingFactors", make_function(get_rayleigh_damping_factors, return_internal_reference<>()), "Get the Rayleigh damping factors.")
  ;

void (XC::NewmarkBase::*set_newmark_gamma)(const double &)= &XC::NewmarkBase::setGamma;
double (XC::NewmarkBase::*get_newmark_gamma)(void) const= &XC::NewmarkBase::getGamma;
class_<XC::NewmarkBase, bases<XC::DampingFactorsIntegrator>, boost::noncopyable >("NewmarkBase", no_init)
  .def("setGamma", set_newmark_gamma, "Set the gamma factor.")
  .def("getGamma", get_newmark_gamma, "Get the gamma factor.")
  ;

void (XC::RayleighBase::*set_rayleigh_base_deltaT)(const double &)= &XC::RayleighBase::setDeltaT;
double (XC::RayleighBase::*get_rayleigh_base_deltaT)(void) const= &XC::RayleighBase::getDeltaT;
class_<XC::RayleighBase , bases<XC::DampingFactorsIntegrator>, boost::noncopyable >("RayleighBase", no_init)
  .def("setDeltaT", set_rayleigh_base_deltaT, "Set the deltaT value.")
  .def("getDeltaT", get_rayleigh_base_deltaT, "Get the deltaT value.")
  ;

class_<XC::TRBDFBase, bases<XC::TransientIntegrator>, boost::noncopyable >("TRBDFBase", no_init);
class_<XC::TRBDF2, bases<XC::TRBDFBase>, boost::noncopyable >("TRBDF2", no_init);
class_<XC::TRBDF3, bases<XC::TRBDFBase>, boost::noncopyable >("TRBDF3", no_init);

#include "newmark/python_interface.tcc"
#include "rayleigh/python_interface.tcc"

