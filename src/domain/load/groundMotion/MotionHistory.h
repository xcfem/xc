// -*-c++-*-
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
//MotionHistory.h

#ifndef MotionHistory_h
#define MotionHistory_h

#include "xc_utils/src/kernel/CommandEntity.h"
#include <vector>

namespace XC {
  class TimeSeries;
  class TimeSeriesIntegrator;
  class Parameter;
  class Information;
  class Vector;

//! @ingroup GMotion
//
//! @brief Data that define acceleration, velocities and
//! displacements due to a earthquake.
class MotionHistory: public CommandEntity
  {
  private:
    TimeSeries *theAccelSeries; //!< Ground acceleration.
    mutable TimeSeries *theVelSeries; //!< Ground velocity.
    mutable TimeSeries *theDispSeries; //!< Ground displacement.
    mutable TimeSeriesIntegrator *theIntegrator; //!< Integrator to use.
    double delta;

    void calcVel(void) const;
    void calcDisp(void) const;
    void clearSeries(void);
  public:
    MotionHistory(const double &dt= 0.0);
    MotionHistory(const MotionHistory &);
    MotionHistory &operator=(const MotionHistory &);
    
    virtual ~MotionHistory(void);

    inline double getDelta(void) const
      { return delta; }
    inline void setDelta(const double &dT)
      { delta= dT; }
    void setAccelHistory(const TimeSeries *);
    TimeSeries *getAccelHistory(void);

    size_t getNumDataPoints(void) const;
    double getDuration(void) const;

    double getPeakAccel(void) const;
    double getPeakVel(void) const;
    double getPeakDisp(void) const;

    double getAccel(double time) const;
    double getVel(double time) const;
    double getDisp(double time) const;
    const Vector &getDispVelAccel(Vector &data,const double &time) const;
    
    void setIntegrator(TimeSeriesIntegrator *integrator);
    TimeSeries *integrate(TimeSeries *theSeries) const; 

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    double getAccelSensitivity(double time);
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    int activateParameter(int parameterID);
    // AddingSensitivity:END ///////////////////////////////////////////
    void loadAccelFile(const std::string &fileNameAccel,const double &timeStep,const double &theFactor);
    void loadAccelTimeFile(const std::string &fileNameAccel, const std::string &fileNameTime,const double &theFactor);

  };
} // end of XC namespace

#endif

