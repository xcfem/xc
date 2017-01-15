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
// $Date: 2003/03/04 00:44:51 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/spectrum/PointsSpectrum.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/domain/spectrum/PointsSpectrum.h>
#include <reliability/domain/spectrum/Spectrum.h>
#include <utility/matrix/Vector.h>
#include <classTags.h>


XC::PointsSpectrum::PointsSpectrum(int tag, Vector freq, Vector ampl)
:Spectrum(tag,SPECTRUM_points)
  {
    // Check that the frequency and the amplitude vectors have the same size
    int numPoints = freq.Size();
    if(numPoints != ampl.Size())
      {
	std::cerr << "Number of points to XC::PointsSpectrum is not consistent!" << std::endl;
      }

    // Check that the frequencies are consequtive
    for(int i=1; i<freq.Size(); i++)
      {
	if(freq(i-1)>freq(i))
          { std::cerr << "ERROR: The given XC::Spectrum frequencies are not consequtive!" << std::endl;	}
      }
    frequencies = new Vector(freq);
    amplitudes = new Vector(ampl);
  }

XC::PointsSpectrum::~PointsSpectrum(void)
  {
    if(frequencies != 0) 
		delete frequencies;
	if (amplitudes != 0)
		delete frequencies;
  }

void XC::PointsSpectrum::Print(std::ostream &s, int flag)  
  {}

double XC::PointsSpectrum::getMinFrequency()
  { return (*frequencies)(0); }

double XC::PointsSpectrum::getMaxFrequency()
  { return (*frequencies)(frequencies->Size()-1); }

double XC::PointsSpectrum::getAmplitude(double frequency)
  {
    double result= 0.0;
    if(!(frequency < (*frequencies)(0)  ||  frequency > (*frequencies)(frequencies->Size()-1) ))
      {
	double dy, dx, a, b;
	for(int i=1; i<frequencies->Size(); i++)
          {
	    if(frequency > (*frequencies)(i-1) && frequency < (*frequencies)(i))
              {
		dy = (*amplitudes)(i)  -  (*amplitudes)(i-1);
		dx = (*frequencies)(i)  -  (*frequencies)(i-1);
		a = dy/dx;
		b = (*amplitudes)(i-1);
		result = a * (frequency-(*frequencies)(i-1)) + b;
	      }
	  }
       }
    return result;
  }
