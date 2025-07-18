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
//Polygon2d.cc

#include "Polygon2d.h"
#include "polygon2d_bool_op.h"
#include <CGAL/create_offset_polygons_2.h>
#include "utility/geom/trf/Trf2d.h"
#include "utility/geom/d1/Segment2d.h"
#include "utility/geom/d1/Polyline2d.h"
#include "utility/geom/d2/HalfPlane2d.h"
#include "utility/geom/d2/BND2d.h"

#include <CGAL/Boolean_set_operations_2.h>
#include "mark_bayazit.h"

#include "utility/geom/center_of_mass.h"
#include "utility/geom/trf/Translation2d.h"
#include "utility/geom/lists/utils_list_pos2d.h"
#include "utility/geom/pos_vec/Pos2dList.h"

// Boost geometry.
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

#include "utility/utils/misc_utils/colormod.h"

Pos2d at(const Polygon2d &p, int i)
  {
    const int s= int(p.getNumVertices());
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
    //XXX Fails if the line self-intersects.
    for(Polyline2d::const_iterator i= p.begin(); i!=p.end(); i++)
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
	  std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; list contains more than a polygon."
		    << Color::def << std::endl;
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; empty list argument."
		<< Color::def << std::endl;
  }

//! @brief Constructor virtual.
GeomObj *Polygon2d::getCopy(void) const
  { return new Polygon2d(*this); }

//! @brief Return a polygon parallel to this one at the given distance.
//! The new polygon will be exterior if the distance is positive.
Polygon2d Polygon2d::offset(const GEOM_FT &d) const
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
	  {
	    // Try using a 2D polyline.	    
	    Polyline2d tmp(*this);
	    retval= Polygon2d(tmp.offset(d));
            // std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	    // 	    << "; we get more than a polygon."
	    //      << Color::def << std::endl;
            // retval.cgpol= *offset_polygons[0];
	  }
	else
	  retval.cgpol= *offset_polygons[0];
      }
    return retval;
  }

