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
//
#include "ParticlePos2d.h"
#include "utility/geom/pos_vec/Pos2d.h"

//! @brief  Constructor
XC::ParticlePos2d::ParticlePos2d(const double &r_coord,
                                 const double &s_coord)
  : r(r_coord), s(s_coord)
  {}

//! @brief  Constructor
XC::ParticlePos2d::ParticlePos2d(const Pos2d &p)
  : r(p.x()), s(p.y())
  {}

//! @brief Set coordinates.
void XC::ParticlePos2d::set_coordinates(const double &r_coord,const double &s_coord)
  {
    this->r = r_coord;
    this->s = s_coord;
  }

//! @brief Set coordinates.
void XC::ParticlePos2d::set_coordinates(const Pos2d &p)
  { set_coordinates(p.x(),p.y());}

//! @brief Equal operator.
bool XC::ParticlePos2d::operator==(const ParticlePos2d &other) const
  { return ((r==other.r) && (s==other.r)); }

//! @brief Print stuff.
void XC::ParticlePos2d::Print(std::ostream &os) const
  {
    os << "r= " << r
       << " s= " << s;
  }


//! @brief Output operator.
std::ostream &XC::operator<<(std::ostream &os, const ParticlePos2d &p)
  {
    p.Print(os);
    return os;
  }





