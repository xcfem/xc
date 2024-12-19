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

#include "domain/load/pattern/time_series_integrator/TrapezoidalTimeSeriesIntegrator.h"
#include <domain/load/pattern/TimeSeries.h>
#include <utility/matrix/Vector.h>
#include <domain/load/pattern/time_series/PathSeries.h>

XC::TrapezoidalTimeSeriesIntegrator::TrapezoidalTimeSeriesIntegrator() 
  :TimeSeriesIntegrator(TIMESERIES_INTEGRATOR_TAG_Trapezoidal)
  {}

//! @brief Virtual constructor.
XC::TimeSeriesIntegrator *XC::TrapezoidalTimeSeriesIntegrator::getCopy(void) const
  { return new TrapezoidalTimeSeriesIntegrator(*this);}

XC::TimeSeries *XC::TrapezoidalTimeSeriesIntegrator::integrate(TimeSeries *theSeries, double delta) const
  {	
    // Check for zero time step, before dividing to get number of steps
    if(delta <= 0.0)
      {
        std::cerr << "TrapezoidalTimeSeriesIntegrator::" << __FUNCTION__
	          << "; attempting to integrate time step"
		  << delta << "<= 0\n";
        return 0;
      }
    // check a TimeSeries object was passed
    if(!theSeries)
      {
        std::cerr << "TrapezoidalTimeSeriesIntegrator::" << __FUNCTION__
	          << "; no TimeSeries given.\n";
        return 0;
      }
    const double duration= theSeries->getDuration();
    if(duration<=0.0)
        std::cerr << "TrapezoidalTimeSeriesIntegrator::" << __FUNCTION__
	          << "; WARNING: duration is zero.\n";
      

    // Add one to get ceiling out of type cast
    const size_t numSteps= static_cast<size_t>(duration/delta + 1.0);

    // create new vector for integrated values
    Vector theIntegratedValues(numSteps);

    // Set the first point

    double dummyTime= theSeries->getStartTime();
    double previousValue= theSeries->getFactor(dummyTime);
    theIntegratedValues[0]= 0.0;
    dummyTime+= delta;
    double currentValue;
    
    for(size_t i=1;i<numSteps;i++)
      {
        currentValue= theSeries->getFactor(dummyTime);
        // Apply the trapezoidal rule to update the integrated value
        theIntegratedValues[i]= theIntegratedValues[i-1]+ delta*(0.5*(currentValue + previousValue));
        previousValue= currentValue;
	dummyTime+= delta;
      }

    // Set the method return value
    PathSeries *retval= new PathSeries(theIntegratedValues, delta);

    if(retval == 0)
      {
        std::cerr << "TrapezoidalTimeSeriesIntegrator::" << __FUNCTION__
		  << "; ran out of memory creating PathSeries\n";
        return 0;
      }
    else
      {
	const PathSeriesBase *tmp= dynamic_cast<const PathSeriesBase *>(theSeries);
	if(tmp)
	  {
	    const bool useLast= tmp->getUseLast();
	    retval->setUseLast(useLast);
	  }
      }
    return retval;
  }

//! @brief Differentiate the given series.
XC::TimeSeries* XC::TrapezoidalTimeSeriesIntegrator::differentiate(TimeSeries *theSeries, double delta) const
  {	
    // Check for zero time step, before dividing to get number of steps
    if(delta <= 0.0)
      {
        std::cerr << "TrapezoidalTimeSeriesIntegrator::" << __FUNCTION__
	          << "; attempting to integrate time step"
		  << delta << "<= 0\n";
        return 0;
      }

    // check a TimeSeries object was passed
    if(!theSeries)
      {
        std::cerr << "TrapezoidalTimeSeriesIntegrator::" << __FUNCTION__
	          << "; no TimeSeries given.\n";
        return 0;
      }
    const double duration= theSeries->getDuration();
    if(duration<=0.0)
        std::cerr << "TrapezoidalTimeSeriesIntegrator::" << __FUNCTION__
	          << "; WARNING: duration is zero.\n";

    // Add one to get ceiling out of type cast
    size_t numSteps= static_cast<size_t>(duration/delta + 1.0)-1;
    
    Vector theDif(numSteps);


    double dummyTime= theSeries->getStartTime(); // Dummy variable for integrating
    // function values
    double Fi= theSeries->getFactor(dummyTime);
    double Fj= theSeries->getFactor(dummyTime+delta);
    double fi= (Fj-Fi)/delta; // derivative value
    double fj;

    for(size_t j= 1; j < numSteps; j++)
      {
	dummyTime+= delta;
        Fj= theSeries->getFactor(dummyTime);

        // Apply the trapezoidal rule to update the derivative
        fj= (Fj - Fi)/delta;
	const double f= (fi+fj)/2.0;

	theDif[j-1]= f; 
	Fi= Fj;
	fi= fj;
      }
    size_t j= numSteps-1;
    theDif[j]= fj;
    
    // Set the method return value
    PathSeries *retval= new PathSeries(theDif, delta);    
    if(retval == 0)
      {
        std::cerr << "TrapezoidalTimeSeriesIntegrator::" << __FUNCTION__
		  << "; ran out of memory creating PathSeries\n";
        return 0;
      }
    else
      {
	const PathSeriesBase *tmp= dynamic_cast<const PathSeriesBase *>(theSeries);
	if(tmp)
	  {
	    const bool useLast= tmp->getUseLast();
	    retval->setUseLast(useLast);
	  }
      }
    return retval;
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
