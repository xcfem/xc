
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
class_<XC::GroundMotion, bases<EntCmd>, boost::noncopyable >("GroundMotion", no_init)
  .def("getDuration",&XC::GroundMotion::getDuration,"Returns motion's duration.")

  .def("getPeakAccel",&XC::GroundMotion::getPeakAccel,"Returns motion's peak acceleration")
  .def("getPeakVel",&XC::GroundMotion::getPeakVel,"Returns motion's peak velocity.")
  .def("getPeakDisp",&XC::GroundMotion::getPeakDisp,"Returns motion's peak displacement.")

  .def("getAccel",&XC::GroundMotion::getAccel,"Returns acceleration at time t.")
  .def("getVel",&XC::GroundMotion::getVel,"Returns velocity at time t.")
  .def("getDisp",&XC::GroundMotion::getDisp,"Returns displacement at time t.")
  .def("getDispVelAccel",make_function(&XC::GroundMotion::getDispVelAccel,return_internal_reference<>()),"Returns displacement, velocity and acceleration at time t.")
  ;

class_<XC::MotionHistory, bases<EntCmd>, boost::noncopyable >("MotionHistory", no_init)
  .add_property("delta", &XC::MotionHistory::getDelta,&XC::MotionHistory::setDelta,"Integration step size.")
  .add_property("accel", make_function( &XC::MotionHistory::getAccelHistory, return_internal_reference<>()),&XC::MotionHistory::setAccelHistory,"Integration step size.")
  .def("getNumberOfDataPoints",&XC::MotionHistory::getNumDataPoints,"Returns the number of data points.")
  ;

class_<XC::GroundMotionRecord , bases<XC::GroundMotion>, boost::noncopyable >("GroundMotionRecord", no_init)
  .add_property("history", make_function( &XC::GroundMotionRecord::getHistory, return_internal_reference<>()),"Ground motion history.")
  ;

class_<XC::InterpolatedGroundMotion, bases<XC::GroundMotion>, boost::noncopyable >("InterpolatedGroundMotion", no_init)
  ;


class_<XC::DqGroundMotions , bases<EntCmd>, boost::noncopyable >("DqGroundMotions", no_init);
