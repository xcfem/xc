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
//Vector3d.h

#ifndef VECTOR3D_H
#define VECTOR3D_H

#include "utility/utils/misc_utils/mchne_eps.h"
#include "../ProtoGeom.h"
#include "../cgal_types.h"

class Pos3d;
class Dir3d;
class FT_matrix;
class SbVec3f;


//! @ingroup GEOM
//
//! @brief Vector en tres dimensiones.
class Vector3d: public ProtoGeom
  {
    CGVector_3 cgvct;
  public:
    Vector3d(void): cgvct(CGAL::NULL_VECTOR){}
    explicit Vector3d(const CGVector_3 &v)
      : cgvct(v) {}
    Vector3d(const GEOM_FT &x,const GEOM_FT &y,const GEOM_FT &z);
    explicit Vector3d(const FT_matrix &m);
    Vector3d(const Pos3d &p1,const Pos3d &p2);
    virtual bool operator==(const Vector3d &) const;
    const CGVector_3 &ToCGAL(void) const
      { return cgvct; }
    void Neg(void)
      { cgvct= -cgvct; }
    bool Nulo(void) const;
    void SetX(const GEOM_FT &vx);
    void SetY(const GEOM_FT &vy);
    void SetZ(const GEOM_FT &vz);
    void Set(unsigned short int i,const GEOM_FT &v);
    inline const GEOM_FT operator()(const size_t &i) const
      { return cgvct.cartesian(i-1); }
    inline const GEOM_FT x(void) const
      { return cgvct.x(); }
    inline const GEOM_FT y(void) const
      { return cgvct.y(); }
    inline const GEOM_FT z(void) const
      { return cgvct.z(); }
    FT_matrix getMatrix(void) const;
    Dir3d getDirection(void) const;
    Vector3d &operator+=(const Vector3d &);
    Vector3d &operator-=(const Vector3d &);
    Vector3d operator+(const Vector3d &) const;
    Vector3d operator-(const Vector3d &) const;
    Vector3d operator*(const GEOM_FT &) const;
    Vector3d operator/(const GEOM_FT &) const;

    //! @brief Return the squared modulus.
    inline virtual GEOM_FT GetModulus2(void) const
      { return (x()*x()+y()*y()+z()*z()); }
    virtual GEOM_FT GetModulus(void) const;
    bool EsUnitario(const double &tol= mchne_eps_dbl) const;
    Vector3d getNormalized(void) const;
    void Normalize(void);
    Vector3d Perpendicular(const Vector3d &v) const;
    virtual GEOM_FT GetDot(const Vector3d &v) const;
    virtual GEOM_FT GetDot(const FT_matrix &m) const;
    Vector3d getCross(const Vector3d &v) const;
    GEOM_FT getSignedAngle(const Vector3d &v) const;
    GEOM_FT getAngle(const Vector3d &v) const;
    GEOM_FT getPolarAngle(void) const;
    GEOM_FT getAzimuthalAngle(void) const;


    inline friend GEOM_FT dot(const Vector3d &v1, const Vector3d &v2)
      { return v1.GetDot(v2); }
    inline friend GEOM_FT dot(const Vector3d &v1, const FT_matrix &m)
      { return v1.GetDot(m); }
    inline friend GEOM_FT dot(const FT_matrix &m, const Vector3d &v1)
      { return v1.GetDot(m); }
    inline friend Vector3d cross(const Vector3d &v1, const Vector3d &v2)
      { return v1.getCross(v2); }
    inline friend Vector3d operator^(const Vector3d &v1, const Vector3d &v2)
      { return cross(v1,v2); }
    friend FT_matrix operator*(const FT_matrix &m,const Vector3d &v);
    friend std::ostream &operator<<(std::ostream &stream,const Vector3d &n);
    
    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);

    inline virtual ~Vector3d(void) {}
  };

inline Vector3d operator-(const Vector3d &d)
  {
    Vector3d retval(d);
    retval.Neg();
    return retval;
  }

const Vector3d I_3d(1,0,0);
const Vector3d J_3d(0,1,0);
const Vector3d K_3d(0,0,1);

//! @brief Return the squared modulus.
inline GEOM_FT Abs2(const Vector3d &v)
  { return v.GetModulus2(); }

GEOM_FT Abs(const Vector3d &v); //Return the vector modulus.

GEOM_FT signedAngle(const Vector3d &v1,const Vector3d &v2);
GEOM_FT angle(const Vector3d &v1,const Vector3d &v2);

//Producto tensorial.
FT_matrix prod_tensor(const Vector3d &u,const Vector3d &v);
FT_matrix operator&(const Vector3d &u,const Vector3d &v);

bool parallel(const Vector3d &v1,const Vector3d &v2);
bool coplanarios(const Vector3d &v1,const Vector3d &v2,const Vector3d &v3);

inline Vector3d operator*(const GEOM_FT &d,const Vector3d &v)
  { return v*d; }

inline Vector3d normalize(const Vector3d &v)
  { return v.getNormalized(); } 
FT_matrix Traspuesta(const Vector3d &v);
FT_matrix skew_symm_matrix_post(const Vector3d &v);
FT_matrix skew_symm_matrix_pre(const Vector3d &v);

const Vector3d VectorNulo3d;

#endif
