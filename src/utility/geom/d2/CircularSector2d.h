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

#ifndef SECTORCIRCULAR2D_H
#define SECTORCIRCULAR2D_H

#include "Circle2d.h"

class Pos2d;
class Pos2dArray;

//! @ingroup GEOM
//
//! @brief 2D circular sector.
class CircularSector2d : public Circle2d
  {
    double theta1; //!< Angle that defines the start point of the arc.
    double theta2; //!< Angle that defines the end point of the arc.
  public:
    CircularSector2d(void);
    CircularSector2d(const Circle2d &c,const double &th1,const double &th2);
    
    virtual bool operator==(const CircularSector2d &) const;
    virtual Surface2d *getCopy(void) const;
    virtual Pos2d getCenterOfMass(void) const;
    virtual Pos2d ArcCentroid(void) const;
    Pos2d PInic(void) const;
    Pos2d PFin(void) const;
    Pos2d PMed(void) const;
    inline const double &Theta1(void) const
      { return theta1; }
    inline double &Theta1(void)
      { return theta1; }
    inline const double &Theta2(void) const
      { return theta2; }
    inline double &Theta2(void)
      { return theta2; }
    double getAngle(const Pos2d &p) const;
    double getLambda(const Pos2d &) const;
    double getIncludedAngle(void) const;
    virtual GEOM_FT getArcLength(void) const;
    virtual GEOM_FT getLength(void) const;
    virtual GEOM_FT getArea(void) const;
    virtual GEOM_FT Ix(void) const;
    virtual GEOM_FT Iy(void) const;
    virtual GEOM_FT Pxy(void) const;
    virtual GEOM_FT Iz(void) const;
    virtual GEOM_FT GetMax(unsigned short int i) const;
    virtual GEOM_FT GetMin(unsigned short int i) const;
    virtual bool In(const Pos2d &p, const double &tol= 0.0) const;
    Pos2dArray getArcPoints(const size_t &n) const;

    void Print(std::ostream &os) const;
    void Plot(Plotter &) const;
  };

CircularSector2d CircularSector2dThreepoints(const Pos2d &,const Pos2d &,const Pos2d &);


#endif



