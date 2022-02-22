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
//BND2d.cc

#include "BND2d.h"
#include "../pos_vec/Pos2d.h"
#include "../pos_vec/Vector2d.h"
#include "utility/geom/d1/Line2d.h"
#include "utility/geom/d1/Ray2d.h"
#include "utility/geom/d1/Segment2d.h"
#include "utility/geom/d1/Polyline2d.h"
#include "utility/geom/d2/2d_polygons/Polygon2d.h"
#include <iostream>
#include <plotter.h>
#include "utility/geom/trf/Trf2d.h"
#include <cmath>

const double no_number= nan("");
const Pos2d P(no_number,no_number);

//! @brief Constructor.
BND2d::BND2d(void)
  : GeomObj2d(),cgrectg(P.ToCGAL(),P.ToCGAL()), undefined(true) {}

//! @brief Constructor.
BND2d::BND2d(const Pos2d &pa,const Pos2d &pb)
  : GeomObj2d(), cgrectg(pos_min(pa,pb).ToCGAL(),pos_max(pa,pb).ToCGAL()),
    undefined(false) {}

void BND2d::Update(const Pos2d &p)
  {
    if(undefined)
      {
	PutPMin(p);
	PutPMax(p);
	undefined= false;
      }
    else
      if(!In(p))
	{
	  GEOM_FT xmin= GetXMin();
	  GEOM_FT xmax= GetXMax();
	  GEOM_FT ymin= GetYMin();
	  GEOM_FT ymax= GetYMax();
	  if(p.x()< xmin)
	    xmin= p.x();
	  else if(p.x()>xmax)
	    xmax= p.x();
	  if(p.y()<ymin)
	    ymin= p.y();
	  else if(p.y()>ymax)
	    ymax= p.y();
	  PutPMin(Pos2d(xmin,ymin));
	  PutPMax(Pos2d(xmax,ymax));
	}
  }
void BND2d::PutPMax(const Pos2d &pmax)
  { BND2d::operator=(BND2d(getPMin(),pmax)); }
void BND2d::PutPMin(const Pos2d &pmin)
  { BND2d::operator=(BND2d(pmin,getPMax())); }
void BND2d::PutPMinMax(const Pos2d &pmin,const Pos2d &pmax)
  { BND2d::operator=(BND2d(pmin,pmax)); }
Pos2d BND2d::getPMax(void) const
  { return Pos2d(cgrectg.max()); }
Pos2d BND2d::getPMin(void) const
  { return Pos2d(cgrectg.min()); }

Polygon2d BND2d::getPolygon(void) const
  {
    if(undefined)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; boundary is undefined." << std::endl;
    const Pos2d p1= getPMin();
    const Pos2d p2= getPMax();
    Polygon2d retval;
    retval.push_back(p1);
    retval.push_back(Pos2d(p2.x(),p1.y()));
    retval.push_back(p2);
    retval.push_back(Pos2d(p1.x(),p2.y()));
    return retval;
  }

//! @brief Return the object area.
GEOM_FT BND2d::getArea(void) const
  {
    if(undefined)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; boundary is undefined." << std::endl;
    return cgrectg.area();
  }
Vector2d BND2d::Diagonal(void) const
  {
    if(undefined)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; boundary is undefined." << std::endl;
    return getPMax() - getPMin();
  }
Pos2d BND2d::getCenterOfMass(void) const
  {
    if(undefined)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; boundary is undefined." << std::endl;
    Pos2d center_of_mass= getPMin() + Diagonal()/2;
    return center_of_mass;
  }
GEOM_FT BND2d::Ix(void) const
  {
    if(undefined)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; boundary is undefined." << std::endl;
    return (getWidth()*getHeight()*getHeight()*getHeight())/12.0;
  }
GEOM_FT BND2d::Iy(void) const
  {
    if(undefined)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; boundary is undefined." << std::endl;
    return (getHeight()*getWidth()*getWidth()*getWidth())/12.0;
  }


//! @brief Return true it the boundary contains the point.
bool BND2d::In(const Pos2d &p) const
  {
    bool retval= false;
    if(!undefined)
      {
	const GEOM_FT xmin= GetXMin();
	const GEOM_FT xmax= GetXMax();
	const GEOM_FT ymin= GetYMin();
	const GEOM_FT ymax= GetYMax();
	if(p.x()< xmin)
	  retval= false;
	else if(p.x()>xmax)
	  retval= false;
	else if(p.y()<ymin)
	  retval= false;
	else if(p.y()>ymax)
	  retval= false;
	else
	  retval= true;
      }
    return retval;
  }

//! @brief Return true if contains the polyline.
bool BND2d::In(const Polyline2d &p) const
  { return In(p.begin(),p.end()); }

