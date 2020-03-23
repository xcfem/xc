// -*-c++-*-
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
// PeriodSeries.h
                                                                        
#ifndef PeriodSeries_h
#define PeriodSeries_h

#include "PulseBaseSeries.h"

namespace XC {
//! @ingroup TSeries
//
//! @brief Periodic function over time.
class PeriodSeries: public PulseBaseSeries
  {
  protected:
    double period; //!< period of pulse series (sec)
    double shift;  //!< phase shift of pulse series (sec)

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public:
    // constructors
    PeriodSeries(int classTag,double tStart,double tFinish,double period,double shift,double cFactor = 1.0);
    PeriodSeries(int classTag);

    inline double getPeriod(void) const
      { return period; }
    inline void setPeriod(const double &d)
      { period= d; }
    inline double getShift(void) const
      { return shift; }
    inline void setShift(const double &d)
      { shift= d; }
    
    void Print(std::ostream &s, int flag =0) const;

  };
} // end of XC namespace

#endif
