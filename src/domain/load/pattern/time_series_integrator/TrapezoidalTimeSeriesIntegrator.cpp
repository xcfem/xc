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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
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
    // check a XC::TimeSeries object was passed
    if(!theSeries)
      {
        std::cerr << "XC::TrapezoidalTimeSeriesIntegrator::integrate() - - no XC::TimeSeries passed\n";
        return 0;
      }

    // Add one to get ceiling out of type cast
    const int numSteps= static_cast<int>(theSeries->getDuration()/delta + 1.0);

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
    PathSeries *returnSeries= new XC::PathSeries(theIntegratedValues, delta);

    if(returnSeries == 0)
      {
        std::cerr << "XC::TrapezoidalTimeSeriesIntegrator::integrate() Ran out of memory creating XC::PathSeries\n";
        return 0;
      }
    return returnSeries;
  }

int XC::TrapezoidalTimeSeriesIntegrator::sendSelf(CommParameters &cp)
  { return 0; }

int XC::TrapezoidalTimeSeriesIntegrator::recvSelf(const CommParameters &cp)
  { return 0; }

void XC::TrapezoidalTimeSeriesIntegrator::Print(std::ostream &s, int flag)
  {
    // Need to implement, return for now
    return;
  }
