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

class_<XC::ElasticPlateBase, bases<XC::SectionForceDeformation>, boost::noncopyable >("ElasticPlateBase", no_init)
    .add_property("E", &XC::ElasticPlateBase::getE, &XC::ElasticPlateBase::setE, "Material's Young modulus.")
.add_property("nu", &XC::ElasticPlateBase::getnu, &XC::ElasticPlateBase::setnu, "Material Poisson's ratio.")
  .add_property("h", &XC::ElasticPlateBase::getH, &XC::ElasticPlateBase::setH,"material thickness.")
   ;

typedef XC::ElasticPlateProto<8> ElasticPlateProto8;
class_<ElasticPlateProto8, bases<XC::ElasticPlateBase>, boost::noncopyable >("ElasticPlateProto8", no_init)
    ;

class_<XC::ElasticMembranePlateSection, bases<ElasticPlateProto8>, boost::noncopyable  >("ElasticMembranePlateSection", no_init)
  .add_property("rho", &XC::ElasticMembranePlateSection::getRho, &XC::ElasticMembranePlateSection::setRho)
  ;

typedef XC::ElasticPlateProto<5> ElasticPlateProto5;
class_<ElasticPlateProto5, bases<XC::ElasticPlateBase>, boost::noncopyable >("ElasticPlateProto5", no_init);
class_<XC::ElasticPlateSection, bases<ElasticPlateProto5>, boost::noncopyable >("ElasticPlateSection", no_init);

class_<XC::MembranePlateFiberSection, bases<XC::SectionForceDeformation>, boost::noncopyable >("MembranePlateFiberSection", no_init);

