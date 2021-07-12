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
//PolygonalSurface2d.cc

#include "PolygonalSurface2d.h"
#include "utility/geom/d1/Segment2d.h"
#include "utility/geom/d1/Polyline2d.h"
#include "utility/geom/d2/HalfPlane2d.h"
#include "utility/geom/pos_vec/Pos2d.h"
#include "carsten_steger.h"
#include "utility/utils/misc_utils/matem.h"
#include "../BND2d.h"
#include <plotter.h>
#include "utility/geom/pos_vec/Pos2dList.h"

//! @brief Return los vertices of the polygon.
GeomObj::list_Pos2d PolygonalSurface2d::getVertices(void) const
  {
    unsigned int nv= getNumVertices();
    GeomObj::list_Pos2d lv;
    for(size_t i= 1; i <= nv; i++) lv.push_back(Vertice(i));
    return lv;
  }

//! @brief Return la normal al lado of the polygon.
Vector2d PolygonalSurface2d::getLado0Normal(const size_t i) const
  { return Lado0(i).Normal(); }


//! @brief Return la normal en el vértice of the polygon.
Vector2d PolygonalSurface2d::getVertex0Normal(const size_t i) const
  {
    const size_t nv= getNumVertices();
    size_t I= i-1;size_t J= i;
    if(i==0)
      I= nv-1;
    const Vector2d n1= getLado0Normal(I);
    const Vector2d n2= getLado0Normal(J);
    return (n1+n2)/2.0;
  }

//! @brief Return the closed polyline formed with the vertices
//! of the polygon.
Polyline2d PolygonalSurface2d::getPolyline(void) const
  {
    Polyline2d retval;
    unsigned int nv= getNumVertices();
    for(size_t i= 1; i <= nv; i++) retval.AgregaVertice(Vertice(i));
    retval.AgregaVertice(Vertice(1)); //Close the polyline.
    return retval;
  }

//! @brief Return the segments between vertices i and j (0->n).
Segment2d PolygonalSurface2d::Lado0(unsigned int i, unsigned int j) const
  { return Segment2d(Vertice0(i),Vertice0(j)); }

//! @brief Return the i-th side (i between 0 and num_sides-1).
Segment2d PolygonalSurface2d::Lado0(unsigned int i) const
  {
    const size_t nl= getNumEdges();
    unsigned int i1= i%nl;
    unsigned int i2= (i+1)%nl;
    return Lado0(i1,i2);
  }

//! @brief Return the i-th edge (i between 1 and num_sides).
Segment2d PolygonalSurface2d::Lado(unsigned int i) const
  { return Lado0(i-1); }

//! @brief Return the perimeter of the surface.
GEOM_FT PolygonalSurface2d::getLength(void) const
  {
    const unsigned int nl= getNumEdges();
    GEOM_FT temp = 0;
    for(unsigned int i= 1; i<=nl;i++)
      temp += Lado(i).getLength();
    return temp;
  }

//! @brief Return the distal edge with respect to the point argument.
//! @para p: 2D point.
int PolygonalSurface2d::getIndexOfDistalEdge(const Pos2d &p) const
  {
    const unsigned int nl= getNumEdges();
    int retval= 1;
    GEOM_FT d2= Lado(1).dist2(p);
    GEOM_FT maxDist2= d2;
    for(unsigned int i= 2; i<=nl;i++)
      {
        d2= Lado(i).dist2(p);
	if(d2>maxDist2)
	  {
	    retval= i;
	    maxDist2= d2;
	  }
      }
    return retval;
  }

//! @brief Return the proximal edge with respect to the point argument.
//! @param p: 2D point.
int PolygonalSurface2d::getIndexOfProximalEdge(const Pos2d &p) const
  {
    const unsigned int nl= getNumEdges();
    int retval= 1;
    GEOM_FT d2= Lado(1).dist2(p);
    GEOM_FT minDist2= d2;
    for(unsigned int i= 2; i<=nl;i++)
      {
        d2= Lado(i).dist2(p);
	if(d2<minDist2)
	  {
	    retval= i;
	    minDist2= d2;
	  }
      }
    return retval;
  }

