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
//GeomObj.h

#ifndef GEOMOBJ2D_H
#define GEOMOBJ2D_H

#include "utility/geom/GeomObj.h"
#include "utility/geom/ref_sys/PrincipalAxesOfInertia2D.h"

class BND2d;
class Dir2d;
class Line2d;
class FT_matrix;

//! @ingroup GEOM
//
//! @brief Base class for the two-dimensional geometric objects.
class GeomObj2d: public GeomObj
  {
  public:
    typedef PolyPos<Pos2d> list_Pos2d;

    GeomObj2d(void) {}
    virtual ~GeomObj2d(void) {}
    //! @brief Return the maximum value of the i-th coordinate.
    virtual GEOM_FT GetMax(unsigned short int i) const= 0;
    //! @brief Return the minimum value of the i-th coordinate.
    virtual GEOM_FT GetMin(unsigned short int i) const= 0;
    GEOM_FT GetXMax(void) const { return GetMax(1); }
    GEOM_FT GetYMax(void) const { return GetMax(2); }
    GEOM_FT GetXMin(void) const { return GetMin(1); }
    GEOM_FT GetYMin(void) const { return GetMin(2); }
    Pos2d getPMax(void) const;
    Pos2d getPMin(void) const;
    virtual Pos2d Projection(const Pos2d &) const;
    virtual Vector2d Projection(const Vector2d &) const;
    BND2d Bnd(void) const;
    virtual bool In(const Pos2d &p, const double &tol= 0.0) const;
    virtual bool Out(const Pos2d &p, const double &tol= 0.0) const;
    //! @brief Return the factor used for the computation
    //! of the center of mass (1.0 by default).
    virtual GEOM_FT getCenterOfMassFactor(void) const
      { return 1.0; }
    virtual Pos2d getCenterOfMass(void) const= 0;
    virtual GEOM_FT getLength(void) const= 0; //?? Por que se re-declara (está en GeomObj).
    virtual GEOM_FT getArea(void) const= 0; //?? Por que se re-declara (está en GeomObj).
    //! @brief Return the object volume
    inline virtual GEOM_FT getVolume(void) const
      { return 0.0; }
    //! @brief Moment of inertia with respect to the center of mass en local axis.
    virtual GEOM_FT Ix(void) const= 0;
    //! @brief Moment of inertia with respect to the center of mass en local axis.
    virtual GEOM_FT Iy(void) const= 0;
    //! @brief ! @brief Product of inertia with respect to the center of mass en local axis.
    virtual GEOM_FT Pxy(void) const= 0;
    //! @brief Moment of inertia polar with respect to the center of mass en local axis.
    inline virtual GEOM_FT Iz(void) const
      { return Ix()+Iy(); }
    double Theta_p(void) const;
    Dir2d IAxisDir_a(void) const;
    Dir2d IAxisDir_b(void) const;
    Line2d IAxis_a(void) const;
    Line2d IAxis_b(void) const;
    Ref2d2d PrincipalAxesOfInertia(void) const;
    GEOM_FT I1(void) const;
    GEOM_FT I2(void) const;
    inline PrincipalAxesOfInertia2D Inertia(void)
      { return PrincipalAxesOfInertia2D(getCenterOfMass(),Ix(),Iy(),Pxy()); }
    GEOM_FT I( const unsigned short int &i,
              const unsigned short int &j) const;
    virtual inline GEOM_FT IArea(void) const
      { return getArea(); }
    GEOM_FT I(const Pos2d &O,const Vector2d &e) const;
    GEOM_FT I(const Line2d &r) const;
    GEOM_FT I( const unsigned short int i,
              const unsigned short int j,
              const Pos2d &o) const;
    GEOM_FT IO(const Pos2d &o) const;
    FT_matrix I(void) const;
    FT_matrix I(const Pos2d &o) const;

    virtual void Move(const Vector2d &);
    virtual void Transform(const Trf2d &trf2d)= 0;    
  };

//! @brief Return the transformed object.
template <class T>
T getTransformed(const T &obj,const Trf2d &trf2d)
  {
    T retval(obj);
    retval.Transform(trf2d);
    return retval;
  }

#endif




