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
//Trf2d.h
//Two-dimensional transformations.

#ifndef TRF2D_H
#define TRF2D_H

#include "Trf.h"
#include "../cgal_types.h"
#include "utility/geom/d2/GeomObj2d.h"

class Pos2d;
class Vector2d;
class Line2d;



//! @ingroup GEOM
//
//! @brief Base class for two-dimensional transformations.
class Trf2d: public Trf
  {
  private:
    CGTrfAfin_2 cgtrf;
    Trf2d(const CGTrfAfin_2 &t)
      : Trf(), cgtrf(t) {}
  protected:
    Trf2d(const CGAL::Translation &tr,const Vector2d &v);
    Trf2d(const CGAL::Rotation &rot,const GEOM_RT &seno,const GEOM_RT &coseno);
/*     //Trf2d(const CGAL::Reflection &ref,const Line2d &r); */
    Trf2d(const CGAL::Scaling &sc,const GEOM_FT &factor_escala);
    explicit Trf2d(const CGAL::Identity_transformation &i);

    CGPoint_2 Transform(const CGPoint_2 &cgpt2) const;
  public:
    Trf2d(void)
      : Trf(), cgtrf(CGAL::Identity_transformation()) {}
/*     Trf2d( const GEOM_RT & m00,const GEOM_RT & m01,const GEOM_RT & m02, */
/*            const GEOM_RT & m10,const GEOM_RT & m11,const GEOM_RT & m12, */
/*            const GEOM_RT & hw = GEOM_RT(1.0)); */
    Trf2d( const GEOM_FT & m00,const GEOM_FT & m01,const GEOM_FT & m02,
           const GEOM_FT & m10,const GEOM_FT & m11,const GEOM_FT & m12);
    Trf2d(const Trf2d &other)
      : Trf(other), cgtrf(other.cgtrf) {}
    Trf2d &operator=(const Trf2d &other)
      {
	Trf::operator=(other);
        cgtrf= other.cgtrf;
        return *this;
      }
    //Return the inverse transformation.
    //Trf2d Inversa(void) const;

    //@brief Return the (i,j) componet of the transformation matrix expressed in
    // cartesian coordinates.
    //       -              -
    //       | m11  m12 m13 |
    //       | m21  m22 m23 |
    //       |   0    0  1  |
    //       -              -
    virtual GEOM_FT Cartesianas(const size_t &i,const size_t &j) const
      { return cgtrf.m(i-1,j-1); }
    
    //@brief Return the (i,j) componet of the transformation matrix expressed in
    // homogeneous coordinates.
    //       -              -
    //       | m11  m12 m13 |
    //       | m21  m22 m23 |
    //       |   0    0  hw |
    //       -              -
    virtual GEOM_FT Homogeneas(const size_t &i,const size_t &j) const
      { return cgtrf.hm(i-1,j-1); }
    virtual FT_matrix Cartesianas(void) const;
    virtual FT_matrix Homogeneas(void) const;
    Pos2d Transform(const Pos2d &p) const;
    Vector2d Transform(const Vector2d &v) const;
    template <class InputIterator>
    void Transform(InputIterator first,InputIterator last) const;
    GeomObj::list_Pos2d Transform(const GeomObj::list_Pos2d &lp2d) const;
    Pos2d operator()(const Pos2d &p) const;
    Vector2d operator()(const Vector2d &v) const;
    GeomObj::list_Pos2d operator()(const GeomObj::list_Pos2d &lp2d) const;
    friend Trf2d operator*(const Trf2d &a,const Trf2d &b);
    virtual ~Trf2d(void) {}
  };

template <class InputIterator>
void Trf2d::Transform(InputIterator first,InputIterator last) const
  {
    for(InputIterator i= first;i!=last;i++)
      (*i)= Transform(*i);
  }

#endif