//! @brief Return true if contains the polygon.
bool BND2d::In(const Polygon2d &p) const
  { return In(p.vertices_begin(),p.vertices_end()); }

//! @brief Return true if the boundary contains the point.
bool BND2d::Overlap(const Pos2d &p) const
  { return In(p); }

//! @brief Return true if the line and the boundary overlap.
bool BND2d::Overlap(const Line2d &r) const
  { return Interseca(r); }
  
//! @brief Return true if the ray and the boundary overlap.
bool BND2d::Overlap(const Ray2d &sr) const
  { return Interseca(sr); }

//! @brief Return true if the segment and the boundary overlap.
bool BND2d::Overlap(const Segment2d &sg) const
  { return Interseca(sg); }

//! @brief Return true if boundary overlap.
bool BND2d::Overlap(const BND2d &bnd) const
  {
    if(undefined)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; boundary is undefined." << std::endl;
    return do_intersect(cgrectg,bnd.cgrectg);
  }

//! @brief Return true if the polyline and the boundary overlap.
bool BND2d::Overlap(const Polyline2d &p) const
  {
    if(undefined)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; boundary is undefined." << std::endl;
    bool retval= Overlap(p.begin(),p.end());
    if(!retval)
      for(Polyline2d::const_iterator j=p.begin();j!=p.end();j++)
        if(Overlap(p.getSegment(j)))
          {
            retval= true;
            break;
          }
    return retval;
  }

bool BND2d::Overlap(const Polygon2d &p) const
  {
    if(undefined)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; boundary is undefined." << std::endl;
    bool retval= Overlap(p.vertices_begin(),p.vertices_end());
    if(!retval)
      retval= p.Overlap(getPolygon());
    if(!retval)
      {
        const unsigned int nl= p.getNumEdges();
        for(unsigned int i= 1; i<=nl;i++)
          if(Overlap(p.Lado(i)))
            {
              retval= true;
               break;
            }
      }
    return retval;
  }

bool BND2d::Overlap(const std::list<Polygon2d> &l) const
  {
    if(undefined)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; boundary is undefined." << std::endl;
    bool retval= false;
    if(!l.empty())
      for(std::list<Polygon2d>::const_iterator i=l.begin();i!=l.end();i++)
        if(Overlap(*i))
          {
            retval= true;
            break;
          }
    return retval;
  }


bool BND2d::Interseca(const Line2d &r) const
  { return do_intersect(cgrectg,r.ToCGAL()); }
bool BND2d::Interseca(const Ray2d &sr) const
  { return do_intersect(cgrectg,sr.ToCGAL()); }
bool BND2d::Interseca(const Segment2d &sg) const
  { return do_intersect(cgrectg,sg.ToCGAL()); }
bool BND2d::Interseca(const BND2d &bnd) const
  { return do_intersect(cgrectg,bnd.cgrectg); }

//! @brief Aplica al objeto la transformación que se
//! pasa como parámetro.
void BND2d::Transform(const Trf2d &trf2d)
  {
    if(undefined)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; boundary is undefined." << std::endl;
    const Pos2d pA= trf2d.Transform(getPMin());
    const Pos2d pB= trf2d.Transform(getPMax());
    (*this)= BND2d(pA,pB);
  }

BND2d &BND2d::operator+=(const Pos2d &p)
  {
    if(undefined)
      Update(p);
    else
      PutPMinMax(pos_min(getPMin(),p),pos_max(getPMax(),p));
    return *this;
  }
BND2d &BND2d::operator+=(const BND2d &a)
  {
    if(undefined)
      BND2d::operator=(a);
    else      
      PutPMinMax(pos_min(getPMin(),a.getPMin()),pos_max(getPMax(),a.getPMax()));
    return *this;
  }
BND2d operator +(const BND2d &a, const BND2d &b)
  {
    BND2d retval(a);
    return (retval+=b);
  }
bool operator ==(const BND2d &a,const BND2d &b)
  {
    if ( a.getPMax() != b.getPMax() ) return false; 
    if ( a.getPMin() != b.getPMin() ) 
      return false;
    else
      return true;
  }
void BND2d::Print(std::ostream &stream) const
  {
    stream << "PMax= " << getPMax() << ','
           << "PMin= " << getPMin();
  }
void BND2d::Plot(Plotter &plotter) const
  {
    const Pos2d pmin= getPMin();
    const Pos2d pmax= getPMax();
    plotter.fline(pmin.x(),pmin.y(),pmax.x(),pmin.y());
    plotter.fline(pmax.x(),pmin.y(),pmax.x(),pmax.y());
    plotter.fline(pmax.x(),pmax.y(),pmin.x(),pmax.y());
    plotter.fline(pmin.x(),pmax.y(),pmin.x(),pmin.y());
  }
