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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.4 $
// $Date: 2003/02/14 23:01:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/TrapezoidalTimeSeriesIntegrator.cpp,v $
                                                                        
                                                                        
// File: ~/domain/load/pattern/TrapezoidalTimeSeriesIntegrator.cpp
// 
// Written: MHS
// Created: 10/99
// Revision: A
//
// Description: This file contains the class definition for 
// a XC::TrapezoidalTimeSeriesIntegrator, which integrates a
// ground motion XC::TimeSeries using the trapezoidal rule.
//
// What: "@(#) TrapezoidalTimeSeriesIntegrator.cpp, revA"

#include <domain/load/pattern/time_series_integrator/TrapezoidalTimeSeriesIntegrator.h>
#include <domain/load/pattern/TimeSeries.h>
#include <utility/matrix/Vector.h>
#include <domain/load/pattern/time_series/PathSeries.h>

XC::TrapezoidalTimeSeriesIntegrator::TrapezoidalTimeSeriesIntegrator() 
  :TimeSeriesIntegrator(TIMESERIES_INTEGRATOR_TAG_Trapezoidal)
  {}

XC::TimeSeries *XC::TrapezoidalTimeSeriesIntegrator::integrate(TimeSeries *theSeries, double delta)
  {	
    // Check for zero time step, before dividing to get number of steps
    if(delta <= 0.0)
      {
        std::cerr << "XC::TrapezoidalTimeSeriesIntegrator::integrate() Attempting to integrate time step" <<
	  delta << "<= 0\n";
        return 0;
      }
    // check a TimeSeries object was passed
    if(!theSeries)
      {
        std::cerr << "TrapezoidalTimeSeriesIntegrator::" << __FUNCTION__
	          << "; no TimeSeries passed.\n";
        return 0;
      }
    const double duration= theSeries->getDuration();
    if(duration<=0.0)
        std::cerr << "TrapezoidalTimeSeriesIntegrator::" << __FUNCTION__
	          << "; WARNING: duration is zero.\n";
      

    // Add one to get ceiling out of type cast
    const int numSteps= static_cast<int>(duration/delta + 1.0);

    Vector theIntegratedValues(numSteps);

    // Set the first point
    // Assuming initial condition is zero, i.e. F(0) = 0
    theIntegratedValues[0]= 0.0; //theSeries->getFactor(0.0) * delta * 0.5;

    double previousValue= theSeries->getFactor(0.0); // Temporary storage to avoid accessing same value twice
	                                             // through identical method calls  
    double dummyTime= delta;
    double currentValue;
    
    for(int i=1;i<numSteps;i++,dummyTime+= delta)
      {
        currentValue= theSeries->getFactor(dummyTime);
        // Apply the trapezoidal rule to update the integrated value
        theIntegratedValues[i]= theIntegratedValues[i-1]+ delta*(0.5*(currentValue + previousValue));
        previousValue= currentValue;
      }

    // Set the method return value
    PathSeries *returnSeries= new PathSeries(theIntegratedValues, delta);

    if(returnSeries == 0)
      {
        std::cerr << "XC::TrapezoidalTimeSeriesIntegrator::integrate() Ran out of memory creating XC::PathSeries\n";
        return 0;
      }
    return returnSeries;
  }

int XC::TrapezoidalTimeSeriesIntegrator::sendSelf(Communicator &comm)
  { return 0; }

int XC::TrapezoidalTimeSeriesIntegrator::recvSelf(const Communicator &comm)
  { return 0; }

void XC::TrapezoidalTimeSeriesIntegrator::Print(std::ostream &s, int flag) const
  {
    // Need to implement, return for now
    return;
  }
