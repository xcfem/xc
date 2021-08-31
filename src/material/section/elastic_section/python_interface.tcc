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
  .def(self_ns::str(self_ns::self))
  ;

const XC::CrossSectionProperties1d &(XC::BaseElasticSection1d::*getCrossSectionProperties1d)(void)const= &XC::BaseElasticSection1d::getCrossSectionProperties;
class_<XC::BaseElasticSection1d, bases<XC::BaseElasticSection>, boost::noncopyable >("BaseElasticSection1d", no_init)
  .add_property("sectionProperties", make_function( getCrossSectionProperties1d, return_internal_reference<>() ), &XC::BaseElasticSection1d::setCrossSectionProperties,"Returns section mechanical properties (area, inertia,...)")
  ;

class_<XC::ElasticSection1d, bases<XC::BaseElasticSection1d>, boost::noncopyable >("ElasticSection1d")
  .def("getStress", &XC::ElasticSection1d::getStress,"Material stress.")
  ;

const XC::CrossSectionProperties2d &(XC::BaseElasticSection2d::*getCrossSectionProperties2d)(void)const= &XC::BaseElasticSection2d::getCrossSectionProperties;
class_<XC::BaseElasticSection2d, bases<XC::BaseElasticSection>, boost::noncopyable >("BaseElasticSection2d", no_init)
  .add_property("sectionProperties", make_function( getCrossSectionProperties2d, return_internal_reference<>() ), &XC::BaseElasticSection2d::setCrossSectionProperties,"Returns section mechanical properties (area, inertia,...)")
  ;

class_<XC::ElasticSection2d, bases<XC::BaseElasticSection2d>, boost::noncopyable >("ElasticSection2d")
  ;

class_<XC::ElasticShearSection2d, bases<XC::BaseElasticSection2d>, boost::noncopyable >("ElasticShearSection2d")
  ;

const XC::CrossSectionProperties3d &(XC::BaseElasticSection3d::*getCrossSectionProperties3d)(void)const= &XC::BaseElasticSection3d::getCrossSectionProperties;
class_<XC::BaseElasticSection3d, bases<XC::BaseElasticSection>, boost::noncopyable >("BaseElasticSection3d", no_init)
  .add_property("sectionProperties", make_function( getCrossSectionProperties3d, return_internal_reference<>() ), &XC::BaseElasticSection3d::setCrossSectionProperties)
  ;

class_<XC::ElasticSection3d, bases<XC::BaseElasticSection3d>, boost::noncopyable >("ElasticSection3d")
  ;

class_<XC::ElasticShearSection3d, bases<XC::BaseElasticSection3d>, boost::noncopyable >("ElasticShearSection3d")
  ;

typedef std::vector<XC::ElasticSection1d *> vectorElasticSection1dMaterial;
class_<vectorElasticSection1dMaterial,boost::noncopyable>("vectorElasticSection1dMaterial")
  .def(vector_indexing_suite<vectorElasticSection1dMaterial>() )
  ;

class_<material_vector_ElasticSection1d,bases<vectorElasticSection1dMaterial,CommandEntity>,boost::noncopyable>("MaterialVectorElasticSection1d", no_init)
  ;

typedef std::vector<XC::ElasticSection2d *> vectorElasticSection2dMaterial;
class_<vectorElasticSection2dMaterial,boost::noncopyable>("vectorElasticSection2dMaterial")
  .def(vector_indexing_suite<vectorElasticSection2dMaterial>() )
  ;

class_<material_vector_ElasticSection2d,bases<vectorElasticSection2dMaterial,CommandEntity>,boost::noncopyable>("MaterialVectorElasticSection2d", no_init)
  ;


typedef std::vector<XC::ElasticSection3d *> vectorElasticSection3dMaterial;
class_<vectorElasticSection3dMaterial,boost::noncopyable>("vectorElasticSection3dMaterial")
  .def(vector_indexing_suite<vectorElasticSection3dMaterial>() )
  ;

class_<material_vector_ElasticSection3d,bases<vectorElasticSection3dMaterial,CommandEntity>,boost::noncopyable>("MaterialVectorElasticSection3d", no_init)
  ;
