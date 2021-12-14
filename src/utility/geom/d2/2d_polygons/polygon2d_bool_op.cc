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
//polygon2d_bool_op.cc
//Operaciones booleanas con polígonos.

#include "polygon2d_bool_op.h"
#include "Polygon2d.h"
#include "../../d1/Segment2d.h"
#include "../../d1/Polyline2d.h"
#include "utility/geom/d2/HalfPlane2d.h"
#include <CGAL/Gmpz.h>
#include <CGAL/Gmpq.h>
#include <CGAL/Filtered_extended_homogeneous.h>
#include <CGAL/Nef_polyhedron_2.h>
#include <list>

//The Nef_Polyhedron types
typedef CGAL::Gmpz RT;
typedef CGAL::Filtered_extended_homogeneous<RT> Extended_kernel;
typedef CGAL::Nef_polyhedron_2<Extended_kernel> Nef_polyhedron;
typedef Nef_polyhedron::Point Point;
typedef Nef_polyhedron::Line Line;

typedef Nef_polyhedron::Explorer  PMCDec;
typedef PMCDec::Face_const_iterator Face_const_iterator;
typedef PMCDec::Hole_const_iterator Hole_const_iterator;
typedef PMCDec::Halfedge_around_face_const_circulator Halfedge_around_face_const_circulator;

const double escala= 1e5;
const double areaMin= 1/(escala*escala);

inline double double2intdouble(const double &d)
  { return sgn(d)*floor(std::abs(d)*escala); }

Point convert_point(const CGPoint_2 &p)
  { return Point(double2intdouble(p.hx()),double2intdouble(p.hy()),double2intdouble(p.hw())); }

Polygon2d return_polygon(const Polygon2d &p)
  {
    Polygon2d retval;
    const size_t num_vertices= p.getNumVertices();
    for(size_t i=1;i<=num_vertices;i++)
      {
        const Pos2d &pto= p.Vertice(i);
        retval.push_back(Pos2d(pto.x()/escala,pto.y()/escala));
      }
    return retval;
  }

Nef_polyhedron Polygon2d_to_Nef_2(const Polygon2d &poly)
  {
    Polygon2d::vertex_iterator it = poly.vertices_begin();
    std::list<Point> l_of_p;        
    while(it != poly.vertices_end())
      {
        l_of_p.push_back(convert_point(*it));
        it++;
      }
    return Nef_polyhedron(l_of_p.begin(),l_of_p.end(),Nef_polyhedron::INCLUDED);
  }

Nef_polyhedron HalfPlane2d_to_Nef_2(const HalfPlane2d &sp)
  {
    Nef_polyhedron N(Line(double2intdouble(sp.a()),double2intdouble(sp.b()),double2intdouble(sp.c())),Nef_polyhedron::EXCLUDED);
    return N.complement();
  }

bool es_estandar(const Halfedge_around_face_const_circulator &c,const PMCDec &explo)
  {
    Halfedge_around_face_const_circulator i(c);
    const Halfedge_around_face_const_circulator c_end(c);

    bool retval=  explo.is_standard(i->vertex());
    i++;
    for(;i!=c_end;i++)
      retval&= explo.is_standard(i->vertex());
    return retval;
  }

//! @brief Workaraound to avoid the segmentation fault in
//! CGAL::SPolynomial<CGAL::Gmpz>::eval_at_R() due to
//! CGAL_STATIC_THREAD_LOCAL_VARIABLE (see Filtered_extended_homogeneous.h)
inline CGAL::Gmpz eval_at_R(const CGAL::SPolynomial<CGAL::Gmpz> &a)
 {
   if(a.m()!=0)
     std::cerr << __FUNCTION__
               << "; workaround failed." << std::endl;
   return a.n();
 }

inline double to_dbl(const CGAL::SQuotient<CGAL::Gmpz> &q)
  {
    auto eval_num= eval_at_R(q.numerator());
    return (CGAL::to_double(eval_num)/
          CGAL::to_double(q.denominator()));
  }