//! @brief Return the distal vertex with respect to the point argument.
//! @param p: 2D point.
int PolygonalSurface2d::getIndexOfDistalVertex(const Pos2d &p) const
  {
    const unsigned int nl= getNumVertices();
    int retval= 1;
    GEOM_FT d2= Vertice(1).dist2(p);
    GEOM_FT maxDist2= d2;
    for(unsigned int i= 2; i<=nl;i++)
      {
        d2= Vertice(i).dist2(p);
	if(d2>maxDist2)
	  {
	    retval= i;
	    maxDist2= d2;
	  }
      }
    return retval;
  }
    
//! @brief Return the proximal vertex with respect to the point argument.
//! @param p: 2D point.
int PolygonalSurface2d::getIndexOfProximalVertex(const Pos2d &p) const
  {
    const unsigned int nl= getNumVertices();
    int retval= 1;
    GEOM_FT d2= Vertice(1).dist2(p);
    GEOM_FT minDist2= d2;
    for(unsigned int i= 2; i<=nl;i++)
      {
        d2= Vertice(i).dist2(p);
	if(d2<minDist2)
	  {
	    retval= i;
	    minDist2= d2;
	  }
      }
    return retval;
  }

//! @brief Return the object area.
GEOM_FT PolygonalSurface2d::getArea(void) const
  { return ::Abs(AreaSigno()); }

//! @brief Return the maximum value of the i-th coordinate.
GEOM_FT PolygonalSurface2d::GetMax(unsigned short int i) const
  { 
    GEOM_FT mx= Vertice(0)(i);
    unsigned int nv= getNumVertices();
    for(unsigned short int j= 0;j < nv;j++)
      mx= max(Vertice(j)(i),mx);
    return mx;
  }

//! @brief Return the minimum value of the i-th coordinate.
GEOM_FT PolygonalSurface2d::GetMin(unsigned short int i) const
  {
    GEOM_FT mn= Vertice(0)(i);
    unsigned int nv= getNumVertices();
    for(unsigned short int j= 0;j < nv;j++)
      mn= min(Vertice(j)(i),mn);
    return mn;
  }

//! @brief Return the center of mass.
Pos2d PolygonalSurface2d::getCenterOfMass(void) const
  {
     const GEOM_FT area= getArea();
     return Pos2d(getMoment(1,0)/area,getMoment(0,1)/area);
  }
//! @brief Return the centroid (point interior to the polygon).
//! Return the centroid (point interior to the polygon).
//! See the book Sistemas de Información Geográfica de
//! Joaquín Bosque Sendra (Ed. Rialp ).
Pos2d PolygonalSurface2d::Centroide(void) const
  {
    Pos2d retval=getCenterOfMass();
    if(In(retval)) return retval; //If the center of mass is inside, 
                                  //we prefer it.
    const BND2d bnd= Bnd();
    const Pos2d c= bnd.getCenterOfMass();
    const Line2d r(c,c+Vector2d(0,100));
    list<Segment2d> intersec= intersection(*this,r);
    if(!intersec.empty())
      {
        Segment2d sg= *intersec.begin();
        retval= sg.getCenterOfMass();
      }
    else
      cerr << getClassName() << "::" << __FUNCTION__
	   << "; error computing the polygon centroid: " << *this << endl;
    return retval;
  }
GEOM_FT PolygonalSurface2d::moment_sign(const int &p,const int &q) const
  { return p_q_moment(*this,p,q); }
GEOM_FT PolygonalSurface2d::getMoment(const int &p,const int &q) const
  {
    GEOM_FT retval= moment_sign(p,q); 
    if(AreaSigno()<0) retval*=-1;
    return retval;
  }

//! @brief Calcula el moment of inertia with respect to an axis parallel to the
//! x axis que pasa por the center of mass of the surface.
//! Ix = Integral y^2 dA
GEOM_FT PolygonalSurface2d::Ix(void) const
  { 
    const GEOM_FT Ixo= getMoment(0,2);
    return Ixo-getArea()*sqr(getCenterOfMass().y()); //Teorema de Steiner.
  }

//! @brief Calcula el moment of inertia with respect to an axis parallel to the
//! y axis that passes through the polygon centroid.
//! Iy = Integral x^2 dA
GEOM_FT PolygonalSurface2d::Iy(void) const
  { 
    const GEOM_FT Iyo= getMoment(2,0);
    return Iyo-getArea()*sqr(getCenterOfMass().x()); //Teorema de Steiner.
  }

