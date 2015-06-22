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


class_<XC::CFactorSeries, bases<XC::TimeSeries>, boost::noncopyable >("CFactorSeries", no_init)
  .add_property("factor", &XC::CFactorSeries::getFactor,&XC::CFactorSeries::setFactor)
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
  .add_property("pathTimeIncr", &XC::PathSeries::getPathTimeIncr,&XC::PathSeries::setPathTimeIncr)
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
