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
class_<XC::RawConcrete, bases<XC::UniaxialMaterial>, boost::noncopyable >("RawConcrete", no_init)
  .add_property("epsc0", &XC::RawConcrete::getEpsc0,  &XC::RawConcrete::setEpsc0,"strain at compressive strength.")
  .add_property("epscu", &XC::RawConcrete::getEpscu,  &XC::RawConcrete::setEpscu,"strain at crushing strength.")
  .add_property("fpc", &XC::RawConcrete::getFpc,  &XC::RawConcrete::setFpc,"compressive strength.")
  ;

class_<XC::ConcreteBase, bases<XC::RawConcrete>, boost::noncopyable >("ConcreteBase", no_init)
  .def("getTangent", &XC::ConcreteBase::getTangent,"returns tangent stiffness.")
  ;

class_<XC::Concrete01, bases<XC::ConcreteBase>, boost::noncopyable >("Concrete01", no_init)
  .add_property("fpcu", &XC::Concrete01::getFpcu,  &XC::Concrete01::setFpcu,"crushing strength.")
  ;

class_<XC::Concrete02, bases<XC::RawConcrete>, boost::noncopyable >("Concrete02", no_init)
  .add_property("fpcu", &XC::Concrete02::getFpcu,  &XC::Concrete02::setFpcu,"crushing strength.")
  .add_property("ft", &XC::Concrete02::getFt,  &XC::Concrete02::setFt,"tensile strength.")
  .add_property("Ets", &XC::Concrete02::getEts,  &XC::Concrete02::setEts,"tension softening stiffness (absolute value) (slope of the linear tension softening branch).")
  .add_property("ratioSlope", &XC::Concrete02::getLambda,  &XC::Concrete02::setLambda,"ratio between unloading slope at epscu and initial slope.")
  ;

class_<XC::Concrete04, bases<XC::ConcreteBase>, boost::noncopyable >("Concrete04", no_init);


