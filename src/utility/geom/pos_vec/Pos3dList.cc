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
//Pos3dList.cc

#include "Pos3dList.h"
#include <plotter.h>

#include "utility/geom/trf/Translation3d.h"
#include "utility/geom/lists/utils_list_pos3d.h"
#include "utility/geom/d3/BND3d.h"
#include "CGAL/linear_least_squares_fitting_3.h"
#include "utility/geom/d2/Polygon3d.h"
#include "utility/geom/d2/Plane.h"

//! @brief Default constructor.
Pos3dList::Pos3dList(void)
  : GeomObj3d(), point_lst() {}

//! @brief Constructor.
Pos3dList::Pos3dList(const GeomObj::list_Pos3d &l)
  : GeomObj3d(), point_lst(l) {}

//! @brief Constructor.
Pos3dList::Pos3dList(const boost::python::list &l)
  : GeomObj3d(), point_lst()
  {
    const size_t sz= len(l);
    for(size_t i=0; i<sz; i++)
      point_lst.push_back(boost::python::extract<Pos3d>(l[i]));
  }

const Pos3d *Pos3dList::appendPoint(const Pos3d &p)
  {
    point_lst.push_back(p);
    return &(*point_lst.rbegin());
  }

//! @brief Applies the transformation to the points.
void Pos3dList::Transform(const Trf3d &trf3d)
  { trf3d.Transform(point_lst.begin(),point_lst.end()); }


//! @brief Return a reference al objeto cuyo
//! índice se pasa como parámetro.
Pos3d &Pos3dList::operator[](const size_t &i)
  {
    if(i<point_lst.size())
      return point_lst[i];
    else
      {
        std::cerr << "Pos3dList; indice: " << i << " fuera de rango. "
                  << std::endl;
        exit(0);
      }
  }

//! @brief Return a reference al objeto cuyo
//! índice se pasa como parámetro.
const Pos3d &Pos3dList::operator[](const size_t &i) const
  {
    if(i<point_lst.size())
      return point_lst[i];
    else
      {
        std::cerr << "Pos3dList; indice: " << i << " fuera de rango. "
                  << std::endl;
        exit(0);
      }
  }

//! @brief Returns true if the points belongs to the list.
bool Pos3dList::In(const Pos3d &p, const double &tol) const
  {
    for(point_const_iterator j=point_lst.begin();j != point_lst.end();j++)
      if(dist2(*j,p)<=tol) return true;
    return false;
  }
//! @brief Return the maximum value of the i coordinate.
GEOM_FT Pos3dList::GetMax(unsigned short int i) const
  { return point_lst.GetMax(i); }

//! @brief Return the minimum value of the i coordinate.
GEOM_FT Pos3dList::GetMin(unsigned short int i) const
  { return point_lst.GetMin(i); }

//! @brief Return a Pos2dList with the points which i coordinate is greater
//! than d.
Pos3dList Pos3dList::GetMayores(unsigned short int i,const GEOM_FT &d) const
  {
    Pos3dList retval;
    retval.point_lst= point_lst.GetMayores(i,d);
    return retval;
  }

//! @brief Return a Pos2dList with the points which i coordinate is less
//! than d.
Pos3dList Pos3dList::GetMenores(unsigned short int i,const GEOM_FT &d) const
  {
    Pos3dList retval;
    retval.point_lst= point_lst.GetMenores(i,d);
    return retval;
  }


//! @brief Return el vértice i-ésimo (el primero es el 1).
const Pos3d &Pos3dList::Point(const size_t &i) const
  { return point_lst[i-1]; }


GEOM_FT Pos3dList::Ix(void) const
  {
    std::cerr << "Pos3dList Ix() not implemented" << std::endl;
    return 0.0;
  }
GEOM_FT Pos3dList::Iy(void) const
  {
    std::cerr << "Pos3dList Iy() not implemented" << std::endl;
    return 0.0;
  }
GEOM_FT Pos3dList::Iz(void) const
  {
    std::cerr << "Pos3dList Iz() not implemented" << std::endl;
    return 0.0;
  }

std::deque<GEOM_FT> &Pos3dList::GetSeparaciones(void) const
  { return point_lst.GetSeparaciones(); }

double Pos3dList::GetSeparacionMedia(void) const
  { return point_lst.GetSeparacionMedia(); }

CGPlane_3 Pos3dList::linearLeastSquaresFittingPlane(GEOM_FT &quality)
  {
    CGPlane_3 retval;
    std::list<CGPoint_3> points;
    for(point_const_iterator i= point_lst.begin(); i!=point_lst.end();i++)
      points.push_back((*i).ToCGAL()); 
    quality= linear_least_squares_fitting_3(points.begin(),points.end(),retval,CGAL::Dimension_tag<0>());
    return retval;
  }

//! @brief Return an average k vector by computing the average of
//! of the cross products of the vectors joining the centroid of
//! the point list with each point.
Vector3d Pos3dList::getAvgKVector(void) const
  {
    Vector3d retval;
    const size_t sz= point_lst.size();
    if(sz<3)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; point list too short at least 3 points needed."
	        << std::endl;
    else
      {
        const Pos3d O= getCenterOfMass();
	point_const_iterator i= point_lst.begin();
        Pos3d A= *i;
	do
	  {
	    point_const_iterator j= ++i;
	    if(j!=point_lst.end())
	      {
		const Pos3d B= *j;
		const Vector3d OA= A-O;
		const Vector3d OB= B-O;
		retval+= OA.getCross(OB);
		A= B;
	      }
	    else
	      break;
 	  }
	while(i!=point_lst.end());
        retval.Normalize();
      }
    return retval;
  }

//! @brief Return true if the point list is oriented clockwise.
//! @param vPoint: position of the point of view.
bool Pos3dList::clockwise(const Pos3d &vPoint) const
  { return !counterclockwise(vPoint); }

//! @brief Return true if the point list is oriented counterclockwise.
//! @param vPoint: position of the point of view.
bool Pos3dList::counterclockwise(const Pos3d &vPoint) const
  {
    bool retval= false;
    if(point_lst.size()<3)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; point list too short at least 3 points needed."
	        << std::endl;
    else
      {
        const Pos3d O= getCenterOfMass();
	const Vector3d K= getAvgKVector();
        const Plane plane(O, K);
	const Ref2d3d ref= Ref2d3d(O,plane.Base1(), plane.Base2());
	Polygon3d tmp(ref);
	for(point_const_iterator i= point_lst.begin(); i!= point_lst.end(); i++)
	  tmp.push_back(*i);
	retval= tmp.counterclockwise(vPoint);
      }  
    return retval;
  }

//! @brief Return the orientation of the point list (clockwise or
//! counterclockwise).
//! @param vPoint: position of the point of view.
std::string Pos3dList::orientation(const Pos3d &vPoint) const
  {
    std::string retval= "counterclockwise";
    if(clockwise(vPoint))
      { retval= "clockwise"; }
    return retval;
  }
//! @brief Print stuff.
void Pos3dList::Print(std::ostream &stream) const
  {
    if(!point_lst.empty())
      {
        point_const_iterator i= point_lst.begin();
        stream << *i; i++;
        for(; i!=point_lst.end(); i++)
          stream << ", " << *i;
      }
  }

void Pos3dList::Plot(Plotter &plotter) const
  {
    std::cerr << "Pos3dList::Plot not implemented." << std::endl;
  }
