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
//Polyline2d.cc

#include "Polyline2d.h"
#include "Segment2d.h"
#include <plotter.h>

#include "utility/geom/lists/utils_list_pos2d.h"
#include "utility/geom/trf/Trf2d.h"
#include "utility/geom/pos_vec/Pos2dList.h"
#include "utility/geom/d2/Circle3d.h" // Curvature functions.
#include "utility/geom/pos_vec/VectorPos2d.h"
#include "utility/kernel/python_utils.h"
#include "utility/geom/d2/2d_polygons/Polygon2d.h"
#include "utility/geom/d2/HalfPlane2d.h"

//! @brief Default constructor.
Polyline2d::Polyline2d(void)
  : Linear2d(), GeomObj::list_Pos2d() {}

//! @brief Constructor.
Polyline2d::Polyline2d(const GeomObj::list_Pos2d &l)
  : Linear2d(), GeomObj::list_Pos2d(l){}

//! @brief Constructor.
Polyline2d::Polyline2d(const Pos2dList &l)
  : Linear2d(), GeomObj::list_Pos2d(l.getPoints()){}

//! @brief Constructor.
Polyline2d::Polyline2d(const Polygon2d &plg)
  : Linear2d(), GeomObj::list_Pos2d(plg.getVertexList())
  {
    if(!isClosed(1e-12))
      close();
  }

//! @brief Constructor (Python interface).
Polyline2d::Polyline2d(const boost::python::list &l)
  { this->setVertices(l); }

//! @brief Append a vertex to the polyline.
const Pos2d *Polyline2d::appendVertex(const Pos2d &p)
  {
    GeomObj::list_Pos2d::push_back(p);
    return &(*GeomObj::list_Pos2d::rbegin());
  }

//! @brief Append a vertex to the beginning of the polyline.
const Pos2d *Polyline2d::appendVertexLeft(const Pos2d &p)
  {
    GeomObj::list_Pos2d::push_front(p);
    return &(*GeomObj::list_Pos2d::begin());
  }

size_t Polyline2d::getNumSegments(void) const
  {
    size_t retval= getNumVertices();
    if(retval>0)
      retval-=1;
    return retval;
  }

//! @brief Return the list of the vertices.
const GeomObj::list_Pos2d &Polyline2d::getVertices(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " is deprecated. Use getVertexList."
              << std::endl;
    return *this;
  }

//! @brief Set the polyline vertices from the given Python list.
void Polyline2d::setVertices(const boost::python::list &l)
  {
    const int sz= len(l);
    // copy the components
    for(int i=0; i<sz; i++)
      push_back(boost::python::extract<Pos2d>(l[i]));
  }

//! @brief Return the list of the vertices.
const GeomObj::list_Pos2d &Polyline2d::getVertexList(void) const
  { return *this; }

//! @brief Return a Python list containing the positions
//! of the polyline vertices.
boost::python::list Polyline2d::getVertexListPy(void) const
  {
    boost::python::list retval;
    GeomObj::list_Pos2d lst= getVertexList();
    GeomObj::list_Pos2d::const_iterator i= lst.begin();
    for(;i!=lst.end();i++)
      retval.append(*i);
    return retval;
  }

//! @brief Applies the transformation to the points que se pasa como parámetro.
void Polyline2d::Transform(const Trf2d &trf2d)
  { trf2d.Transform(begin(),end()); }

//! @brief Return true if the point is in the polyline.
bool Polyline2d::In(const Pos2d &p, const double &tol) const
  {
    bool retval= false;    
    if(!empty())
      {
	list_Pos2d::const_iterator first= begin();
	list_Pos2d::const_iterator last= std::prev(end());
	for(list_Pos2d::const_iterator j=first;j != last;j++)
          if(getSegment(j).In(p,tol))
	    {
	      retval= true;
	      break;
	    }
      }
    return retval;
  }

//! @brief Return the length along the polyline upto the given point.
GEOM_FT Polyline2d::getLengthUpTo(const Pos2d &p) const
  {
    GEOM_FT retval= 0.0;    
    if(!empty())
      {
	const_iterator nearestSegmentIter= this->getNearestSegmentIter(p);
	const_iterator first= begin();
	const_iterator last= std::prev(end());
	list_Pos2d::const_iterator j=first;
	for(;j != nearestSegmentIter;j++)
	  {
	    const Segment2d &sg= getSegment(j);
	    retval+= sg.getLength();
	  }
	const Pos2d lastVertex= *j;
	retval+= lastVertex.dist(p);
      }
    return retval;
  }

