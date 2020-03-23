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
//SmallDispCrdTransf2d.h

#ifndef SmallDispCrdTransf2d_h
#define SmallDispCrdTransf2d_h

#include "CrdTransf2d.h"

namespace XC {

//! @ingroup ElemCT
//
//! @brief Base class for small displacements 2D coordinate transformations.
class SmallDispCrdTransf2d: public CrdTransf2d
  {
  protected:
    Vector &basic_to_local_resisting_force(const XC::Vector &pb, const XC::Vector &p0) const;
    const Vector &local_to_global_resisting_force(const Vector &pl) const;
    DbTagData &getDbTagData(void) const;
  public:
    SmallDispCrdTransf2d(int tag, int classTag);
    const Vector &getPointGlobalCoordFromLocal(const Vector &) const;
    const Vector &getPointGlobalDisplFromBasic(double xi, const Vector &) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace

#endif
