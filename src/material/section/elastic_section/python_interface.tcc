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

class_<XC::BaseElasticSection, bases<XC::PrismaticBarCrossSection>, boost::noncopyable >("BaseElasticSection", no_init)
  .add_property("rho", &XC::BaseElasticSection::getRho, &XC::BaseElasticSection::setRho,"Material density.")
  .def("sectionGeometry",&XC::BaseElasticSection::sectionGeometry)
  ;

class_<XC::BaseElasticSection2d, bases<XC::BaseElasticSection>, boost::noncopyable >("BaseElasticSection2d", no_init)
  .add_property("sectionProperties", make_function( &XC::BaseElasticSection2d::getCrossSectionProperties, return_internal_reference<>() ), &XC::BaseElasticSection2d::setCrossSectionProperties,"Returns section mechanical properties (area, inertia,...)")
  ;

class_<XC::ElasticSection2d, bases<XC::BaseElasticSection2d>, boost::noncopyable >("ElasticSection2d")
  ;

class_<XC::ElasticShearSection2d, bases<XC::BaseElasticSection2d>, boost::noncopyable >("ElasticShearSection2d")
  ;

class_<XC::BaseElasticSection3d, bases<XC::BaseElasticSection>, boost::noncopyable >("BaseElasticSection3d", no_init)
  .add_property("sectionProperties", make_function( &XC::BaseElasticSection3d::getCrossSectionProperties, return_internal_reference<>() ), &XC::BaseElasticSection3d::setCrossSectionProperties)
  ;

class_<XC::ElasticSection3d, bases<XC::BaseElasticSection3d>, boost::noncopyable >("ElasticSection3d")
  ;

class_<XC::ElasticShearSection3d, bases<XC::BaseElasticSection3d>, boost::noncopyable >("ElasticShearSection3d")
  ;
