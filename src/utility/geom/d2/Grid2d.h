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
//Grid2d.h

#ifndef Grid2d_h 
#define Grid2d_h 

#include "Surface2d.h"
#include "../pos_vec/Pos2dArray.h"

//! @ingroup GEOM
//
//! @brief Grid in a two-dimensional space.
class Grid2d: public Surface2d
  {
    Pos2dArray points; //!< mesh points
  public:
    Grid2d(void);
    Grid2d(int numSubdivIJ, int numSubdivJK);
    explicit Grid2d(const Pos2dArray &m)
      : Surface2d(), points(m) {}
    virtual ~Grid2d(void);
    virtual GeomObj *getCopy(void) const
      { return new Grid2d(*this); }

    inline const Pos2d &operator()(const size_t &i,const size_t &j) const
      { return points(i,j); }
    inline Pos2d &operator()(const size_t &i,const size_t &j)
      { return points(i,j); }
    inline Pos2dArray &getPoints(void)
      { return points; }

    GEOM_FT GetMax(unsigned short int i) const;
    GEOM_FT GetMin(unsigned short int i) const;
    GEOM_FT getArea(void) const;
    GEOM_FT Ix(void) const;
    GEOM_FT Iy(void) const;
    GEOM_FT Pxy(void) const;
    Pos2d getCenterOfMass(void) const;
    const FT_matrix &GetVertCoords(const size_t &,const size_t &) const;
    Pos2d getPoint(const size_t &,const size_t &) const;
    Quadrilateral2d GetQuad(const size_t &i,const size_t &j) const;
    bool In(const Pos2d &, const double &) const;
    
    void Transform(const Trf2d &trf2d);

    void Print(std::ostream &os) const;
  };

#endif

