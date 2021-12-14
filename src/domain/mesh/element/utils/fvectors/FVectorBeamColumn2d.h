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
//FVectorBeamColumn2d.h

#ifndef FVectorBeamColumn2d_h
#define FVectorBeamColumn2d_h

#include "FVectorData.h"

namespace XC {


//! @ingroup ElemFV
//
//! @brief Internal forces for a beam-column 2D element.
class FVectorBeamColumn2d: public FVectorData<3>
  {
  public:
    FVectorBeamColumn2d(void);
    FVectorBeamColumn2d(const FVectorBeamColumn2d &);
    explicit FVectorBeamColumn2d(const Vector &);
    FVectorBeamColumn2d &operator=(const FVectorBeamColumn2d &);

    void zero(void);
    FVectorBeamColumn2d &operator*=(const double &d);
    FVectorBeamColumn2d &operator+=(const FVectorBeamColumn2d &);
    FVectorBeamColumn2d &operator-=(const FVectorBeamColumn2d &);

    void Print(std::ostream &os) const;
  };
} // end of XC namespace

#endif


