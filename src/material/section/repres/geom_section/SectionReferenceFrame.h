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
//SectionReferenceFrame.h

#ifndef SISREFSCC_H
#define SISREFSCC_H

#include "EntGeomSection.h"

class Vector2d;
class Pos2d;

namespace XC {

//! @ingroup MATSCCEntGeom
//
//! @brief Reference system used for make easier defining point positions.
class SectionReferenceFrame: public EntGeomSection
  {
  protected:

  public:
    //! @brief Constructor.
    SectionReferenceFrame(GeomSection *m)
      : EntGeomSection("",m) {}
    SectionReferenceFrame(const std::string &name= "",GeomSection *m= nullptr)
      : EntGeomSection(name,m) {}

    //! @brief Return the i-th axis unit vector 
    //! expressed in global coordinates 
    //! for the position being passed as parameter.
    virtual Vector2d getAxisVDir(const size_t &,const Pos2d &) const= 0;
    Vector2d GetI(const Pos2d &) const;
    Vector2d GetJ(const Pos2d &) const;

    //! @brief Return the position of point p expressed in local coordinates
    //! expresada en global coordinates.
    virtual Pos2d GetPosGlobal(const Pos2d &p) const= 0;
    //! @brief Return the vector v expressed in local coordinates
    //! expressed in global coordinates.
    virtual Vector2d GetCooGlobales(const Vector2d &v) const= 0;
    //! @brief Return the position of point p expressed in global coordinates
    //! expresada en local coordinates.
    virtual Pos2d GetPosLocal(const Pos2d &p) const= 0;
    //! @brief Return the vector v expressed in global coordinates
    //! expressed in local coordinates.
    virtual Vector2d GetCooLocales(const Vector2d &v) const= 0;


  };

} //end of XC namespace

#endif
