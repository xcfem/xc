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

class_<XC::ThreedimLoad, bases<XC::ElementBodyLoad>, boost::noncopyable >("ThreedimLoad", no_init)
  .add_property("category", &XC::ThreedimLoad::Category)
  ;

class_<XC::ThreedimMecLoad, bases<XC::ThreedimLoad>, boost::noncopyable >("ThreedimMecLoad", no_init)
  .def("getResultant", &XC::ThreedimMecLoad::getResultant,"Returns load resultant (force and moment integration over the elements).")
  ;

class_<XC::BrickSelfWeight, bases<XC::ThreedimLoad>, boost::noncopyable >("BrickSelfWeight", no_init)
  ;

class_<XC::ThreedimStrainLoad, bases<XC::ThreedimLoad>, boost::noncopyable >("ThreedimStrainLoad", no_init)
  .def("getStrain",make_function(&XC::ThreedimStrainLoad::getStrain, return_internal_reference<>() ))
  .def("getStrains",&XC::ThreedimStrainLoad::getStrainsPy, "Return the values of the strains for each gauss point.")
  .def("setStrainComp",&XC::ThreedimStrainLoad::setStrainComp)
  ;

class_<XC::BrickStrainLoad, bases<XC::ThreedimStrainLoad>, boost::noncopyable >("QuadStrainLoad", no_init)
  ;
