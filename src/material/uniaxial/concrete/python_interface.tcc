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
class_<XC::ConcreteBase, bases<XC::UniaxialMaterial>, boost::noncopyable >("ConcreteBase", no_init)
  .add_property("epsc0", &XC::ConcreteBase::getEpsc0,  &XC::ConcreteBase::setEpsc0)
  .add_property("epscu", &XC::ConcreteBase::getEpscu,  &XC::ConcreteBase::setEpscu)
  .add_property("fpc", &XC::ConcreteBase::getFpc,  &XC::ConcreteBase::setFpc)
  .def("getTangent", &XC::ConcreteBase::getTangent)
  ;

class_<XC::Concrete01, bases<XC::ConcreteBase>, boost::noncopyable >("Concrete01", no_init)
  .add_property("fpcu", &XC::Concrete01::getFpcu,  &XC::Concrete01::setFpcu)
  ;

class_<XC::Concrete02, bases<XC::UniaxialMaterial>, boost::noncopyable >("Concrete02", no_init);

class_<XC::Concrete04, bases<XC::ConcreteBase>, boost::noncopyable >("Concrete04", no_init);


