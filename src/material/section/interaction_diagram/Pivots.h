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
//Pivots.h
//Pivots for a bending plane.


#ifndef PIVOTS_H
#define PIVOTS_H

#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"

namespace XC {

class ComputePivots;

//! @ingroup MATSCCDiagInt
//
//! @brief Pivot positions computed for a bending plane.
class Pivots
  {
    Pos3d A; //!< Pivot A (max strain in tensioned steel).
    Pos3d B; //!< Pivot B (min strain in compressed concrete).
    Pos3d C; //!< Pivot C (ver diagrama de pivots en EHE).
    Pos3d D; //!< Zero strain concrete.
    bool ok; //!< True if positions are ok (see checkPositions).

  public:
    Pivots(const Pos3d &a,const Pos3d &b,const Pos3d &c,const Pos3d &d);
    Pivots(const ComputePivots &cp);
    inline const Pos3d &getDPoint(void) const
      { return D; }
    inline const Pos3d &getAPivot(void) const
      { return A; }
    inline const Pos3d &getBPivot(void) const
      { return B; }
    inline const Pos3d &getCPivot(void) const
      { return C; }
    inline Pos3d getAPoint(const double &epsilon)
      { return Pos3d(epsilon,A.y(),A.z()); }
    inline Pos3d getBPoint(const double &epsilon)
      { return Pos3d(epsilon,B.y(),B.z()); }
    inline Pos3d getDPoint(const double &epsilon)
      { return Pos3d(epsilon,D.y(),D.z()); }
    inline double getEpsilonA(void) const
      { return A.x(); }
    inline double getEpsilonB(void) const
      { return B.x(); }
    inline double getEpsilonC(void) const
      { return C.x(); }
    inline double getEpsilonD(void) const
      { return B.x(); }
    inline Pos2d getAPivotPosition(void) const
      { return Pos2d(A.y(),A.z()); }
    inline Pos2d getBPivotPosition(void) const
      { return Pos2d(B.y(),B.z()); }
    inline Pos2d getCPivotPosition(void) const
      { return Pos2d(C.y(),C.z()); }
    inline Pos2d getPointDPosition(void) const
      { return Pos2d(D.y(),D.z()); }
    inline bool Ok(void) const
      { return ok; }
    bool checkPositions(void) const;
    void print(std::ostream &) const;
  };

inline std::ostream &operator<<(std::ostream &os, const Pivots &p)
  {
    p.print(os);
    return os;
  }

} // end of XC namespace

#endif
