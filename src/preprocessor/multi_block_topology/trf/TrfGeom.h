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
//TrfGeom.h

#ifndef TRFGEOM_H
#define TRFGEOM_H

#include "preprocessor/EntMdlrBase.h"

class Vector3d;
class Pos3d;
class Pos3dArray;
class Pos3dArray3d;

namespace XC {


//! @ingroup MultiBlockTopology
//!
//! @defgroup MultiBlockTopologyTrf Geometric transformations.
//
//!  @ingroup MultiBlockTopologyTrf
//! 
//!  Geometric transformation that can be applied to the components of a set.
class TrfGeom: public EntMdlrBase
  {
  public:
    //! @brief Constructor.
    TrfGeom(Preprocessor *m)
      : EntMdlrBase("",m) {}
    inline virtual ~TrfGeom(void)
      {}
    //! @brief Applies the transformation to the point.
    virtual Pos3d Transform(const Pos3d &p) const= 0;
    virtual Vector3d Transform(const Vector3d &v) const= 0;
    const Pos3dArray &Transform(const Pos3dArray &m) const;
    const Pos3dArray3d &Transform(const Pos3dArray3d &m) const;
  };
} //end of XC namespace

#endif