Polygon2d face_cycle(const Halfedge_around_face_const_circulator &c)
  {
    Polygon2d retval;
    Halfedge_around_face_const_circulator i(c);
    const Halfedge_around_face_const_circulator c_end(c);
    const auto pt= i->vertex()->point();
    retval.push_back(Pos2d(to_dbl(pt.x()),to_dbl(pt.y())));
    i++;
    for(;i!=c_end;i++)
      retval.push_back(Pos2d(to_dbl(i->vertex()->point().x()),
 	                     to_dbl(i->vertex()->point().y())));
    return retval;
  }

std::list<Polygon2d> Nef_2_to_Polygon2d(const Nef_polyhedron &n)
  {
    std::list<Polygon2d> retval;
    PMCDec D = n.explorer(); 

    Face_const_iterator fit = D.faces_begin();
    for (++fit; fit != D.faces_end(); ++fit)
      {
        Halfedge_around_face_const_circulator hfc(fit->halfedge());//Face edge.
        if(fit->mark())
          {
            if(es_estandar(hfc,D))
              retval.push_back(Polygon2d(face_cycle(hfc)));
            Hole_const_iterator hit;
            for (hit = fit->fc_begin(); hit != fit->fc_end(); ++hit)
              {
                hfc = Halfedge_around_face_const_circulator(hit);
                if(es_estandar(hfc,D))
                  retval.push_back(return_polygon(Polygon2d(face_cycle(hfc))));
              }
          }
      }
    return retval;
  }

Nef_polyhedron une(const Polygon2d &p1,const Polygon2d &p2)
  {
    Nef_polyhedron retval;
    Nef_polyhedron n1=Polygon2d_to_Nef_2(p1);
    Nef_polyhedron n2=Polygon2d_to_Nef_2(p2);
    if(p1.empty())
      retval= n2;
    else if(p2.empty())
      retval= n1;
    else
      retval= n1.join(n2);
    return retval;
  }

Nef_polyhedron une(const Nef_polyhedron &n1,const Polygon2d &p2)
  {
    if(p2.empty())
      return n1;
    else
      {
        Nef_polyhedron n2=Polygon2d_to_Nef_2(p2);
        return n1.join(n2);
      }
  }

std::list<Polygon2d> join(const Polygon2d &p1,const Polygon2d &p2)
  {
    std::list<Polygon2d> retval;
    if(p1.empty())
      if(!p2.empty())
        retval.push_back(p2);
    if(p2.empty())
      if(!p1.empty())
        retval.push_back(p1);
    if(!p1.empty() && !p2.empty())
      retval= Nef_2_to_Polygon2d(une(p1,p2));
    return retval;
  }

//! @brief Return la unión de los polígonos de la lista.
std::list<Polygon2d> join(const std::list<Polygon2d> &l)
  {
    std::list<Polygon2d> retval;
    if(!l.empty())
      {
        std::list<Polygon2d>::const_iterator i= l.begin();
        if(l.size()>1)
          {
            Nef_polyhedron nfRetval= Polygon2d_to_Nef_2(*i);
            i++;
            for(;i!=l.end();i++)
              if(!(i->empty()))
                nfRetval= une(nfRetval,*i);
            retval= Nef_2_to_Polygon2d(nfRetval);
          }
        else
          retval.push_back(*i);
      }
    return retval;
  }

//! @brief Return la unión de los polígonos de la lista con el que se pasa
//! como parámetro.
std::list<Polygon2d> join(const std::list<Polygon2d> &l,const Polygon2d &p)
  {
    std::list<Polygon2d> retval;
    if(p.empty())
      retval= join(l);
    else
      {
        if(!l.empty())
          {
            retval= l;
            retval.push_back(p);
            retval= join(retval);
          }
        else
          retval.push_back(p);
      }
    return retval;
  }

