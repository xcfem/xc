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
// CFactorSeries.h

#ifndef CFactorSeries_h
#define CFactorSeries_h

#include <domain/load/pattern/TimeSeries.h>

namespace XC {
//! @ingroup TSeries
//
//! @brief Base class for constant factor time series.
class CFactorSeries: public TimeSeries
  {
  protected:
    double cFactor; //!< factor that multiplies series values.

    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);

  public:
    CFactorSeries(int classTag,double cf= 1.0);

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);

    //! @brief Return load factor at specified time.
    inline double getFactor(double pseudoTime) const
      { return cFactor; }
    //! @brief Sets factor value.
    void setFactor(const double &d)
      { cFactor= d; }
  };
} // end of XC namespace

#endif
