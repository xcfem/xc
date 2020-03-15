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
//SectionCartesianReferenceFrame.h
//Cartesian coordinate reference frame.

#ifndef SISREFSCCCARTESIANAS2D_H
#define SISREFSCCCARTESIANAS2D_H

#include "SectionReferenceFrame.h"
#include "xc_utils/src/geom/sis_ref/Ref2d2d.h"

namespace XC {

//! @ingroup MATSCCEntGeom
//
//! @brief Bidimensional reference system for a section.
class SectionCartesianReferenceFrame: public SectionReferenceFrame
  {
    Ref2d2d ref;//!< Reference system.
  protected:

  public:
    //! @brief Constructor.
    SectionCartesianReferenceFrame(GeomSection *m)
      : SectionReferenceFrame(m) {}
    SectionCartesianReferenceFrame(const std::string &name= "",GeomSection *m= nullptr)
      : SectionReferenceFrame(name,m) {}

    Vector2d getAxisVDir(const size_t &,const Pos2d &) const;


    Pos2d GetPosGlobal(const Pos2d &p) const;
    Vector2d GetCooGlobales(const Vector2d &v) const;
    Pos2d GetPosLocal(const Pos2d &p) const;
    Vector2d GetCooLocales(const Vector2d &v) const;


  };

} //end of XC namespace

#endif
