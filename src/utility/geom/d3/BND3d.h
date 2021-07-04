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
//BND3D.h

#ifndef BND3D_H
#define BND3D_H

#include <iostream>
#include "GeomObj3d.h"
#include "../cgal_types.h"


//! @ingroup GEOM
//
//! @brief "boundary" en tres dimensiones.
class BND3d: public GeomObj3d
  {
    CGIsoCuboid_3 cgisocub;
    bool undefined;
  protected:
    bool LBClipTest(const GEOM_FT &p,const GEOM_FT &q,GEOM_FT &u1,GEOM_FT &u2) const;
    bool LBClipLine(const Pos3d &p1,const Pos3d &p2) const;
  public:
    BND3d(void);
    BND3d(const Pos3d &p_min,const Pos3d &p_max);
    virtual GeomObj *getCopy(void) const
      { return new BND3d(*this); }
    //! @brief Return the dimension of the object 0, 1, 2 or 3.
    inline virtual unsigned short int Dimension(void) const
      { return 3; }
    //! @brief Return object length (x axis).
    inline virtual GEOM_FT getLength(void) const
      { return GetXMax()-GetXMin(); }
    //! @brief Return object width (y axis).
    inline GEOM_FT getWidth(void) const
      { return GetYMax()-GetYMin(); }
    //! @brief Return object height (z axis).
    inline GEOM_FT getHeight(void) const
      { return GetZMax()-GetZMin(); }
    Vector3d Diagonal(void) const;
    GEOM_FT getAreaXYFace(void) const;
    GEOM_FT getAreaXZFace(void) const;
    GEOM_FT getAreaYZFace(void) const;
    virtual GEOM_FT getArea(void) const;
    //! @brief Return the object volume
    inline virtual GEOM_FT getVolume(void) const
      { return cgisocub.volume(); }
    virtual GEOM_FT Ix(void) const;
    virtual GEOM_FT Iy(void) const;
    virtual GEOM_FT Iz(void) const;
    void Update(const Pos3d &);
    void PutPMax(const Pos3d &);
    void PutPMin(const Pos3d &);
    void PutPMinMax(const Pos3d &,const Pos3d &);
    Pos3d GetPMax(void) const;
    Pos3d GetPMin(void) const;
    inline GEOM_FT GetMax(unsigned short int i) const
      { return cgisocub.max_coord(i-1); }
    inline GEOM_FT GetMin(unsigned short int i) const
      { return cgisocub.min_coord(i-1); }
    int GetNumCaras(void) const
      { return 6; }
    CGBbox_3 GetCGALBbox_3(void) const;
    Pos3d getCenterOfMass(void) const;
    Pos3d Vertice(unsigned int i) const;
    unsigned short int RegionCode(const Pos3d &,const double &tol= 0.0) const;
    bool In(const Pos3d &p,const double &tol= 0.0) const;
    bool ClipLine(const Pos3d &p1,const Pos3d &p2) const;
    BND3d Offset(const GEOM_FT &o) const;
    BND3d &operator +=(const Pos3d &p);
    BND3d &operator +=(const BND3d &a);
    friend BND3d operator +(const BND3d &a, const BND3d &b);
    friend bool operator ==(const BND3d &a,const BND3d &b);
    void Print(std::ostream &stream) const;
  };

#endif
