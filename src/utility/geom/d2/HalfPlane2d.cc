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
//HalfPlane2d.cc

#include "HalfPlane2d.h"
#include "GeomGroup2d.h"
#include "utility/geom/d1/Ray2d.h"
#include "utility/geom/d1/Segment2d.h"
#include "utility/geom/d1/Polyline2d.h"
#include "utility/geom/d2/2d_polygons/Polygon2d.h"

#include "../pos_vec/Dir2d.h"
#include "../pos_vec/Vector2d.h"

HalfPlane2d::HalfPlane2d(void)
  : Surface2d(), lim() {}

HalfPlane2d::HalfPlane2d(const Line2d &r)
  : Surface2d(), lim(r) {}

HalfPlane2d::HalfPlane2d(const Line2d &r, const Pos2d &p)
  : Surface2d(), lim(r)
  {
    if(!In(p))
      swap();
  }

HalfPlane2d::HalfPlane2d(const Pos2d &p1,const Pos2d &p2)
  : Surface2d(), lim(p1,p2) {}

//! @brief Comparison operator.
bool HalfPlane2d::operator==(const HalfPlane2d &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= Surface2d::operator==(other);
        if(retval)
          retval= (lim==other.lim);
       }
    return retval;
  }

GeomObj *HalfPlane2d::getCopy(void) const
  { return new HalfPlane2d(*this); }
void HalfPlane2d::swap(void)
  { lim.swap(); }

//! @brief Return true if part of the ray is inside
//! the half-plane.
bool HalfPlane2d::intersects(const Ray2d &sr) const
  { return sr.intersects(lim); }

//! @brief Return true if part of the segment is inside
//! the half-plane.
bool HalfPlane2d::intersects(const Segment2d &sg) const
  { return sg.intersects(lim); }

//! @brief Return true if part of the polyline is inside
//! the half-space.
bool HalfPlane2d::intersects(const Polyline2d &pline) const
  {
    bool retval= false;
    list_Pos2d::const_iterator i= pline.begin();
    bool pline_in= this->In(*i);
    i++;
    for(; i!= pline.end(); i++)
      {
	const Pos2d &pos= *i;
	if(!this->In(pos) and pline_in)
	  {
	    retval= true;
	    break;
	  }
	else if(this->In(pos) and !pline_in)
	  {
	    retval= true;
	    break;
	  }
      }
    return retval;
  }

HalfPlane2d HalfPlane2d::getSwap(void) const
  {
    HalfPlane2d retval(*this);
    retval.swap();
    return retval;
  }

//! @brief Return true if the given point is inside the half-space.
//! The points of the edge plane belong to the half-plane.
bool HalfPlane2d::In(const Pos2d &p, const double &tol) const
  {
    bool retval= false;
    if(lim.negativeSide(p))
      retval= true;
    else if(lim.In(p,tol))
      retval= true;
    return retval;
  }

//! @brief Return true if the given line is inside the half-space.
bool HalfPlane2d::In(const Line2d &l, const double &tol) const
  {
    bool retval= false;
    if(!lim.intersects(l))
      retval= In(l.Point());
    return retval;
  }

//! @brief Return true if the given ray is inside the half-space.
bool HalfPlane2d::In(const Ray2d &r, const double &tol) const
  {
    bool retval= false;
    if(!lim.intersects(r))
      retval= In(r.getFromPoint());
    return retval;
  }

//! @brief Return true if the given segment is inside the half-space.
bool HalfPlane2d::In(const Segment2d &sg, const double &tol) const
  {
    bool retval= false;
    if(In(sg.getFromPoint()) && In(sg.getToPoint()))
      retval= true;
    return retval;
  }

//! @brief Return true if the given polyline is inside the half-space.
bool HalfPlane2d::In(const Polyline2d &pline, const double &tol) const
  {
    bool retval= true;
    for(list_Pos2d::const_iterator i= pline.begin(); i!= pline.end(); i++)
      {
	const Pos2d &pos= *i;
	if(!this->In(pos))
	  {
	    retval= false;
	    break;
	  }
      }
    return retval;
  }

//! @brief Return true if the given polygon is inside the half-space.
bool HalfPlane2d::In(const Polygon2d &plg, const double &tol) const
  {
    return this->In(plg.getPolyline(), tol);
  }

//! @brief Return true if the given point is inside the half-space.
//! The points of the edge plane belong to the half-plane.
bool HalfPlane2d::Out(const Pos2d &p, const double &tol) const
  {
    const HalfPlane2d opposite= this->getSwap();
    return opposite.In(p, tol);
  }

//! @brief Return true if the given line is inside the half-space.
bool HalfPlane2d::Out(const Line2d &l, const double &tol) const
  {
    const HalfPlane2d opposite= this->getSwap();
    return opposite.In(l, tol);
  }

//! @brief Return true if the given ray is inside the half-space.
bool HalfPlane2d::Out(const Ray2d &r, const double &tol) const
  {
    const HalfPlane2d opposite= this->getSwap();
    return opposite.In(r, tol);
  }

//! @brief Return true if the given segment is inside the half-space.
bool HalfPlane2d::Out(const Segment2d &sg, const double &tol) const
  {
    const HalfPlane2d opposite= this->getSwap();
    return opposite.In(sg, tol);
  }

//! @brief Return true if the given polyline is inside the half-space.
bool HalfPlane2d::Out(const Polyline2d &pline, const double &tol) const
  {
    const HalfPlane2d opposite= this->getSwap();
    return opposite.In(pline, tol);
  }

//! @brief Return true if the given polygon is inside the half-space.
bool HalfPlane2d::Out(const Polygon2d &plg, const double &tol) const
  {
    const HalfPlane2d opposite= this->getSwap();
    return opposite.In(plg, tol);
  }

