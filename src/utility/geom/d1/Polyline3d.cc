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
//Polyline3d.cc

#include "Polyline3d.h"
#include "utility/geom/d2/Plane.h"

//! @brief Default constructor.
Polyline3d::Polyline3d(void)
  : Linear3d(), GeomObj::list_Pos3d() {}

//! @brief Constructor.
Polyline3d::Polyline3d(const Pos3dList &l)
  : Linear3d(), GeomObj::list_Pos3d(l.getPoints()){}

//! @brief Constructor (Python interface).
Polyline3d::Polyline3d(const boost::python::list &l)
  {
    const int sz= len(l);
    // copy the components
    for(int i=0; i<sz; i++)
      push_back(boost::python::extract<Pos3d>(l[i]));
  }

//! @brief Comparison operator.
bool Polyline3d::operator==(const Polyline3d &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= Linear3d::operator==(other);
        if(retval)
          retval= GeomObj::list_Pos3d::operator==(other);
       }
    return retval;
  }

//! @brief Return the list of the vertices.
const GeomObj::list_Pos3d &Polyline3d::getVertices(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " is deprecated. Use getVertexList."
              << std::endl;
    return *this;
  }

//! @brief Return the list of the vertices.
const GeomObj::list_Pos3d &Polyline3d::getVertexList(void) const
  { return *this; }

//! @brief Return a Python list containing the positions
//! of the polygon vertices.
boost::python::list Polyline3d::getVertexListPy(void) const
  {
    boost::python::list retval;
    GeomObj::list_Pos3d lst= getVertexList();
    GeomObj::list_Pos3d::const_iterator i= lst.begin();
    for(;i!=lst.end();i++)
      retval.append(*i);
    return retval;
  }

//! @brief Append a vertex to the polyline.
const Pos3d *Polyline3d::appendVertex(const Pos3d &p)
  {
    GeomObj::list_Pos3d::push_back(p);
    return &(*rbegin());
  }

//! @brief Append a vertex to the beginning of the polyline.
const Pos3d *Polyline3d::appendVertexLeft(const Pos3d &p)
  {
    GeomObj::list_Pos3d::push_front(p);
    return &(*GeomObj::list_Pos3d::begin());
  }

size_t Polyline3d::getNumSegments(void) const
  {
    size_t retval= getNumVertices();
    if(retval>0)
      retval-=1;
    return retval;
  }

//! @brief Return the segment pointed by the iterator.
Segment3d Polyline3d::getSegment(const list_Pos3d::const_iterator &i) const
  {
    list_Pos3d::const_iterator j= i; j++;
    Segment3d s(*i,*j);
    return s;
  }

//! @brief Return the i-th segment (firt on has index 1).
Segment3d Polyline3d::getSegment(const size_t &i) const
  {
    const size_t ns= getNumSegments();
    if(i>ns)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; you asked for the " << i
	        << "-th and " << ns
                << "-th is the last one." << std::endl;
    return Segment3d((*this)[i-1],(*this)[i]);
  }


//! @brief Moves the polyline.
void Polyline3d::Move(const Vector3d &v)
  {
    for(list_Pos3d::iterator j=begin();j != end();j++)
      (*j)= (*j) + v;
  }

//! @brief Return true if the point is in the polyline.
bool Polyline3d::In(const Pos3d &p, const double &tol) const
  {
    bool retval= false;    
    if(!empty())
      {
	list_Pos3d::const_iterator first= begin();
	list_Pos3d::const_iterator last= std::prev(end());
	for(list_Pos3d::const_iterator j=first;j != last;j++)
          if(getSegment(j).In(p,tol))
	    {
	      retval= true;
	      break;
	    }
      }
    return retval;
  }

