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
//PlanePolyline3d.cc

#include "PlanePolyline3d.h"
#include "utility/geom/ref_sys/Ref2d3d.h"
#include "utility/geom/d1/Line2d.h"
#include "utility/geom/d1/Segment2d.h"
#include "utility/geom/d1/Segment3d.h"
#include "utility/geom/d1/Line3d.h"
#include "utility/geom/d3/GeomGroup3d.h"
#include "utility/geom/d3/HalfSpace3d.h"
#include "utility/geom/lists/auxiliary.h"
#include "utility/geom/pos_vec/VectorPos2d.h"
#include "utility/geom/pos_vec/VectorPos3d.h"
#include "utility/kernel/python_utils.h"


//! @brief Constructor.
PlanePolyline3d::PlanePolyline3d(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3)
  : D2to3d(p1,p2,p3), pline2d()
  {
    push_back(p1);
    push_back(p2);
    push_back(p3);
  }

//! @brief Constructor.
PlanePolyline3d::PlanePolyline3d(const Ref2d3d &rf,const Polyline2d &p)
  : D2to3d(rf), pline2d(p) {}

//! @brief Constructor.
PlanePolyline3d::PlanePolyline3d(const Ref3d3d &rf,const Polyline2d &p)
  : D2to3d(rf), pline2d(p) {}

//! @brief Constructor.
PlanePolyline3d::PlanePolyline3d(const GeomObj::list_Pos3d &vertices)
  : D2to3d(), pline2d()
  {
    PlanePolyline3d tmp(vertices.begin(), vertices.end());
    (*this)= tmp;
  }

//! @brief Constructor.
PlanePolyline3d::PlanePolyline3d(const boost::python::list &l)
  : D2to3d(), pline2d()
  {
    GeomObj::list_Pos3d vertices;
    const size_t sz= len(l);
    for(size_t i=0; i<sz; i++)
      vertices.push_back(boost::python::extract<Pos3d>(l[i]));

    (*this)= PlanePolyline3d(vertices);
  }

//! @brief Return a Python list containing the positions
//! of the polyline vertices.
GeomObj::list_Pos3d PlanePolyline3d::getVertexList(void) const
  {
    GeomObj::list_Pos3d retval;
    Polyline2d::const_iterator i= pline2d.vertices_begin();
    for(;i!=pline2d.vertices_end();i++)
      retval.push_back(to_3d(Pos2d(*i)));
    return retval;
  }

//! @brief Return a Python list containing the positions
//! of the polyline vertices.
boost::python::list PlanePolyline3d::getVertexListPy(void) const
  {
    boost::python::list retval;
    GeomObj::list_Pos3d lst= getVertexList();
    GeomObj::list_Pos3d::const_iterator i= lst.begin();
    for(;i!=lst.end();i++)
      retval.append(*i);
    return retval;
  }

//! @brief Return true if there are no vertices.
bool PlanePolyline3d::empty(void) const
  { return (getNumVertices()==0); }

GEOM_FT PlanePolyline3d::GetXMax(void) const
  {
    GEOM_FT retval(NAN);
    if(!empty())
      {
        Polyline2d::const_iterator i= pline2d.vertices_begin();
	retval= to_3d(*i).x();
	i++;
	for(;i!=pline2d.vertices_end();i++)
	  {
	    GEOM_FT x= to_3d(*i).x();
	    if(x>retval)
	      retval= x;
	  }
      }
    return retval;
  }

GEOM_FT PlanePolyline3d::GetYMax(void) const
  {
    GEOM_FT retval(NAN);
    if(!empty())
      {
        Polyline2d::const_iterator i= pline2d.vertices_begin();
	retval= to_3d(*i).y();
	i++;
	for(;i!=pline2d.vertices_end();i++)
	  {
	    GEOM_FT y= to_3d(*i).x();
	    if(y>retval)
	      retval= y;
	  }
      }
    return retval;
  }

GEOM_FT PlanePolyline3d::GetZMax(void) const
  {
    GEOM_FT retval(NAN);
    if(!empty())
      {
        Polyline2d::const_iterator i= pline2d.vertices_begin();
	retval= to_3d(*i).z();
	i++;
	for(;i!=pline2d.vertices_end();i++)
	  {
	    GEOM_FT z= to_3d(*i).x();
	    if(z>retval)
	      retval= z;
	  }
      }
    return retval;
  }

