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

class_<XC::EquiSolnAlgo, bases<XC::SolutionAlgorithm>, boost::noncopyable >("EquiSolnAlgo", no_init);

class_<XC::EquiSolnConvAlgo, bases<XC::EquiSolnAlgo>, boost::noncopyable >("EquiSolnConvAlgo", no_init);

class_<XC::BFBRoydenBase, bases<XC::EquiSolnConvAlgo>, boost::noncopyable >("BFBRoydenBase", no_init);

class_<XC::BFGS, bases<XC::BFBRoydenBase>, boost::noncopyable >("BFGS", no_init);

class_<XC::Broyden, bases<XC::BFBRoydenBase>, boost::noncopyable >("Broyden", no_init);

class_<XC::KrylovNewton, bases<XC::EquiSolnAlgo>, boost::noncopyable >("KrylovNewton", no_init)
  .add_property("maxDimension", &XC::KrylovNewton::getMaxDimension, &XC::KrylovNewton::setMaxDimension,"max number of iterations until the tangent is reformed and the acceleration restarts (default = 3)")
  ;

class_<XC::Linear, bases<XC::EquiSolnAlgo>, boost::noncopyable >("Linear", no_init);

class_<XC::NewtonBased, bases<XC::EquiSolnAlgo>, boost::noncopyable >("NewtonBased", no_init)  
  ;

class_<XC::HallFactorsNewton, bases<XC::NewtonBased>, boost::noncopyable >("HallFactorsNewton", no_init)
  .def("useInitialTangent", &XC::HallFactorsNewton::useInitialTangent, "Instruct the solver to use the model’s initial stiffness matrix for every iteration within every time step.")
 .def("useSecantTangent", &XC::HallFactorsNewton::useSecantTangent, "Instruct the solver to use the model’s secant stiffness matrix for every iteration within every time step.")
  .def("useHallTangent", &XC::HallFactorsNewton::useHallTangent, "Instruct the solver to use the model’s Hall stiffness matrix for every iteration within every time step.")
  .def("useCurrentTangent", &XC::HallFactorsNewton::useCurrentTangent, "Instruct the solver to use the model’s current stiffness matrix for every iteration within every time step.")
  ;


class_<XC::ModifiedNewton, bases<XC::HallFactorsNewton>, boost::noncopyable >("ModifiedNewton", no_init);

class_<XC::NewtonRaphson, bases<XC::HallFactorsNewton>, boost::noncopyable >("NewtonRaphson", no_init)
  .def("useInitialThenCurrentTangent", &XC::HallFactorsNewton::useCurrentTangent, "Instruct the solver to use the model’s initial stiffness on first step and then current on subsequent steps.")
  ;

class_<XC::NewtonLineSearch, bases<XC::EquiSolnAlgo>, boost::noncopyable >("NewtonLineSearch", no_init)
  .def("setLineSearchMethod", &XC::NewtonLineSearch::setLineSearchMethod, "set the line search method to use (bisection_line_search, initial_interpolated_line_search, regula_falsi_line_search, secant_line_search)")
  ;


class_<XC::PeriodicNewton, bases<XC::NewtonBased>, boost::noncopyable >("PeriodicNewton", no_init);

#include "line_search/python_interface.tcc"