//! @brief Return the squared distance to the polyline.
GEOM_FT Polyline2d::dist2(const Pos2d &p) const
  {
    GEOM_FT retval= std::numeric_limits<GEOM_FT>::infinity();    
    if(!empty())
      {
	list_Pos2d::const_iterator first= begin();
	list_Pos2d::const_iterator last= std::prev(end());
	for(list_Pos2d::const_iterator j=first;j != last;j++)
	  {
	    const GEOM_FT dist2= getSegment(j).dist2(p);
            if(dist2<retval)
	      { retval= dist2; }
	  }
      }
    return retval;
  }

//! @brief Return the distance from the point to the segment.
GEOM_FT Polyline2d::dist(const Pos2d &p) const
  { return sqrt(this->dist2(p)); }

//! @brief Return the nearest segment to the given point.
//! @param: point to find the nearest segment to.
Polyline2d::const_iterator Polyline2d::getNearestSegmentIter(const Pos2d &p) const    
  {
    Polyline2d::const_iterator retval= end();
    const size_t nv= getNumVertices();
    if(nv>=2)
      {
	list_Pos2d::const_iterator first= begin();
	GEOM_FT minDist2= getSegment(first).dist2(p);
	retval= first;
	if(nv>=3)
	  {
	    list_Pos2d::const_iterator j=first;
	    j++;
	    list_Pos2d::const_iterator last= std::prev(end());
	    for(;j != last;j++)
	      {
		double dist2= getSegment(j).dist2(p);
		if(dist2<minDist2)
		    {
		      retval= j;
		      minDist2= dist2;
		    }
	      }
	  }
      }
    return retval;
  }

//! @brief Return the index of the nearest 3D segment.
//! @param p: point to which the returned segment is the closest one.
int Polyline2d::getNearestSegmentIndex(const Pos2d &p) const
  {
    const_iterator i= this->getNearestSegmentIter(p);
    return std::distance(this->begin(), i);
  }

//! @brief Return the nearest 2D segment.
//! @param p: point to which the returned segment is the closest one.
Segment2d Polyline2d::getNearestSegment(const Pos2d &p) const
  {
    Segment2d retval;
    const_iterator i= this->getNearestSegmentIter(p);
    const_iterator j= i+1;
    if((i!=this->end()) and (j!=this->end()))
      retval= Segment2d(*i,*j);
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; nearest segment to:" << p
	          << " not found. ";
        if(this->size()>2)
	  std::cerr << " Unknown error.";
	else
	  std::cerr << " Polyline is empty.";
	std::cerr << std::endl;
      }
    return retval;
  }

//! @brief Return the projection of the given point into the polyline.
//! @param p: point to be projected.
Pos2d Polyline2d::Projection(const Pos2d &p) const
  {
    Segment2d nearestSegment= this->getNearestSegment(p);
    const Pos2d retval= nearestSegment.Projection(p);
    return retval;
  }

//! @brief Return the maximum value of the i coordinate.
GEOM_FT Polyline2d::GetMax(unsigned short int i) const
  { return GeomObj::list_Pos2d::GetMax(i); }

//! @brief Return the minimum value of the i coordinate.
GEOM_FT Polyline2d::GetMin(unsigned short int i) const
  { return GeomObj::list_Pos2d::GetMin(i); }

//! @brief Return a Polyline2d with the vertices whose i-th coordinate
//! is greater than d.
Polyline2d Polyline2d::GetMayores(unsigned short int i,const GEOM_FT &d) const
  {
    Polyline2d retval(GeomObj::list_Pos2d::GetMayores(i,d));
    return retval;
  }

//! @brief Return a Polyline2d with the vertices whose i-th coordinate
//! is smaller than d.
Polyline2d Polyline2d::GetMenores(unsigned short int i,const GEOM_FT &d) const
  {
    Polyline2d retval(GeomObj::list_Pos2d::GetMenores(i,d));
    return retval;
  }
// //Return the intersection of the polyline with the plane defined by
// // the equation coord_i= d
// Polyline2d::list_Pos2d Polyline2d::Int(unsigned short int i,const double &d) const
//   {
//     list_Pos2d l_int; //List of intersections
// 	const_iterator ultimo= end();
//     ultimo--;
//     for(const_iterator j=begin();j != ultimo;j++)
//       {
//         Segment2d s= getSegment(j);
//         list_Pos2d l_temp= s.Int(i,d);
//         copy(l_temp.begin(),l_temp.end(), back_inserter(l_int)); 
//       }
// 	//l_int.unique();
//      return l_int;
//   }
// //Return the 2D polyline defined by the vertex obtained from cutting
// // this polyline with the plane defined by
// // the equation coord_i= d
// Polyline2d Polyline2d::Corta(unsigned short int i,const double &d) const
//   {
//     Polyline2d result; //List of intersections
// 	const_iterator ultimo= end();
//     ultimo--;
//     for(const_iterator j=begin();j != ultimo;j++)
//       {
//         Segment2d s= getSegment(j);
//         list_Pos2d l_temp= s.Int(i,d);
//         result.push_back(*j);
//         if (!l_temp.empty()) result.push_back(*l_temp.begin());
//       }
//      result.push_back(*ultimo);
//      //result.unique();
//      return result;
//   }