GEOM_FT PlanePolyline3d::GetXMin(void) const
  {
    GEOM_FT retval(NAN);
    if(!empty())
      {
        Polyline2d::const_iterator i= pline2d.vertices_begin();
	retval= to_3d(*i).x();
	i++;
	for(;i!=pline2d.vertices_end();i++)
	  {
	    GEOM_FT x= to_3d(*i).x();
	    if(x<retval)
	      retval= x;
	  }
      }
    return retval;
  }

GEOM_FT PlanePolyline3d::GetYMin(void) const
  {
    GEOM_FT retval(NAN);
    if(!empty())
      {
        Polyline2d::const_iterator i= pline2d.vertices_begin();
	retval= to_3d(*i).y();
	i++;
	for(;i!=pline2d.vertices_end();i++)
	  {
	    GEOM_FT y= to_3d(*i).x();
	    if(y<retval)
	      retval= y;
	  }
      }
    return retval;
  }

GEOM_FT PlanePolyline3d::GetZMin(void) const
  {
    GEOM_FT retval(NAN);
    if(!empty())
      {
        Polyline2d::const_iterator i= pline2d.vertices_begin();
	retval= to_3d(*i).z();
	i++;
	for(;i!=pline2d.vertices_end();i++)
	  {
	    GEOM_FT z= to_3d(*i).x();
	    if(z<retval)
	      retval= z;
	  }
      }
    return retval;
  }

//! @brief Append the projection of the vertex to the polyline.
const Pos3d PlanePolyline3d::appendVertex(const Pos3d &p)
  {
    const Plane plane= getPlane(); // Get the plane containing the polyline.
    const Pos3d prj= plane.Projection(p); // Compute projection.
    const Pos2d p2d(to_2d(prj)); // Convert to 2D.
    pline2d.appendVertex(p2d); // Append it to the polyline.
    return prj; // Return a copy of the projected vertex.
  }

//! @brief Append a vertex to the beginning of the polyline.
const Pos3d PlanePolyline3d::appendVertexLeft(const Pos3d &p)
  {
    const Plane plane= getPlane(); // Get the plane containing the polyline.
    const Pos3d prj= plane.Projection(p); // Compute projection.
    const Pos2d p2d(to_2d(prj)); // Convert to 2D.
    pline2d.appendVertexLeft(p2d); // Append it to the polyline.
    return prj; // Return a copy of the projected vertex.
  }

//! @brief Insert the point argurment as vertex by
//! splitting the nearest segment.
void PlanePolyline3d::insertVertex(const Pos3d &p, const GEOM_FT &tol)
  {
    const Plane plane= getPlane(); // Get the plane containing the polyline.
    const Pos3d prj= plane.Projection(p); // Compute projection.
    const Pos2d p2d(to_2d(prj)); // Convert to 2D.
    this->pline2d.insertVertex(p2d); // Insert it into the polyline.
  }

//! @brief Assuming that p is a vertex of the polyline
//! Return the chunk:
//! from the beginning to p if sgn < 0
//! from p to the end if sgn >= 0
PlanePolyline3d PlanePolyline3d::getChunk(const Pos3d &p,const short int &sgn, const GEOM_FT &tol) const
  {
    const Plane plane= getPlane(); // Get the plane containing the polyline.
    const Pos3d prj= plane.Projection(p); // Compute projection.
    const Pos2d p2d(to_2d(prj)); // Convert to 2D.
    Polyline2d pline= this->pline2d.getChunk(p2d, sgn, tol);
    return PlanePolyline3d(this->getRef(), pline);
  }

//! @brief Return the polyline chunk that goes from its beginning
//! to the nearest vertex to p. If distance from vertex to p is
//! greater than tol add the vertex to the resulting polyline.
PlanePolyline3d PlanePolyline3d::getLeftChunk(const Pos3d &p, const GEOM_FT &tol) const
  {
    const Plane plane= getPlane(); // Get the plane containing the polyline.
    const Pos3d prj= plane.Projection(p); // Compute projection.
    const Pos2d p2d(to_2d(prj)); // Convert to 2D.
    Polyline2d pline= this->pline2d.getLeftChunk(p2d, tol);
    return PlanePolyline3d(this->getRef(), pline);
  }

