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
// $Date: 2003/02/14 23:00:59 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/ConstantSeries.h,v $

#ifndef ConstantSeries_h
#define ConstantSeries_h

// File: ~/domain/load/pattern/ConstantSeries.h
//
// Written: fmk 
// Created: 07/99
// Revision: A
//
// Purpose: This file contains the class definition for ConstantSeries.
// ConstantSeries is a concrete class. A ConstantSeries object provides
// a linear time series. the factor is given by the pseudoTime and 
// a constant factor provided in the constructor.
//
// What: "@(#) ConstantSeries.h, revA"

#include <domain/load/pattern/time_series/CFactorSeries.h>

namespace XC {
//! @ingroup TSeries
//
//! @brief Constant function over time.
class ConstantSeries : public CFactorSeries
  {
  public:
    ConstantSeries(double cf= 1.0);
    TimeSeries *getCopy(void) const;

    // None of the following functions should be invoked on this type
    // of object
    double getDuration(void) const {return 0.0;} // dummy function
    double getPeakFactor(void) const {return cFactor;} // dummy function
    double getTimeIncr(double pseudoTime) const {return 1.0;} // dummy function
    
    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif
