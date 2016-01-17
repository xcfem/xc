//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
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
//MotionHistory.cpp


#include "MotionHistory.h"
#include <domain/load/pattern/TimeSeries.h>
#include <domain/load/pattern/TimeSeriesIntegrator.h>
#include <domain/load/pattern/time_series_integrator/TrapezoidalTimeSeriesIntegrator.h>
#include <domain/load/pattern/time_series/PathSeries.h>
#include <domain/load/pattern/time_series/PathTimeSeries.h>
#include <utility/matrix/Vector.h>
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"


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
    //No copiamos el integrador.
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
    //No copiamos el integrador.
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
    TimeSeries *retval= nullptr;
    // check that an integrator & accel series exist
    if(!theIntegrator)
      {
        if(verborrea>1)
          std::cerr << "WARNING: MotionHistory::integrate() - no XC::TimeSeriesIntegrator provided - will use Trapezoidal. \n";
        theIntegrator= new XC::TrapezoidalTimeSeriesIntegrator();
        if(!theIntegrator)
          {
            std::cerr << "WARNING: MotionHistory::integrate() - no XC::TimeSeriesIntegrator provided - failed to create a Trapezoidal .. memory problems! \n";
            return retval;
          }
      }
    if(!theSeries)
      {
        std::cerr << "MotionHistory::integrate - no TimeSeries specified\n";
        return retval;
      }

    // integrate the series, if no vel series exists set it to new one
    if(delta>0)
      {
        retval= theIntegrator->integrate(theSeries, delta);
        if(!retval)
          std::cerr << "MotionHistory::integrate - no XC::TimeSeriesIntegrator failed to integrate\n";
      }
    else
      std::cerr << "MotionHistory::integrate - el incremento para la integración no debe ser nulo, dT=" << delta << std::endl;
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
    if(theVelSeries) return; //Ya están calculadas.
    if(theAccelSeries) 
      theVelSeries = this->integrate(theAccelSeries); //Calcula velocidades.
  }

// if theVel or theAccel is not nullptr, integrate vel series to get disp series
void XC::MotionHistory::calcDisp(void) const
  {
    if(theDispSeries) return; //Ya están calculados.
    if(!theVelSeries)
      calcVel(); //Calcula velocidades.
    if(theVelSeries)
      theDispSeries = this->integrate(theVelSeries); //Calcula desplazamientos.
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
      std::cerr << "XC::MotionHistory::loadAccelFile - unable to create XC::PathSeries\n";
  }

void XC::MotionHistory::loadAccelTimeFile(const std::string &fileNameAccel, const std::string &fileNameTime,const double &theFactor)
  {
    clearSeries();
    theAccelSeries = new PathTimeSeries(fileNameAccel, fileNameTime, theFactor);
    if(!theAccelSeries)
      std::cerr << "XC::MotionHistory::loadAccelFile - unable to create XC::PathTimeSeries\n";
  }