//! @brief Calcula el product of inertia with respect to the axis parallel
//! to the axis x and y with origin in the polygon centroid.
//! Pxy = Integral x*y dA
GEOM_FT PolygonalSurface2d::Pxy(void) const
  {
    const GEOM_FT Ixy= getMoment(1,1);
    const Pos2d center_of_mass=getCenterOfMass();
    const GEOM_FT dx= center_of_mass.x();
    const GEOM_FT dy= center_of_mass.y();
    return Ixy-getArea()*dx*dy; //Theorem of parallel axis.
  }

//! @brief Return the points for which the polygon is almost tangent
//! to the direction argument.
// XXX Posiblemente falle con polígonos no convexos.
GeomObj::list_Pos2d PolygonalSurface2d::getApproxTangentPositions(const Vector2d &v) const
  {
    const size_t sz= getNumEdges();
    std::vector<double> tangsEnVertices(sz+1);
    Segment2d li= Lado0(sz-1);
    Segment2d lj= Lado0(0);
    GEOM_FT angleI= li.getSignedAngle(v),angleJ= 0;
    const GEOM_FT dosPi= 2*M_PI;
    if(angleI>=dosPi)
      angleI-=dosPi;
    for(size_t i=0;i<sz;i++)
      {
        angleJ= angleI+lj.getSignedAngle(li);
        tangsEnVertices[i]= (angleI+angleJ)/2.0; 
        li= lj;
        angleI= angleJ;
        lj= Lado0(i+1);
      }
    tangsEnVertices[sz]= tangsEnVertices[0]+dosPi;

    GeomObj::list_Pos2d retval;
    for(size_t i=0;i<sz;i++)
      {
        li= Lado0(i);
        angleI= tangsEnVertices[i];
        angleJ= tangsEnVertices[i+1];
        if((angleI<dosPi) && (angleJ>=dosPi))
          {
            angleI-=dosPi;
            angleJ-=dosPi;
            const double s= li.getLength()*(-angleI)/(angleJ-angleI);
            retval.AgregaSiNuevo(li.PtoParametricas(s));
          }
        else if(angleI*angleJ<=0)
          {
            const double s= li.getLength()*(-angleI)/(angleJ-angleI);
            retval.AgregaSiNuevo(li.PtoParametricas(s));
          }
      }
    return retval;
  }

//! @brief Return the distance from the point to the perímeter
//! The distance is computed as the maximum of:
//! -The signed distances from the point to each of the half-planes
//!  that contain a side.
// If the vertices are in conterclockwise order, positive distances
// correspond to point AT THE SAME SIDE OF THE POLYGON with respect
// to the segment, otherwise the sign of the computed distance must
// be changed.
GEOM_FT PolygonalSurface2d::DistSigno(const Pos2d &p,const bool &clockwise) const
  {
     const short int signo= (clockwise ? 1 : -1);
     const size_t nv= getNumVertices();
     GEOM_FT retval= NAN;
     if(nv>0)
       {
         if(nv==1)
           retval= dist(Vertice(1),p);
         else if(nv==2)
           {
             HalfPlane2d half_plane(Vertice(1),Vertice(2));
             retval= signo*half_plane.DistSigno(p);
           }
         else
           {
             //Distance to the first side
             Pos2d p1= Vertice(1);
             Pos2d p2= Vertice(2);
             HalfPlane2d half_plane(p1,p2);
             retval= signo*half_plane.DistSigno(p);
             for(unsigned int i=2; i<nv; i++)
               {
                 p1= Vertice(i);
                 p2= Vertice(i+1);
                 half_plane= HalfPlane2d(p1,p2);
                 const GEOM_FT d_i= signo*half_plane.DistSigno(p);
                 retval= max(retval,d_i);
               }
             //Distance to the last side
             p1= Vertice(nv);
             p2= Vertice(1);
             half_plane= HalfPlane2d(p1,p2);
             const GEOM_FT d_i= signo*half_plane.DistSigno(p);
             retval= max(retval,d_i);
           }
       }
    return retval;
  }

