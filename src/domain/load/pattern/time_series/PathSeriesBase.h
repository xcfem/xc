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
//PathSeriesBase.h

#ifndef PathSeriesBase_h
#define PathSeriesBase_h

#include <domain/load/pattern/time_series/CFactorSeries.h>
#include "utility/matrix/Vector.h"

namespace XC {
class Vector;

//! @ingroup TSeries
//
//! @brief Base class for time-dependent functions defined
//! by a series of points (ti,fi).
class PathSeriesBase: public CFactorSeries
  {
  protected:
    Vector thePath; //!< vector containg the data points
    int lastSendCommitTag;

    static size_t load_vector_from_file(Vector &v,std::istream &is);

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    // constructors
    PathSeriesBase(int classTag,const double &cfactor= 1.0);
    PathSeriesBase(int classTag,const Vector &thePath,const double &cfactor= 1.0);

    size_t getNumDataPoints(void) const;
    size_t getNumDataPointsOnFile(const std::string &) const;
    double getPeakFactor(void) const;

    inline Vector getPath(void) const
      { return thePath; }
    inline void setPath(const Vector &d)
      { thePath= d; }
    inline int getLastSendCommitTag(void) const
      { return lastSendCommitTag; }
    inline void setLastSendCommitTag(const int &d)
      { lastSendCommitTag= d; }

    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif

