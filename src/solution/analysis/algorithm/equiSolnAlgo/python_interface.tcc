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

class_<XC::EquiSolnAlgo, bases<XC::SolutionAlgorithm>, boost::noncopyable >("EquiSolnAlgo", no_init);

class_<XC::EquiSolnConvAlgo, bases<XC::EquiSolnAlgo>, boost::noncopyable >("EquiSolnConvAlgo", no_init);

class_<XC::BFBRoydenBase, bases<XC::EquiSolnConvAlgo>, boost::noncopyable >("BFBRoydenBase", no_init);

class_<XC::BFGS, bases<XC::BFBRoydenBase>, boost::noncopyable >("BFGS", no_init);

class_<XC::Broyden, bases<XC::BFBRoydenBase>, boost::noncopyable >("Broyden", no_init);

class_<XC::KrylovNewton, bases<XC::EquiSolnAlgo>, boost::noncopyable >("KrylovNewton", no_init)
  .add_property("maxDimension", &XC::KrylovNewton::getMaxDimension, &XC::KrylovNewton::setMaxDimension,"max number of iterations until the tangent is reformed and the acceleration restarts (default = 3)")
  ;

class_<XC::Linear, bases<XC::EquiSolnAlgo>, boost::noncopyable >("Linear", no_init);

class_<XC::NewtonBased, bases<XC::EquiSolnAlgo>, boost::noncopyable >("NewtonBased", no_init);

class_<XC::ModifiedNewton, bases<XC::NewtonBased>, boost::noncopyable >("ModifiedNewton", no_init);

class_<XC::NewtonLineSearch, bases<XC::EquiSolnAlgo>, boost::noncopyable >("NewtonLineSearch", no_init);

class_<XC::NewtonRaphson, bases<XC::NewtonBased>, boost::noncopyable >("NewtonRaphson", no_init);

class_<XC::PeriodicNewton, bases<XC::NewtonBased>, boost::noncopyable >("PeriodicNewton", no_init);

#include "lineSearch/python_interface.tcc"
