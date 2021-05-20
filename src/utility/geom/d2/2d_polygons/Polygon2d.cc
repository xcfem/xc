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
//Polygon2d.cc

#include "Polygon2d.h"
#include "polygon2d_bool_op.h"
#include<CGAL/create_offset_polygons_2.h>
#include "utility/geom/trf/Trf2d.h"
#include "utility/geom/d1/Segment2d.h"
#include "utility/geom/d1/Polyline2d.h"
#include "utility/geom/d2/HalfPlane2d.h"
#include "utility/geom/d2/BND2d.h"

#include <CGAL/Boolean_set_operations_2.h>
#include <boost/any.hpp>
#include "mark_bayazit.h"

#include "utility/geom/center_of_mass.h"
#include "utility/geom/trf/Translation2d.h"
#include "utility/geom/lists/utils_list_pos2d.h"
#include "utility/geom/pos_vec/Pos2dList.h"

Pos2d at(const Polygon2d &p, int i)
  {
    const int s= int(p.GetNumVertices());
    return p.Vertice0(i < 0 ? s - 1 - ((-i - 1) % s) : i % s);
  }

//! @brief Default constructor.
Polygon2d::Polygon2d(void)
  : PolygonalSurface2d(), cgpol() {}

//! @brief Constructor.
Polygon2d::Polygon2d(const CGPolygon_2 &p)
  : PolygonalSurface2d(), cgpol(p) {}

//! @brief Constructor.
Polygon2d::Polygon2d(const GeomObj::list_Pos2d &lv)
  {
    for(list_Pos2d::const_iterator i= lv.begin(); i!=lv.end(); i++)
      push_back(*i);
  }

//! @brief Constructor.
Polygon2d::Polygon2d(const Polyline2d &p)
  {
    //XXX Falla si la linea se interseca a sí misma.
    for(Polyline2d::const_iterator i= p.begin(); i!=p.begin(); i++)
      push_back(*i);
  }

//! @brief Constructor (Python interface).
Polygon2d::Polygon2d(const boost::python::list &l)
  {
    const int sz= len(l);
    // copy the components
    for(int i=0; i<sz; i++)
      push_back(boost::python::extract<Pos2d>(l[i]));
  }

