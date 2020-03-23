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
//
#ifndef ParticlePos3d_H
#define ParticlePos3d_H

#include <iostream>

class Pos2d;
class Pos3d;

namespace XC {

//! @ingroup FEMisc
//
//! @brief Natural coordinates of an element's particle
class ParticlePos3d
  {
  protected:
    double r;
    double s;
    double t;

  public:
    ParticlePos3d(const double &r_coord = 0,
                  const double &s_coord = 0,
		  const double &t_coord = 0);
    ParticlePos3d(const Pos2d &);
    ParticlePos3d(const Pos3d &);
    void set_coordinates(const double &r_coord = 0,const double &s_coord = 0,const double &t_coord = 0);
    void set_coordinates(const Pos3d &);
    void set_coordinates(const Pos2d &);
    
    inline const double &r_coordinate(void) const
      { return r; }
    inline void set_r_coordinate(const double &r_coord)
      { r= r_coord; }
    inline const double &s_coordinate(void) const
      { return s; }
    inline void set_s_coordinate(const double &s_coord)
      { s= s_coord; }
    inline const double &t_coordinate(void) const
      { return t; }
    inline void set_t_coordinate(const double &t_coord)
      { r= t_coord; }
    void Print(std::ostream &) const;
  };

std::ostream &operator<<(std::ostream &, const ParticlePos3d &);

} // end of XC namespace

#endif 
//