//! @brief Return verdadero si el polígono pl1 se superpone
//! el p2.
bool overlap(const Polygon2d &p1,const Polygon2d &p2)
  { return p1.Overlap(p2); }

//! @brief Return verdadero si alguno de los polígonos de l1 se superpone
//! con alguno de los de l2.
bool overlap(const std::list<Polygon2d> &l1,const std::list<Polygon2d> &l2)
  {
    bool retval= false;
    if(!l1.empty() && !l2.empty())
      {
        for(std::list<Polygon2d>::const_iterator i= l1.begin();i!=l1.end();i++)
          {
            const Polygon2d &p1= *i;
            for(std::list<Polygon2d>::const_iterator j= l2.begin();j!=l2.end();j++)
              if(p1.Overlap(*j))
                {
                  retval= true;
                  break;
                }
            if(retval)
              break;
          }
      }
    return retval;
  }

Nef_polyhedron interseca(const Polygon2d &p1,const Nef_polyhedron &n2)
  {
    Nef_polyhedron n1=Polygon2d_to_Nef_2(p1);
    return n1.intersection(n2);
  }
Nef_polyhedron interseca(const Polygon2d &p1,const Polygon2d &p2)
  {
    Nef_polyhedron n1=Polygon2d_to_Nef_2(p1);
    Nef_polyhedron n2=Polygon2d_to_Nef_2(p2);
    return n1.intersection(n2);
  }

//! @brief Return the intersection of the polygon with the half-plane.
// When the direction vector (VDir()) of the line has a very big modulus,
// , the function crashes when computing the intersection. The problem was
// fixed defining the line with points that were nearer between them
// (100 units apart).
Nef_polyhedron interseca(const Polygon2d &p,const HalfPlane2d &sp)
  {
    Nef_polyhedron n2=HalfPlane2d_to_Nef_2(sp);
    Nef_polyhedron n1=Polygon2d_to_Nef_2(p);
    Nef_polyhedron retval= n1.intersection(n2);
    return retval;
  }

//! @brief Return the polygons that result from clipping the polygons on the
//! list with the polygon argument.
std::list<Polygon2d> clip(const std::list<Polygon2d> &l,const Polygon2d &p)
  {
    std::list<Polygon2d> retval;
    if(!p.empty() && (p.getArea()>areaMin))
      {
        Nef_polyhedron tmp=Polygon2d_to_Nef_2(p);
        if(!l.empty())
          {
            std::list<Polygon2d> tmpLst;
            for(std::list<Polygon2d>::const_iterator i= l.begin();i!=l.end();i++)
              {
                if((*i).getArea()>areaMin)
                  {
                    tmpLst= Nef_2_to_Polygon2d(interseca(*i,tmp));
                    if(!tmpLst.empty())
                      retval.insert(retval.end(),tmpLst.begin(),tmpLst.end());
                  }
              }
          }
      }
    return retval;
  }

//! @brief Return the intersection de los dos polígonos.
std::list<Polygon2d> intersection(const Polygon2d &p1,const Polygon2d &p2)
  { return Nef_2_to_Polygon2d(interseca(p1,p2)); }

//! @brief Return la intersection of the polygon and the half plane.
std::list<Polygon2d> intersection(const Polygon2d &p,const HalfPlane2d &sp)
  { return Nef_2_to_Polygon2d(interseca(p,sp)); }

//! @brief Return the intersection of the polygons in the list with the
//! half plane.
std::list<Polygon2d> intersection(const std::list<Polygon2d> &l,const HalfPlane2d &sp)
  {
    std::list<Polygon2d> retval;
    if(!l.empty())
      {
        std::list<Polygon2d> tmpLst;
        for(std::list<Polygon2d>::const_iterator i= l.begin();i!=l.end();i++)
          {
            tmpLst= intersection(*i,sp);
            if(!tmpLst.empty())
              retval.insert(retval.end(),tmpLst.begin(),tmpLst.end());
          }
      }
    return retval;
  }


