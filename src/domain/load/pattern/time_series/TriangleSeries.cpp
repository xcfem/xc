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

// $Revision: 1.1 $
// $Date: 2005/12/15 00:35:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/TriangleSeries.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/04
// Revision: A
//
// Purpose: This file contains the class implementation of XC::TriangleSeries.
//
// What: "@(#) TriangleSeries.cpp, revA"


#include <domain/load/pattern/time_series/TriangleSeries.h>
#include <utility/matrix/Vector.h>
#include <classTags.h>
#include <cmath>
#include "utility/actor/actor/MovableVector.h"
#include "utility/matrix/ID.h"


XC::TriangleSeries::TriangleSeries(double startTime, double finishTime,
    double T, double phi, double theFactor)
  : PeriodSeries(TSERIES_TAG_TriangleSeries,startTime,finishTime,T,phi,theFactor) {}


XC::TriangleSeries::TriangleSeries()
  : PeriodSeries(TSERIES_TAG_TriangleSeries) {}

//! @brief Return load factor at specified time.
double XC::TriangleSeries::getFactor(double pseudoTime) const
  {	
    if(tStart <= pseudoTime && pseudoTime <= tFinish)
      {
	const double slope= cFactor/(period/4);
	const double k= (pseudoTime+shift)/period - floor((pseudoTime+shift)/period);
        if(k < 0.25)
	  return slope*k*period;
	else if(k < 0.75)
	  return cFactor-slope*(k-0.25)*period;
	else if(k < 1.00)
	  return -cFactor + slope*(k-0.75)*period;
	else
	  return 0;
      }
    else
      return 0;
  }

int XC::TriangleSeries::sendSelf(CommParameters &cp)
  {
    static ID data(3);
    int result= sendData(cp);

    const int dataTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << "PeriodSeries::sendSelf() - ch failed to send data\n";
    return result;
  }


int XC::TriangleSeries::recvSelf(const CommParameters &cp)
  {
    static ID data(3);

    const int dataTag = this->getDbTag();  
    int result = cp.receiveIdData(getDbTagData(),dataTag);
    if(result<0)
      std::cerr << "PeriodSeries::sendSelf() - ch failed to receive data\n";
    else
      result+= recvData(cp);
    return result;
  }



void XC::TriangleSeries::Print(std::ostream &s, int flag) const
  {
    s << "Triangle Series" << std::endl;
    s << "\tFactor: " << cFactor << std::endl;
    s << "\ttStart: " << tStart << std::endl;
    s << "\ttFinish: " << tFinish << std::endl;
    s << "\tPeriod: " << period << std::endl;
    s << "\tPhase Shift: " << shift << std::endl;
  }
