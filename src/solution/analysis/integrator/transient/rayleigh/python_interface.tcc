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

class_<XC::CentralDifference, bases<XC::RayleighBase>, boost::noncopyable >("CentralDifference", no_init);

class_<XC::Collocation , bases<XC::RayleighBase>, boost::noncopyable >("Collocation", no_init);

class_<XC::HHTGeneralized , bases<XC::RayleighBase>, boost::noncopyable >("HHTGeneralized", no_init);

class_<XC::WilsonTheta, bases<XC::RayleighBase>, boost::noncopyable >("WilsonTheta", no_init);

class_<XC::HHTRayleighBase, bases<XC::RayleighBase>, boost::noncopyable >("HHTRayleighBase", no_init);

class_<XC::HHTExplicit , bases<XC::HHTRayleighBase>, boost::noncopyable >("HHTExplicit", no_init);

class_<XC::HHTBase, bases<XC::HHTRayleighBase>, boost::noncopyable >("HHTBase", no_init);

class_<XC::CollocationHybridSimulation , bases<XC::HHTBase>, boost::noncopyable >("CollocationHybridSimulation", no_init);

class_<XC::HHT, bases<XC::HHTBase>, boost::noncopyable >("HHT", no_init);

class_<XC::HHTGeneralizedExplicit , bases<XC::HHTBase>, boost::noncopyable >("HHTGeneralizedExplicit", no_init);

class_<XC::HHTHybridSimulation , bases<XC::HHTBase>, boost::noncopyable >("HHTHybridSimulation", no_init);

class_<XC::AlphaOSBase , bases<XC::HHTBase>, boost::noncopyable >("AlphaOSBase", no_init);

class_<XC::AlphaOS , bases<XC::AlphaOSBase>, boost::noncopyable >("AlphaOS", no_init);

class_<XC::AlphaOSGeneralized, bases<XC::AlphaOSBase>, boost::noncopyable >("AlphaOSGeneralized", no_init);



