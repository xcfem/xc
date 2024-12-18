//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
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
//MotionHistory.cpp


#include "MotionHistory.h"
#include <domain/load/pattern/TimeSeries.h>
#include <domain/load/pattern/TimeSeriesIntegrator.h>
#include "domain/load/pattern/time_series_integrator/TrapezoidalTimeSeriesIntegrator.h"
#include "domain/load/pattern/time_series_integrator/SimpsonTimeSeriesIntegrator.h"
#include <domain/load/pattern/time_series/PathSeries.h>
#include <domain/load/pattern/time_series/PathTimeSeries.h>
#include <utility/matrix/Vector.h>
#include "utility/utils/misc_utils/colormod.h"




XC::MotionHistory::MotionHistory(const double &dt)
  :CommandEntity(), theAccelSeries(nullptr), theVelSeries(nullptr), theDispSeries(nullptr),
  theIntegrator(nullptr), delta(dt) {}

XC::MotionHistory::MotionHistory(TimeSeries *dispSeries, 
				 TimeSeries *velSeries, 
				 TimeSeries *accelSeries,
				 TimeSeriesIntegrator *theIntegratr,
				 double dTintegration)
  : CommandEntity(), 
    theAccelSeries(accelSeries), theVelSeries(velSeries), 
    theDispSeries(dispSeries), theIntegrator(theIntegratr),
    delta(dTintegration)
  {

    if(!theVelSeries)
      calcVel();
    if(!theDispSeries)
      calcDisp();
    if(!theAccelSeries)
      calcAccel();
  }

XC::MotionHistory::MotionHistory(const MotionHistory &other)
  : CommandEntity(other), theAccelSeries(nullptr), theVelSeries(nullptr), theDispSeries(nullptr),
  theIntegrator(nullptr), delta(other.delta)
  {
    if(other.theAccelSeries)
      theAccelSeries= other.theAccelSeries->getCopy();
    if(other.theVelSeries)
      theVelSeries= other.theVelSeries->getCopy();
    if(other.theDispSeries)
      theDispSeries= other.theDispSeries->getCopy();
    if(other.theIntegrator)
      this->setIntegrator(other.theIntegrator); //Copy integrator.
  }

XC::MotionHistory &XC::MotionHistory::operator=(const MotionHistory &other)
  {
    CommandEntity::operator=(other);
    clearSeries();
    clearIntegrator();
    if(other.theAccelSeries)
      theAccelSeries= other.theAccelSeries->getCopy();
    if(other.theVelSeries)
      theVelSeries= other.theVelSeries->getCopy();
    if(other.theDispSeries)
      theDispSeries= other.theDispSeries->getCopy();
    if(other.theIntegrator)
      this->setIntegrator(other.theIntegrator); //Copy integrator.
    return *this;
  }

//! @brief Set acceleration history.
void XC::MotionHistory::setAccelHistory(const TimeSeries *ts)
  {
    clearSeries();
    theAccelSeries= ts->getCopy();
  }

//! @brief Return the acceleration history.
XC::TimeSeries *XC::MotionHistory::getAccelHistory(void)
  {
    if(!theAccelSeries)
      calcAccel();
    return theAccelSeries;
  }

//! @brief Set velocities history.
void XC::MotionHistory::setVelHistory(const TimeSeries *ts)
  {
    clearSeries();
    theVelSeries= ts->getCopy();
  }

//! @brief Return the acceleration history.
XC::TimeSeries *XC::MotionHistory::getVelHistory(void)
  {
    if(!theVelSeries)
      calcVel();
    return theVelSeries;
  }
  
//! @brief Set displacements history.
void XC::MotionHistory::setDispHistory(const TimeSeries *ts)
  {
    clearSeries();
    theDispSeries= ts->getCopy();
  }

//! @brief Return the displacement history.
XC::TimeSeries *XC::MotionHistory::getDispHistory(void)
  {
    if(!theDispSeries)
      calcDisp();
    return theDispSeries;
  }

XC::MotionHistory::~MotionHistory(void)
  {
    clearSeries();
    clearIntegrator();
  }

void XC::MotionHistory::clearSeries(void)
  {
    if(theAccelSeries) 
      {
        delete theAccelSeries;
        theAccelSeries= nullptr;
      }
    if(theVelSeries) 
      {
        delete theVelSeries;
        theVelSeries= nullptr;
      }
    if(theDispSeries) 
      {
        delete theDispSeries;
        theDispSeries= nullptr;
      }
  }

void XC::MotionHistory::clearIntegrator(void)
  {
    if(theIntegrator)
      delete theIntegrator;
    theIntegrator= nullptr;    
  }

