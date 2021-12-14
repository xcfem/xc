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

class_<XC::DampingFactorsIntegrator, bases<XC::TransientIntegrator>, boost::noncopyable >("DampingFactorsIntegrator", no_init);

class_<XC::NewmarkBase, bases<XC::DampingFactorsIntegrator>, boost::noncopyable >("NewmarkBase", no_init);

class_<XC::RayleighBase , bases<XC::DampingFactorsIntegrator>, boost::noncopyable >("RayleighBase", no_init);

class_<XC::HHT1, bases<XC::DampingFactorsIntegrator>, boost::noncopyable >("HHT1", no_init);

#include "newmark/python_interface.tcc"
#include "rayleigh/python_interface.tcc"

