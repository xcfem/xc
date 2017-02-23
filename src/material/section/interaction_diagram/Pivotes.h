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
//Pivotes.h
//Calcula los pivotes para un plano de flexión determinado.


#ifndef PIVOTES_H
#define PIVOTES_H

#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"

namespace XC {

class CalcPivotes;

//! @ingroup MATSCCDiagInt
//
//! @brief Pivot positions computed for a bending plane.
class Pivotes
  {
    Pos3d A; //!< Pivote A (max strain in tensioned steel).
    Pos3d B; //!< Pivote B (min strain in compressed concrete).
    Pos3d C; //!< Pivote C (ver diagrama de pivotes en EHE).
    Pos3d D; //!< Zero strain concrete.
    bool ok; //!< True if positions are ok (see checkPositions).

  public:
    Pivotes(const Pos3d &a,const Pos3d &b,const Pos3d &c,const Pos3d &d);
    Pivotes(const CalcPivotes &cp);
    inline const Pos3d &getPuntoD(void) const
      { return D; }
    inline const Pos3d &getPivoteA(void) const
      { return A; }
    inline const Pos3d &getPivoteB(void) const
      { return B; }
    inline const Pos3d &getPivoteC(void) const
      { return C; }
    inline Pos3d getPuntoA(const double &epsilon)
      { return Pos3d(epsilon,A.y(),A.z()); }
    inline Pos3d getPuntoB(const double &epsilon)
      { return Pos3d(epsilon,B.y(),B.z()); }
    inline Pos3d getPuntoD(const double &epsilon)
      { return Pos3d(epsilon,D.y(),D.z()); }
    inline double getEpsilonA(void) const
      { return A.x(); }
    inline double getEpsilonB(void) const
      { return B.x(); }
    inline double getEpsilonC(void) const
      { return C.x(); }
    inline double getEpsilonD(void) const
      { return B.x(); }
    inline Pos2d getPosPivoteA(void) const
      { return Pos2d(A.y(),A.z()); }
    inline Pos2d getPosPivoteB(void) const
      { return Pos2d(B.y(),B.z()); }
    inline Pos2d getPosPivoteC(void) const
      { return Pos2d(C.y(),C.z()); }
    inline Pos2d getPosPuntoD(void) const
      { return Pos2d(D.y(),D.z()); }
    inline bool Ok(void) const
      { return ok; }
    bool checkPositions(void) const;
    void print(std::ostream &) const;
  };

inline std::ostream &operator<<(std::ostream &os, const Pivotes &p)
  {
    p.print(os);
    return os;
  }

} // end of XC namespace

#endif