//! @brief Return the nearest segment to the argument.
Polyline3d::const_iterator Polyline3d::getNearestSegment(const Pos3d &p) const    
  {
    Polyline3d::const_iterator retval= end();
    const size_t nv= getNumVertices();
    if(nv>=2)
      {
	list_Pos3d::const_iterator first= begin();
	GEOM_FT minDist2= getSegment(first).dist2(p);
	retval= first;
	if(nv>=3)
	  {
	    list_Pos3d::const_iterator j=first;
	    j++;
	    list_Pos3d::const_iterator last= std::prev(end());
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

//! @brief Return the points of intersection of the polyline with
//! the argument.
GeomObj3d::list_Pos3d Polyline3d::getIntersection(const Plane &p) const
  {
    list_Pos3d retval;
    list_Pos3d::iterator i= retval.end();
    Segment3d s;
    if(!empty())
      {
	list_Pos3d::const_iterator first= begin();
	list_Pos3d::const_iterator last= std::prev(end());
	for(list_Pos3d::const_iterator j=first;j != last;j++)
	  {
	    s= getSegment(j);
	    list_Pos3d lp= intersection(p,s);
	    retval.insert(i,lp.begin(),lp.end());
	    i= retval.end();
	  }
      }
    return retval;
  }

//!@brief Return the maximum value of the i-th coordinate.
GEOM_FT Polyline3d::GetMax(unsigned short int i) const
  { 
    if(empty()) return 0.0;
    const list_Pos3d::const_iterator primero= begin();
    GEOM_FT mx= (*primero)(i);
    for(list_Pos3d::const_iterator j=primero;j != end();j++)
      mx= std::max((*j)(i),mx);
    return mx;
  }

//!@brief Return the minimum value of the i-th coordinate.
GEOM_FT Polyline3d::GetMin(unsigned short int i) const
  {
    if(empty()) return 0.0;
    const list_Pos3d::const_iterator primero= begin();
    GEOM_FT mn= (*primero)(i);
    for(list_Pos3d::const_iterator j=primero;j != end();j++)
      mn= std::min((*j)(i),mn);
    return mn;
  }

//! @brief Return a Polyline3d with the vertices whose i-th coordinate
//! is greater than d.
Polyline3d Polyline3d::GetMayores(unsigned short int i,const GEOM_FT &d) const
  {
    Polyline3d retval;
    for(list_Pos3d::const_iterator j=begin();j != end();j++)
      if ((*j)(i) > d) retval.push_back(*j);
    return retval;
  }

//! @brief Return a Polyline3d with the vertices whose i-th coordinate
//! is smaller than d.
Polyline3d Polyline3d::GetMenores(unsigned short int i,const GEOM_FT &d) const
  {
    Polyline3d retval;
    for(list_Pos3d::const_iterator j=begin();j != end();j++)
      if ((*j)(i) < d) retval.push_back(*j);
    return retval;
  }

// //! @brief Return the intersections of the 3D polyline with the plane
// //! given by the equation coord_i= d
// Polyline3d::list_Pos3d Polyline3d::Int(unsigned short int i,const double &d) const
//   {
//     list_Pos3d l_int; //List of intersections
// 	list_Pos3d::const_iterator ultimo= end();
//     ultimo--;
//     for(list_Pos3d::const_iterator j= begin();j != ultimo;j++)
//       {
//         Segment3d s= getSegment(j);
//         list_Pos3d l_temp= s.Int(i,d);
//         copy(l_temp.begin(),l_temp.end(), back_inserter(l_int)); 
//       }
// 	//l_int.unique();
//      return l_int;
//   }

//    //Return a 3D polyline formed with the vertex obtainde from
//    //cutting this polyline with the plane definde by the
//    //equation coord_i= d
// Polyline3d Polyline3d::Corta(unsigned short int i,const double &d) const
//   {
//     Polyline3d result; //List of intersections
// 	list_Pos3d::const_iterator ultimo= end();
//     ultimo--;
//     for(list_Pos3d::const_iterator j=begin();j != ultimo;j++)
//       {
//         Segment3d s= getSegment(j);
//         list_Pos3d l_temp= s.Int(i,d);
//         result.push_back(*j);
//         if (!l_temp.empty()) result.push_back(*l_temp.begin());
//       }
//      result.push_back(*ultimo);
//      //result.unique();
//      return result;
//   }

//! @brief Return the moment of inertia about the x axis.
GEOM_FT Polyline3d::Ix(void) const
  {
    std::cerr << "Polyline3d::" << __FUNCTION__
              << "Ix() not implemented." << std::endl;
    return 0.0;
  }

//! @brief Return the moment of inertia about the y axis.
GEOM_FT Polyline3d::Iy(void) const
  {
    std::cerr << "Polyline3d::" << __FUNCTION__
              << "Iy() not implemented." << std::endl;
    return 0.0;
  }

//! @brief Return the moment of inertia about the z axis.
GEOM_FT Polyline3d::Iz(void) const
  {
    std::cerr << "Polyline3d::" << __FUNCTION__
              << "Iz() not implemented." << std::endl;
    return 0.0;
  }

//! @brief Return the the point at a distance "s" measured along
//! the polyline from its origin.
//! @param s: distance measured along the polyline from its origin.
Pos3d Polyline3d::getPointAtLength(const GEOM_FT &s) const
  {
    Pos3d retval;
    const const_iterator i= getSegmentAtLength(s);
    if(i!=this->end()) // found it
      {
	const GEOM_FT lengthUntil= this->getLengthUntilVertex(i);
	const GEOM_FT remainderLength= s-lengthUntil;
	const Segment3d sg= getSegment(i);
	retval= sg.PtoParametricas(remainderLength);
      }
    return retval;
  }

//! @brief Return the I vector of the segment that lies at the
//! point at a distance "s" measured along the polyline from
//! its origin.
//! @param s: distance measured along the polyline from its origin.
Vector3d Polyline3d::getIVectorAtLength(const GEOM_FT &s) const
  {
    Vector3d retval;
    const int i= getIndexOfSegmentAtLength(s);
    if(i>=0) // found it
      {
	const Segment3d sg= getSegment(i+1);
	retval= sg.getIVector();
      }
    return retval;
  }

//! @brief Return the J vector of the segment that lies at the
//! point at a distance "s" measured along the polyline from
//! its origin.
//! @param s: distance measured along the polyline from its origin.
Vector3d Polyline3d::getJVectorAtLength(const GEOM_FT &s) const
  {
    Vector3d retval;
    const int i= getIndexOfSegmentAtLength(s);
    if(i>=0) // found it
      {
	const Segment3d sg= getSegment(i+1);
	retval= sg.getJVector();
      }
    return retval;
  }

//! @brief Return the K vector of the segment that lies at the
//! point at a distance "s" measured along the polyline from
//! its origin.
//! @param s: distance measured along the polyline from its origin.
Vector3d Polyline3d::getKVectorAtLength(const GEOM_FT &s) const
  {
    Vector3d retval;
    const int i= getIndexOfSegmentAtLength(s);
    if(i>=0) // found it
      {
	const Segment3d sg= getSegment(i+1);
	retval= sg.getKVector();
      }
    return retval;
  }

//! @brief Insert the point argurment as vertex by
//! splitting the nearest segment.
void Polyline3d::insertVertex(const Pos3d &p, const GEOM_FT &tol)
  {
    const_iterator nearestVertexIter= getNearestPoint(p);
    const Pos3d nearestVertex= *nearestVertexIter;
    const GEOM_FT distToVertex= dist(nearestVertex,p);
    if(distToVertex>tol) // No vertices close to p
      {
        const_iterator vertexIter= getNearestSegment(p)+1; //Segment end vertex.
        insert(vertexIter, p);
      }
  }

//! @brief Assuming that p is a vertex of the polyline
//! Return the chunk:
//! from the beginning to p if sgn < 0
//! from p to the end if sgn >= 0
Polyline3d Polyline3d::getChunk(const Pos3d &p,const short int &sgn, const GEOM_FT &tol) const
  {
    Polyline3d result;
    const_iterator i= find(p);
    GEOM_FT distToVertex= 0.0;
    if (i == end())
      {
        i= getNearestPoint(p);
	Pos3d nearestVertex= *i;
	distToVertex= dist(nearestVertex,p);
	// Deal with the case in which two
	// vertices are equally distant from p.
	const_iterator nearestSegmentIter= getNearestSegment(p);
	const Segment3d nearestSegment= getSegment(nearestSegmentIter);
	const GEOM_FT distToSegment= nearestSegment.dist(p);
	if((distToSegment<distToVertex) and (sgn>0))
	  {
	    i= nearestSegmentIter+1; // end point of the segment.
	    nearestVertex= *i;
	    distToVertex= dist(nearestVertex,p);
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
Polyline3d Polyline3d::getLeftChunk(const Pos3d &p, const GEOM_FT &tol) const
  { return getChunk(p, -1, tol); }
  
//! @brief Return the polyline chunk that goes the nearest vertex to
//! to p to its end. If distance from vertex to p is
//! greater than tol add the vertex to the resulting polyline.
Polyline3d Polyline3d::getRightChunk(const Pos3d &p, const GEOM_FT &tol) const
  { return getChunk(p, 1, tol); }

//! @brief Return the two polylines that result from splitting
//! this one on the point nearest to the argument.
boost::python::list Polyline3d::split(const Pos3d &p) const
  {
    Polyline3d retvalA, retvalB;
    const_iterator nearestVertexIter= getNearestPoint(p);
    const Pos3d nearestVertex= *nearestVertexIter;
    const_iterator nearestSegmentIter= getNearestSegment(p);
    const Segment3d nearestSegment= getSegment(nearestSegmentIter);
    GEOM_FT distToVertex= dist(nearestVertex,p);
    GEOM_FT distToSegment= nearestSegment.dist(p);
    if(distToVertex<=distToSegment)
      {
        retvalA= getLeftChunk(nearestVertex,0.0);
	retvalB= getRightChunk(nearestVertex,0.0);
      }
    else
      {
	const Pos3d splittingVertexA= *nearestSegmentIter;
	retvalA= getLeftChunk(splittingVertexA,0.0);
	retvalA.push_back(p);
	const Pos3d splittingVertexB= *(nearestSegmentIter+1);
	retvalB= getRightChunk(splittingVertexB,0.0);
	retvalB.push_front(p);
      }
    boost::python::list retval;
    retval.append(retvalA);
    retval.append(retvalB);
    return retval;
  }

/**
   * @param i1 iterator to the first point.
   * @param i2 iterator to the second point.
   * @param pMaxDist pointer to the maximum distance of _line[return index].
   * @return the index of the point farthest from the segment (t1,t2).
   */
Polyline3d::iterator Polyline3d::getFarthestPointFromSegment(iterator it1, iterator it2, GEOM_FT &pMaxDist)
  {
    // Keep track of the point with the maximum distance.
    iterator maxIt = it1;
    maxIt++;
    Segment3d s= Segment3d(*it1,*it2);
    GEOM_FT maxDist= s.dist(*maxIt);

    iterator it = it1;
    it++;
    while(it != it2)
      {
        Segment3d s= Segment3d(*it1,*it2);
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


void Polyline3d::Print(std::ostream &stream) const
  {
    if(empty()) return;
    Polyline3d::list_Pos3d::const_iterator i= begin();
    stream << *i; i++;
    for(; i!=end(); i++)
      stream << ", " << *i;
  }
