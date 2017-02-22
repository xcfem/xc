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
//PulseBaseSeries.h
                                                                        
#ifndef PulseBaseSeries_h
#define PulseBaseSeries_h

#include "CFactorSeries.h"

namespace XC {
//! @ingroup TSeries
//
//! @brief Base class for pulse type time series.
class PulseBaseSeries: public CFactorSeries
  {
  protected:
    double tStart;    // start time of time series (sec)
    double tFinish;   // end time of time series (sec)

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public:
    // constructors
    PulseBaseSeries(int classTag,const double &tS=0.0,const double &tF= 0.0,const double &factor=1.0);
    
    inline double getDuration(void) const {return tFinish-tStart;}
    inline double getPeakFactor(void) const {return cFactor;}
    inline double getTimeIncr(double pseudoTime) const {return tFinish-tStart;}

    inline double getStart(void) const
      { return tStart; }
    inline void setStart(const double &d)
      { tStart= d; }
    inline double getFinish(void) const
      { return tFinish; }
    inline void setFinish(const double &d)
      { tFinish= d; }
    
    // methods for output    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0) const;    

  };
} // end of XC namespace

#endif