//! @brief Return the i-th vertex (the first one has the index 1).
const Pos2d &Polyline2d::Vertice(const size_t &i) const
  { return GeomObj::list_Pos2d::operator[](i-1); }

//! @brief Return the i-th vertex (the first one has the index 0).
const Pos2d &Polyline2d::Vertice0(const size_t &i) const
  { return GeomObj::list_Pos2d::operator[](i); }

//! @brief Return a polyline parallel to this one at the distance
//! being passed as parameter. If the distance is positive the new
//! polyline will be placed on the right.
Polyline2d Polyline2d::offset(const GEOM_FT &d) const
  {
    Pos2dList tmp(*this);
    Polyline2d retval(tmp.offset(d));
    return retval;
  }

//! @brief Return a buffer polygon around the segment.
//!
//! Return a 2D polygon with boundaries that buffer this segment
//! by a distance d.
//! @param d: distance around the segment.
//! @param n: number of vertex of the buffers around the endpoints.
Polygon2d Polyline2d::getBufferPolygon(const GEOM_FT &d, const size_t &numVertices) const
  {
    // Rectangle around the segment.
    Polyline2d s1= this->offset(d);
    Polyline2d s2= this->offset(-d);
    for(const_reverse_iterator i= s2.rbegin(); i!=s2.rend(); i++)
        s1.push_back(*i);
    Polygon2d retval(s1);
    if(numVertices>1) // create buffers around the endpoints.
      {
	Polygon2d b1= this->getFromPoint().getBufferPolygon(d, numVertices);
	retval.une(b1);
	Polygon2d b2= this->getToPoint().getBufferPolygon(d, numVertices);
	retval.une(b2);
      }
    return retval;
  }

//! @brief Return the segment that follows the *i vertex.
Segment2d Polyline2d::getSegment(const const_iterator &i) const
  {
    const_iterator j= i; j++;
    if(j==end())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << ": the vertex is the last one "
		  << " of the polyline." << std::endl;
        return Segment2d();
      }
    return Segment2d(*i,*j);
  }

//! @brief Return i-th segment (the first one is the 1).
Segment2d Polyline2d::getSegment(const size_t &i) const
  {
    const size_t ns= getNumSegments();
    if(i>ns)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; you asked for the " << i
	        << "-th and " << ns
                << "-th is the last one." << std::endl;
    Segment2d s(Vertice(i),Vertice(i+1));
    return s;
  }

//! @brief Return i-th segment (the first one is the 0).
Segment2d Polyline2d::getSegment0(const size_t &i) const
  {
    const size_t ns= getNumSegments();
    if(i>ns)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; you asked for the " << i
	        << "-th and " << ns
                << "-th is the last one." << std::endl;
    Segment2d s(Vertice0(i),Vertice0(i+1));
    return s;
  }

//! @brief Return a vector containing the consecutive segments of the polyline.
std::vector<Segment2d> Polyline2d::getSegments(void) const
  {
    const size_t ns= getNumSegments();
    std::vector<Segment2d> retval(ns);
    if(ns>0)
      {
	list_Pos2d::const_iterator i= begin();
	Pos2d vi(*i);
	size_t count= 0;
	for(list_Pos2d::const_iterator j=i+1;j!=end(); j++, count++)
	  {
	    const Pos2d vj(*j);
	    retval[count]= Segment2d(vi, vj);
	    vi= vj;
	  }
      }
    return retval;
  }

//! @brief Return the approximate curvature of the polyline at
//! the vertex pointed by the iterator argument.
//
//! @param nth: iterator pointing to the desired vertex.
GEOM_FT Polyline2d::getCurvatureAtVertex(const_iterator nth) const
  {
    double retval;
    const size_t sz= this->size();
    if(sz<3) //Only one segment.
      retval= 0.0;
    else
      {
	const_iterator previousVertexIter= nth-1;
	const_iterator thisVertexIter= nth;
	const_iterator nextVertexIter= nth+1;
	if(nth == this->begin()) // No previous vertex.
	  {
	    previousVertexIter= nth;
	    thisVertexIter= nth+1;
	    nextVertexIter= nth+2;
	  }
	else if(nth == this->end()) // No following vertex.
	  {
	    previousVertexIter= nth-2;
	    thisVertexIter= nth-1;
	    nextVertexIter= nth;
	  }
	const Pos2d &A= *thisVertexIter;
	const Pos2d &B= *previousVertexIter;
	const Pos2d &C= *nextVertexIter;
	retval= curvature(A,B,C);
      }
    return retval;
  }

