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
XC::VectorReinfBar &(XC::ReinfLayer::*getReinfBarsRef)(void)= &XC::ReinfLayer::getReinfBars;
class_<XC::ReinfLayer, bases<XC::DiscretBase>, boost::noncopyable >("ReinfLayer", no_init)
  .add_property("numReinfBars",&XC::ReinfLayer::getNumReinfBars,&XC::ReinfLayer::setNumReinfBars,"Number of bars.")
  .add_property("barDiameter",make_function(&XC::ReinfLayer::getReinfBarDiameter, return_value_policy<return_by_value>()),&XC::ReinfLayer::setReinfBarDiameter,"Diameter of bars.")
  .add_property("barArea",make_function(&XC::ReinfLayer::getReinfBarArea, return_value_policy<return_by_value>()),&XC::ReinfLayer::setReinfBarArea,"Area of bars.")
  .add_property("getReinfBars",make_function(getReinfBarsRef,return_internal_reference<>()),"Returns reinforcement bars.")
  .def("getRecubrimiento",&XC::ReinfLayer::getRecubrimiento)
  .def("getCdg",&XC::ReinfLayer::getCdg)
  .def("getArea",&XC::ReinfLayer::getArea)
  ;

class_<XC::BarraSuelta , bases<XC::ReinfLayer>, boost::noncopyable >("BarraSuelta", no_init);

class_<XC::CircReinfLayer , bases<XC::ReinfLayer>, boost::noncopyable >("CircReinfLayer", no_init)
  .add_property("initAngle",&XC::CircReinfLayer::getInitAngle,&XC::CircReinfLayer::setInitAngle,"Initial angle.")
  .add_property("finalAngle",&XC::CircReinfLayer::getFinalAngle,&XC::CircReinfLayer::setFinalAngle,"Final angle.")
  .add_property("radius",&XC::CircReinfLayer::getArcRadius,&XC::CircReinfLayer::setArcRadius,"Final angle.")
  .add_property("center",&XC::CircReinfLayer::getCenterPosition,&XC::CircReinfLayer::setCenterPosition,"Center position.")
  ;

class_<XC::StraightReinfLayer , bases<XC::ReinfLayer>, boost::noncopyable >("StraightReinfLayer", no_init)
  .add_property("p1",&XC::StraightReinfLayer::getInitialPos,&XC::StraightReinfLayer::setInitialPos,"Initial position.")
  .add_property("p2",&XC::StraightReinfLayer::getFinalPos,&XC::StraightReinfLayer::setFinalPos,"Final position.")
  ;

class_<XC::ListReinfLayer, bases<XC::SeccionInerte>, boost::noncopyable >("ListReinfLayer", no_init)
  .def("__iter__", boost::python::iterator<XC::ListReinfLayer>())
  .def("newStraightReinfLayer",make_function(&XC::ListReinfLayer::newStraightReinfLayer,return_internal_reference<>()))
  .def("newCircReinfLayer",make_function(&XC::ListReinfLayer::newCircReinfLayer,return_internal_reference<>()))
  .def("newReinfBar",make_function(&XC::ListReinfLayer::newReinfBar,return_internal_reference<>()))
  .add_property("getNumReinfBars",&XC::ListReinfLayer::getNumReinfBars,"Number of bars.")
  ;