//! @brief Return the distance from the point to the polígono.
//! The distance is computed as the maximim of:
//! -The signed distances from the point to each of the planes that contain
//! one of the sides and are perpendicular to the surface.
//! If the point is inside the surface 0 is returned.
GEOM_FT PolygonalSurface2d::Dist(const Pos2d &p) const
  {
    const GEOM_FT retval= DistSigno(p);
    return (retval >= 0 ? retval : 0);
  }

//! @brief Return the distance from the point to the nearest edge.
GEOM_FT PolygonalSurface2d::getCover(const Pos2d &p) const
  { return -DistSigno(p); }

//! @brief Return the distance from the point to the nearest
//! of the intersections of the ray defined by the point and
//! the vector with the nearest edge.
GEOM_FT PolygonalSurface2d::getCover(const Pos2d &p, const Vector2d &vdir) const
  {
    GEOM_FT retval= std::nan("0");
    Ray2d r(p,vdir);
    GeomObj::list_Pos2d intersections= getPolyline().getIntersection(r);
    if(!intersections.empty())
      {
	GeomObj::list_Pos2d::const_iterator i= intersections.begin();
	Pos2d q= *i;
	retval= dist2(p,q);
	i++;
	for(; i!= intersections.end(); i++)
	  {
	    q= *i;
            retval= min(retval, dist2(p,q));
	  }
	retval= sqrt(retval);
      }
    return retval;
  }

void PolygonalSurface2d::Print(std::ostream &os) const
  {
    unsigned int nv= getNumVertices();
    if(nv<1) return;
    os << Vertice(1);
    for(unsigned int i= 2; i <= nv; i++)
      os << ", " << Vertice(i);
  }
void PolygonalSurface2d::Plot(Plotter &plotter) const
  {
    unsigned int nv= getNumVertices();
    if(nv<2) return;
    Pos2d p1= Vertice(1);
    for(unsigned int i= 2; i <= nv; i++)
      {
        Pos2d p2= Vertice(i);
        plotter.fline(p1.x(),p1.y(),p2.x(),p2.y());
        p1=p2;
      }
  }


//! @brief Return true if the line and the polygonal surface overlap.
bool PolygonalSurface2d::Overlap(const Line2d &r) const
  {
    GeomObj::list_Pos2d tmp= getPolyline().getIntersection(r);
    return !tmp.empty();
  }
  
//! @brief Return true if the ray and the polygonal surface overlap.
bool PolygonalSurface2d::Overlap(const Ray2d &sr) const
  {
    GeomObj::list_Pos2d tmp= getPolyline().getIntersection(sr);
    return !tmp.empty();
  }

// ONLY to be called from Clip(line)
std::list<Segment2d> empalma(const std::list<Segment2d> &lista)
  {
    std::list<Segment2d> retval;
    if(lista.size()>1)
      {
        list<Segment2d>::const_iterator i=lista.begin();
        Segment2d s1= *i; i++;
        for(;i!=lista.end();i++)
          {
            const Segment2d &s2= *i;
            if(dist2(s1.getToPoint(),s2.getFromPoint())<sqrt_mchne_eps_dbl)
              s1= Segment2d(s1.getFromPoint(),s2.getToPoint());
            else
              {
                retval.push_back(s1);
                s1= s2;
              }
          }
        retval.push_back(s1);
      }
    else
      retval= lista;
    return retval;
  }

//! @brief Return the intersection of the polygon with the line (if exists).
Segment2d PolygonalSurface2d::Clip(const Line2d &r) const
  {
    Segment2d retval;
    list<Segment2d> sg_list= without_degenerated(intersection(*this,r));
    if(sg_list.size()>1)
      sg_list= empalma(sg_list);
    if(!sg_list.empty())
      {
        retval= *sg_list.begin();
        if(sg_list.size()>1)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
  		      << "; the polygon clips the line in"
                      << sg_list.size() << " segments." << std::endl;
	    std::cerr << "sg_list: ";
            ::print(std::cerr,sg_list.begin(),sg_list.end());
	    std::cerr << std::endl;
          }
      }
    return retval;
  }

