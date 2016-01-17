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
** (C) Copyright 2001, The Regents of the University of California    **
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

XC::SimulatedRandomProcessSeries::SimulatedRandomProcessSeries(
								RandomNumberGenerator *theRandNumGenerator,
								Spectrum *theSpectr,
								int numFreqInt,
								double pmean)
:TimeSeries(TSERIES_TAG_SimulatedRandomProcessSeries)
{
	theRandomNumberGenerator = theRandNumGenerator;
	theSpectrum = theSpectr;
	numFreqIntervals = numFreqInt;
	mean = pmean;

	
	// Generate random numbers, uniformly distributed between 0 and 2pi
	double pi = 3.14159265358979;
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

double XC::SimulatedRandomProcessSeries::getFactor(double time) const
  {
//static ofstream outputFile( "simulated_process.out" , ios::out );

    if(time == 0.0)
      { return 0.0; }
    else
      {
        // Add up over all frequency intervals
	double factor = 0.0;
	double W, S;
	for(int i=0; i<numFreqIntervals; i++)
          {
	    W = (i+0.5)*deltaW+theSpectrum->getMinFrequency();
	    S = theSpectrum->getAmplitude(W);
	    factor += sqrt(2.0*S*deltaW) * A(i) * cos(W*time+theta(i));
	  }
//outputFile << (mean+factor) << endl;
	return (mean + factor);
      }
  }




int XC::SimulatedRandomProcessSeries::sendSelf(CommParameters &cp)
{
	return 0;
}


int XC::SimulatedRandomProcessSeries::recvSelf(const CommParameters &cp)
  { return 0; }


void XC::SimulatedRandomProcessSeries::Print(std::ostream &s, int flag) const
  {}
