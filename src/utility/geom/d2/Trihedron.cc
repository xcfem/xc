//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//Trihedron.cc

#include "Trihedron.h"
#include "utility/geom/d2/Plane.h"
#include "utility/geom/d3/3d_polyhedrons/Polyhedron3d.h"
#include "utility/geom/d1/Line3d.h"

Trihedron::Trihedron(void)
  : GeomObj3d(), p0(Pos3d(0,0,0)), tr(Pos3d(1,0,0),Pos3d(0,1,0),Pos3d(0,0,1)) {}
Trihedron::Trihedron( const Pos3d &P0, const Pos3d &p1, const Pos3d &p2, const Pos3d &p3)
  : GeomObj3d(), p0(P0), tr(p1,p2,p3) {}
Trihedron::Trihedron(const Pos3d &P0, const Triangle3d &TR)
  : GeomObj3d(), p0(P0), tr(TR){}

Polyhedron3d Trihedron::getPolyhedron3d(void) const
  {
    Polyhedron3d retval(Vertice(0),Vertice(1),Vertice(2),Vertice(3));
    return retval;
  }

Plane Trihedron::get_plane(const size_t &i) const
  {
    Polyhedron3d tmp(getPolyhedron3d());
    Polyhedron3d::Facet_const_iterator j= tmp.facets_begin();
    for(size_t k=0;k<i;k++)
      j++;
    return tmp.getPlaneFromFace(j);
  }

//! @brief Return the triangle that is the trihedron base.
const Triangle3d &Trihedron::Base(void) const
  { return tr; }

//! @brief Return the trihedron apex.
const Pos3d &Trihedron::Cuspide(void) const
  { return p0; }


//! @brief Return the straight line that passes through the trihedron apex.
Line3d Trihedron::Axis(void) const
  { return Line3d(p0,tr.getCenterOfMass()); }

//! @brief Return the angle of the cone that has the same apex
//! and contains the trihedron.
GEOM_FT Trihedron::getConeHalfAngle(void) const
  {
    const Line3d axis= Axis();
    GEOM_FT angConico= angle(axis,Line3d(p0,Vertice(1)));
    angConico= std::max(angConico, angle(axis,Line3d(p0,Vertice(2))));
    angConico= std::max(angConico, angle(axis,Line3d(p0,Vertice(3))));
    return angConico;
  }

Pos3d Trihedron::Vertice(const size_t &i) const
  {
    if(i==0)
      return p0;
    else
      return tr.Vertice(i);
  }

//! @brief Return the SIGNED distance from the point to the trihedron.
//!
//! The distance is computed as the maximum of the distances from the point to
//! each of the three planes that meet in the apex.
GEOM_FT Trihedron::PseudoDist(const Pos3d &p) const
  {
    Polyhedron3d tmp= getPolyhedron3d();
    Polyhedron3d::Facet_const_iterator j= tmp.facets_begin();
    Plane plane= tmp.getPlaneFromFace(j);
    GEOM_FT dmax= -plane.PseudoDist(p);
    j++;
    for(;j!=tmp.facets_end();j++)
      {
        plane= tmp.getPlaneFromFace(j);
        dmax= std::max(dmax,-plane.PseudoDist(p));
      }
    return dmax;
  }

//! @brief Return true if the point is inside the thriedron.
bool Trihedron::In(const Pos3d &p,const double &tol) const
  {
    const Line3d axis= Axis();
    GEOM_FT d= axis.dist(p);
    GEOM_FT cylinderRadius= axis.dist(Vertice(1));
    cylinderRadius= std::max(cylinderRadius,axis.dist(Vertice(2)));
    cylinderRadius= std::max(cylinderRadius,axis.dist(Vertice(3)));
    bool retval= true;
    if((d-cylinderRadius)>tol)
      return false;
    else
      {
        const GEOM_FT angConico= fabs(getConeHalfAngle());
        const GEOM_FT ang= fabs(angle(axis,Line3d(p0,p)));
        if(ang<1.1*angConico)
          {
            Polyhedron3d tmp= getPolyhedron3d();
            Plane plane;
            for(Polyhedron3d::Facet_const_iterator j= tmp.facets_begin();j!=tmp.facets_end();j++)
              {
                plane= tmp.getPlaneFromFace(j);
                d= -plane.PseudoDist(p);
                if(d>=tol)
                  {
                    retval= false;
                    break;
                  }
              }
          }
        else
          return false;
      }
    return retval;
  }

void Trihedron::Put( const Pos3d &P0, const Pos3d &p1,const Pos3d &p2, const Pos3d &p3)
  { Put(P0,Triangle3d(p1,p2,p3)); }

void Trihedron::Put(const Pos3d &P0,const Triangle3d &TR)
  {
    p0= P0;
    tr= TR;
  }

GEOM_FT Trihedron::GetMax(short unsigned int i) const
  { return getPolyhedron3d().GetMax(i); }
GEOM_FT Trihedron::GetMin(short unsigned int i) const
  { return getPolyhedron3d().GetMin(i); }
Pos3d Trihedron::getCenterOfMass() const
  { return getPolyhedron3d().getCenterOfMass(); }

//! @brief Return true if any vertex touches the quadrant argument.
bool Trihedron::TocaCuadrante(const int &cuadrante) const
  {
    return tr.TocaCuadrante(cuadrante); 
  }

void Trihedron::Print(std::ostream &os) const
  {
    Polyhedron3d tmp= getPolyhedron3d();
    Polyhedron3d::Vertex_const_iterator i= tmp.vertices_begin();
    for(;i!=tmp.vertices_end();i++)
      os << i->point() << ", ";
  }
