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
//Polyline2d.cc

#include "Polyline2d.h"
#include "Segment2d.h"
#include <plotter.h>

#include "utility/geom/lists/utils_list_pos2d.h"
#include "utility/geom/trf/Trf2d.h"


#include "utility/geom/pos_vec/Pos2dList.h"

//! @brief Default constructor.
Polyline2d::Polyline2d(void)
  : Linear2d(), GeomObj::list_Pos2d() {}

//! @brief Constructor.
Polyline2d::Polyline2d(const GeomObj::list_Pos2d &l)
  : Linear2d(), GeomObj::list_Pos2d(l){}

//! @brief Constructor.
Polyline2d::Polyline2d(const Pos2dList &l)
  : Linear2d(), GeomObj::list_Pos2d(l.getPoints()){}

//! @brief Constructor (Python interface).
Polyline2d::Polyline2d(const boost::python::list &l)
  {
    const int sz= len(l);
    // copy the components
    for(int i=0; i<sz; i++)
      push_back(boost::python::extract<Pos2d>(l[i]));
  }

const Pos2d *Polyline2d::AgregaVertice(const Pos2d &p)
  {
    GeomObj::list_Pos2d::push_back(p);
    return &(*GeomObj::list_Pos2d::rbegin());
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

//! @brief Return the list of the vertices.
const GeomObj::list_Pos2d &Polyline2d::getVertexList(void) const
  { return *this; }

//! @brief Return a Python list containing the positions
//! of the polygon vertices.
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
    for(const_iterator j=begin();j != end();j++)
      if(getSegment(j).In(p,tol)) return true;
    return false;
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

//! @brief Return el vértice i-ésimo (el primero es el 1).
const Pos2d &Polyline2d::Vertice(const size_t &i) const
  { return GeomObj::list_Pos2d::operator[](i-1); }

//! @brief Return a polyline parallel to this one at the distance
//! being passed as parameter. If the distance is positive the new
//! polyline will be placed on the right.
Polyline2d Polyline2d::Offset(const GEOM_FT &d) const
  {
    Pos2dList tmp(*this);
    Polyline2d retval(tmp.Offset(d));
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

//! @brief Suponemos que p es vertice de la Polyline2d
//! Return el trozo de Polyline2d:
//! hasta p si sgn < 0
//! desde p si sgn >= 0
Polyline2d Polyline2d::Separa(const Pos2d &p,const short int &sgn) const
  {
    Polyline2d result;
    const_iterator i= find(p);
    if (i == end()) return result;
    if (sgn < 0)
      copy(begin(),++i,back_inserter(result));
    else
      copy(i,end(),back_inserter(result));
    return result;
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
          retval.Agrega(tmp);
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
          retval.Agrega(tmp);
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
          retval.Agrega(tmp);
      }
    return retval;
  }

/**
   * @param i1 iterator to the first point.
   * @param i2 iterator to the second point.
   * @param pMaxDist pointer to the maximum distance of _line[return index].
   * @return the index of the point farthest fromthe segment (t1,t2).
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
