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
//CircularSector2d.h

#ifndef SECTORCIRCULAR3D_H
#define SECTORCIRCULAR3D_H

#include "D2to3d.h"
#include "CircularSector2d.h"

class Pos3d;

//! @ingroup GEOM
//
//! @brief Sector de círculo en tres dimensiones.
class CircularSector3d : public D2to3d
  {
    CircularSector2d sect_circ;
  public:
    CircularSector3d(void) : D2to3d(), sect_circ() {}
    CircularSector3d(const Pos3d centro,const GEOM_FT &rad,const double &th1,const double &th2);
    CircularSector3d(const GEOM_FT &rad2,const Pos3d &centro,const double &th1,const double &th2);
    CircularSector3d(const Pos3d &c,const double &r,const Vector3d &n,const Vector3d &Xaxis,const double &angIni,const double &angFin);
    CircularSector3d(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3);
    
    virtual D2to3d *clon(void) const;
    Pos3d Centro(void) const;
    Pos3d PInic(void) const;
    Pos3d PFin(void) const;
    Pos3d PMed(void) const;
    virtual Pos3d getCenterOfMass(void) const;
    virtual Pos3d ArcCentroid(void) const;
    GEOM_FT getSquaredRadius(void) const;
    GEOM_FT getRadius(void) const;
    GEOM_FT getDiameter(void) const;
    inline double getIncludedAngle(void) const
      { return sect_circ.getIncludedAngle(); }
    double getAngle(const Pos3d &) const;
    double getLambda(const Pos3d &) const;
    virtual GEOM_FT getLength(void) const
      { return sect_circ.getLength(); }
    virtual GEOM_FT getArcLength(void) const
      { return sect_circ.getArcLength(); }
    //! @brief Return the perimeter of the object.
    inline GEOM_FT getPerimeter(void) const
      { return getLength(); }
    //! @brief Return the area of the object.
    virtual GEOM_FT getArea(void) const
      { return sect_circ.getArea(); }
    //! @brief Return the volume of the object.
    virtual GEOM_FT getVolume(void) const
      { return 0; }
    inline const double &Theta1(void) const
      { return sect_circ.Theta1(); }
    inline double &Theta1(void)
      { return sect_circ.Theta1(); }
    inline const double &Theta2(void) const
      { return sect_circ.Theta2(); }
    inline double &Theta2(void)
      { return sect_circ.Theta2(); }
    virtual GEOM_FT Ix(void) const;
    virtual GEOM_FT Iy(void) const;
    virtual GEOM_FT Pxy(void) const;
    virtual GEOM_FT Iz(void) const;

    virtual GEOM_FT GetMax(unsigned short int i) const;
    virtual GEOM_FT GetMin(unsigned short int i) const;

    virtual bool In(const Pos3d &p, const double &tol= 0.0) const;
    Pos3dArray getArcPoints(const size_t &n) const;

    friend bool operator ==(const CircularSector3d &a,const CircularSector3d &b);

    
    void Print(std::ostream &os) const;
  };

#endif