GeomGroup2d HalfPlane2d::getIntersection(const Line2d &r) const
  {
    GeomGroup2d retval;
    if(!intersects(r)) return retval;
    GeomObj::list_Pos2d lista= intersection(lim,r);
    const Pos2d pint= *lista.begin();
    const Vector2d vd= r.VDir();
    const Pos2d p1= pint+vd;
    if(In(p1))
      retval.push_back(Ray2d(pint,vd));
    else
      retval.push_back(Ray2d(pint,-vd));
    return retval;
  }

GeomGroup2d HalfPlane2d::getIntersection(const Ray2d &sr) const
  {
    GeomGroup2d retval;
    if(!intersects(sr)) return retval;
    GeomObj::list_Pos2d lista= intersection(lim,sr);
    const Pos2d pint= *lista.begin();
    const Vector2d vd= sr.VDir();
    const Pos2d p1= pint+vd;
    if(In(p1))
      retval.push_back(Ray2d(pint,vd));
    return retval;
  }

GeomGroup2d HalfPlane2d::getIntersection(const Segment2d &sg) const
  {
    GeomGroup2d retval;
    if(!intersects(sg)) return retval;
    GeomObj::list_Pos2d lista= intersection(lim,sg);
    const Pos2d pint= *lista.begin();
    const Pos2d p1= sg.getToPoint();
    const Pos2d p2= sg.getFromPoint();
    if(In(p1))
      retval.push_back(Segment2d(pint,p1));
    else
      retval.push_back(Segment2d(pint,p2));
    return retval;
  }

//! @brief Returns the part of the line that is inside the half-space.
Ray2d HalfPlane2d::clip(const Line2d &l) const
  {
    Ray2d retval;
    if(intersects(l))
      {
	if(In(l))
	  std::cerr << getClassName() << "::" << __FUNCTION__
	 	    << "; the line is inside the half-space."
		    << " Can't do the clipping." << std::endl;
	else
	  {
	    GeomObj2d::list_Pos2d lst= lim.getIntersection(l);
	    Pos2d pt= *lst.begin();
            retval= Ray2d(pt,l.VDir());
	  }
      }
    else
      {
	 std::cerr << getClassName() << "::" << __FUNCTION__
		   << "; the line is outside the half-space."
		   << " Can't do the clipping." << std::endl;
      }
    return retval;
  }
      
//! @brief Returns the part of the ray that is inside the half-space.
Ray2d HalfPlane2d::clip(const Ray2d &r) const
  {
    Ray2d retval;
    if(intersects(r))
      {
	if(In(r))
	  std::cerr << getClassName() << "::" << __FUNCTION__
	 	    << "; the ray is inside the half-space."
		    << " Can't do the clipping." << std::endl;
	else
	  {
            Pos2d pt= lim.getIntersection(r);
	    Pos2d fromPoint= r.getFromPoint();
	    if(In(fromPoint))
	      std::cerr << getClassName() << "::" << __FUNCTION__
		        << "; the result of the clipping is a segment."
		        << " Can't deal with this case yet." << std::endl;
	    else
              retval= Ray2d(pt,r.VDir());
	  }
      }
    else
      {
	 std::cerr << getClassName() << "::" << __FUNCTION__
		   << "; the ray is outside the half-space."
		   << " Can't do the clipping." << std::endl;
      }
    return retval;
  }

//! @brief Returns the part of the segment that is inside the half-space.
Segment2d HalfPlane2d::clip(const Segment2d &sg) const
  {
    Segment2d retval;
    if(In(sg))
      retval= sg;
    else
      {
	Pos2d p0= sg.getFromPoint();
	Pos2d p1= sg.getToPoint();
	if(In(p0))
	  {
            Pos2d pt= lim.getIntersection(sg);
            return Segment2d(p0,pt);
	  }
	else if(In(p1))
	  {
            Pos2d pt= lim.getIntersection(sg);
            return Segment2d(p1,pt);
	  }
	else
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; the segment is outside the half-space."
	              << " Can't do the clipping." << std::endl;
	  }
      }
    return retval;
  }

//! @brief Return the polyline chunks that result from clipping the given
//! polyline with this half plane.
std::deque<Polyline2d> HalfPlane2d::clip(const Polyline2d &pline, const GEOM_FT &tol) const
  { return pline.clip(*this, tol); } 

//! @brief Return the polyline chunks that result from clipping the given
//! polyline with this half plane.
boost::python::list HalfPlane2d::clipPy(const Polyline2d &pline, const GEOM_FT &tol) const
  { return pline.clipPy(*this, tol); } 
  
GeomGroup2d intersection(const HalfPlane2d &sp,const Line2d &r)
  { return sp.getIntersection(r); }
GeomGroup2d intersection(const HalfPlane2d &sp,const Ray2d &sr)
  { return sp.getIntersection(sr); }
GeomGroup2d intersection(const HalfPlane2d &sp,const Segment2d &sg)
  { return sp.getIntersection(sg); }
GeomGroup2d intersection(const Line2d &r,const HalfPlane2d &sp)
  { return intersection(sp,r); }
GeomGroup2d intersection(const Ray2d &sr,const HalfPlane2d &sp)
  { return intersection(sp,sr); }
GeomGroup2d intersection(const Segment2d &sg,const HalfPlane2d &sp)
  { return intersection(sp,sg); }

HalfPlane2d HalfPlane2d::getNormalized(void) const
  { return HalfPlane2d(lim.getNormalized()); }

//! @brief Aplica al objeto la transformación que se
//! pasa como parámetro.
void HalfPlane2d::Transform(const Trf2d &trf2d)
  { lim.Transform(trf2d); }