//! @brief Return los polígonos que resultan de intersecar los de la lista
//! con el que se pasa como parámetro.
std::list<Polygon2d> intersection(const std::list<Polygon2d> &l,const Polygon2d &p)
  {
    const std::list<Polygon2d> retval= clip(l,p);
    return join(retval);
  }

//! @brief Return los polígonos que resultan de intersecar los de la lista
//! l1 con cada uno de los de la lista l2.
std::list<Polygon2d> intersection(const std::list<Polygon2d> &l1,const std::list<Polygon2d> &l2)
  {
    std::list<Polygon2d> retval;
    if(!l2.empty())
      {
        std::list<Polygon2d> tmpLst;
        for(std::list<Polygon2d>::const_iterator i= l2.begin();i!=l2.end();i++)
          {
            tmpLst= intersection(l1,*i);
            if(!tmpLst.empty())
              retval.insert(retval.end(),tmpLst.begin(),tmpLst.end());
          }
      }
    return join(retval);
  }

//! @brief Return the partition of the common area of both polygons
//! using the Voronoi algorithm.
void particiona(const Pos2d &c1,Polygon2d &p1,const Pos2d &c2,Polygon2d &p2)
  {
    if(overlap(p1,p2))
      {
        const Line2d m= perpendicular_bisector(c1,c2);
        const HalfPlane2d sp1(m,c1);
        const HalfPlane2d sp2(m,c2);
        p1= Polygon2d(intersection(p1,sp1));
        p2= Polygon2d(intersection(p2,sp2));
      }
  }

//! @brief Return the partition of the common area of both polygons
//! using the Voronoi algorithm.
void particiona(Polygon2d &p1,Polygon2d &p2)
  {
    if(overlap(p1,p2))
      {
        const Pos2d c1= p1.getCenterOfMass();
        const Pos2d c2= p2.getCenterOfMass();
        particiona(c1,p1,c2,p2);
      }
  }

//! @brief Return the partition of the common area of both polygons lists
//! using the Voronoi algorithm.
void particiona(const Pos2d &c1,std::list<Polygon2d> &lp1,const Pos2d &c2,std::list<Polygon2d> &lp2)
  {
    if(overlap(lp1,lp2))
      {
        const Line2d m= perpendicular_bisector(c1,c2);
        const HalfPlane2d sp1(m,c1);
        const HalfPlane2d sp2(m,c2);
        lp1= intersection(lp1,sp1);
        lp2= intersection(lp2,sp2);
      }
  }


//! @brief Return the partition using the Voronoi algorithm.
void particiona(const std::list<Pos2d> &centros,std::list<Polygon2d> &areas)
  {
    const size_t sz= areas.size();
    if(sz>1)
      {
        assert(centros.size()==sz);
        std::list<Pos2d>::const_iterator cI= centros.begin();
        for(std::list<Polygon2d>::iterator i= areas.begin();i!=areas.end();i++,cI++)
          {
	    Polygon2d &pI= *i;
            std::list<Pos2d>::const_iterator cJ= cI; cJ++;
            std::list<Polygon2d>::iterator j= i; j++;
            for(;j!=areas.end();j++,cJ++)
              {
	        Polygon2d &pJ= *j;
                if(overlap(pI,pJ))
                  particiona(*cI,pI,*cJ,pJ);
              }
          }
      }
  }


//! @brief Return the partition using the Voronoi algorithm.
void particiona(std::list<Polygon2d> &areas)
  {
    if(areas.size()>1)
      {
        std::list<Pos2d> centros;
        for(std::list<Polygon2d>::iterator i= areas.begin();i!=areas.end();i++)
          centros.push_back((*i).getCenterOfMass());
        particiona(centros,areas);
      }
  }