//! @brief Return the polyline chunk that goes the nearest vertex to
//! to p to its end. If distance from vertex to p is
//! greater than tol add the vertex to the resulting polyline.
PlanePolyline3d PlanePolyline3d::getRightChunk(const Pos3d &p, const GEOM_FT &tol) const
  {
    const Plane plane= getPlane(); // Get the plane containing the polyline.
    const Pos3d prj= plane.Projection(p); // Compute projection.
    const Pos2d p2d(to_2d(prj)); // Convert to 2D.
    Polyline2d pline= this->pline2d.getRightChunk(p2d, tol);
    return PlanePolyline3d(this->getRef(), pline);
  }

boost::python::list PlanePolyline3d::split(const Pos3d &p) const
  {
    PlanePolyline3d retvalA= getLeftChunk(p,0.0);
    PlanePolyline3d retvalB= getRightChunk(p,0.0);
    boost::python::list retval;
    retval.append(retvalA);
    retval.append(retvalB);
    return retval;
  }

//! @brief Returns true if the polyline is closed.
bool PlanePolyline3d::isClosed(const GEOM_FT &tol) const
  { return this->pline2d.isClosed(); }

//! @brief Return an iterator pointing to the vertex that is just before the
//! point at a distance "s" measured along the polyline from its origin.
//
//! @param s: distance measured along the polyline from its origin.
PlanePolyline3d::const_iterator PlanePolyline3d::getSegmentAtLength(const GEOM_FT &s) const
  { return this->pline2d.getSegmentAtLength(s); }

//! @brief Return the index of the segment that lies at the
//! point at a distance "s" measured along the polyline from
//! its origin.
//! @param s: distance measured along the polyline from its origin.
int PlanePolyline3d::getIndexOfSegmentAtLength(const GEOM_FT &s) const
  { return this->pline2d.getIndexOfSegmentAtLength(s); }
    
//! @brief Return the index of the segment that lies at the
//! point at a distance "lambda*L" measured along the polyline from
//! its origin.
//! @param lambda: parameter (0.0->start of the pline, 1.0->end of the pline).
int PlanePolyline3d::getIndexOfSegmentAtParam(const GEOM_FT &lambda) const
  { return this->pline2d.getIndexOfSegmentAtParam(lambda); }

//! @brief Return the i-th edge (i between 1 and num_sides).
Segment3d PlanePolyline3d::getSegment(unsigned int i) const
  {
    Segment2d sg2d= pline2d.getSegment(i);
    return to_3d(sg2d);
  }

//! @brief Return the i-th side (i between 0 and num_sides-1).
Segment3d PlanePolyline3d::getSegment0(unsigned int i) const
  {
    Segment2d sg2d= pline2d.getSegment0(i);
    return to_3d(sg2d);
  }

//! @brief Return the the point at a distance "s" measured along
//! the polyline from its origin.
//! @param s: distance measured along the polyline from its origin.
Pos3d PlanePolyline3d::getPointAtLength(const GEOM_FT &s) const
  {
    Pos2d p2d= this->pline2d.getPointAtLength(s);
    return to_3d(p2d);
  }

//! @brief Return the I vector of the segment that lies at the
//! point at a distance "s" measured along the polyline from
//! its origin.
//! @param s: distance measured along the polyline from its origin.
Vector3d PlanePolyline3d::getIVectorAtLength(const GEOM_FT &s) const
  {
    const Vector2d v2d= this->pline2d.getIVectorAtLength(s);
    return to_3d(v2d);
  }

//! @brief Return the J vector of the segment that lies at the
//! point at a distance "s" measured along the polyline from
//! its origin.
//! @param s: distance measured along the polyline from its origin.
Vector3d PlanePolyline3d::getJVectorAtLength(const GEOM_FT &s) const
  {
    const Vector2d v2d= this->pline2d.getJVectorAtLength(s);
    return to_3d(v2d);
  }

//! @brief Return the K vector of the segment that lies at the
//! point at a distance "s" measured along the polyline from
//! its origin.
//! @param s: distance measured along the polyline from its origin.
Vector3d PlanePolyline3d::getKVectorAtLength(const GEOM_FT &) const
  {
    const Plane plane= getPlane();
    return plane.Normal();
  }

//! @brief Return the points that results from the segment division.
//!
//! @param num_partes: number of segments.
VectorPos3d PlanePolyline3d::Divide(int num_partes) const
  {
    const VectorPos2d tmp= this->pline2d.Divide(num_partes);
    const size_t sz= tmp.size();
    VectorPos3d retval(sz);
    size_t k= 0;
    for(VectorPos2d::const_iterator i= tmp.begin();i!=tmp.end(); i++, k++)
      { retval[k]= to_3d(*i); }
    return retval;
  }

