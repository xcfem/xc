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
//
#include "GaussPoint.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"

//! @brief  Constructor
XC::GaussPoint::GaussPoint(const double &r_coord,
               const double &s_coord,
               const double &t_coord,
               const double &r_weight,
               const double &s_weight,
               const double &t_weight)
  : ParticlePos3d(r_coord,s_coord,t_coord)
  { set_weights(r_weight,s_weight,t_weight); }

//! @brief  Constructor
XC::GaussPoint::GaussPoint(const Pos2d &p,const double &w)
  : ParticlePos3d(p)
  { set_weights(w); }

//! @brief  Constructor
XC::GaussPoint::GaussPoint(const Pos3d &p,const double &w)
  : ParticlePos3d(p)
  { set_weights(w); }

//! @brief Asigna valores a los pesos.
void XC::GaussPoint::set_weights(const double &r_weight, const double &s_weight, const double &t_weight)
  {
    rw = r_weight;
    sw = s_weight;
    tw = t_weight;
  }






