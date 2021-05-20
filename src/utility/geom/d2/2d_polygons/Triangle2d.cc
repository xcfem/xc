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
//Triangle2d.cc

#include "Triangle2d.h"
#include "Polygon2d.h"
#include "utility/geom/d1/Segment2d.h"

#include <plotter.h>
#include "utility/geom/trf/Trf2d.h"

//! @brief Return la base opuesta al vértice i.
Segment2d Triangle2d::Base(const size_t &i) const
  { return Lado(i%3+1); }
//! @brief Return the height corresponding to the i-th vertex.
Segment2d Triangle2d::Altura(const size_t &i) const
  {
    if(isDegenerated())
      {
        cerr << getClassName() << "::" << __FUNCTION__
	     << "; warning!, is a degenerated triangle." << endl;
      }
    const Line2d rbase= Base(i).getSupportLine();
    const Line2d perp= rbase.Perpendicular(Vertice(i));
    const Pos2d pint= *intersection(rbase,perp).begin();
    return Segment2d(Vertice(i),pint);
  }
//! @brief Return object length.
GEOM_FT Triangle2d::getLength(void) const
  {
    GEOM_FT retval= Lado(1).getLength();
    retval+= Lado(2).getLength();
    retval+= Lado(3).getLength();
    return retval;
  }
Pos2d Triangle2d::getCenterOfMass(void) const
  {
    Vector2d v= Vertice(1).VectorPos();
    v= v+Vertice(2).VectorPos();
    v= v+Vertice(3).VectorPos();
    return Origin2d+v/3.0;
  }

GEOM_FT Triangle2d::GetMax(unsigned short int i) const
  {
    GEOM_FT retval= Vertice(1)(i);
    retval= max(retval,Vertice(2)(i));
    retval=max(retval,Vertice(3)(i));
    return retval;
  }
GEOM_FT Triangle2d::GetMin(unsigned short int i) const
  {
    GEOM_FT retval= Vertice(1)(i);
    retval= min(retval,Vertice(2)(i));
    retval=min(retval,Vertice(3)(i));
    return retval;
  }

//! @brief Return the triangle as a polygon.
Polygon2d Triangle2d::getPolygon(void) const
  {
    Polygon2d retval;
    retval.push_back(Vertice(1));
    retval.push_back(Vertice(2));
    retval.push_back(Vertice(3));
    return retval;
  }


void Triangle2d::Print(std::ostream &os) const
  { os << cgtriang; }
void Triangle2d::Plot(Plotter &plotter) const
  {
    const Pos2d p1= Vertice(1);
    const Pos2d p2= Vertice(2);
    const Pos2d p3= Vertice(3);
    plotter.fline(p1.x(),p1.y(),p2.x(),p2.y());
    plotter.fline(p2.x(),p2.y(),p3.x(),p3.y());
    plotter.fline(p3.x(),p3.y(),p1.x(),p1.y());
  }

//! @brief Applies to the triangle the transformation argument.
void Triangle2d::Transform(const Trf2d &trf2d)
  {
    const Pos2d pA= trf2d.Transform(Vertice(1));
    const Pos2d pB= trf2d.Transform(Vertice(2));
    const Pos2d pC= trf2d.Transform(Vertice(3));
    (*this)= Triangle2d(pA,pB,pC);
  }