//! @brief Return the approximate curvature of the polyline at
//! the the point at a distance "s" measured along the polyline
//! from its origin.
//
//! @param s: distance measured along the polyline from its origin.
GEOM_FT Polyline2d::getCurvatureAtLength(const GEOM_FT &s) const
  {
    double retval= 0.0;
    const size_t sz= this->size();
    if(sz<3) //Only one segment.
      retval= 0.0;
    else
      {
	const const_iterator i= getSegmentAtLength(s);
	if(i!=this->end()) // found it
	  {
	    GEOM_FT k1= this->getCurvatureAtVertex(i);
            const_iterator j= i+1;
	    if(j==this->end())
	      retval= k1;
	    else
	      {
		GEOM_FT k2= this->getCurvatureAtVertex(i+1);
		const GEOM_FT lengthUntil= this->getLengthUntilVertex(i);
		const GEOM_FT remainderLength= s-lengthUntil;
		const Segment2d sg= getSegment(i);
		const GEOM_FT factor= remainderLength/sg.getLength();
		retval= (1-factor)*k1+factor*k2;
	      }
	  }
      }
    return retval;
  }

//! @brief Return the approximate curvature of the polyline at
//! the vertex pointed by the iterator argument.
//
//! @param nth: iterator pointing to the desired vertex.
Vector2d Polyline2d::getCurvatureVectorAtVertex(const_iterator nth) const
  {
    Vector2d retval;
    const size_t sz= this->size();
    if(sz<2) // No segments.
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	      << ";ERROR: no segments, so no curvature vector." << std::endl;
      }
    else if(sz==2) // One segment only.
      {
	Segment2d sg= this->getSegment(1);
	retval= sg.getJVector();
      }
    else
      {
	const_iterator previousVertexIter= nth-1;
	const_iterator thisVertexIter= nth;
	const_iterator nextVertexIter= nth+1;
	if(nth == this->begin()) // No previous vertex.
	  {
	    previousVertexIter= nth;
	    thisVertexIter= nth+1;
	    nextVertexIter= nth+2;
	  }
	else if(nth == this->end()) // No following vertex.
	  {
	    previousVertexIter= nth-2;
	    thisVertexIter= nth-1;
	    nextVertexIter= nth;
	  }
	const Pos2d &A= *thisVertexIter;
	const Pos2d &B= *previousVertexIter;
	const Pos2d &C= *nextVertexIter;
	retval= curvature_vector(A,B,C);
      }
    return retval;
  }

//! @brief Return the approximate curvature of the polyline at
//! each of its vertices.
std::vector<Vector2d> Polyline2d::getCurvatureVectorAtVertices(void) const
  {
    const size_t sz= this->getNumVertices();
    std::vector<Vector2d> retval(sz);
    if(sz<2) // No segments.
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << ";ERROR: no segments, so no curvature vectors."
		  << std::endl;
      }
    else if(sz==2) // One segment only.
      {
	Segment2d sg= this->getSegment(1);
	retval[0]= sg.getJVector();
	retval[1]= retval[0];
      }
    else // 3 vertex at least.
      {
	std::vector<Vector2d> vertex_normals(sz);
	size_t count= 0; // vertex iterator.
	for(Polyline2d::const_iterator vi= this->begin(); vi!=this->end(); vi++, count++)
	  {
	    Polyline2d::const_iterator previousVertexIter= vi-1;
	    Polyline2d::const_iterator thisVertexIter= vi;
	    Polyline2d::const_iterator nextVertexIter= vi+1;
	    if(thisVertexIter == this->begin()) // No previous vertex.
	      {
		previousVertexIter= vi;
		thisVertexIter= vi+1;
		nextVertexIter= vi+2;
	      }
	    else if(thisVertexIter == this->end()) // No following vertex.
	      {
		previousVertexIter= vi-2;
		thisVertexIter= vi-1;
		nextVertexIter= vi;
	      }
	    const Pos2d &A= *thisVertexIter;
	    const Pos2d &B= *previousVertexIter;
	    const Pos2d &C= *nextVertexIter;
	    retval[count]= curvature_vector(A,B,C);
	  }
      }
    return retval;
  }