//! @brief Constructor.
Polygon2d::Polygon2d(const std::list<Polygon2d> &lp)
  {
    if(!lp.empty())
      {
        (*this)= *lp.begin();
        if(lp.size()>1)
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; list contains more than a polygon." << std::endl;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; empty list argument." << std::endl;
  }

//! @brief Constructor virtual.
GeomObj *Polygon2d::clon(void) const
  { return new Polygon2d(*this); }

//! @brief Return a polygon parallel to this one at the distance
//! being passed as parameter. The new polygon will be exterior
//! if the distance is positive.
Polygon2d Polygon2d::Offset(const GEOM_FT &d) const
  {
    Polygon2d retval;
    typedef boost::shared_ptr<CGPolygon_2> PolygonPtr;
    typedef std::vector<PolygonPtr> PolygonPtrVector;
    PolygonPtrVector offset_polygons;
    if(d<0)
      offset_polygons= CGAL::create_interior_skeleton_and_offset_polygons_2(-d,cgpol);
    else
      offset_polygons= CGAL::create_exterior_skeleton_and_offset_polygons_2(d,cgpol);
    if(!offset_polygons.empty())
      {
        if(offset_polygons.size()!=1)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; we get more than a polygon." << std::endl;
        retval.cgpol= *offset_polygons[0];
      }
    return retval;
  }

//! @brief Return true if the point is inside the polygon.
bool Polygon2d::In(const Pos2d &p, const double &tol) const
  {
    if(cgpol.has_on_boundary(p.ToCGAL())) return true;
    if(cgpol.has_on_bounded_side(p.ToCGAL())) return true;
    return false;
  }

//! @brief Return true if the polyline is inside the polygon.
bool Polygon2d::In(const Polyline2d &p) const
  { return In(p.begin(),p.end()); }

//! @brief Return true if this polygon contains the polygon argument.
bool Polygon2d::In(const Polygon2d &p) const
  { return In(p.vertices_begin(),p.vertices_end()); }

//! @brief Makes the polygon counter clockwise.
void Polygon2d::makeCounterClockWise(void)
  {
    if(clockwise())
      swap();
  }

//! @brief Return true if the polygon contains the point.
bool Polygon2d::Overlap(const Pos2d &p) const
  { return In(p); }


//! @brief Return true if the line and the polygon overlap.
bool Polygon2d::Overlap(const Line2d &r) const
  { return PolygonalSurface2d::Overlap(r); }

  
//! @brief Return true if the ray and the polygon overlap.
bool Polygon2d::Overlap(const Ray2d &sr) const
  { return PolygonalSurface2d::Overlap(sr); }

//! @brief Return true if the segment and the polygon overlap.
bool Polygon2d::Overlap(const Segment2d &sg) const
  {
    bool retval= false;
    if(In(sg.getFromPoint()))
      retval= true;
    else if(In(sg.getToPoint()))
      retval= true;
    else
      {
        GeomObj::list_Pos2d tmp= getPolyline().getIntersection(sg);
        retval= !tmp.empty();
      }
    return retval;
  }

//! @brief Return true if the boundary and the polygon overlap.
bool Polygon2d::Overlap(const BND2d &bnd) const
  { return bnd.Overlap(*this); }

//! @brief Return true if the polyline and the polygon overlap.
bool Polygon2d::Overlap(const Polyline2d &p) const
  {
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

//! @brief Return verdadero si el polígono se superpone
//! con el que se pasa como parámetro.
bool Polygon2d::Overlap(const Polygon2d &p) const
  { 
    bool retval= Overlap(p.vertices_begin(),p.vertices_end());
    if(!retval)
      retval= p.Overlap(vertices_begin(),vertices_end());
    else if(!retval)
      {
        const unsigned int nl= p.GetNumLados();
        for(unsigned int i= 1; i<=nl;i++)
          if(Overlap(p.Lado(i)))
            {
              retval= true;
               break;
            }
      }
    else if(!retval)
      {
        const unsigned int nl= GetNumLados();
        for(unsigned int i= 1; i<=nl;i++)
          if(p.Overlap(Lado(i)))
            {
              retval= true;
              break;
            }
      }
    return retval;
  }

//! @brief Return verdadero si el polígono se superpone
//! con alguno de los de la lista que se pasa como parámetro.
bool Polygon2d::Overlap(const std::list<Polygon2d> &l) const
  {
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

//! @brief Return the maximum value of the i-th coordinate.
GEOM_FT Polygon2d::GetMax(unsigned short int i) const
  {
    if (GetNumVertices() < 1) return 0.0;
    GEOM_FT retval;
    const size_t j= (i-1)%2+1;
    if(j==1)
      retval=(*cgpol.right_vertex()).x();
    else
      retval=(*cgpol.top_vertex()).y(); //j==2
    return retval;
  }

//! @brief Return the minimum value of the i-th coordinate.
GEOM_FT Polygon2d::GetMin(unsigned short int i) const
  {
    if (GetNumVertices() < 1) return 0.0;
    GEOM_FT retval;
    const size_t j= (i-1)%2+1;
    if(j==1)
      retval= (*cgpol.left_vertex()).x();
    else
      retval= (*cgpol.bottom_vertex()).y(); //j==2
    return retval;
  }

GeomObj::list_Pos2d Polygon2d::getVertexList(void) const
  {
    GeomObj::list_Pos2d lv;
    if(GetNumVertices() > 0)
      for(CGPolygon_2::Vertex_iterator j=cgpol.vertices_begin(); j != cgpol.vertices_end();j++)
        lv.push_back(Pos2d(*j));
    return lv;
  }

//! @brief Return a Python list containing the positions
//! of the polygon vertices.
boost::python::list Polygon2d::getVertexListPy(void) const
  {
    boost::python::list retval;
    GeomObj::list_Pos2d lst= getVertexList();
    GeomObj::list_Pos2d::const_iterator i= lst.begin();
    for(;i!=lst.end();i++)
      retval.append(*i);
    return retval;
  }

//! @brief Aplica a los vértices la transformación que se pasa como parámetro.
void Polygon2d::Transform(const Trf2d &trf2d)
  { trf2d.Transform(cgpol.vertices_begin(),cgpol.vertices_end()); }

//! @brief Return the polygons that result from clipping the polygon
//! with the line.
//! 
//! @param p: polygon to clip.
//! @param r: clipping line.
std::list<Polygon2d> corta(const Polygon2d &p,const Line2d &r)
  {
    std::list<Polygon2d> retval;
    if(!intersecan(p.Bnd(),r)) return retval;
    HalfPlane2d sp1(r);
    HalfPlane2d sp2= sp1.getSwap();
    retval= intersection(p,sp1);
    const std::list<Polygon2d> inter2= intersection(p,sp2);
    retval.insert(retval.end(),inter2.begin(),inter2.end());
    return retval;
  }

//! @brief Return the polygon area.
GEOM_FT Polygon2d::getArea(void) const
  { return ::Abs(AreaSigno()); }

//! @brief Return the polygons that form the tributary area of each vertex.
std::vector<Polygon2d> Polygon2d::getTributaryPolygons(void) const
  {
    const size_t nv= GetNumVertices();
    const Pos2d center_of_mass=getCenterOfMass();
    const Polygon2d pMed= append_mid_points(*this);
    const size_t nvPMed= pMed.GetNumVertices();
    std::vector<Polygon2d> retval(nv);
    for(size_t i=0;i<nv;i++)
      {
        const size_t j1= 2*i;
        const Pos2d v1= pMed.Vertice0(j1);
        size_t j2= j1+1;
        if(j2>=nvPMed) j2= 0;
        const Pos2d v2= pMed.Vertice0(j2);
        size_t j3= nvPMed-1;
        if(j1>=1) j3= j1-1;
        const Pos2d v3= pMed.Vertice0(j3);
        Polygon2d tmp;
        tmp.push_back(v1);
        tmp.push_back(v2);
        tmp.push_back(center_of_mass);
        tmp.push_back(v3);
        retval[i]= tmp;
      }
    return retval;
  }

//! @brief Return the areas of the tributary polygons (one for each vertex).
std::vector<double> Polygon2d::getTributaryAreas(void) const
  {
    const size_t nv= GetNumVertices();
    std::vector<double> retval(nv,0.0);
    std::vector<Polygon2d> plgs= getTributaryPolygons();
    for(size_t i=0;i<nv;i++)
      retval[i]= plgs[i].getArea();
    return retval;
  }

//! @brief Return the cover of the positions in the argument.
std::deque<GEOM_FT> &Polygon2d::getCovers(const Pos2dList &l) const
  { return l.getCovers(*this); }

//! @brief Return a polygon with a vertex on the mid-point of each side of
//! the argument (used in GeomSection::getAnchoMecanico).
Polygon2d append_mid_points(const Polygon2d &plg)
  {
    Polygon2d retval;
    const size_t num_vertices= plg.GetNumVertices();
    if(num_vertices>1)
      {
        Pos2d p1= plg.Vertice(1);
        retval.push_back(p1);
        Pos2d pmed,p2;
        for(size_t i=2;i<=num_vertices;i++)
          {
            p2= plg.Vertice(i);
            pmed= Segment2d(p1,p2).getCenterOfMass();
            retval.push_back(pmed);
            retval.push_back(p2);
            p1= p2;
          }
        p2= plg.Vertice(1);
        retval.push_back(Segment2d(p1,p2).getCenterOfMass());
      }
    return retval;
  }

//! @brief Return the intersection of the polygon with the line.
Segment2d Polygon2d::Clip(const Line2d &r) const
  { return PolygonalSurface2d::Clip(r); }

//! @brief Return the intersection of the polygon and the ray.
Segment2d Polygon2d::Clip(const Ray2d &sr) const
  { return PolygonalSurface2d::Clip(sr); }

//! @brief Return the intersection of the polygon and the segment.
Segment2d Polygon2d::Clip(const Segment2d &sg) const
  { return PolygonalSurface2d::Clip(sg); }

//! @brief Return the polygons that result from clipping this one
//! with the BND argument.
std::list<Polygon2d> Polygon2d::Clip(const BND2d &bnd) const
  { return Clip(bnd.getPolygon()); }

//! @brief Return the polygons that result from clipping this one
//! with the polygon argument.
std::list<Polygon2d> Polygon2d::Clip(const Polygon2d &other) const
  { return intersection(*this,other); }

//! @brief Clip this polygont with the polygon argument.
void Polygon2d::clipBy(const Polygon2d &plg)
  { (*this)= Polygon2d(Clip(plg)); }

//! @brief 
std::list<Polygon2d> Polygon2d::getBayazitDecomposition(void) const
  {
    std::list<Polygon2d> retval;
    decompose_poly(*this,retval);
    return retval;
  }

//! @brief Return the intersection with the polygon parameter.
std::list<Polygon2d> Polygon2d::getIntersection(const HalfPlane2d &sp) const
  { return intersection(*this,sp); }

//! @brief Return the union of this polygon with the argument.
Polygon2d Polygon2d::getUnion(const Polygon2d &other) const
  {
    Polygon2d retval;
    std::list<Polygon2d> polUnion= join(*this,other);
    if(!polUnion.empty())
      retval= Polygon2d(polUnion);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; unknown error." << std::endl;
    return retval;
  }

void Polygon2d::une(const Polygon2d &other)
  { (*this)= getUnion(other); }

void Polygon2d::une(const std::list<Polygon2d> &l)
  {
    const std::string str_error= "; error en unión de polígono con lista.";
    if(!l.empty())
      {
        const std::list<Polygon2d> tmp= join(l,*this);
        if(tmp.empty())
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << str_error << " Union is empty." << std::endl;
        else
          (*this)= Polygon2d(tmp);
      }
  }

// //! @brief Return la lista de polígonos que resulta de interpretar la text string que se pasa como parámetro.
// std::list<Polygon2d> Polygon2d::create_polygon2d_list(const std::string &str) const
//   {
//     check_comillas(str);
//     const std::deque<boost::any> tmp= crea_deque_boost_any(str);
//     const size_t sz= tmp.size();
//     std::list<Polygon2d> retval;
//     for(size_t i= 0;i<sz;i++)
//       retval.push_back(convert_to_polygon2d(tmp[i]));
//     return retval;
//   }

Pos2d center_of_mass(const std::list<Polygon2d> &l)
  { return center_of_mass(l.begin(),l.end()); }


