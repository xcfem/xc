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
                                                                        
// $Revision: 1.6 $
// $Date: 2005/10/20 21:58:54 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/PathTimeSeries.h,v $
                                                                        
                                                                        
#ifndef PathTimeSeries_h
#define PathTimeSeries_h

// File: ~/domain/load/pattern/PathTimeSeries.h
// 
// Written: fmk 
// Created: 07/99
// Revision: A
//
// Description: This file contains the class definition for PathTimeSeries.
// PathTimeSeries is a CFactorSeries class which 
// object. 
//
// What: "@(#) PathTimeSeries.h, revA"

#include "domain/load/pattern/time_series/PathSeriesBase.h"

namespace XC {
class Vector;

//! @ingroup TSeries
//
//! @brief Time-dependent function that linear interpolates the
//! load factor using user specified control points provided in a vector object.
//!
//! The relationship between the pseudo time and the load factor follows
//! a user specified path. The path points are specified at user specified
//! time values. For a pseudo time not at a path point, linear interpolation
//! is performed to determine the load factor. If the time specified is
//! beyond the last path point a load factor of \f$0.0\f$ will be returned.
class PathTimeSeries : public PathSeriesBase
  {
  private:
    Vector time; //!< vector containing the time values of data points
    mutable int currentTimeLoc; //!< current location in time
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public:
    // constructors  
    PathTimeSeries(void);
    PathTimeSeries(const Vector &thePath,const Vector &theTime,  double cfactor = 1.0);
    PathTimeSeries(const std::string &fileNamePath, const std::string &fileNameTime, double cfactor = 1.0);    
    PathTimeSeries(const std::string &fileName,double cfactor = 1.0);
    TimeSeries *getCopy(void) const
      { return new PathTimeSeries(*this); }
    

    // method to get factor
    double getFactor(double pseudoTime) const;
    double getDuration(void) const;
    double getTimeIncr(double pseudoTime) const;

    inline Vector getTime(void) const
      { return time; }
    inline void setTime(const Vector &d)
      { time= d; }
    inline int getCurrentTimeLoc(void) const
      { return currentTimeLoc; }
    inline void setCurrentTimeLoc(const int &d)
      { currentTimeLoc= d; }

    void readFromFile(const std::string &fileName);
    void readFromFiles(const std::string &filePathName,const std::string &fileTimeName);

    // methods for output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif

