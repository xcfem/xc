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
//Rotation.h

#ifndef ROTATION_H
#define ROTATION_H

#include "TrfGeom.h"
#include "xc_utils/src/geom/trf/Rotation3d.h"

namespace XC {

//! \ingroup CadTrf
//!
//! \brief Rotation transformation.
class Rotation: public TrfGeom
  {
    Rotation3d rr; //!< Rotación.
  protected:

  public:
    //! @brief Constructor.
    Rotation(Preprocessor *m)
      : TrfGeom(m), rr() {}

    void setAxisAng(const Recta3d &r,const double &ang)
      { rr= Rotation3d(r,ang); }
    virtual Pos3d Transforma(const Pos3d &p) const;
    virtual Vector3d Transforma(const Vector3d &v) const;
 };
} //end of XC namespace

#endif
