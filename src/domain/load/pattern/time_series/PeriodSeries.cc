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
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/PeriodSeries.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/04
// Revision: A
//
// Purpose: This file contains the class implementation of XC::PeriodSeries.
//
// What: "@(#) PeriodSeries.cpp, revA"


#include "PeriodSeries.h"
#include <utility/matrix/Vector.h>
#include "utility/actor/actor/MovableVector.h"
#include "utility/matrix/ID.h"



//! @brief Constructor.
XC::PeriodSeries::PeriodSeries(int classTag,double startTime, double finishTime,
    double T, double phi, double theFactor)
  : PulseBaseSeries(classTag,startTime,finishTime,theFactor), period(T),shift(phi)
  {
    if(period == 0.0)
      {
        std::cerr << "PeriodSeries::PeriodSeries -- input period is zero, setting period to 1\n";
        period = 1;
      }
  }

//! @brief Default constructor.
XC::PeriodSeries::PeriodSeries(int classTag)
  : PulseBaseSeries(classTag,1.0), period(1.0),shift(0.0) {}

//! @brief Send object members through the channel being passed as parameter.
int XC::PeriodSeries::sendData(CommParameters &cp)
  {
    int res= PulseBaseSeries::sendData(cp);
    res+= cp.sendDoubles(period,shift,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::PeriodSeries::recvData(const CommParameters &cp)
  {
    int res= PulseBaseSeries::recvData(cp);
    res+= cp.receiveDoubles(period,shift,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Imprime el objeto.
void XC::PeriodSeries::Print(std::ostream &s, int flag) const
  {
    s << "Period Series" << std::endl;
    s << "\tFactor: " << cFactor << std::endl;
    s << "\ttStart: " << tStart << std::endl;
    s << "\ttFinish: " << tFinish << std::endl;
    s << "\tPeriod: " << period << std::endl;
    s << "\tPhase Shift: " << shift << std::endl;
  }