//! @brief Return the intersection of the polygon with the ray (if it exists).
Segment2d PolygonalSurface2d::Clip(const Ray2d &sr) const
  {
    Segment2d retval;
    list<Segment2d> sg_list= without_degenerated(intersection(*this,sr));
    if(!sg_list.empty())
      {
        retval= *sg_list.begin();
        if(sg_list.size()>1)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; the polygon clips the ray in"
                    << sg_list.size() << " segments." << std::endl;
      }
    return retval;
  }

//! @brief Return the intersection of the polygon with the segment
//! argument (if exists).
Segment2d PolygonalSurface2d::Clip(const Segment2d &sg) const
  {
    Segment2d retval;
    list<Segment2d> sg_list= without_degenerated(intersection(*this,sg));
    if(!sg_list.empty())
      {
        retval= *sg_list.begin();
        if(sg_list.size()>1)
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; the polygon clips the segment on "
                    << sg_list.size() << " segments." << std::endl;
      }
    else
      { retval.setExists(false); }
    return retval;
  }

//! @brief Return the intersection of the polygon with the line.
list<Segment2d> intersection(const PolygonalSurface2d &pg,const Line2d &r)
  {
    list<Segment2d> retval;
    if(pg.getNumVertices()>0)
      {
        Polyline2d pl= pg.getPolyline();
        GeomObj::list_Pos2d ptos= intersection(r,pl);
        if(!ptos.empty())
          {
            GeomObj::list_Pos2d ordenados= r.Ordena(ptos);

            Polyline2d tmp;
            tmp.assign(ordenados.begin(),ordenados.end());
            const size_t ns= tmp.getNumSegments();
            for(size_t i= 1;i<=ns;i++)
              {
                const Segment2d s= tmp.getSegment(i);
                if(pg.In(s.getCenterOfMass()))
                  retval.push_back(s);
              }
          }
      }
    return retval;     
  }

//! @brief Return the intersection of the polygon with the line.
list<Segment2d> intersection(const Line2d &r,const PolygonalSurface2d &pg)
  { return intersection(pg,r); }

//! @brief Return the intersection of the polygon with the ray.
list<Segment2d> intersection(const PolygonalSurface2d &pg,const Ray2d &sr)
  {
    
    list<Segment2d> retval;
    if(!pg.getNumVertices()) return retval;

    Polyline2d pl= pg.getPolyline();
    
    GeomObj::list_Pos2d ptos= intersection(sr,pl);
    const Pos2d org= sr.getFromPoint();
    if(pg.In(org))
      ptos.push_front(org);

    GeomObj::list_Pos2d ordenados= sr.Ordena(ptos);

    Polyline2d tmp;
    tmp.assign(ordenados.begin(),ordenados.end());
    const size_t ns= tmp.getNumSegments();
    for(size_t i= 1;i<=ns;i++)
      {
        const Segment2d s= tmp.getSegment(i);
        if(pg.In(s.getCenterOfMass()))
          retval.push_back(s);
      }
    return retval;
  }

//! @brief Return the intersection of the polygon with the ray.
list<Segment2d> intersection(const Ray2d &sr,const PolygonalSurface2d &pg)
  { return intersection(pg,sr); }

//! @brief Return the intersection of the polygon with the segment.
list<Segment2d> intersection(const PolygonalSurface2d &pg,const Segment2d &sg)
  {
    
    list<Segment2d> retval;
    if(!pg.getNumVertices()) return retval;

    Polyline2d pl= pg.getPolyline();
    
    GeomObj::list_Pos2d ptos= intersection(sg,pl);
    const Pos2d org= sg.getFromPoint();
    if(pg.In(org))
      ptos.push_front(org);
    const Pos2d dest= sg.getToPoint();
    if(pg.In(org))
      ptos.push_back(dest);
    if(ptos.size()<2)
      return retval;

    GeomObj::list_Pos2d ordenados= sg.Ordena(ptos);

    Polyline2d tmp;
    tmp.assign(ordenados.begin(),ordenados.end());
    const size_t ns= tmp.getNumSegments();
    for(size_t i= 1;i<=ns;i++)
      {
        const Segment2d s= tmp.getSegment(i);
        if(pg.In(s.getCenterOfMass()))
          retval.push_back(s);
      }
    return retval;
  }

//! @brief Return the intersection of the polygon with the segment.
list<Segment2d> intersection(const Segment2d &sg,const PolygonalSurface2d &pg)
  { return intersection(pg,sg); }
