// -*-c++-*-
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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:01:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/SimpsonTimeSeriesIntegrator.h,v $
                                                                        
                                                                        
// File: src/domain/SimpsonTimeSeriesIntegrator.h
// 
// Written: MHS
// Created: 10/99
// Revision: A
// Revision: A
//
// Description: This file contains the class definition for 
// a SimpsonTimeSeriesIntegrator, which integrates a
// ground motion TimeSeries using the Simpson's rule.

#ifndef SimpsonTimeSeriesIntegrator_h
#define SimpsonTimeSeriesIntegrator_h

#include <domain/load/pattern/TimeSeriesIntegrator.h>
#include <iostream>

namespace XC {

//! @ingroup TSeries
//
//! @brief integrates a ground motion TimeSeries using the simpson rule.
class SimpsonTimeSeriesIntegrator: public TimeSeriesIntegrator
  {
   public:
     SimpsonTimeSeriesIntegrator(void);
     TimeSeriesIntegrator *getCopy(void) const;

     TimeSeries* integrate(TimeSeries *theTimeSeries, double delta) const;
     TimeSeries* differentiate(TimeSeries *theSeries, double delta) const;

     // methods for output    
     int sendSelf(Communicator &);
     int recvSelf(const Communicator &);
     void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif
