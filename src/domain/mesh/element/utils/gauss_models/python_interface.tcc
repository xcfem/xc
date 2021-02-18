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

class_<XC::GaussPoint, bases<XC::ParticlePos3d>, boost::noncopyable >("GaussPoint", no_init)
  .add_property("r_weight", make_function( &XC::GaussPoint::r_weight, return_value_policy<return_by_value>()), &XC::GaussPoint::set_r_weight, "r weight.")
  .add_property("s_weight", make_function( &XC::GaussPoint::s_weight, return_value_policy<return_by_value>()), &XC::GaussPoint::set_s_weight, "s weight.")
  .add_property("t_weight", make_function( &XC::GaussPoint::t_weight, return_value_policy<return_by_value>()), &XC::GaussPoint::set_t_weight, "t weight.")
  ;

class_<std::deque<XC::GaussPoint> >("GaussPointDq")
  .def(vector_indexing_suite<std::deque<XC::GaussPoint> >())
  ;

class_<XC::GaussModel, boost::noncopyable >("GaussModel", no_init)
  .def("getGaussPoints", make_function(&XC::GaussModel::getGaussPoints,return_internal_reference<>() ), "Return Gauss points.")
  ;
