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
#ifndef GAUSSPOINT_H
#define GAUSSPOINT_H

#include "domain/mesh/element/utils/ParticlePos3d.h"

namespace XC {

//! @ingroup FEMisc
//
//! @brief 3D position of Gauss points.
class GaussPoint: public ParticlePos3d
  {
  protected:
    double rw;
    double sw;
    double tw;

  public:
    GaussPoint(const double &r_coord = 0,
               const double &s_coord = 0,
               const double &t_coord = 0,
               const double &r_weight = 0,
               const double &s_weight = 0,
               const double &t_weight = 0);
    GaussPoint(const Pos2d &,const double &);
    GaussPoint(const Pos3d &,const double &);
    void set_weights(const double &r_weight = 0, const double &s_weight = 0, const double &t_weight = 0);
    
    inline const double &weight() const
      { return rw; }
    inline const double &r_weight() const
      { return rw; }
    inline const double &s_weight() const
      { return sw; }
    inline const double &t_weight() const    
      { return tw; }
    inline void set_r_weight(const double &r_weight)
      { rw= r_weight; }
    inline void set_s_weight(const double &s_weight)
      { sw= s_weight; }
    inline void set_t_weight(const double &t_weight)
      { tw= t_weight; }
    bool operator==(const GaussPoint &) const;
  };
} // end of XC namespace

#endif 
//
