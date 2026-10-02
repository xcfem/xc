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

void (XC::HHT1::*set_hht1_alpha)(const double &)= &XC::HHT1::setAlpha;
double (XC::HHT1::*get_hht1_alpha)(void) const= &XC::HHT1::getAlpha;
void (XC::HHT1::*set_hht1_beta)(const double &)= &XC::HHT1::setBeta;
double (XC::HHT1::*get_hht1_beta)(void) const= &XC::HHT1::getBeta;
void (XC::HHT1::*set_hht1_gamma)(const double &)= &XC::HHT1::setGamma;
double (XC::HHT1::*get_hht1_gamma)(void) const= &XC::HHT1::getGamma;
class_<XC::HHT1, bases<XC::DampingFactorsIntegrator>, boost::noncopyable >("HHT1", no_init)
  .def("setAlpha", set_hht1_alpha, "Set the alpha factor.")
  .def("getAlpha", get_hht1_alpha, "Get the alpha factor.")
  .def("setBeta", set_hht1_beta, "Set the beta factor.")
  .def("getBeta", get_hht1_beta, "Get the beta factor.")
  .def("setGamma", set_hht1_gamma, "Set the gamma factor.")
  .def("getGamma", get_hht1_gamma, "Get the gamma factor.")
  ;

class_<XC::HHTGeneralized , bases<XC::RayleighBase>, boost::noncopyable >("HHTGeneralized", no_init);

void (XC::HHTRayleighBase::*set_hht_rayleigh_alpha)(const double &)= &XC::HHTRayleighBase::setAlpha;
double (XC::HHTRayleighBase::*get_hht_rayleigh_alpha)(void) const= &XC::HHTRayleighBase::getAlpha;
void (XC::HHTRayleighBase::*set_hht_rayleigh_gamma)(const double &)= &XC::HHTRayleighBase::setGamma;
double (XC::HHTRayleighBase::*get_hht_rayleigh_gamma)(void) const= &XC::HHTRayleighBase::getGamma;
class_<XC::HHTRayleighBase, bases<XC::RayleighBase>, boost::noncopyable >("HHTRayleighBase", no_init)
  .def("setAlpha", set_hht_rayleigh_alpha, "Set the alpha factor.")
  .def("getAlpha", get_hht_rayleigh_alpha, "Get the alpha factor.")
  .def("setGamma", set_hht_rayleigh_gamma, "Set the gamma factor.")
  .def("getGamma", get_hht_rayleigh_gamma, "Get the gamma factor.")
  ;

class_<XC::HHTExplicit , bases<XC::HHTRayleighBase>, boost::noncopyable >("HHTExplicit", no_init);

void (XC::HHTBase::*set_hht_base_beta)(const double &)= &XC::HHTBase::setBeta;
double (XC::HHTBase::*get_hht_base_beta)(void) const= &XC::HHTBase::getBeta;
class_<XC::HHTBase, bases<XC::HHTRayleighBase>, boost::noncopyable >("HHTBase", no_init)
  .def("setBeta", set_hht_base_beta, "Set the beta factor.")
  .def("getBeta", get_hht_base_beta, "Get the beta factor.")
  ;

class_<XC::HHT, bases<XC::HHTBase>, boost::noncopyable >("HHT", no_init);

void (XC::HHTBaseAlphaF::*set_hht_base_alpha_f)(const double &)= &XC::HHTBaseAlphaF::setAlphaF;
double (XC::HHTBaseAlphaF::*get_hht_base_alpha_f)(void) const= &XC::HHTBaseAlphaF::getAlphaF;
void (XC::HHTBaseAlphaF::*set_hht_base_alpha_i)(const double &)= &XC::HHTBaseAlphaF::setAlphaI;
double (XC::HHTBaseAlphaF::*get_hht_base_alpha_i)(void) const= &XC::HHTBaseAlphaF::getAlphaI;
class_<XC::HHTBaseAlphaF, bases<XC::HHTBase>, boost::noncopyable >("HHTBase", no_init)
  .def("setAlphaI", set_hht_base_alpha_i, "Set the alphaI factor.")
  .def("getAlphaI", get_hht_base_alpha_i, "Get the alphaF factor.")
  .def("setAlphaF", set_hht_base_alpha_f, "Set the alphaF factor.")
  .def("getAlphaF", get_hht_base_alpha_f, "Get the alphaF factor.")
  ;

class_<XC::HHTGeneralizedExplicit , bases<XC::HHTBaseAlphaF>, boost::noncopyable >("HHTGeneralizedExplicit", no_init);
class_<XC::HHTHybridSimulation , bases<XC::HHTBaseAlphaF>, boost::noncopyable >("HHTHybridSimulation", no_init);

class_<XC::AlphaOSBase , bases<XC::HHTBase>, boost::noncopyable >("AlphaOSBase", no_init);
class_<XC::AlphaOS , bases<XC::AlphaOSBase>, boost::noncopyable >("AlphaOS", no_init);
class_<XC::AlphaOSGeneralized, bases<XC::AlphaOSBase>, boost::noncopyable >("AlphaOSGeneralized", no_init);

class_<XC::CollocationHybridSimulation , bases<XC::HHTBase>, boost::noncopyable >("CollocationHybridSimulation", no_init);
