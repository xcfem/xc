//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//Tetrahedron3d.cc

#include "Tetrahedron3d.h"
#include "Polyhedron3d.h"
#include "utility/geom/pos_vec/Pos3d.h"
#include "utility/geom/d3/HalfSpace3d.h"



#include "utility/matrices/m_double.h"
#include "utility/geom/pos_vec/Pos3dList.h"

const Pos3d v1PorDefecto(1.0,0.0,0.0);
const Pos3d v2PorDefecto(0.0,1.0,0.0);
const Pos3d v3PorDefecto(0.0,0.0,1.0);
const Pos3d v4PorDefecto(0.0,0.0,0.0);

//! @brief Default constructor.
Tetrahedron3d::Tetrahedron3d(void)
  : cgtetrahedron(v1PorDefecto.ToCGAL(),v2PorDefecto.ToCGAL(),v3PorDefecto.ToCGAL(),v4PorDefecto.ToCGAL()) {}

//! @brief Constructor.
Tetrahedron3d::Tetrahedron3d(const Pos3d &p0, const Pos3d &p1,const Pos3d &p2, const Pos3d &p3)
  : cgtetrahedron(p0.ToCGAL(),p1.ToCGAL(),p2.ToCGAL(),p3.ToCGAL()){}

//! @brief Constructor.
Tetrahedron3d::Tetrahedron3d(const HalfSpace3d &se0, const HalfSpace3d &se1,const HalfSpace3d &se2, const HalfSpace3d &se3)
  : cgtetrahedron()
  {
    Polyhedron3d tmp(se0,se1,se2,se3);
    GeomObj::list_Pos3d vertices= tmp.getVertices();
    assert (vertices.size()==4);
    cgtetrahedron= CGTetrahedron_3(vertices[0].ToCGAL(),vertices[1].ToCGAL(),vertices[2].ToCGAL(),vertices[3].ToCGAL());
  }

Polyhedron3d Tetrahedron3d::getPolyhedron3d(void) const
  {
    return Polyhedron3d(Pos3d(cgtetrahedron.vertex(0)),Pos3d(cgtetrahedron.vertex(1)),Pos3d(cgtetrahedron.vertex(2)),Pos3d(cgtetrahedron.vertex(3)));
  }

GEOM_FT Tetrahedron3d::GetMax(unsigned short int i) const
  {
    CGPoint_3 vi= cgtetrahedron.vertex(0);
    GEOM_FT retval= vi.cartesian(i-1);
    for(int j=1;j<4;j++)
      {
        vi= cgtetrahedron.vertex(j);
        retval= std::max(retval,vi.cartesian(i-1));
      }
    return retval;
  }
GEOM_FT Tetrahedron3d::GetMin(unsigned short int i) const
  {
    CGPoint_3 vi= cgtetrahedron.vertex(0);
    GEOM_FT retval= vi.cartesian(i-1);
    for(int j=1;j<4;j++)
      {
        vi= cgtetrahedron.vertex(j);
        retval= std::min(retval,vi.cartesian(i-1));
      }
    return retval;
  }

//! @brief Return the object area.
GEOM_FT Tetrahedron3d::getArea(void) const
  { return getPolyhedron3d().getArea(); }

//! @brief Return the volume of the body with sign.
GEOM_FT Tetrahedron3d::getVolumeWithSign(void) const
  {
    return cgtetrahedron.volume();
  }

//! @brief Return the volume of the object.
GEOM_FT Tetrahedron3d::getVolume(void) const
  { return std::abs(getVolumeWithSign()); }

//! @brief Returns true if point inside tetrahedron.
bool Tetrahedron3d::In(const Pos3d &p,const double &tol) const
  {
    CGAL::Bounded_side side= cgtetrahedron.bounded_side(p.ToCGAL());
    return (side != CGAL::ON_UNBOUNDED_SIDE);
  }

//! @brief Prints object.
void Tetrahedron3d::Print(std::ostream &os) const
  {
    CGAL::set_ascii_mode(os);
    CGAL::set_pretty_mode(os);
    os << cgtetrahedron << std::endl;
  }

PolygonMap<CGPolyhedron_3> getPolygonMap(const Tetrahedron3d &t)
  { return t.getPolyhedron3d().GetPolygonMap(); }

