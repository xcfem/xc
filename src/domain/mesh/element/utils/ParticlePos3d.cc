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
#include "ParticlePos3d.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"

//! @brief  Constructor
XC::ParticlePos3d::ParticlePos3d(const double &r_coord,
                                 const double &s_coord,
				 const double &t_coord)
  {
    set_coordinates(r_coord,s_coord,t_coord);
  }

//! @brief  Constructor
XC::ParticlePos3d::ParticlePos3d(const Pos2d &p)
  { set_coordinates(p); }

//! @brief  Constructor
XC::ParticlePos3d::ParticlePos3d(const Pos3d &p)
  { set_coordinates(p); }

//! @brief Set coordinates.
void XC::ParticlePos3d::set_coordinates(const double &r_coord,const double &s_coord,const double &t_coord)
  {
    r = r_coord;
    s = s_coord;
    t = t_coord;
  }

//! @brief Set coordinates.
void XC::ParticlePos3d::set_coordinates(const Pos3d &p)
  { set_coordinates(p.x(),p.y(),p.z());}

//! @brief Set coordinates.
void XC::ParticlePos3d::set_coordinates(const Pos2d &p)
  { set_coordinates(p.x(),p.y());}

void XC::ParticlePos3d::Print(std::ostream &os) const
  {
    os << "r= " << r
       << " s= " << s
       << " t= " << t;
  }


std::ostream &XC::operator<<(std::ostream &os, const ParticlePos3d &p)
  {
    p.Print(os);
    return os;
  }





