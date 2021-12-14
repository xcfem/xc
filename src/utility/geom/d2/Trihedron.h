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
//Trihedron.h

#ifndef TRIHEDRON_H
#define TRIHEDRON_H

#include "../d3/GeomObj3d.h"
#include "utility/geom/pos_vec/Pos3d.h"
#include "utility/geom/d2/Triangle3d.h"

class Plane;
class Polyhedron3d;

//! @ingroup GEOM
//
//! @brief A geometric figure composed of three planes meeting at
//! a single vertex.
class Trihedron: public GeomObj3d
  {
    Pos3d p0;
    Triangle3d tr;
  protected:
    Plane get_plane(const size_t &i) const;
  public:
    Trihedron(void);
    Trihedron(const Pos3d &p0, const Pos3d &p1,const Pos3d &p2, const Pos3d &p3);
    Trihedron(const Pos3d &p0, const Triangle3d &tr);
    virtual GeomObj *getCopy(void) const
      { return new Trihedron(*this); }
    ~Trihedron(void) {}
    void Put( const Pos3d &p0, const Pos3d &p1,
              const Pos3d &p2, const Pos3d &p3);
    void Put(const Pos3d &p0, const Triangle3d &tr);
    inline virtual unsigned int getNumVertices(void) const
      { return 4; }
    inline virtual unsigned int getNumVerticesFaceta(unsigned int faceta) const
      { return 3; }
    inline virtual unsigned int getNumEdges(void) const
      { return 3; }
    inline virtual unsigned int GetNumFacetas(void) const
      { return 3; }
    //! @brief Return the length of the object.
    inline virtual GEOM_FT getLength(void) const
      { return 0.0; }
    //! @brief Return the object area.
    virtual GEOM_FT getArea(void) const
      { return NAN; }
    //! @brief Return the volume of the object.
    virtual GEOM_FT getVolume(void) const
      { return NAN; }
    virtual GEOM_FT Ix(void) const
      { return NAN; }
    virtual GEOM_FT Iy(void) const
      { return NAN; }
    virtual GEOM_FT Iz(void) const
      { return NAN; }
    short unsigned int Dimension() const
      { return 3; }
    GEOM_FT GetMax(short unsigned int) const;
    GEOM_FT GetMin(short unsigned int) const;
    Pos3d getCenterOfMass() const;
    bool TocaCuadrante(const int &) const;

    const Triangle3d &Base(void) const;
    const Pos3d &Cuspide(void) const;
    Line3d Axis(void) const;
    GEOM_FT getConeHalfAngle(void) const;

    Polyhedron3d getPolyhedron3d(void) const;
    Pos3d Vertice(const size_t &i) const;
    virtual GEOM_FT PseudoDist(const Pos3d &p) const;
    virtual bool In(const Pos3d &p,const double &tol= 0.0) const;

    void Print(std::ostream &os) const;
  };


#endif



