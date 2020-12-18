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

class_<XC::PlateBase, bases<XC::SectionForceDeformation>, boost::noncopyable >("ElasticPlateBase", no_init)
  .add_property("h", &XC::PlateBase::getH, &XC::PlateBase::setH,"material thickness.")
  .add_property("rho", &XC::PlateBase::getRho, &XC::PlateBase::setRho)
  .add_property("arealDensity", &XC::PlateBase::getArealDensity, &XC::PlateBase::setArealDensity)
   ;

class_<XC::ElasticPlateBase, bases<XC::PlateBase>, boost::noncopyable >("ElasticPlateBase", no_init)
  .add_property("E", &XC::ElasticPlateBase::getE, &XC::ElasticPlateBase::setE, "Material's Young modulus.")
  .add_property("nu", &XC::ElasticPlateBase::getnu, &XC::ElasticPlateBase::setnu, "Material Poisson's ratio.")
   ;

typedef XC::ElasticPlateProto<8> ElasticPlateProto8;
class_<ElasticPlateProto8, bases<XC::ElasticPlateBase>, boost::noncopyable >("ElasticPlateProto8", no_init)
 .add_property("initialStrain", make_function(&ElasticPlateProto8::getInitialSectionDeformation, return_internal_reference<>() ),&ElasticPlateProto8::setInitialSectionDeformation, "Material initial strain.")
  ;

class_<XC::ElasticMembranePlateSection, bases<ElasticPlateProto8>, boost::noncopyable  >("ElasticMembranePlateSection", no_init)
  ;

typedef XC::ElasticPlateProto<5> ElasticPlateProto5;
class_<ElasticPlateProto5, bases<XC::ElasticPlateBase>, boost::noncopyable >("ElasticPlateProto5", no_init)
 .add_property("initialStrain", make_function(&ElasticPlateProto5::getInitialSectionDeformation, return_internal_reference<>() ),&ElasticPlateProto5::setInitialSectionDeformation, "Material initial strain.")
  ;

class_<XC::ElasticPlateSection, bases<ElasticPlateProto5>, boost::noncopyable >("ElasticPlateSection", no_init)
  ;

class_<XC::MembranePlateFiberSection, bases<XC::PlateBase>, boost::noncopyable >("MembranePlateFiberSection", no_init)
  .def("setMaterial", &XC::MembranePlateFiberSection::setMaterial, " assigns the material to the fibers.")
  ;

