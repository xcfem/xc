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
//BLOCKPYRAMID.h

#ifndef BLOCKPYRAMID_H
#define BLOCKPYRAMID_H

#include <iostream>
#include "../GeomObj3d.h"
#include "utility/geom/pos_vec/Pos3d.h"
#include "utility/geom/d3/HalfSpace3d.h"
#include "utility/matrices/m_double.h"


//! @ingroup GEOM
//
//! @brief Polyhedral angle.
class BlockPyramid: public GeomObj3d
  {
  protected:
    std::deque<HalfSpace3d> half_spaces; //!< Half spaces that define the object.

    std::deque<Vector3d> haz_vectores(void) const;
    static bool es_interior(const HalfSpace3d &,const Vector3d &);
    bool es_interior(const Vector3d &) const;
    std::deque<Vector3d> haz_vectores_interiores(void) const;
  public:
    BlockPyramid(void);
    explicit BlockPyramid(const std::deque<HalfSpace3d> &);
    virtual GeomObj *getCopy(void) const
      { return new BlockPyramid(*this); }
    inline unsigned short int Dimension(void) const
      { return 3; }
    GEOM_FT getLength(void) const;
    GEOM_FT getArea(void) const;
    GEOM_FT getVolume(void) const;
    GEOM_FT Ix(void) const;
    GEOM_FT Iy(void) const;
    GEOM_FT Iz(void) const;
    Pos3d getCenterOfMass(void) const;
    GEOM_FT GetMax(unsigned short int i) const;
    GEOM_FT GetMin(unsigned short int i) const;
    m_double getVectoresBorde(void) const;
    Vector2d getVectorExterno1(const Ref2d3d &) const;
    Vector2d getVectorExterno2(const Ref2d3d &) const;
    std::deque<Vector2d> getVectoresExternos(const Ref2d3d &) const;
    bool Vacio(void) const;
    void Print(std::ostream &os) const;
  };

#endif
