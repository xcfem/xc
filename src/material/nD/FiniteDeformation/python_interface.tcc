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

class_<XC::FiniteDeformationElastic3D , bases<XC::NDMaterial>, boost::noncopyable >("FiniteDeformationElastic3D", no_init);

class_<XC::FiniteDeformationEP3D, bases<XC::NDMaterial>, boost::noncopyable >("FiniteDeformationEP3D", no_init);

class_<XC::FDdecoupledElastic3D, bases<XC::FiniteDeformationElastic3D>, boost::noncopyable >("FDdecoupledElastic3D", no_init);

class_<XC::NeoHookeanCompressible3D , bases<XC::FiniteDeformationElastic3D>, boost::noncopyable >("NeoHookeanCompressible3D", no_init);

#include "fdFlow/python_interface.tcc"
#include "WEnergy/python_interface.tcc"
#include "fdEvolution/python_interface.tcc"
#include "fdYield/python_interface.tcc"

