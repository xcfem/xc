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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/10/27 23:04:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/filter/StandardLinearOscillatorAccelerationFilter.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include "StandardLinearOscillatorAccelerationFilter.h"
#include <classTags.h>
#include <cmath>


XC::StandardLinearOscillatorAccelerationFilter::StandardLinearOscillatorAccelerationFilter(int tag, double period, double dampingRatio)
:Filter(tag,FILTER_standardLinearOscillator)
  {
    double pi = 3.14159265358979;
    wn = 2*pi/period;
    xi = dampingRatio;
  }

double XC::StandardLinearOscillatorAccelerationFilter::getAmplitude(double time) const
  {
    if(time<0.0)
      {	return 0.0; }
    else
      {
    	double wd = wn * sqrt(1.0-pow(xi,2.0));
    	return (  ( xi*xi*wn*wn*sin(wd*time) - 2.0*xi*wn*wd*cos(wd*time) - wd*wd*sin(wd*time) ) * exp(-xi*wn*time)  );
      }
  }

double XC::StandardLinearOscillatorAccelerationFilter::getMaxAmplitude(void) const
  {
    //const double wd = wn * sqrt(1.0-pow(xi,2.0));
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; ERROR: method not implemented for acceleration filter."
	      << std::endl;
    return 0.0;
  }

double XC::StandardLinearOscillatorAccelerationFilter::getTimeOfMaxAmplitude(void) const
  {
    //double wd = wn * sqrt(1.0-pow(xi,2.0));
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; ERROR: method not implemented for acceleration filter."
	      << std::endl;
    return 0.0;
  }

//! @brief Print stuff.
void XC::StandardLinearOscillatorAccelerationFilter::Print(std::ostream &s, int flag) const
  {}
