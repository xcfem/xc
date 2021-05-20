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
//AnnulusSector2d.h

#ifndef SECTORANILLOCIRCULAR2D_H
#define SECTORANILLOCIRCULAR2D_H

#include "CircularSector2d.h"

class Pos2d;
class Pos2dArray;
class Polygon2d;

//! @ingroup GEOM
//
//! @brief Sector de un anillo circular en dos dimensiones.
class AnnulusSector2d : public CircularSector2d
  {
    double inner_radius; //!< inner radius.
  protected:
    CircularSector2d getSector(const double &R) const;
    CircularSector2d SectorInterior(void) const;
  public:
    AnnulusSector2d(void) : CircularSector2d(), inner_radius(0.0) {}
    AnnulusSector2d(const CircularSector2d &c,const double &inner_radius);

    virtual bool operator==(const AnnulusSector2d &) const;
    virtual Surface2d *clon(void) const
      { return new AnnulusSector2d(*this); }
    virtual Pos2d getCenterOfMass(void) const;
    Pos2d PInicInt(void) const;
    Pos2d PInicExt(void) const;
    Pos2d PFinInt(void) const;
    Pos2d PFinExt(void) const;
    Pos2d PMedInt(void) const;
    Pos2d PMedExt(void) const;
    //! @brief Return the outer radius.
    inline double outerRadius(void) const
      { return getRadius(); }
    inline const double &innerRadius(void) const
      { return inner_radius; }
    inline double &innerRadius(void)
      { return inner_radius; }
    virtual GEOM_FT getInteriorArcLength(void) const;
    virtual GEOM_FT getExteriorArcLength(void) const;
    virtual GEOM_FT getLength(void) const;
    virtual GEOM_FT getArea(void) const;
    virtual GEOM_FT Ix(void) const;
    virtual GEOM_FT Iy(void) const;
    virtual GEOM_FT Pxy(void) const;
    virtual GEOM_FT Iz(void) const;
    virtual GEOM_FT GetMax(unsigned short int i) const;
    virtual GEOM_FT GetMin(unsigned short int i) const;
    virtual bool In(const Pos2d &p, const double &tol= 0.0) const;
    Pos2dArray getExtArcPoints(const size_t &n) const;
    Pos2dArray getIntArcPoints(const size_t &n) const;
    Pos2dArray getPointsOnPerimeter(const size_t &n) const;
    Pos2dArray genMesh(const size_t &m,const size_t &n) const;
    Polygon2d getPolygon2d(const size_t &n) const;

    void Print(std::ostream &os) const;
  };

#endif



