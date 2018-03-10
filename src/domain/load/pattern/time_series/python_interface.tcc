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


class_<XC::CFactorSeries, bases<XC::TimeSeries>, boost::noncopyable >("CFactorSeries", no_init)
  .def("getFactor",&XC::CFactorSeries::getFactor,"getFactor(time): return load factor value at specified time.")
  .def("setFactor",&XC::CFactorSeries::setFactor,"setFactor(factor): sets factor value.")
  ;

class_<XC::ConstantSeries, bases<XC::CFactorSeries>, boost::noncopyable >("ConstantSeries", no_init)
  ;

class_<XC::LinearSeries , bases<XC::CFactorSeries>, boost::noncopyable >("LinearSeries", no_init);

class_<XC::PulseBaseSeries, bases<XC::CFactorSeries>, boost::noncopyable >("PulseBaseSeries", no_init)
  .add_property("tStart", &XC::PulseBaseSeries::getStart,&XC::PulseBaseSeries::setStart)
  .add_property("tFinish", &XC::PulseBaseSeries::getFinish,&XC::PulseBaseSeries::setFinish)
  ;
class_<XC::PeriodSeries, bases<XC::PulseBaseSeries>, boost::noncopyable >("PeriodSeries", no_init)
  .add_property("period", &XC::PeriodSeries::getPeriod,&XC::PeriodSeries::setPeriod)
  .add_property("shift", &XC::PeriodSeries::getShift,&XC::PeriodSeries::setShift)
  ;

class_<XC::PulseSeries, bases<XC::PeriodSeries>, boost::noncopyable >("PulseSeries", no_init)
  ;

class_<XC::RectangularSeries, bases<XC::PulseBaseSeries>, boost::noncopyable >("RectangularSeries", no_init)
  ;

class_<XC::PathSeriesBase, bases<XC::CFactorSeries>, boost::noncopyable >("PathSeriesBase", no_init)
  .add_property("path", &XC::PathSeriesBase::getPath,&XC::PathSeriesBase::setPath)
  .add_property("lastSendCommitTag", &XC::PathSeriesBase::getLastSendCommitTag,&XC::PathSeriesBase::setLastSendCommitTag)
  ;

class_<XC::PathSeries, bases<XC::PathSeriesBase>, boost::noncopyable >("PathSeries", no_init)
  .def("getTimeIncr",&XC::PathSeries::getTimeIncr,"getTimeIncr(time): return time increment.")
  .def("setTimeIncr",&XC::PathSeries::setTimeIncr,"setTimeIncr(factor): sets time increment.")
  .def("readFromFile",&XC::PathSeries::readFromFile,"Read motion data from file.")
  ;

class_<XC::PathTimeSeries, bases<XC::PathSeriesBase>, boost::noncopyable >("PathTimeSeries", no_init)
  .add_property("time", &XC::PathTimeSeries::getTime,&XC::PathTimeSeries::setTime)
  .add_property("currentTimeLoc", &XC::PathTimeSeries::getCurrentTimeLoc,&XC::PathTimeSeries::setCurrentTimeLoc)
  .def("readFromFile",&XC::PathTimeSeries::readFromFile,"Read motion data from file.")
  .def("readFromFiles",&XC::PathTimeSeries::readFromFiles,"Read motion data from files.")
  ;

class_<XC::DiscretizedRandomProcessSeries, bases<XC::TimeSeries>, boost::noncopyable >("DiscretizedRandomProcessSeries", no_init)
  ;

class_<XC::SimulatedRandomProcessSeries, bases<XC::TimeSeries>, boost::noncopyable >("SimulatedRandomProcessSeries", no_init)
  ;

class_<XC::TriangleSeries, bases<XC::PeriodSeries>, boost::noncopyable >("TriangleSeries", no_init)
  ;

class_<XC::TrigSeries, bases<XC::PeriodSeries>, boost::noncopyable >("TrigSeries", no_init)
  ;
