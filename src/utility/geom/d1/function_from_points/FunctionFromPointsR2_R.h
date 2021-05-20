// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.  
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//FunctionFromPointsR2_R.h

#ifndef FUNCTIONFROMPOINTSR2_R_H
#define FUNCTIONFROMPOINTSR2_R_H

#include "FunctionFromPointsR2_T.h"
#include <deque>

typedef std::deque<double> dq_double;

//! @brief f:R2->R defined on a set of points.
//!
//! Function that assigns a real to the points of its domain.
class FunctionFromPointsR2_R: public FunctionFromPointsR2_T<double>
  {
  public:
    typedef FunctionFromPointsR2_T<double> fR2_R;
    typedef std::pair<int,int> row_column;
    //! Constructor.
    FunctionFromPointsR2_R(const size_t &n_rows=1,const size_t &n_columns=1)
      : fR2_R(n_rows,n_columns) {}
    //! Constructor.
    FunctionFromPointsR2_R(const Pos2dArray &dom,const double &vdef)
      : fR2_R(dom,vdef) {}
    FunctionFromPointsR2_R(const Pos2dArray &dom,const m_values &v)
      : fR2_R(dom,v) {}
    row_column PosMax(void) const;
    double Max(void) const;
    Pos2d getMaxPoint(void) const;
    row_column PosMin(void) const;
    double Min(void) const;
    Pos2d getMinPoint(void) const;
  };


#endif