//! @brief Return a Python list containing the points that results
//! from the segment division.
//!
//! @param num_partes: number of segments.
boost::python::list PlanePolyline3d::DividePy(int num_partes) const
  {
    VectorPos3d tmp= this->Divide(num_partes);
    boost::python::list retval;
    for(VectorPos3d::const_iterator i= tmp.begin();i!=tmp.end(); i++)
      retval.append(*i);
    return retval;
  }

//! @brief Return the points that divide the segments in the proportions
//! being passed as parameter.
//!
//! @param proportions: list of float numbers that contain
//! the unitary length of the desired intervals (i.e. [1] for one
//! interval only with the full length of the segment, [0.5,0.5] for
//! two intervals with half the length of the segment, [0.25, 75] for
//! two intervals with a quarter and three quarters of the segment...
VectorPos3d PlanePolyline3d::Divide(const std::vector<double> &proportions) const
  {
    const VectorPos2d tmp= this->pline2d.Divide(proportions);
    const size_t sz= tmp.size();
    VectorPos3d retval(sz);
    size_t k= 0;
    for(VectorPos2d::const_iterator i= tmp.begin();i!=tmp.end(); i++, k++)
      { retval[k]= to_3d(*i); }
    return retval;
  }

//! @brief Return a Python list containing the points that results
//! from the segment division.
//!
//! @param num_partes: number of segments.
boost::python::list PlanePolyline3d::DividePy(const boost::python::list &proportions) const
  {
    std::vector<double> v_double= vector_double_from_py_list(proportions);
    VectorPos3d tmp= Divide(v_double);
    boost::python::list retval;
    for(VectorPos3d::const_iterator i= tmp.begin();i!=tmp.end(); i++)
      retval.append(*i);
    return retval;
  }

Plane PlanePolyline3d::getPlaneFromSide(unsigned int i) const
  {
    Segment3d lado= getSegment(i);
    Vector3d v= -getPlane().Normal(); // interior points -> negative distance.
    return Plane(lado,v);
  }

Plane PlanePolyline3d::getPlaneFromSide0(unsigned int i) const
  { return getPlaneFromSide(i+1); }

//! @brief Return true if the point lies inside the polyline
//! (i. e. the distance from the point to the polyline is less
//! than the tolerance argument).
bool PlanePolyline3d::In(const Pos3d &p,const double &tol) const
  {
    bool retval= false;
    const Plane plane= getPlane();
    if(!plane.In(p,tol))
      { retval= false; }
    else
      {
	const Pos3d prj= plane.Projection(p);
        const Pos2d p2d(to_2d(prj));
        retval= pline2d.In(p2d,tol);
      }
    return retval;
  }

//! @brief Return the center of mass of the polyline.
Pos3d PlanePolyline3d::getCenterOfMass(void) const
  {
    const Pos2d centroid= pline2d.getCenterOfMass();
    return to_3d(centroid);
  }

//! @brief Moment of inertia with respect to an axis parallel to
//! the x axis passing through the center of mass.
GEOM_FT PlanePolyline3d::Ix(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented, 0 is returned." << std::endl;
    return 0;
  }

//! @brief Moment of inertia with respect to an axis parallel to
//! the y axis passing through the center of mass.
GEOM_FT PlanePolyline3d::Iy(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented, 0 is returned." << std::endl;
    return 0;
  }

//! @brief Moment of inertia with respect to an axis parallel to
//! the z axis passing through the center of mass.
GEOM_FT PlanePolyline3d::Iz(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented, 0 is returned." << std::endl;
    return 0;
  }

GEOM_FT PlanePolyline3d::GetMax(unsigned short int i) const
  {
    const GeomObj::list_Pos3d lv= getVertexList();
    return get_max_i(i,lv.begin(),lv.end());
  }
GEOM_FT PlanePolyline3d::GetMin(unsigned short int i) const
  {
    const GeomObj::list_Pos3d lv= getVertexList();
    return get_min_i(i,lv.begin(),lv.end());
  }

void PlanePolyline3d::Print(std::ostream &os) const
  {
    unsigned int nv= getNumVertices();
    if(nv<1) return;
    os << Vertice(1);
    for(unsigned int i= 2; i <= nv; i++)
      os << ", " << Vertice(i);
  }

