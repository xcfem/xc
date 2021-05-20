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
//HalfPlane2d.cc

#include "HalfPlane2d.h"
#include "GeomGroup2d.h"
#include "utility/geom/d1/Ray2d.h"
#include "utility/geom/d1/Segment2d.h"

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

GeomObj *HalfPlane2d::clon(void) const
  { return new HalfPlane2d(*this); }
void HalfPlane2d::swap(void)
  { lim.swap(); }

bool HalfPlane2d::intersects(const Ray2d &sr) const
  { return sr.intersects(lim); }
bool HalfPlane2d::intersects(const Segment2d &sg) const
  { return sg.intersects(lim); }

HalfPlane2d HalfPlane2d::getSwap(void) const
  {
    HalfPlane2d retval(*this);
    retval.swap();
    return retval;
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