void XC::MotionHistory::setIntegrator(const std::string &integratorType)
  {
    if(integratorType=="trapezoidal")
      {
	TrapezoidalTimeSeriesIntegrator tmp;
	this->setIntegrator(&tmp);
      }
    else if(integratorType=="simpson")
      {
	SimpsonTimeSeriesIntegrator tmp;
	this->setIntegrator(&tmp);
      }
    else
      {
        std::cerr << getClassName() << "::" <<__FUNCTION__
	          << "; unknown integrator type: '"
	          << integratorType
	          << "'; using trapezoidal type as default."
	          << std::endl;
	TrapezoidalTimeSeriesIntegrator tmp;
	this->setIntegrator(&tmp);
      }
    if(!theIntegrator)
      {
	std::cerr << getClassName() << "::" <<__FUNCTION__
		  << "; no TimeSeriesIntegrator provided"
		  << " and failed to create a Trapezoidal one..."
		  << " memory problems! \n";
      }		
  }

void XC::MotionHistory::setIntegrator(const TimeSeriesIntegrator *integrator)
  {
    clearIntegrator();
    if(integrator)
      theIntegrator= integrator->getCopy();
  }

XC::TimeSeriesIntegrator *XC::MotionHistory::getIntegrator(void)
  { return theIntegrator; }

XC::TimeSeries *XC::MotionHistory::integrate(TimeSeries *theSeries) const
  {
    const std::string method_identifier=  getClassName() +"::"+__FUNCTION__;
    TimeSeries *retval= nullptr;
    // check that an integrator & accel series exist
    if(!theIntegrator)
      {
        if(verbosity>1)
          std::cerr << method_identifier
		    << ";no TimeSeriesIntegrator provided,"
	            << " will use Trapezoidal.\n";
	MotionHistory *this_no_const= const_cast<MotionHistory *>(this);
	this_no_const->setIntegrator("trapezoidal");
      }
    if(!theSeries)
      {
        std::cerr << method_identifier
	          << "; no TimeSeries specified.\n";
        return retval;
      }

    // integrate the series, if no vel series exists set it to new one
    const double dT= getDelta();
    if(dT>0.0)
      {
        retval= theIntegrator->integrate(theSeries, dT);
        if(!retval)
          std::cerr << method_identifier
	            << "; no TimeSeriesIntegrator defined, "
		    << "failed to integrate\n";
      }
    else
      std::cerr << method_identifier
		<< "; integration increment must not be zero, dT="
		<< dT << std::endl;
    return retval;
  }


XC::TimeSeries* XC::MotionHistory::differentiate(TimeSeries *theSeries) const
  {
    // check that an integrator & accel series exist
    const std::string method_identifier=  getClassName() +"::"+__FUNCTION__;
    TimeSeries *retval= nullptr;
    if(theIntegrator == 0)
      {
        if(verbosity>1)
          std::cerr << method_identifier
		    << ";no TimeSeriesIntegrator provided,"
	            << " will use Trapezoidal.\n";
	MotionHistory *this_no_const= const_cast<MotionHistory *>(this);
	this_no_const->setIntegrator("trapezoidal");
      }

    if(!theSeries)
      {
        std::cerr << method_identifier
	          << "; no TimeSeries specified.\n";
        return retval;
      }

    // differentiate the series, if no vel series exists set it to new one
    const double dT= getDelta();
    if(dT>0.0)
      {
	retval= theIntegrator->differentiate(theSeries, dT);
        if(!retval)
          std::cerr << method_identifier
	            << "; no TimeSeriesIntegrator defined, "
		    << "failed to integrate\n";
      }
    else
      std::cerr << method_identifier
		<< "; differentiation increment must not be zero, dT="
		<< dT << std::endl;
    return retval;
  }

size_t XC::MotionHistory::getNumDataPoints(void) const
  {
    const PathSeriesBase *tmp= dynamic_cast<const PathSeriesBase *>(theAccelSeries);
    if(tmp)
      return tmp->getNumDataPoints();
    else
      return 0;
  }

//! @brief Return the duration of the motion history.
double XC::MotionHistory::getDuration(void) const
  {
    double retval= 0.0;
    if(theAccelSeries)
      retval= theAccelSeries->getDuration();
    else if(theVelSeries)
      retval= theVelSeries->getDuration();
    else if(theDispSeries)
      retval= theDispSeries->getDuration();
    return retval;
  }

//! @brief If necessary, compute the acceleration series.
void XC::MotionHistory::calcAccel(void) const
  {
    if(!theAccelSeries) //Not allready computed.
      {
        if(theVelSeries) 
          theAccelSeries= this->differentiate(theVelSeries); // Compute accelerations.
	else if(theDispSeries)
	  {
	    theVelSeries= this->differentiate(theDispSeries); // Compute velocities.
	    if(theVelSeries)
	      theAccelSeries= this->differentiate(theVelSeries); // Compute displacements.
	  }
      }
  }

