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
//Polyline3d.cc

#include "Polyline3d.h"
#include "utility/geom/d2/Plane.h"

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

const Pos3d *Polyline3d::AgregaVertice(const Pos3d &p)
  {
    GeomObj::list_Pos3d::push_back(p);
    return &(*rbegin());
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
bool Polyline3d::In(const Pos3d &p, const double &tol) const
  {
    for(list_Pos3d::const_iterator j=begin();j != end();j++)
      if(getSegment(j).In(p,tol)) return true;
    return false;
  }

GeomObj3d::list_Pos3d Polyline3d::getIntersection(const Plane &p) const
  {
    list_Pos3d retval;
    list_Pos3d::iterator i= retval.end();
    Segment3d s;
    for(list_Pos3d::const_iterator j=begin();j != end();j++)
      {
        s= getSegment(j);
        list_Pos3d lp= intersection(p,s);
        retval.insert(i,lp.begin(),lp.end());
        i= retval.end();
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

//! @brief Suponemos que p es vertice de la Polyline3d
//! Return el trozo de Polyline3d:
//! hasta p si sgn < 0
//! desde p si sgn >= 0
Polyline3d Polyline3d::Separa(const Pos3d &p,const short int &sgn) const
  {
    Polyline3d result;
    GeomObj::list_Pos3d::const_iterator i= find(p);
    if (i == end()) return result;
    if(sgn < 0)
      copy(begin(),++i,back_inserter(result));
    else
      copy(i,end(),back_inserter(result));
    return result;
  }

//! @brief If the segment argument is not connected to this segment, return
//! (-1,-1) if it's connected at its first point, return (0,0) or (0,1)
//! or else return (1,0) or (1,1).
std::pair<int,int> Polyline3d::connected(const Segment3d &s, const GEOM_FT &tol) const
  {
    std::pair<int,int> retval(-1,-1);
    const Pos3d p00= getFromPoint();
    const Pos3d p01= getToPoint();
    const Pos3d p10= s.getFromPoint();
    const Pos3d p11= s.getToPoint();
    const GEOM_FT tol2= tol*tol;
    const bool connected00= (p00.dist2(p10)<tol2);
    const bool connected01= (p00.dist2(p11)<tol2);
    const bool connected10= (p10.dist2(p10)<tol2);
    const bool connected11= (p01.dist2(p11)<tol2);
    if(connected00) // connected (0,0)
      {
	retval= std::pair<int,int>(0,0);
	if(connected11) // connected (1,1) too
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; error: both segments are the same."
		      << std::endl;
	    retval= std::pair<int,int>(-1,-1);
	  }
      }
    else if(connected01) // connected (0,1)
      { retval= std::pair<int,int>(0,1); }
    else if(connected10) // connected (1,0)
      { retval= std::pair<int,int>(1,0); }
    else if(connected11) // connected (1,1)
      { retval= std::pair<int,int>(1,1); }
    return retval;
  }

//! @brief Connect the segment to the polyline if they are connected
//! (they share an end vertex).
bool Polyline3d::connect(const Segment3d &s, const GEOM_FT &tol)
  {
    bool retval= false;
    std::pair<int,int> conn= connected(s);
    if(conn == std::pair<int,int>(0,0)) // connected (from, from)
      {
	const Pos3d newPoint= s.getToPoint();
	push_front(newPoint);
	retval= true;
      }
    else if(conn == std::pair<int,int>(0,1)) // connected (from, to)
      {
	const Pos3d newPoint= s.getFromPoint();
	push_front(newPoint);
	retval= true;
      }
    else if(conn == std::pair<int,int>(1,0)) // connected (to, from)
      {
	const Pos3d newPoint= s.getToPoint();
	push_back(newPoint);
	retval= true;
      }
    else if(conn == std::pair<int,int>(1,1)) // connected (to, to)
      {
	const Pos3d newPoint= s.getFromPoint();
	push_back(newPoint);
	retval= true;
      }
    return retval;
  }

/**
   * @param i1 iterator to the first point.
   * @param i2 iterator to the second point.
   * @param pMaxDist pointer to the maximum distance of _line[return index].
   * @return the index of the point farthest fromthe segment (t1,t2).
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
