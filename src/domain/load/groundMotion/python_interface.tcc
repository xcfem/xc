
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
class_<XC::GroundMotion, bases<CommandEntity>, boost::noncopyable >("GroundMotion", no_init)
  .def("getDuration",&XC::GroundMotion::getDuration,"Returns motion's duration.")

  .def("getPeakAccel",&XC::GroundMotion::getPeakAccel,"Returns motion's peak acceleration")
  .def("getPeakVel",&XC::GroundMotion::getPeakVel,"Returns motion's peak velocity.")
  .def("getPeakDisp",&XC::GroundMotion::getPeakDisp,"Returns motion's peak displacement.")

  .def("getAccel",&XC::GroundMotion::getAccel,"Returns acceleration at time t.")
  .def("getVel",&XC::GroundMotion::getVel,"Returns velocity at time t.")
  .def("getDisp",&XC::GroundMotion::getDisp,"Returns displacement at time t.")
  .def("getDispVelAccel",make_function(&XC::GroundMotion::getDispVelAccel,return_internal_reference<>()),"Returns displacement, velocity and acceleration at time t.")
  ;

class_<XC::MotionHistory, bases<CommandEntity>, boost::noncopyable >("MotionHistory", no_init)
  .add_property("delta", &XC::MotionHistory::getDelta,&XC::MotionHistory::setDelta,"Integration step size.")
  .add_property("accel", make_function( &XC::MotionHistory::getAccelHistory, return_internal_reference<>()),&XC::MotionHistory::setAccelHistory,"Integration step size.")
  .def("getNumberOfDataPoints",&XC::MotionHistory::getNumDataPoints,"Returns the number of data points.")
  ;

class_<XC::GroundMotionRecord , bases<XC::GroundMotion>, boost::noncopyable >("GroundMotionRecord", no_init)
  .add_property("history", make_function( &XC::GroundMotionRecord::getHistory, return_internal_reference<>()),"Ground motion history.")
  ;

class_<XC::InterpolatedGroundMotion, bases<XC::GroundMotion>, boost::noncopyable >("InterpolatedGroundMotion", no_init)
  ;


class_<XC::DqGroundMotions , bases<CommandEntity>, boost::noncopyable >("DqGroundMotions", no_init);
