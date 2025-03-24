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

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::PeriodSeries::getPyDict(void) const
  {
    boost::python::dict retval= PulseBaseSeries::getPyDict();
    retval["period"]= this->period;
    retval["shift"]= this->shift;
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::PeriodSeries::setPyDict(const boost::python::dict &d)
  {
    PulseBaseSeries::setPyDict(d);
    this->period= boost::python::extract<double>(d["period"]);
    this->shift= boost::python::extract<double>(d["shift"]);
  }

//! @brief Send object members through the communicator argument.
int XC::PeriodSeries::sendData(Communicator &comm)
  {
    int res= PulseBaseSeries::sendData(comm);
    res+= comm.sendDoubles(period,shift,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::PeriodSeries::recvData(const Communicator &comm)
  {
    int res= PulseBaseSeries::recvData(comm);
    res+= comm.receiveDoubles(period,shift,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Print stuff.
void XC::PeriodSeries::Print(std::ostream &s, int flag) const
  {
    s << "Period Series" << std::endl;
    s << "\tFactor: " << cFactor << std::endl;
    s << "\ttStart: " << tStart << std::endl;
    s << "\ttFinish: " << tFinish << std::endl;
    s << "\tPeriod: " << period << std::endl;
    s << "\tPhase Shift: " << shift << std::endl;
  }