//! @brief Return the approximate curvature vector of the polyline at
//! the the point at a distance "s" measured along the polyline
//! from its origin.
//
//! @param s: distance measured along the polyline from its origin.
Vector2d Polyline2d::getCurvatureVectorAtLength(const GEOM_FT &s) const
  {
    Vector2d retval;
    const size_t sz= this->size();
    if(sz<2) // No segments.
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	      << ";ERROR: no segments, so no curvature vector." << std::endl;
      }
    else if(sz==2) // One segment only.
      {
	Segment2d sg= this->getSegment(1);
	retval= sg.getJVector();
      }
    else
      {
	const const_iterator i= getSegmentAtLength(s);
	if(i!=this->end()) // found it
	  {
	    Vector2d k1= this->getCurvatureVectorAtVertex(i);
            const_iterator j= i+1;
	    if(j==this->end())
	      retval= k1;
	    else
	      {
		Vector2d k2= this->getCurvatureVectorAtVertex(i+1);
		const GEOM_FT lengthUntil= this->getLengthUntilVertex(i);
		const GEOM_FT remainderLength= s-lengthUntil;
		const Segment2d sg= getSegment(i);
		const GEOM_FT factor= remainderLength/sg.getLength();
		retval= (1-factor)*k1+factor*k2;
	      }
	  }
      }
    return retval;
  }

//! @brief Return the point at a distance "s" measured along
//! the polyline from its origin.
//
//! @param s: distance measured along the polyline from its origin.
Pos2d Polyline2d::getPointAtLength(const GEOM_FT &s) const
  {
    Pos2d retval;
    const const_iterator i= getSegmentAtLength(s);
    if(i!=this->end()) // found it
      {
	const GEOM_FT lengthUntil= this->getLengthUntilVertex(i);
	const GEOM_FT remainderLength= s-lengthUntil;
	const Segment2d sg= getSegment(i);
	retval= sg.PtoParametricas(remainderLength);
      }
    return retval;
  }

//! @brief Return the I vector of the segment that lies at the
//! point at a distance "s" measured along the polyline from
//! its origin.
//! @param s: distance measured along the polyline from its origin.
Vector2d Polyline2d::getIVectorAtLength(const GEOM_FT &s) const
  {
    Vector2d retval;
    const int i= getIndexOfSegmentAtLength(s);
    if(i>=0) // found it
      {
	const Segment2d sg= getSegment(i+1);
	retval= sg.getIVector();
      }
    return retval;
  }

//! @brief Return the J vector of the segment that lies at the
//! point at a distance "s" measured along the polyline from
//! its origin.
//! @param s: distance measured along the polyline from its origin.
Vector2d Polyline2d::getJVectorAtLength(const GEOM_FT &s) const
  {
    Vector2d retval;
    const int i= getIndexOfSegmentAtLength(s);
    if(i>=0) // found it
      {
	const Segment2d sg= getSegment(i+1);
	Vector2d jVector= -sg.getJVector();
	// Get orientation from the approximated curvature vector.
	const Vector2d vK= getCurvatureVectorAtLength(s);
	const GEOM_FT dt= dot(jVector, vK);
	if(dt<0)
	  jVector= -jVector;
	retval= jVector;
      }
    return retval;
  }

//! @brief Return the points that results from the division of the polyline.
//!
//! @param num_partes: number of segments.
VectorPos2d Polyline2d::Divide(int num_partes) const
  {
    const double L= this->getLength();
    const double segmentLength= L/num_partes;
    const size_t numPoints= num_partes+1;
    VectorPos2d retval(numPoints);
    retval[0]= this->getFromPoint(); // first point.
    for(int i= 1; i<num_partes;i++) // intermediate points.
      { retval[i]= this->getPointAtLength(i*segmentLength); }
    retval[num_partes]= this->getToPoint(); //last point: num_partes==numPoints-1
    return retval;
  }

//! @brief Return a Python list containing the points that results
//! from the division of the polyline.
//!
//! @param num_partes: number of segments.
boost::python::list Polyline2d::DividePy(int num_partes) const
  {
    VectorPos2d tmp= this->Divide(num_partes);
    boost::python::list retval;
    for(VectorPos2d::const_iterator i= tmp.begin();i!=tmp.end(); i++)
      retval.append(*i);
    return retval;
  }

