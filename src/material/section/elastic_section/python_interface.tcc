//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//python_interface.tcc

class_<XC::BaseElasticSection, bases<XC::SeccionBarraPrismatica>, boost::noncopyable >("BaseElasticSection", no_init)
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
