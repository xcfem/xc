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
//Point3d.h

#ifndef POINT3D_H
#define POINT3D_H

#include "../d3/GeomObj3d.h"
#include "../pos_vec/Pos3d.h"

//! @ingroup GEOM
//
//! @brief Point in 3D.
class Point3d : public GeomObj3d
  {
    Pos3d org;
  public:
    Point3d(void) : GeomObj3d() {}
    Point3d(GEOM_FT x,GEOM_FT y,GEOM_FT z=0)
      : GeomObj3d(), org(Pos3d(x,y,z)) {}
    Point3d &operator=(const Point3d &p)
      {
	GeomObj3d::operator=(p);
        org= p.org;
        return *this;
      }
    Point3d &operator=(const Pos3d &p)
      {
	org= p;
        return *this;
      }
    operator const Pos3d &(void)
      { return org; }
    //! @brief Return the position of the point in global coordinates.
    inline Pos3d getPos(void) const
      { return org; }
    virtual GeomObj3d *clon(void) const
      { return new Point3d(*this); }
    virtual Pos3d getCenterOfMass(void) const
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
    virtual GEOM_FT Iz(void) const
      { return 0.0; }
    inline bool domina_a(const Point3d &b) const
      { return org.domina_a(b.org); }
    inline virtual GEOM_FT GetMax(unsigned short int i) const
      { return org(i); }
    inline virtual GEOM_FT GetMin(unsigned short int i) const
      { return org(i); }
    friend int operator ==(const Point3d &a,const Point3d &b)
      { return ( a.org == b.org ); };
    inline void Print(std::ostream &os) const
      { os << org; }
  };
#endif



