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
//GeomObj3d.h

#ifndef GEOMOBJ3D_H
#define GEOMOBJ3D_H

#include "../GeomObj.h"

#include "../lists/PolyPos.h"

class SoGroup;
class Pos3d;
class Vector3d;
class Line3d;
class BND3d;
class Ref3d3d;
class FT_matrix;
class HalfSpace3d;


//! @ingroup GEOM
//
//! @brief Clase base para los objetos en tres dimensiones.
class GeomObj3d: public GeomObj
  {
  protected:
    //std::deque<HalfSpace3d> interpretaHalfSpaces3d(const std::string &str);
  public:
    typedef PolyPos<Pos3d> list_Pos3d;

    GeomObj3d(void) {}
    virtual ~GeomObj3d(void) {}
    virtual GeomObj *getCopy(void) const=0;
    //! @brief Return the maximum value of the i-th coordinate.
    virtual GEOM_FT GetMax(unsigned short int i) const= 0;
    //! @brief Return the minimum value of the i-th coordinate.
    virtual GEOM_FT GetMin(unsigned short int i) const= 0;
    GEOM_FT GetXMax(void) const { return GetMax(1); }
    GEOM_FT GetYMax(void) const { return GetMax(2); }
    GEOM_FT GetZMax(void) const { return GetMax(3); }
    GEOM_FT GetXMin(void) const { return GetMin(1); }
    GEOM_FT GetYMin(void) const { return GetMin(2); }
    GEOM_FT GetZMin(void) const { return GetMin(3); }
    virtual void XProjection(void) {}
    virtual void YProjection(void) {}
    virtual void ZProjection(void) {}
    virtual Pos3d Projection(const Pos3d &) const;
    virtual Vector3d Projection(const Vector3d &) const;
    Pos3d GetPMax(void) const;
    Pos3d GetPMin(void) const;
    BND3d Bnd(void) const;
    virtual bool In(const Pos3d &p, const double &tol= 0.0) const;
    virtual bool Out(const Pos3d &p, const double &tol= 0.0) const;
    //! @brief Return the factor used for the computation
    //! of the center of mass (1.0 by default).
    virtual GEOM_FT getCenterOfMassFactor(void) const
      { return 1.0; }
    virtual Pos3d getCenterOfMass(void) const= 0;
    //! @brief Return the object length.
    virtual GEOM_FT getLength(void) const= 0;
    //! @brief Return the object area.
    virtual GEOM_FT getArea(void) const= 0;
    //! @brief Return the object volume
    virtual GEOM_FT getVolume(void) const= 0;
    virtual GEOM_FT Ix(void) const= 0;
    virtual GEOM_FT Iy(void) const= 0;
    virtual GEOM_FT Iz(void) const= 0;

    //product of inertia with respect to the center of mass in local coordinates.
    //Suponemos orthonormal axis.
    inline virtual GEOM_FT Pxy(void) const
      { return Ix()+Iy()-Iz(); }
    //product of inertia with respect to the center of mass in local coordinates.
    //Suponemos orthonormal axis.
    inline virtual GEOM_FT Pxz(void) const
      { return Ix()+Iz()-Iy(); }
    //product of inertia with respect to the center of mass in local coordinates.
    //Suponemos orthonormal axis.
    inline virtual GEOM_FT Pyz(void) const
      { return Iy()+Iz()-Ix(); }
    inline GEOM_FT I( const unsigned short int &i,
                     const unsigned short int &j) const;
    //Return el area que se emplea para calcular el moment of inertia
    virtual inline GEOM_FT IArea(void) const
      { return 0.0; }
    GEOM_FT I(const Pos3d &O,const Vector3d &e) const;
    GEOM_FT I(const Line3d &r) const;
    GEOM_FT I( const unsigned short int i,
              const unsigned short int j,
              const Pos3d &o) const;
    GEOM_FT IO(const Pos3d &o) const;
    FT_matrix I(void) const;
    FT_matrix I(const Pos3d &o) const;
    
  };

#endif




