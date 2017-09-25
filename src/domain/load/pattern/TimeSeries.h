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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/03/04 00:48:12 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/TimeSeries.h,v $
                                                                        
                                                                        
#ifndef TimeSeries_h
#define TimeSeries_h

// Written: fmk 
// Created: 07/99
// Revision: A
//
// Purpose: This file contains the class definition for TimeSeries.
// TimeSeries is an abstract class. 
//
// What: "@(#) TimeSeries.h, revA"

#include "utility/actor/actor/MovableObject.h"
#include "xc_utils/src/nucleo/EntCmd.h"

namespace XC {
  class Information;
  class Vector;

//! @ingroup LPatterns
//!
//!
//! @defgroup TSeries Time series.
//
//! @ingroup TSeries
//
//! @brief Time variation of loads.A TimeSeries object is used to
//! determine the load factor to be applied to the loads in a pettern.
//! to the model. 
class TimeSeries: public MovableObject, public EntCmd
  {
  public:
    TimeSeries(int classTag);
    inline virtual ~TimeSeries(void)
      {}

    virtual TimeSeries *getCopy(void) const= 0;

    // pure virtual functions
    virtual double getFactor(double pseudoTime) const= 0;
    virtual double getDuration(void) const= 0;
    virtual double getPeakFactor(void) const= 0;

    virtual double getTimeIncr(double pseudoTime) const= 0;
    // This is defined to be the time increment from the argument
    // 'pseudoTime' to the NEXT point in the time series path
    // THIS MAY CHANGE -- MAY BE BETTER TO GET THE TIME INCREMENT
    // FROM THE PREVIOUS POINT IN THE PATH UP TO 'pseudoTime', WILL
    // DECIDE ONCE GroundMotionIntegrator IS IMPLEMENTED

    virtual void Print(std::ostream &s, int flag = 0) const= 0;        

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    virtual int setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int updateParameter(int parameterID, Information &info);
    virtual int activateParameter(int parameterID);
    virtual double getFactorSensitivity(double pseudoTime);
    // AddingSensitivity:BEGIN //////////////////////////////////////////
  };

int sendTimeSeriesPtr(TimeSeries *,int posClassTag, int posDbTag,DbTagData &,CommParameters &cp);
TimeSeries *receiveTimeSeriesPtr(TimeSeries *,int posClassTag, int posDbTag,DbTagData &,const CommParameters &cp);

} // end of XC namespace

#endif
