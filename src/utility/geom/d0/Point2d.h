// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
//Point2d.h

#ifndef POINT2D_H
#define POINT2D_H

#include "../d2/GeomObj2d.h"
#include "../pos_vec/Pos2d.h"

//! @ingroup GEOM
//
//! @brief Point in 2D.
class Point2d : public GeomObj2d
  {
    Pos2d org;
  public:
    Point2d(void) : GeomObj2d() {}
    explicit Point2d(const Pos2d &p): GeomObj2d(), org(p) {}
    Point2d(GEOM_FT x,GEOM_FT y) : GeomObj2d(), org(x,y)
      {}
    operator const Pos2d &()
      { return org; }
    //! @brief Return the position in the global coordinates.
    inline Pos2d GetPos(void) const
      { return org; }
    virtual GeomObj2d *clon(void) const
      { return new Point2d(*this); }
    virtual Pos2d getCenterOfMass(void) const
      { return org; }
    inline virtual unsigned short int Dimension(void) const
      { return 0; }
    //! @brief Return object length (zero in this case).
    virtual GEOM_FT getLength(void) const
      { return 0.0; }
    //! @brief Return the object area.
    virtual GEOM_FT getArea(void) const
      { return 0.0; }
    //! @brief Return the object volume.
    virtual GEOM_FT getVolume(void) const
      { return 0.0; }
    virtual GEOM_FT Ix(void) const
      { return 0.0; }
    virtual GEOM_FT Iy(void) const
      { return 0.0; }
    virtual GEOM_FT Pxy(void) const
      { return 0.0; }
    virtual GEOM_FT Iz(void) const
      { return 0.0; }
    inline bool domina_a(const Point2d &b) const
      { return org.domina_a(b.org); }
    inline virtual GEOM_FT GetMax(unsigned short int i) const
      { return org(i); }
    inline virtual GEOM_FT GetMin(unsigned short int i) const
      { return org(i); }

    void Transform(const Trf2d &trf2d);

    friend int operator ==(const Point2d &a,const Point2d &b)
      { return ( a.org == b.org ); };
    
    void Print(std::ostream &os) const;
    void Plot(Plotter &psos) const;
  };
#endif



