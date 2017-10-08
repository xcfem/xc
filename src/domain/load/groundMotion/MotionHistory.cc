//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
#include <domain/load/pattern/time_series_integrator/TrapezoidalTimeSeriesIntegrator.h>
#include <domain/load/pattern/time_series/PathSeries.h>
#include <domain/load/pattern/time_series/PathTimeSeries.h>
#include <utility/matrix/Vector.h>




XC::MotionHistory::MotionHistory(const double &dt)
  :EntCmd(), theAccelSeries(nullptr), theVelSeries(nullptr), theDispSeries(nullptr),
  theIntegrator(nullptr), delta(dt) {}

XC::MotionHistory::MotionHistory(const MotionHistory &otro)
  : EntCmd(otro), theAccelSeries(nullptr), theVelSeries(nullptr), theDispSeries(nullptr),
  theIntegrator(nullptr), delta(otro.delta)
  {
    if(otro.theAccelSeries)
      theAccelSeries= otro.theAccelSeries->getCopy();
    if(otro.theVelSeries)
      theVelSeries= otro.theVelSeries->getCopy();
    if(otro.theDispSeries)
      theDispSeries= otro.theDispSeries->getCopy();
    //No copiamos el integrator.
  }

XC::MotionHistory &XC::MotionHistory::operator=(const MotionHistory &otro)
  {
    EntCmd::operator=(otro);
    clearSeries();
    if(theIntegrator)
      delete(theIntegrator);
    theIntegrator= nullptr;
    if(otro.theAccelSeries)
      theAccelSeries= otro.theAccelSeries->getCopy();
    if(otro.theVelSeries)
      theVelSeries= otro.theVelSeries->getCopy();
    if(otro.theDispSeries)
      theDispSeries= otro.theDispSeries->getCopy();
    //No copiamos el integrator.
    return *this;
  }

//! @brief Set acceleration history.
void XC::MotionHistory::setAccelHistory(const TimeSeries *ts)
  {
    clearSeries();
    theAccelSeries= ts->getCopy();
  }

XC::TimeSeries *XC::MotionHistory::getAccelHistory(void)
  { return theAccelSeries; }

XC::MotionHistory::~MotionHistory(void)
  {
    clearSeries();
    if(theIntegrator) delete theIntegrator;
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

void XC::MotionHistory::setIntegrator(TimeSeriesIntegrator *integrator)
  {
    if(theIntegrator)
      delete theIntegrator;
    theIntegrator= integrator;
  }

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
        theIntegrator= new TrapezoidalTimeSeriesIntegrator();
        if(!theIntegrator)
          {
            std::cerr << method_identifier
		      << "; no TimeSeriesIntegrator provided"
	              << " and failed to create a Trapezoidal one..."
	              << " memory problems! \n";
            return retval;
          }
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

size_t XC::MotionHistory::getNumDataPoints(void) const
  {
    const PathSeriesBase *tmp= dynamic_cast<const PathSeriesBase *>(theAccelSeries);
    if(tmp)
      return tmp->getNumDataPoints();
    else
      return 0;
  }

double XC::MotionHistory::getDuration(void) const
  {
    if(theAccelSeries)
      return theAccelSeries->getDuration();
    else
      return 0.0;
  }

double XC::MotionHistory::getPeakAccel(void) const
  {
    if(theAccelSeries)
      return theAccelSeries->getPeakFactor();
    else
      return 0.0;
  }

// if theAccel is not nullptr, integrate accel series to get a vel series
void XC::MotionHistory::calcVel(void) const
  {
    if(theVelSeries) return; //Already computed.
    if(theAccelSeries) 
      theVelSeries = this->integrate(theAccelSeries); //Computes velocities.
  }

// if theVel or theAccel is not nullptr, integrate vel series to get disp series
void XC::MotionHistory::calcDisp(void) const
  {
    if(theDispSeries) return; //Already computed.
    if(!theVelSeries)
      calcVel(); //Calcula velocidades.
    if(theVelSeries)
      theDispSeries = this->integrate(theVelSeries); //Computes displacements.
  }

double XC::MotionHistory::getPeakVel(void) const
  {
    if(!theVelSeries) calcVel();
    if(theVelSeries)
      return theVelSeries->getPeakFactor();
    else
      return 0.0;
  }

double XC::MotionHistory::getPeakDisp(void) const
  {
    if(!theDispSeries) calcDisp();
    if(theDispSeries)
      return theDispSeries->getPeakFactor();
    else
      return 0.0;
  }

double XC::MotionHistory::getAccel(double time) const
  {
    if(time < 0.0)
      return 0.0;
    if(theAccelSeries)
      return theAccelSeries->getFactor(time);
    else
      return 0.0;
  }     

double XC::MotionHistory::getAccelSensitivity(double time)
  {
    if(time < 0.0)
      return 0.0;
  
    if(theAccelSeries)
      return theAccelSeries->getFactorSensitivity(time);
    else
      return 0.0;
  }

double XC::MotionHistory::getVel(double time) const
  {
    if(time < 0.0)
      return 0.0;
    if(!theVelSeries) calcVel();
    if(theVelSeries)
      return theVelSeries->getFactor(time);      
    else
      return 0.0;
  }

double XC::MotionHistory::getDisp(double time) const
  {
    if(time < 0.0)
      return 0.0;
    if(!theDispSeries) calcDisp();
    if(theDispSeries)
      return theDispSeries->getFactor(time);
    else
      return 0.0;
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
