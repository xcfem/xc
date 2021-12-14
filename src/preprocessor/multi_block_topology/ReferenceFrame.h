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
//ReferenceFrame.h

#ifndef SISREF_H
#define SISREF_H

#include "preprocessor/EntMdlrBase.h"

class Vector3d;
class Pos3d;

namespace XC {

//! @ingroup MultiBlockTopology
//!
//! @defgroup MultiBlockTopologySR Reference systems.
//
//! @ingroup MultiBlockTopologySR
//!
//! @brief Reference system to be used for simpliflying the definition
//! of point positions.
class ReferenceFrame: public EntMdlrBase
  {
  protected:

  public:
    //! @brief Constructor.
    ReferenceFrame(Preprocessor *m)
      : EntMdlrBase("",m) {}
    ReferenceFrame(const std::string &name= "",Preprocessor *m= nullptr)
      : EntMdlrBase(name,m) {}
    inline virtual ~ReferenceFrame(void)
      {}
    //! @brief Returns the i-th axis unit vector
    //! for the position being passed as parameter.
    virtual Vector3d getAxisVDir(const size_t &,const Pos3d &) const= 0;
    Vector3d getIVector(const Pos3d &) const;
    Vector3d getJVector(const Pos3d &) const;
    Vector3d getKVector(const Pos3d &) const;

    //! @brief Return the global coordinates of point p.
    virtual Pos3d getGlobalPosition(const Pos3d &p) const= 0;
    //! @brief Return the global coordinates of vector v.
    virtual Vector3d getGlobalCoordinates(const Vector3d &v) const= 0;
    //! @brief Return the local coordinates of point p.
    virtual Pos3d getLocalPosition(const Pos3d &p) const= 0;
    //! @brief Return the local coordinates of vector v.
    virtual Vector3d getLocalCoordinates(const Vector3d &v) const= 0;


  };

} //end of XC namespace

#endif
