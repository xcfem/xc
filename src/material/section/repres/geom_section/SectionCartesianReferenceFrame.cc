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
//SectionCartesianReferenceFrame.cc

#include "SectionCartesianReferenceFrame.h"
#include "Spot.h"
#include "xc_utils/src/geom/pos_vec/Vector2d.h"


//! @brief Returns the i-th axis unit vector expressed in global coordinates
//! for the position being passed as parameter.
Vector2d XC::SectionCartesianReferenceFrame::GetVDirEje(const size_t &i,const Pos2d &) const
  { return ref.GetVDirEje(i); }

//! @brief Return the posicion del punto p expresado en locales
//! expresada en coordenadas globales.
Pos2d XC::SectionCartesianReferenceFrame::GetPosGlobal(const Pos2d &p) const
  { return ref.GetPosGlobal(p); }
//! @brief Returns the vector v expresado en locales
//! expresado en coordenadas globales.
Vector2d XC::SectionCartesianReferenceFrame::GetCooGlobales(const Vector2d &v) const
  { return ref.GetCooGlobales(v); }
//! @brief Return the posicion del punto p expresado en globales
//! expresada en coordenadas locales.
Pos2d XC::SectionCartesianReferenceFrame::GetPosLocal(const Pos2d &p) const
  { return ref.GetPosLocal(p); }
//! @brief Returns the vector v expresado en globales
//! expresado en coordenadas locales.
Vector2d XC::SectionCartesianReferenceFrame::GetCooLocales(const Vector2d &v) const
  { return ref.GetCooLocales(v); }
