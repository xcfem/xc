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

class_<XC::EigenSOE, bases<XC::SystemOfEqn>, boost::noncopyable >("EigenSOE", "Base class for eigenproblem systems of equations.", no_init)
.def("newSolver", &XC::EigenSOE::newSolver,return_internal_reference<>()," \n""newSolver(tipo)""Define the solver to be used.""Parameters: \n""tipo: type of solver. Available types: 'band_arpack_solver', 'band_arpackpp_solver', 'sym_band_eigen_solver', 'full_gen_eigen_solver', 'sym_arpack_solver'")
  ;

class_<XC::ArpackSOE, bases<XC::EigenSOE>, boost::noncopyable >("ArpackSOE", no_init)
  .add_property("shift", make_function(&XC::ArpackSOE::getShift, return_value_policy<copy_const_reference>() ),&XC::ArpackSOE::setShift)
  ;

class_<XC::BandArpackSOE, bases<XC::ArpackSOE>, boost::noncopyable >("BandArpackSOE", no_init)
  ;

class_<XC::BandArpackppSOE, bases<XC::ArpackSOE>, boost::noncopyable >("BandArpackppSOE", no_init)
  ;

class_<XC::SymArpackSOE, bases<XC::ArpackSOE>, boost::noncopyable >("SymArpackSOE", no_init)
  ;

class_<XC::FullGenEigenSOE, bases<XC::EigenSOE>, boost::noncopyable >("FullGenEigenSOE", no_init)
  ;

class_<XC::SymBandEigenSOE, bases<XC::EigenSOE>, boost::noncopyable >("SymBandEigenSOE", no_init)
  ;

class_<XC::EigenSolver, bases<XC::Solver>, boost::noncopyable >("EigenSolver", no_init);

class_<XC::BandArpackppSolver, bases<XC::EigenSolver>, boost::noncopyable >("BandArpackppSolver", no_init)
  ;

class_<XC::SymArpackSolver, bases<XC::EigenSolver>, boost::noncopyable >("SymArpackSolver", no_init)
  ;

class_<XC::BandArpackSolver, bases<XC::EigenSolver>, boost::noncopyable >("BandArpackSolver", no_init)
  ;

class_<XC::FullGenEigenSolver, bases<XC::EigenSolver>, boost::noncopyable >("FullGenEigenSolver", no_init)
  ;

class_<XC::SymBandEigenSolver, bases<XC::EigenSolver>, boost::noncopyable >("SymBandEigenSolver", no_init)
  ;
