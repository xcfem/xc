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
//Trf3d.h
//Three dimensional transformation.

#ifndef TRF3D_H
#define TRF3D_H

#include "Trf.h"
#include "../cgal_types.h"

class Pos3d;
class Vector3d;
class Pos3dArray;
class Pos3dArray3d;


//! @ingroup GEOM
//
//! @brief Three-dimensional transformation.
class Trf3d: public Trf
  {
  private:
    CGTrfAfin_3 cgtrf;
    Trf3d(const CGTrfAfin_3 &t)
      : Trf(), cgtrf(t) {}
  protected:
    Trf3d(const CGAL::Translation &tr,const Vector3d &v);
    //Trf3d(const CGAL::Rotation &rot,const GEOM_FT &seno,const GEOM_FT &coseno);
    Trf3d(const CGAL::Scaling &sc,const GEOM_FT &factor_escala);
    explicit Trf3d(const CGAL::Identity_transformation &i);
  public:
    Trf3d(void)
      : Trf(), cgtrf(CGAL::Identity_transformation()) {}
    Trf3d( const GEOM_FT & m00,const GEOM_FT & m01,const GEOM_FT & m02,const GEOM_FT & m03,
           const GEOM_FT & m10,const GEOM_FT & m11,const GEOM_FT & m12,const GEOM_FT & m13,
           const GEOM_FT & m20,const GEOM_FT & m21,const GEOM_FT & m22,const GEOM_FT & m23);
    //Trf3d Inversa(void) const;
      //Return the inverse transformation.
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
    void putHomogenousMatrix(const FT_matrix &mh);
    virtual FT_matrix Cartesianas(void) const;
    virtual FT_matrix Homogeneas(void) const;
    Pos3d Transform(const Pos3d &p) const;
    Vector3d Transform(const Vector3d &v) const;
    template <class InputIterator>
    void Transform(InputIterator first,InputIterator last) const;
    void Transform(Pos3dArray &m) const;
    const Pos3dArray &Transform(const Pos3dArray &m) const;
    void Transform(Pos3dArray3d &m) const;
    const Pos3dArray3d &Transform(const Pos3dArray3d &m) const;
    Pos3d operator()(const Pos3d &p) const;
    Vector3d operator()(const Vector3d &v) const;
    Pos3dArray operator()(const Pos3dArray &m) const;
    friend Trf3d operator*(const Trf3d &a,const Trf3d &b);
    friend Trf3d giroX3d(const double &ang_rad);
    friend Trf3d giroY3d(const double &ang_rad);
    friend Trf3d giroZ3d(const double &ang_rad);
    virtual ~Trf3d(void) {}
  };

Trf3d giroX3d(const double &ang_rad);
Trf3d giroY3d(const double &ang_rad);
Trf3d giroZ3d(const double &ang_rad);

inline Trf3d giroXYZ3d(const double &rx,const double &ry,const double &rz)
  { return giroZ3d(rz) * giroY3d(ry) * giroX3d(rx); }

template <class InputIterator>
void Trf3d::Transform(InputIterator first,InputIterator last) const
  {
    for(InputIterator i= first;i!=last;i++)
      (*i)= Transform(*i);
  }

#endif