// //! @brief Return the squared distance from from point to polyline.
// //!
// //! The distance is computed as the maximum of:
// //!  -The distance from the point to the plane that contains the polyline.
// //!  -The signed distances from the point to each of the half spaces
// //!  defined by the plane that contains the polyline.
// //!
// // If the vertices are in counterclockwise order, positive distances
// // correspond to point AT THE SAME SIDE OF THE POLYLINE with respect
// // to the segment, otherwise the sign of the computed distance must
// // be changed.
// GEOM_FT PlanePolyline3d::distSigno2(const Pos3d &p) const
//   {
//     const Pos2d p2d(to_2d(p));
//     return pline2d.distSigno2(p2d);
//   }

// GEOM_FT PlanePolyline3d::distSigno(const Pos3d &p) const
//   { return sqrt_FT(::Abs(distSigno2(p))); }

// //! @brief Return the distance from point to polyline.
// //! Distance is computed as the maximum of the distances
// //! (signed) from the point to each of the planes
// //! that contain a side and are normal to the plane 
// //! that contains the polyline.
// //! If the point is inside the polyline 0 is returned.
// GEOM_FT PlanePolyline3d::dist2(const Pos3d &p) const
//   {
//     const GEOM_FT retval= distSigno2(p);
//     return (retval >= 0 ? retval : 0);
//   }


// //! @brief Return the distance from point to polyline.
// //! This distance is computed as the maximum of the signed distances from the
// //! point to each of the planes that contain a side and are perpendicular to the
// //! plane that contains the polyline.
// //! In the point is inside the polyline 0 is returned.
// GEOM_FT PlanePolyline3d::dist(const Pos3d &p) const
//   {
//     const GEOM_FT retval= distSigno(p);
//     return (retval >= 0 ? retval : 0);
//   }

//! @brief Return true if the line intersects the polyline.
bool PlanePolyline3d::intersects(const Line3d &r) const
  {
    bool retval= false;
    const Line2d r2d= to_2d(r);
    const size_t numSegments= getNumSegments();
    for(size_t i= 0; i<numSegments; i++)
      {
	Segment2d s= pline2d.getSegment0(i);
	if(s.intersects(r2d))
	  {
	    retval= true;
	    break;
	  }
      }
    return retval;
  }

//! @brief Return the intersection with the line argument.
GeomObj3d::list_Pos3d PlanePolyline3d::getIntersection(const Line3d &r) const
  {
    const Line2d r2d= to_2d(r);
    GeomObj2d::list_Pos2d tmp= pline2d.getIntersection(r2d);
    GeomObj3d::list_Pos3d retval;
    for(GeomObj2d::list_Pos2d::const_iterator i= tmp.begin(); i!=tmp.end(); i++)
      retval.push_back(to_3d(*i));
    return retval;
  }

//! @brief Return true if the plane intersects the polyline.
bool PlanePolyline3d::intersects(const Plane &p) const
  {
    const Plane pl= getPlane(); // get the plane that contains this polyline.
    const Line3d rInt= p.getIntersection(pl); // compute the intersection of
                                              // both planes.
    return this->intersects(rInt); // Test if that line intersects.
  }

//! @brief Return the intersection with the plane argument.
GeomObj3d::list_Pos3d PlanePolyline3d::getIntersection(const Plane &p) const
  {
    const Plane pl= getPlane(); // get the plane that contains this polyline.
    const Line3d rInt= p.getIntersection(pl); // compute the intersection of
                                              // both planes.
    return this->getIntersection(rInt);
  }

//! @brief Return true if alguno de los vertices toca el cuadrante
//! que se pasa como parámetro.
bool PlanePolyline3d::TocaCuadrante(const int &cuadrante) const
  {
    Polyline2d::const_iterator i= pline2d.vertices_begin();
    for(;i!=pline2d.vertices_end();i++)
      if(to_3d(Pos2d(*i)).Cuadrante() == cuadrante)
        return true;
    return false;
  }

void PlanePolyline3d::swap(void)
  { pline2d.swap(); }

/**
   * Douglas Peucker algorithm implementation. 
   * Recursively delete points that are within epsilon.
   * @param epsilon the higher the more aggressive.
   * @param iterator of the first point in a segment.
   * @param iterator of the last point in a segment.
   */
void PlanePolyline3d::simplify(GEOM_FT epsilon, iterator it1, iterator it2)
  { this->pline2d.simplify(epsilon, it1, it2); }

/**
 * @param epsilon the higher, the more points gotten rid of.
 */
void PlanePolyline3d::simplify(GEOM_FT epsilon)
  { this->pline2d.simplify(epsilon); }
