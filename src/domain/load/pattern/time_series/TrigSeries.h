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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:01:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/TrigSeries.h,v $
                                                                        
                                                                        
#ifndef TrigSeries_h
#define TrigSeries_h

// File: ~/domain/load/pattern/TrigSeries.h
//
// Written: MHS 
// Created: July 2000
// Revision: A
//
// Purpose: This file contains the class definition for TrigSeries.
// TrigSeries is a concrete class. 
//
// What: "@(#) TrigSeries.h, revA"

#include "PeriodSeries.h"

namespace XC {
//! @ingroup TSeries
//
//! @brief Trigonometric functions over time. A TrigSeries object provides
//! a sine time series. the factor is given by the pseudoTime (x),
//! pulse period (T), phase shift (phi), i.e. by sin(2*PI*(x-xo)/T + phi),
//! and a constant factor provided in the constructor,
//! the duration by tStart and tFinal;
class TrigSeries: public PeriodSeries
  {
  public:
    // constructors
    TrigSeries(double tStart, double tFinish,
		double period, double shift, double cFactor = 1.0);
    TrigSeries(void);
    //! @brief Virtual constructor.
    TimeSeries *getCopy(void) const
      { return new TrigSeries(*this); }
    
    // method to get load factor
    double getFactor(double pseudoTime) const;
    
    // methods for output    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0) const;        

  };
} // end of XC namespace

#endif
