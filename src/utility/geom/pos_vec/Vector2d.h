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
//Vector2d.h

#ifndef VECTOR2D_H
#define VECTOR2D_H

#include "utility/utils/misc_utils/mchne_eps.h"
#include "utility/matrices/op_tensor.h"
#include "../cgal_types.h"
#include "../ProtoGeom.h"

class Pos2d;
class Dir2d;
class Plotter;
class FT_matrix;


//! @ingroup GEOM
//
//! @brief Vector en dos dimensiones.
class Vector2d: public ProtoGeom
  {
    CGVector_2 cgvct;
  public:
    Vector2d(void): ProtoGeom(), cgvct(CGAL::NULL_VECTOR) {}
    explicit Vector2d(const CGVector_2 &v)
      : ProtoGeom(), cgvct(v) {}
    explicit Vector2d(const CGDirection_2 &dir)
      : ProtoGeom(), cgvct(dir.vector()) {}
    Vector2d(const GEOM_FT &x,const GEOM_FT &y);
/*     Vector2d(const double &x,const double &y); */
    explicit Vector2d(const FT_matrix &m);
    Vector2d(const Pos2d &p1,const Pos2d &p2);

    virtual bool operator==(const Vector2d &) const;
    const CGVector_2 &ToCGAL(void) const
      { return cgvct; }
    void Neg(void)
      { cgvct= -cgvct; }
    inline Vector2d operator-(void) const
      {
	Vector2d retval(*this);
	retval.Neg();
	return retval;
      }
    bool Nulo(void) const;
    void SetX(const GEOM_FT &vx);
    void SetY(const GEOM_FT &vy);
    void Set(unsigned short int i,const GEOM_FT &v);

    inline const GEOM_FT operator()(const size_t &i) const //Base 1.
      { return cgvct.cartesian(i-1); }
    inline const GEOM_FT operator[](const size_t &j) const //Base 0.
      { return cgvct.cartesian(j); }
    inline const GEOM_FT x() const
      { return Vector2d::operator()(1); }
    inline const GEOM_FT y() const
      { return Vector2d::operator()(2); }
    FT_matrix getMatrix(void) const;
    Vector2d &operator+=(const Vector2d &);
    Vector2d &operator-=(const Vector2d &);
    Vector2d operator+(const Vector2d &) const;
    Vector2d operator-(const Vector2d &) const;
    Vector2d operator*(const GEOM_FT &) const;
    Vector2d operator/(const GEOM_FT &) const;

    inline virtual GEOM_FT GetModulus2(void) const
      { return (x()*x()+y()*y()); }
    virtual GEOM_FT GetModulus(void) const;
    bool EsUnitario(const double &tol= mchne_eps_dbl) const;

    //! @brief Return el versor (vector de módulo unidad) correspondiente a éste vector.
    inline Vector2d getNormalized(void) const
      { return (*this)/GetModulus(); }
    void Normalize(void);
    Vector2d Normal(void) const;
    Dir2d getDirection(void) const;
    GEOM_FT getSignedAngle(const Vector2d &v) const;
    GEOM_FT getAngle(const Vector2d &v) const;
    GEOM_FT XAxisAngle(void) const;
    GEOM_FT YAxisAngle(void) const;

    //Producto escalar.
    inline virtual GEOM_FT GetDot(const Vector2d &v) const
      { return (x()*v.x()+y()*v.y()); }
    virtual GEOM_FT GetDot(const FT_matrix &m) const;
    inline friend GEOM_FT dot(const Vector2d &v1, const Vector2d &v2)
      { return v1.GetDot(v2); }
    friend GEOM_FT dot(const Vector2d &v1, const FT_matrix &m);
    friend GEOM_FT dot(const FT_matrix &m, const Vector2d &v1);

    friend FT_matrix operator*(const FT_matrix &m,const Vector2d &v);

    //void Gira(const GEOM_FT &ang_rad);
    //Vector2d GetGirado(const double &ang_rad) const;
    Vector2d Perpendicular(const Orientacion &o) const;
    Vector2d Perpendicular(const Vector2d &v) const;
    
    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);
    
    void Print(std::ostream &stream) const;
    void Plot(Plotter &psos) const;
    friend std::ostream &operator<<(std::ostream &stream,const Vector2d &n);
    inline virtual ~Vector2d(void) {}
  };

inline GEOM_FT Abs2(const Vector2d &v)
  { return v.GetModulus2(); }
inline GEOM_FT Abs(const Vector2d &v)
  { return v.GetModulus(); }
double signedAngle(const Vector2d &v1,const Vector2d &v2);
double angle(const Vector2d &v1,const Vector2d &v2);

//! @breif Return el producto por un escalar.
inline Vector2d operator*(const GEOM_FT &d,const Vector2d &v)
  { return v*d; }
//! @brief Return el versor (vector de módulo unidad) correspondiente a éste vector.
inline Vector2d normalize(const Vector2d &v)
  { return v.getNormalized(); }

//Producto tensorial.
FT_matrix prod_tensor(const Vector2d &u,const Vector2d &v);
FT_matrix operator&(const Vector2d &u,const Vector2d &v);

bool colineales(const Vector2d &v1,const Vector2d &v2);
bool parallel(const Vector2d &v1,const Vector2d &v2);

const Vector2d VectorNulo2d;

#endif