//! @brief Return the peak value of the acceleration.
double XC::MotionHistory::getPeakAccel(void) const
  {
    if(!theAccelSeries) calcAccel();
    if(theAccelSeries)
      return theAccelSeries->getPeakFactor();
    else
      return 0.0;
  }

//! @brief If necessary, compute the velocity series.
void XC::MotionHistory::calcVel(void) const
  {
    if(!theVelSeries) //Not allready computed.
      {
        if(theAccelSeries) 
          theVelSeries= this->integrate(theAccelSeries); //Computes velocities.
	else if(theDispSeries)
	  theVelSeries= this->differentiate(theDispSeries);
      }
  }

// if theVel or theAccel is not nullptr, integrate vel series to get disp series
void XC::MotionHistory::calcDisp(void) const
  {
    if(!theDispSeries) // Not already computed.
      {
        if(!theVelSeries)
          calcVel(); //Calcula velocidades.
        if(theVelSeries)
          theDispSeries= this->integrate(theVelSeries); //Computes displacements.
      }
  }

//! @brief Return the peak value of the velocity.
double XC::MotionHistory::getPeakVel(void) const
  {
    if(!theVelSeries) calcVel();
    if(theVelSeries)
      return theVelSeries->getPeakFactor();
    else
      return 0.0;
  }

//! @brief Return the peak value of the displacement.
double XC::MotionHistory::getPeakDisp(void) const
  {
    if(!theDispSeries) calcDisp();
    if(theDispSeries)
      return theDispSeries->getPeakFactor();
    else
      return 0.0;
  }

//! @brief Return acceleration at specified time.
double XC::MotionHistory::getAccel(double time) const
  {
    double retval= 0.0;
    if(time>=0.0)
      {
	if(!theAccelSeries)
	  calcAccel();
	if(theAccelSeries)
	  retval= theAccelSeries->getFactor(time);
      }
    return retval;
  }     

//! @brief Return acceleration sensitivity at specified time.
double XC::MotionHistory::getAccelSensitivity(double time)
  {
    double retval= 0.0;
    if(time>=0.0)
      {
	if(!theAccelSeries)
	  calcAccel();
	if(theAccelSeries)
	  retval= theAccelSeries->getFactorSensitivity(time);
      }
    return retval;
  }

//! @brief Return velocity at specified time.
double XC::MotionHistory::getVel(double time) const
  {
    double retval= 0.0;
    if(time>=0.0)
      {

	if(!theVelSeries)
	  calcVel();
	if(theVelSeries)
	  retval= theVelSeries->getFactor(time);      
      }
    return retval;
  }

//! @brief Return displacement at specified time.
double XC::MotionHistory::getDisp(double time) const
  {
    double retval= 0.0;
    if(time>=0.0)
      {
	if(!theDispSeries)
	  calcDisp();
	if(theDispSeries)
	  retval= theDispSeries->getFactor(time);
      }
    return retval;
  }

const XC::Vector &XC::MotionHistory::getDispVelAccel(Vector &data,const double &time) const
  {
    if(time < 0.0)
      {
        data(0) = 0.0;
        data(1) = 0.0;
        data(2) = 0.0;
        return data;
      }
    else
      {
        data(2) = this->getAccel(time);
        data(1) = this->getVel(time);
        data(0) = this->getDisp(time);
      }
    return data;
  }

// AddingSensitivity:BEGIN ////////////////////////////////////
int XC::MotionHistory::setParameter(const std::vector<std::string> &argv, Parameter &param)
  { return theAccelSeries->setParameter(argv, param); }

int XC::MotionHistory::updateParameter(int parameterID, Information &info)
  { return theAccelSeries->updateParameter(parameterID,info); }

int XC::MotionHistory::activateParameter(int pparameterID)
  { return theAccelSeries->activateParameter(pparameterID); }
// AddingSensitivity:END ////////////////////////////////////


void XC::MotionHistory::loadAccelFile(const std::string &fileNameAccel,const double &timeStep,const double &theFactor)
  {
    clearSeries();
    theAccelSeries = new PathSeries(fileNameAccel, timeStep, theFactor);
    if(!theAccelSeries)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; unable to create PathSeries.\n";
  }

void XC::MotionHistory::loadAccelTimeFile(const std::string &fileNameAccel, const std::string &fileNameTime,const double &theFactor)
  {
    clearSeries();
    theAccelSeries = new PathTimeSeries(fileNameAccel, fileNameTime, theFactor);
    if(!theAccelSeries)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; unable to create PathTimeSeries.\n";
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::MotionHistory::getPyDict(void) const
  {
    boost::python::dict retval= CommandEntity::getPyDict();
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	      << "; not implemented."
	      << Color::def << std::endl;
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::MotionHistory::setPyDict(const boost::python::dict &d)
  {
    CommandEntity::setPyDict(d);
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	      << "; not implemented."
	      << Color::def << std::endl;
  }
