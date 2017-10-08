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
// $Date: 2003/02/14 23:01:00 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/RectangularSeries.cpp,v $
                                                                        
                                                                        
// File: ~/domain/load/pattern/RectangularSeries.C
//
// Written: fmk 
// Created: 07/99
// Revision: A
//
// Purpose: This file contains the class implementation of XC::RectangularSeries.
//
// What: "@(#) RectangularSeries.C, revA"


#include <domain/load/pattern/time_series/RectangularSeries.h>
#include <utility/matrix/Vector.h>

//! @brief Constructor.
//!
//! @param startTime: beginning of the rectangular pulse.
//! @param finishTime: end of the rectangular pulse.
//! @param theFactor: factor that multiplies the signal.
XC::RectangularSeries::RectangularSeries(double startTime, double finishTime,double theFactor)
  :PulseBaseSeries(TSERIES_TAG_RectangularSeries,startTime,finishTime,theFactor) {}

//! @brief Default constructor.
XC::RectangularSeries::RectangularSeries(void)
  :PulseBaseSeries(TSERIES_TAG_RectangularSeries) {}

//! @brief Returns factor (zero if pseudoTime outside of [tStart,tFinish]).
double XC::RectangularSeries::getFactor(double pseudoTime) const
  {	
    if(pseudoTime >= tStart && pseudoTime <= tFinish)
      return cFactor;
    else
      return 0;
  }

