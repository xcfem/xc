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
** (C) Copyright 2001, The Regents of the University of California    **
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
** Reliability module developed by:                                   **
**   Terje Haukaas (haukaas@ce.berkeley.edu)                          **
**   Armen Der Kiureghian (adk@ce.berkeley.edu)                       **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.1 $
// $Date: 2003/03/04 00:49:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/SimulatedRandomProcessSeries.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu), February 2002
// Revised: 
//

#include <domain/load/pattern/time_series/SimulatedRandomProcessSeries.h>
#include <reliability/domain/spectrum/Spectrum.h>
#include <reliability/analysis/randomNumber/RandomNumberGenerator.h>
#include <utility/matrix/Vector.h>
#include <reliability/domain/spectrum/Spectrum.h>
#include <reliability/analysis/randomNumber/RandomNumberGenerator.h>
#include <classTags.h>
#include <math.h>

XC::SimulatedRandomProcessSeries::SimulatedRandomProcessSeries(RandomNumberGenerator *theRandNumGenerator,
								Spectrum *theSpectr,
								int numFreqInt,
								double pmean)
  : TimeSeries(TSERIES_TAG_SimulatedRandomProcessSeries)
  {
	theRandomNumberGenerator = theRandNumGenerator;
	theSpectrum = theSpectr;
	numFreqIntervals = numFreqInt;
	mean = pmean;

	
	// Generate random numbers, uniformly distributed between 0 and 2pi
	const double pi= M_PI;
	theRandomNumberGenerator->generate_nIndependentUniformNumbers(numFreqIntervals,0.0,(2*pi));
	Vector theta1 = theRandomNumberGenerator->getGeneratedNumbers();
	theta= Vector(theta1);
	

	// Generate standard normal random numbers
	theRandomNumberGenerator->generate_nIndependentStdNormalNumbers(numFreqIntervals);
	Vector A1 = theRandomNumberGenerator->getGeneratedNumbers();
	A= Vector(A1);


	// Length of each interval
	deltaW = (theSpectrum->getMaxFrequency()-theSpectrum->getMinFrequency())/numFreqIntervals;
  }

//! @brief Return load factor at specified time.
double XC::SimulatedRandomProcessSeries::getFactor(double time) const
  {
//static ofstream outputFile( "simulated_process.out" , ios::out );

    if(time == 0.0)
      { return 0.0; }
    else
      {
        // Add up over all frequency intervals
	double factor= 0.0;
	for(int i=0; i<numFreqIntervals; i++)
          {
	    const double W = (i+0.5)*deltaW+theSpectrum->getMinFrequency();
	    const double S = theSpectrum->getAmplitude(W);
	    factor+= sqrt(2.0*S*deltaW) * A(i) * cos(W*time+theta(i));
	  }
//outputFile << (mean+factor) << endl;
	return (mean + factor);
      }
  }




int XC::SimulatedRandomProcessSeries::sendSelf(Communicator &comm)
{
	return 0;
}


int XC::SimulatedRandomProcessSeries::recvSelf(const Communicator &comm)
  { return 0; }


void XC::SimulatedRandomProcessSeries::Print(std::ostream &s, int flag) const
  {}
