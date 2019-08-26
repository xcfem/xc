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

class_<XC::SystemOfEqn, bases<XC::MovableObject,CommandEntity>, boost::noncopyable >("SystemOfEqn", no_init);

class_<XC::Solver, bases<XC::MovableObject,CommandEntity>, boost::noncopyable >("Solver", no_init)
  .def("getDeterminant",&XC::Solver::getDeterminant,"Return the determinant of the matrix.")
  .def("getRCond",&XC::Solver::getRCond,"Return the the reciprocal of the condition number of the matrix (1: 1-norm, I: infinity-norm).")
  ;

#include "linearSOE/python_interface.tcc"
#include "eigenSOE/python_interface.tcc"


