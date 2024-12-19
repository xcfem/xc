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

// $Revision$
// $Date$
// $URL$

// Written: Andreas Schellenberg
// Created: 05/15
// Revision: A
//
// Description: This file contains the class definition for 
// a SimpsonTimeSeriesIntegrator, which integrates a
// ground motion TimeSeries using the simpson rule.


#include "domain/load/pattern/time_series_integrator/SimpsonTimeSeriesIntegrator.h"
#include <domain/load/pattern/TimeSeries.h>
#include <utility/matrix/Vector.h>
#include <domain/load/pattern/time_series/PathSeries.h>


XC::SimpsonTimeSeriesIntegrator::SimpsonTimeSeriesIntegrator()
  : TimeSeriesIntegrator(TIMESERIES_INTEGRATOR_TAG_Simpson)
  {}


//! @brief Virtual constructor.
XC::TimeSeriesIntegrator *XC::SimpsonTimeSeriesIntegrator::getCopy(void) const
  { return new SimpsonTimeSeriesIntegrator(*this);}

XC::TimeSeries* XC::SimpsonTimeSeriesIntegrator::integrate(TimeSeries *theSeries, double delta) const
  {
    // Check for zero time step, before dividing to get number of steps
    if(delta <= 0.0)
      {
        std::cerr << "SimpsonTimeSeriesIntegrator::" << __FUNCTION__
	          << "; attempting to integrate time step"
		  << delta << "<= 0\n";
        return 0;
      }
    // check a TimeSeries object was passed
    if(!theSeries)
      {
        std::cerr << "SimpsonTimeSeriesIntegrator::" << __FUNCTION__
	          << "; no TimeSeries given.\n";
        return 0;
      }
    const double duration= theSeries->getDuration();
    if(duration<=0.0)
        std::cerr << "SimpsonTimeSeriesIntegrator::" << __FUNCTION__
	          << "; WARNING: duration is zero.\n";

    // add one to get ceiling out of type cast
    const size_t numSteps= static_cast<size_t>(duration/delta + 1.0);

    // create new vector for integrated values
    Vector theIntegratedValues(numSteps);


    double dummyTime= theSeries->getStartTime();
    // function values
    double fi= theSeries->getFactor(dummyTime);
    double fj= theSeries->getFactor(dummyTime+delta);
    double fk= theSeries->getFactor(dummyTime+2*delta);  
    // Integral values
    double Fi= 0.0;
    double Fj= delta/12.0*(5.0*fi + 8.0*fj - fk);
    double Fk= 0.0; 
    theIntegratedValues[0]= Fi;
    theIntegratedValues[1]= Fj;
    for(size_t i = 2; i < numSteps-1; i++)
      {
	// Apply the Simpson's rule to update the integral
	Fk = Fi + delta / 3.0 * (fi + 4.0 * fj + fk);

	theIntegratedValues[i]= Fk;

	fi= fj;
	fj= fk;

	Fi= Fj;
	Fj= Fk;
	dummyTime+= delta;
	fk= theSeries->getFactor(dummyTime);
	std::cout << "fi= " << fi << " fj= " << fj << " fk= " << fk << std::endl;
      }

    // Set the method return value
    PathSeries *retval= new PathSeries(theIntegratedValues, delta);

    if(retval == 0)
      {
        std::cerr << "TrapezoidalTimeSeriesIntegrator::" << __FUNCTION__
		  << "; ran out of memory creating PathSeries\n";
        return 0;
      }
    return retval;
  }


XC::TimeSeries* XC::SimpsonTimeSeriesIntegrator::differentiate(TimeSeries *theSeries, double delta) const
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
    Vector theDif(numSteps);
  
    double Fi= 0.0, Fj= 0.0, Fk= 0.0;  //function values
    double fi= 0.0, fj= 0.0, fk= 0.0;  //derivative values

    double dummyTime = theSeries->getStartTime();

    for(size_t i = 0; i < numSteps; i++, dummyTime += delta)
      {

	Fk= theSeries->getFactor(dummyTime);
	// Apply the Simpson's rule to update the derivative
	fk = 3.0 * (Fk - Fi) / delta - fi - 4.0 * fj;

	theDif[i] = fk;

	fi = fj;
	fj = fk;

	Fi = Fj;
	Fj = Fk;
      }

    // Set the method return value
    PathSeries *retval= new PathSeries(theDif, delta);
    if(retval == 0)
      {
        std::cerr << "TrapezoidalTimeSeriesIntegrator::" << __FUNCTION__
		  << "; ran out of memory creating PathSeries\n";
        return 0;
      }
    return retval;
  }


int XC::SimpsonTimeSeriesIntegrator::sendSelf(Communicator &comm)
  { return 0; }


int XC::SimpsonTimeSeriesIntegrator::recvSelf(const Communicator &comm)
  { return 0; }


void XC::SimpsonTimeSeriesIntegrator::Print(std::ostream &s, int flag) const
  {
    // Need to implement, return for now
    return;
  }