//! @brief Return the points that divide the polyline in the proportions
//! being passed as parameter.
//!
//! @param proportions: list of float numbers that contain
//! the unitary length of the desired intervals (i.e. [1] for one
//! interval only with the full length of the segment, [0.5,0.5] for
//! two intervals with half the length of the segment, [0.25, 75] for
//! two intervals with a quarter and three quarters of the segment...
VectorPos2d Polyline2d::Divide(const std::vector<double> &proportions) const
  {
    const size_t sz= proportions.size();
    const size_t numPoints= sz+1;
    VectorPos2d retval(numPoints);
    GEOM_FT lambda= 0.0;
    const GEOM_FT length= getLength();
    if(sz>0) // At least two points.
      {
	retval[0]= getFromPoint(); // First point.
	retval[sz]= getToPoint(); // Last point.
      }
    if(sz>1) //At least one intermediate point.
      {
	lambda= proportions[0];
        for(size_t i= 1; i<sz; i++) // sz==(numPoints-1)
	  {
	    retval[i]= this->getPointAtLength(lambda*length);
	    lambda+= proportions[i];
	  }  
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << " argument must not be empty." << std::endl;
    return retval;
  }

//! @brief Return a Python list containing the points that results
//! from the division of the polyline.
//!
//! @param num_partes: number of segments.
boost::python::list Polyline2d::DividePy(const boost::python::list &proportions) const
  {
    std::vector<double> v_double= vector_double_from_py_list(proportions);
    VectorPos2d tmp= Divide(v_double);
    boost::python::list retval;
    for(VectorPos2d::const_iterator i= tmp.begin();i!=tmp.end(); i++)
      retval.append(*i);
    return retval;
  }

GEOM_FT Polyline2d::Ix(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented" << std::endl;
    return 0.0;
  }
GEOM_FT Polyline2d::Iy(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented" << std::endl;
    return 0.0;
  }
GEOM_FT Polyline2d::Iz(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented" << std::endl;
    return 0.0;
  }

//! @brief Insert the point argument as vertex by
//! splitting the nearest segment if the nearest vertex
//! is not closer than tol. Otherwise do nothing.
void Polyline2d::insertVertex(const Pos2d &p, const GEOM_FT &tol)
  {
    const_iterator nearestVertexIter= getNearestPoint(p);
    const Pos2d nearestVertex= *nearestVertexIter;
    const GEOM_FT distToVertex= p.dist(nearestVertex);
    if(distToVertex>tol) // No vertices close to p
      {
        const_iterator vertexIter= getNearestSegmentIter(p)+1; //Segment end vertex.
        insert(vertexIter,p);
      }
  }


//! @brief Assuming that p is a vertex of the polyline
//! Return the chunk:
//! from the beginning to p if sgn < 0
//! from p to the end if sgn >= 0
//! @param p: position of the vertex
//! @param sgn: if < 0 get left chunk else get right chunk.
//! @param tol: if >0 insert p as vertex if nearest vertex is not closer than tol.
Polyline2d Polyline2d::getChunk(const Pos2d &p,const short int &sgn, const GEOM_FT &tol) const
  {
    Polyline2d result;
    const_iterator i= find(p);
    GEOM_FT distToVertex= 0.0;
    if (i == end())
      {
        i= getNearestPoint(p);
	Pos2d nearestVertex= *i;
	distToVertex= p.dist(nearestVertex);
	// Deal with the case in which two
	// vertices are equally distant from p.
	const_iterator nearestSegmentIter= getNearestSegmentIter(p);
	const Segment2d nearestSegment= getSegment(nearestSegmentIter);
	const GEOM_FT distToSegment= nearestSegment.dist(p);
	if((distToSegment<distToVertex) and (sgn>0))
	  {
	    i= nearestSegmentIter+1; // end point of the segment.
	    nearestVertex= *i;
	    distToVertex= p.dist(nearestVertex);
	  }   
      }
    if(sgn < 0)
      {
        copy(begin(),++i,back_inserter(result));
	if(distToVertex>tol)
	  result.appendVertex(p);
      }
    else
      {
        copy(i,end(),back_inserter(result));
	if(distToVertex>tol)
	  result.appendVertexLeft(p);
      }	
    return result;
  }

//! @brief Return the polyline chunk that goes from its beginning
//! to the nearest vertex to p. If distance from vertex to p is
//! greater than tol add the vertex to the resulting polyline.
Polyline2d Polyline2d::getLeftChunk(const Pos2d &p, const GEOM_FT &tol) const
  { return getChunk(p, -1, tol); }
  
//! @brief Return the polyline chunk that goes the nearest vertex to
//! to p to its end. If distance from vertex to p is
//! greater than tol add the vertex to the resulting polyline.
Polyline2d Polyline2d::getRightChunk(const Pos2d &p, const GEOM_FT &tol) const
  { return getChunk(p, 1, tol); }

//! @brief Return the two polylines that result from splitting
//! this one on the point nearest to the argument.
boost::python::list Polyline2d::split(const Pos2d &p) const
  {
    Polyline2d retvalA, retvalB;
    const_iterator nearestVertexIter= getNearestPoint(p);
    const Pos2d nearestVertex= *nearestVertexIter;
    const_iterator nearestSegmentIter= getNearestSegmentIter(p);
    const Segment2d nearestSegment= getSegment(nearestSegmentIter);
    const GEOM_FT distToVertex= p.dist(nearestVertex);
    const GEOM_FT distToSegment= nearestSegment.dist(p);
    if(distToVertex<=distToSegment)
      {
        retvalA= getLeftChunk(nearestVertex,0.0);
	retvalB= getRightChunk(nearestVertex,0.0);
      }
    else
      {
	const Pos2d splittingVertexA= *nearestSegmentIter;
	retvalA= getLeftChunk(splittingVertexA,0.0);
	retvalA.push_back(p);
	const Pos2d splittingVertexB= *(nearestSegmentIter+1);
	retvalB= getRightChunk(splittingVertexB,0.0);
	retvalB.push_front(p);
      }
    boost::python::list retval;
    retval.append(retvalA);
    retval.append(retvalB);
    return retval;
  }
  
//! @brief Return the iterators to the origin vertices of the polyline segments
//! where the intersection with the plane occur.
std::deque<Polyline2d::const_iterator> Polyline2d::getIntersectionIters(const Line2d &l) const
  {
    std::deque<Polyline2d::const_iterator> retval;
    if(!empty())
      {
	list_Pos2d::const_iterator first= begin();
	list_Pos2d::const_iterator last= std::prev(end());
	for(list_Pos2d::const_iterator j=first;j != last;j++)
	  {
	    const Segment2d s= getSegment(j);
	    if(l.intersects(s))
	      retval.push_back(j);
	  }
      }
    return retval;
  }

//! @brief Return the points of intersection of the polyline with
//! the argument.
GeomObj::list_Pos2d Polyline2d::getIntersection(const Line2d &r) const
  {
    list_Pos2d retval;
    const_iterator j=begin();
    const_iterator k= j;k++;
    for(;k != end();j++,k++)
      {
        Segment2d s(*j,*k);
        list_Pos2d tmp= intersection(r,s);
        if(!tmp.empty())
          retval.AgregaSiNuevo(tmp);
      }
    return retval;
  }

//! @brief Return the points of intersection of the polyline with
//! the argument.
GeomObj::list_Pos2d Polyline2d::getIntersection(const Ray2d &sr) const
  {
    list_Pos2d retval;
    const_iterator j=begin();
    const_iterator k= j;k++;
    for(;k != end();j++,k++)
      {
        Segment2d s(*j,*k);
        list_Pos2d tmp= intersection(sr,s);
        if(!tmp.empty())
          retval.AgregaSiNuevo(tmp);
      }
    return retval;
  }

//! @brief Return the points of intersection of the polyline with
//! the argument.
GeomObj::list_Pos2d Polyline2d::getIntersection(const Segment2d &sg) const
  {
    list_Pos2d retval;
    const_iterator j=begin();
    const_iterator k= j;k++;
    for(;k != end();j++,k++)
      {
        Segment2d s(*j,*k);
        list_Pos2d tmp= intersection(sg,s);
        if(!tmp.empty())
          retval.AgregaSiNuevo(tmp);
      }
    return retval;
  }

//! @brief Return the polyline chunks that result from clippin this polyline
//! with the given half space.
std::deque<Polyline2d> Polyline2d::clip(const HalfPlane2d &hp, const GEOM_FT &tol) const
  {
    std::deque<Polyline2d> retval;
    if(!empty())
      {
	const bool intersects= hp.intersects(*this);
	if(!intersects)
	  {
	    if(hp.In(*this))
	      retval.push_back(*this); // The whole polyline is inside the
	                               // half space.
	  }
	else // vertices at both sides of the half-space.
	  {
	    const Line2d boundary_line= hp.getBoundaryLine();
	    Polyline2d::const_iterator start= this->begin();
	    Polyline2d::const_iterator last= std::prev(this->end());
	    bool start_inside=  hp.In(*start, tol);
	    std::deque<Polyline2d::const_iterator> int_iters= this->getIntersectionIters(hp.getBoundaryLine());
	    std::deque<Polyline2d::const_iterator>::const_iterator i= int_iters.begin();
	    do
	      {
		std::deque<Polyline2d::const_iterator>::const_iterator ii= std::next(i);
		Polyline2d::const_iterator stop;
		
		if(ii != int_iters.end())
		  stop= *ii;
		else
		  stop= std::next(*i);
		const bool stop_inside= hp.In(*stop, tol);
		const bool pline_enters= !start_inside and stop_inside;
		const bool pline_exits= start_inside and !stop_inside;
		if(pline_enters)
		  {
		    Polyline2d chunk;
		    Segment2d sg= this->getSegment(start);
		    list_Pos2d tmp= intersection(sg,boundary_line);
		    chunk.appendVertex(tmp[0]);
		    start++;
		    for(list_Pos2d::const_iterator j= start; j!= stop; j++)
		      { chunk.appendVertex(*j); }
		    chunk.appendVertex(*stop);
		    i++;
		    if(stop!=last)
		      {
			sg= this->getSegment(stop);
			tmp= intersection(sg,boundary_line);
			chunk.appendVertex(tmp[0]);
			stop++;
		      }
		    retval.push_back(chunk);
		  }
		if(pline_exits)
		  {
		    Polyline2d chunk;
		    chunk.appendVertex(*start); // Starts inside.
		    Segment2d sg= this->getSegment(start);
		    list_Pos2d tmp= intersection(sg,boundary_line);
		    chunk.appendVertex(tmp[0]);
		    retval.push_back(chunk);
		  }
		start= stop;
		start_inside= hp.In(*start, tol);
		i++;
		if(start==last)
		  break;
	      }
	    while(i!= int_iters.end());
	  }
      }
    return retval;
  }

boost::python::list Polyline2d::clipPy(const HalfPlane2d &hp, const GEOM_FT &tol) const
  {
    boost::python::list retval;
    std::deque<Polyline2d> tmp= this->clip(hp, tol);
    std::deque<Polyline2d>::const_iterator i= tmp.begin();
    for(;i!=tmp.end();i++)
      retval.append(*i);
    return retval;
  }

/**
   * @param i1 iterator to the first point.
   * @param i2 iterator to the second point.
   * @param pMaxDist pointer to the maximum distance of _line[return index].
   * @return the index of the point farthest from the segment (t1,t2).
   */
Polyline2d::iterator Polyline2d::getFarthestPointFromSegment(iterator it1, iterator it2, GEOM_FT &pMaxDist)
  {
    // Keep track of the point with the maximum distance.
    iterator maxIt = it1;
    maxIt++;
    Segment2d s= Segment2d(*it1,*it2);
    GEOM_FT maxDist= s.dist(*maxIt);

    iterator it = it1;
    it++;
    while(it != it2)
      {
        Segment2d s= Segment2d(*it1,*it2);
        GEOM_FT dist= s.dist(*it);
        if(dist > maxDist)
	  {
            maxIt = it;
            maxDist = dist;
          }

        it++;
      }
    // Assign pMaxDist.
    pMaxDist = maxDist;
    return maxIt;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict Polyline2d::getPyDict(void) const
  {
    boost::python::dict retval= Linear2d::getPyDict(); // Call parent method.
    const boost::python::list vList= this->getVertexListPy(); // Get vertex list.
    retval["vertex_list"]= vList; // Put the list in the dictionary.
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void Polyline2d::setPyDict(const boost::python::dict &d)
  {
    Linear2d::setPyDict(d); // Call parent method.
    const boost::python::list vList= boost::python::extract<boost::python::list>(d["vertex_list"]); // Retrieve vertex list.
    this->clear(); // Remove current vertices (if any).
    this->setVertices(vList); // Set the new ones.
  }

void Polyline2d::Print(std::ostream &stream) const
  {
    if(empty()) return;
    const_iterator i= begin();
    stream << *i; i++;
    for(; i!=end(); i++)
      stream << ", " << *i;
  }
void Polyline2d::Plot(Plotter &plotter) const
  {
    if(size()<2) return;
    const_iterator i= begin();
    Pos2d pto1= *i; i++;
    for(; i!=end(); i++)
      {
        Pos2d pto2= *i;
        plotter.fline(pto1.x(),pto1.y(),pto2.x(),pto2.y());
        pto1= pto2;
      }
  }

GeomObj::list_Pos2d intersection(const Polyline2d &p,const Line2d &r)
  { return p.getIntersection(r); }
GeomObj::list_Pos2d intersection(const Polyline2d &p,const Ray2d &sr)
  { return p.getIntersection(sr); }
GeomObj::list_Pos2d intersection(const Polyline2d &p,const Segment2d &sg)
  { return p.getIntersection(sg); }
GeomObj::list_Pos2d intersection(const Line2d &r,const Polyline2d &p)
  { return p.getIntersection(r); }
GeomObj::list_Pos2d intersection(const Ray2d &sr,const Polyline2d &p)
  { return p.getIntersection(sr); }
GeomObj::list_Pos2d intersection(const Segment2d &sg,const Polyline2d &p)
  { return p.getIntersection(sg); }
