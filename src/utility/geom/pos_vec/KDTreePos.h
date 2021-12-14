// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//KDTreePos.h
#ifndef KDTreePos_h
#define KDTreePos_h

#include <cstdlib>

class Pos3d;

//! @brief Base class for KDTree positions.
//! @ingroup GEOM
class KDTreePos
  {
  public:
    typedef double value_type;
  private:
    value_type d[3];
  public:
    explicit KDTreePos(const Pos3d &p);
    double distance_to(const KDTreePos &other) const;
    inline value_type operator[](const size_t &N) const { return d[N]; }
  };

#endif