//! @brief Returns the buffer (a polygon being the spatial point set
//! collection within a specified maximum distance from this polygon) of this
//! polygon. 
Polygon2d Polygon2d::buffer(const GEOM_FT &buffer_distance) const
  {
    typedef boost::geometry::model::d2::point_xy<GEOM_FT> point_xy;
    typedef boost::geometry::model::polygon<point_xy> polygon_type;

    const GeomObj::list_Pos2d vertexList= this->getVertexList();
    // Store boost points in vector.
    std::vector< point_xy > pointList;
    GeomObj::list_Pos2d::const_iterator firstPointIter= vertexList.begin();
    for(GeomObj::list_Pos2d::const_iterator i= firstPointIter; i!= vertexList.end(); i++)
      {
	const Pos2d &p= (*i);
	pointList.push_back(point_xy(p.x(),p.y()));
      }
    // Close the polygon.
    const Pos2d &firstPoint= *firstPointIter;
    pointList.push_back(point_xy(firstPoint.x(), firstPoint.y()));
    bool clockwise= this->clockwise();
    if(!clockwise)
      std::reverse(pointList.begin(), pointList.end());
    // create Boost polygon
    polygon_type poly;
    // assign points to Boost polygon
    boost::geometry::assign_points(poly, pointList);
    
    // Declare output
    boost::geometry::model::multi_polygon<polygon_type> result;
    // Declare strategies
    const int points_per_circle = 36;
    boost::geometry::strategy::buffer::distance_symmetric<GEOM_FT> distance_strategy(buffer_distance);
    boost::geometry::strategy::buffer::join_round join_strategy(points_per_circle);
    boost::geometry::strategy::buffer::end_round end_strategy(points_per_circle);
    boost::geometry::strategy::buffer::point_circle circle_strategy(points_per_circle);
    boost::geometry::strategy::buffer::side_straight side_strategy;
    // Create the buffer of a polygon
    boost::geometry::buffer(poly, result,
                distance_strategy, side_strategy,
                join_strategy, end_strategy, circle_strategy);
    // Extract results.
    Polygon2d retval;
    const size_t numPolygons= result.size();
    if(numPolygons>0)
      {
	if(numPolygons>1)
	  std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; we get more than a polygon. Only the first one will be returned."
		    << Color::def << std::endl;
	const polygon_type &polygon_result= result[0];
    
	//getting the vertices back (except the last one because it's repeated).
	for(auto it = boost::begin(boost::geometry::exterior_ring(polygon_result)); it != std::prev(boost::end(boost::geometry::exterior_ring(polygon_result))); ++it)
	  {
	    const double &x= boost::geometry::get<0>(*it);
	    const double &y= boost::geometry::get<1>(*it);
	    retval.push_back(Pos2d(x,y));
	  }
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; no buffer were computed. Something went wrong."
		<< Color::def << std::endl;
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
        const unsigned int nl= p.getNumEdges();
        for(unsigned int i= 1; i<=nl;i++)
          if(Overlap(p.Lado(i)))
            {
              retval= true;
               break;
            }
      }
    else if(!retval)
      {
        const unsigned int nl= getNumEdges();
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
    if (getNumVertices() < 1) return 0.0;
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
    if (getNumVertices() < 1) return 0.0;
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
    if(getNumVertices() > 0)
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
    const size_t nv= getNumVertices();
    const Pos2d center_of_mass=getCenterOfMass();
    const Polygon2d pMed= append_mid_points(*this);
    const size_t nvPMed= pMed.getNumVertices();
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
    const size_t nv= getNumVertices();
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
//! the argument (used in SectionGeometry::getAnchoMecanico).
Polygon2d append_mid_points(const Polygon2d &plg)
  {
    Polygon2d retval;
    const size_t num_vertices= plg.getNumVertices();
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
//! with the BND argument in a Python list.
boost::python::list Polygon2d::ClipPy(const BND2d &bnd) const
  {
    const std::list<Polygon2d> tmp= this->Clip(bnd);
    boost::python::list retval;
    for(std::list<Polygon2d>::const_iterator i= tmp.begin(); i!= tmp.end(); i++)
      retval.append(*i);
    return retval;
  }

//! @brief Return the polygons that result from clipping this one
//! with the BND argument.
std::list<Polygon2d> Polygon2d::Clip(const HalfPlane2d &hp) const
  { return Polygon2d::getIntersection(hp); }

//! @brief Return the polygons that result from clipping this one
//! with the BND argument in a Python list.
boost::python::list Polygon2d::ClipPy(const HalfPlane2d &hp) const
  { return Polygon2d::getIntersectionPy(hp); }


//! @brief Return the polygons that result from clipping this one
//! with the polygon argument.
std::list<Polygon2d> Polygon2d::Clip(const Polygon2d &other) const
  { return intersection(*this,other); }

//! @brief Return the polygons that result from clipping this one
//! with the polygon argument in a Python list.
boost::python::list Polygon2d::ClipPy(const Polygon2d &plg) const
  {
    const std::list<Polygon2d> tmp= this->Clip(plg);
    boost::python::list retval;
    for(std::list<Polygon2d>::const_iterator i= tmp.begin(); i!= tmp.end(); i++)
      retval.append(*i);
    return retval;
  }

//! @brief Clip this polygont with the polygon argument.
void Polygon2d::clipBy(const Polygon2d &plg)
  { (*this)= Polygon2d(Clip(plg)); }

//! @brief Return the intersection of the polygon with the given line.
Segment2d Polygon2d::getIntersection(const Line2d &l) const
  { return Clip(l); }

//! @brief Return the intersection of the polygon with the given ray.
Segment2d Polygon2d::getIntersection(const Ray2d &r) const
  { return Clip(r); }

//! @brief Return the intersection of the polygon with the given segment.
Segment2d Polygon2d::getIntersection(const Segment2d &s) const
  { return Clip(s); }

//! @brief Return the intersection with the given half plane.
std::list<Polygon2d> Polygon2d::getIntersection(const HalfPlane2d &hp) const
  { return intersection(*this, hp); }

//! @brief Return the intersection with the given half plane in a Python list.
boost::python::list Polygon2d::getIntersectionPy(const HalfPlane2d &hp) const
  {
    const std::list<Polygon2d> tmp= intersection(*this, hp);
    boost::python::list retval;
    for(std::list<Polygon2d>::const_iterator i= tmp.begin(); i!= tmp.end(); i++)
      retval.append(*i);
    return retval;
  }

//! @brief Return the intersection with the given polygon.
std::list<Polygon2d> Polygon2d::getIntersection(const Polygon2d &plg) const
  { return intersection(*this, plg); }

//! @brief Return the intersection with the given polygon in a Python list.
boost::python::list Polygon2d::getIntersectionPy(const Polygon2d &plg) const
  {
    const std::list<Polygon2d> tmp= intersection(*this, plg);
    boost::python::list retval;
    for(std::list<Polygon2d>::const_iterator i= tmp.begin(); i!= tmp.end(); i++)
      retval.append(*i);
    return retval;
  }

//! @brief Return the decomposition obtained using the Mark Bayazit algorithm.
std::list<Polygon2d> Polygon2d::getBayazitDecomposition(void) const
  {
    std::list<Polygon2d> retval;
    decompose_poly(*this,retval);
    return retval;
  }

//! @brief Return the union of this polygon with the argument.
Polygon2d Polygon2d::getUnion(const Polygon2d &other) const
  {
    Polygon2d retval;
    std::list<Polygon2d> polUnion= join(*this,other);
    if(!polUnion.empty())
      retval= Polygon2d(polUnion);
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; unknown error."
		<< Color::def << std::endl;
    return retval;
  }

//! @brief Join the given polygon to this one.
void Polygon2d::une(const Polygon2d &other)
  { (*this)= getUnion(other); }

void Polygon2d::une(const std::list<Polygon2d> &l)
  {
    const std::string str_error= "; error en unión de polígono con lista.";
    if(!l.empty())
      {
        const std::list<Polygon2d> tmp= join(l,*this);
        if(tmp.empty())
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << str_error << " Union is empty."
		    << Color::def << std::endl;
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


//! @brief Remove the consecutive vertices that are at distance less than than tol.
Polygon2d remove_duplicated_vertices(const Polygon2d &p, const GEOM_FT &tol)
  {
    Polygon2d retval;
    Polyline2d tmp(p.getVertexList());
    tmp= remove_duplicated_vertices(tmp, tol);
    // Compute tolerance.
    GEOM_FT local_tol= tol;
    const size_t sz= tmp.getNumVertices();
    if(local_tol<0.0)
      {
	if(sz>1)
	  {
	    // Compute average segment length.
	    const GEOM_FT avgLength= p.getLength()/GEOM_FT(sz-1);
	    local_tol= avgLength/1e4;
	  }
      }
    if(sz>1)
      {
	// Check endpoints.
	const Pos2d &first= tmp.Vertice0(0);
	const Pos2d &last= tmp.Vertice0(sz-1);
	const GEOM_FT d= first.dist(last);
	if(d<local_tol)
	  {
	    const Segment2d s(first, last);
	    const Pos2d newP= s.getMidPoint();
	    retval.push_back(newP);
	    for(size_t i= 1;i<(sz-1);i++)
	      retval.push_back(tmp.Vertice0(i));
	  }
	else
	  retval= Polygon2d(tmp);
      }
    else
      retval= Polygon2d(tmp);
    return retval;
  }
