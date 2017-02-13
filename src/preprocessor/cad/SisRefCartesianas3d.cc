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
//SisRefCartesianas3d.cc

#include "SisRefCartesianas3d.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/cad/entidades/Pnt.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"

#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"

Pos3d XC::SisRefCartesianas3d::getOrg(void) const
  { return ref.Org(); }

void XC::SisRefCartesianas3d::setOrg(const Pos3d &p)
  { ref.Org()= p; }

void XC::SisRefCartesianas3d::TresPuntos(const Pos3d &p1,const Pos3d &p2, const Pos3d &p3)
  { ref= Ref3d3d(p1,p2,p3); }

//! @brief Returns the vector unitario en la dirección del eje que se
//! pasa como parámetro expresado en el sistema global 
//! para la posición being passed as parameter.
Vector3d XC::SisRefCartesianas3d::GetVDirEje(const size_t &i,const Pos3d &) const
  { return ref.GetVDirEje(i); }

//! @brief Return the posicion del punto p expresado en locales
//! expresada en coordenadas globales.
Pos3d XC::SisRefCartesianas3d::GetPosGlobal(const Pos3d &p) const
  { return ref.GetPosGlobal(p); }
//! @brief Returns the vector v expresado en locales
//! expresado en coordenadas globales.
Vector3d XC::SisRefCartesianas3d::GetCooGlobales(const Vector3d &v) const
  { return ref.GetCooGlobales(v); }
//! @brief Return the posicion del punto p expresado en globales
//! expresada en coordenadas locales.
Pos3d XC::SisRefCartesianas3d::GetPosLocal(const Pos3d &p) const
  { return ref.GetPosLocal(p); }
//! @brief Returns the vector v expresado en globales
//! expresado en coordenadas locales.
Vector3d XC::SisRefCartesianas3d::GetCooLocales(const Vector3d &v) const
  { return ref.GetCooLocales(v); }
