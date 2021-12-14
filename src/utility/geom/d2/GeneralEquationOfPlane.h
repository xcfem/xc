// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.  
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//GeneralEquationOfPlane.h

#ifndef GENERALEQUATIONOFPLANE_H
#define GENERALEQUATIONOFPLANE_H

#include <vector>
#include "../cgal_types.h"

class Pos3d;
class Pos2d;

//! @ingroup GEOM
//
//! @brief Equation for a plane in general form: ax + by + cz + d = 0
//! where v[0]= d, v[1]= a, v[2]= b, v[3]= c.
class GeneralEquationOfPlane: private std::vector<GEOM_FT>
  {
    std::vector<GEOM_RT> v; //!< v[0]= d, v[1]= a, v[2]= b, v[3]= c.
  public:
/*     GeneralEquationOfPlane(const GEOM_RT &a,const GEOM_RT &b,const GEOM_RT &c,const GEOM_RT &d); */
    GeneralEquationOfPlane(const GEOM_FT &fa,const GEOM_FT &fb,const GEOM_FT &fc,const GEOM_FT &fd);
    inline const GEOM_RT &a(void) const
      { return v[1]; }
    inline const GEOM_RT &b(void) const
      { return v[2]; }
    inline const GEOM_RT &c(void) const
      { return v[3]; }
    inline const GEOM_RT &d(void) const
      { return v[0]; }
    GEOM_FT Eval(const Pos3d &p) const;

    GEOM_FT x(const GEOM_FT &y,const GEOM_FT &z) const;
    GEOM_FT x(const Pos2d &p) const;
    GEOM_FT y(const GEOM_FT &x,const GEOM_FT &z) const;
    GEOM_FT y(const Pos2d &p) const;
    GEOM_FT z(const GEOM_FT &x,const GEOM_FT &y) const;
    GEOM_FT z(const Pos2d &p) const;
  };

#endif
