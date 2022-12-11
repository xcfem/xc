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
//Quadrilateral2d.cc

#include "Quadrilateral2d.h"
#include "Triangle2d.h"
#include "utility/geom/pos_vec/Pos2dArray.h"
#include "utility/geom/d1/Segment2d.h"
#include <vector>

// Vertex order: 1->2->3->4.
//
// 4 +---------------+ 3
//   |               |
//   |               |
//   |               |
// 1 +---------------+ 2

//! @brief Default constructor.
Quadrilateral2d::Quadrilateral2d(void)
  : Polygon2d() 
  {
    push_back(Pos2d(0,0));
    push_back(Pos2d(1,0));
    push_back(Pos2d(1,1));
    push_back(Pos2d(0,1));
  }

//! @brief Constructor.
Quadrilateral2d::Quadrilateral2d(const Pos2d &p1,const Pos2d &p2,const Pos2d &p3,const Pos2d &p4): Polygon2d()
  {
    push_back(p1);
    push_back(p2);
    push_back(p3);
    push_back(p4);
  }

//! @brief Back inserter.
void Quadrilateral2d::push_back(const Pos2d &p)
  {
    if(getNumVertices()<5)
      Polygon2d::push_back(p);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< " maximum number of vertices reached." << std::endl;
  }

//! @brief Return natural coordinates for point xy (in cartesian coord.)
//! based on \$ 23.6 from Felippa book:"Introduction to Finite Element Methods"
//! see IFEM.Ch23.pdf
//!
//! @param p: position in cartesian coordinates.
std::vector<double> Quadrilateral2d::getNaturalCoordinates(const Pos2d &p) const
  {
    std::vector<double> retval(2,0.0);
    // Store convenience values.
    const Pos2d &v1= Vertice(1); const Pos2d &v2= Vertice(2);
    const Pos2d &v3= Vertice(3); const Pos2d &v4= Vertice(4);
    const double &x1= v1.x();
    const double &y1= v1.y();
    const double &x2= v2.x();
    const double &y2= v2.y();
    const double &x3= v3.x();
    const double &y3= v3.y();
    const double &x4= v4.x();
    const double &y4= v4.y();
    // Compute natural coordinates.
    const double xb= x1-x2+x3-x4;
    const double yb= y1-y2+y3-y4;
    const double xcChi= x1+x2-x3-x4;
    const double ycChi= y1+y2-y3-y4;
    const double xcEta= x1-x2-x3+x4;
    const double ycEta= y1-y2-y3+y4;
    const double x0= (x1+x2+x3+x4)/4.0;
    const double y0= (y1+y2+y3+y4)/4.0;
    const double J0= (x3-x1)*(y4-y2)-(x4-x2)*(y3-y1); 
    const double A= J0/2.0;
    const double J1= (x3-x4)*(y1-y2)-(x1-x2)*(y3-y4);
    const double J2= (x2-x3)*(y1-y4)-(x1-x4)*(y2-y3);
    
    const double dx= p.x()-x0;
    const double dy= p.y()-y0;
    const double bChi= A-dx*yb+dy*xb;
    const double bEta= -A-dx*yb+dy*xb;
    const double cChi= dx*ycChi-dy*xcChi;
    const double cEta= dx*ycEta-dy*xcEta;
    retval[0]= 2*cChi/(-sqrt(bChi*bChi-2*J1*cChi)-bChi);
    retval[1]= 2*cEta/( sqrt(bEta*bEta+2*J2*cEta)-bEta);
    return retval;
  }

//! @brief Return natural coordinates for points xy (in cartesian coord.)
//! @param points: positions in cartesian coordinates.
std::vector<std::vector<double> > Quadrilateral2d::getNaturalCoordinates(const std::list<Pos2d> &points) const
  {    
    // Store convenience values.
    const Pos2d &v1= Vertice(1); const Pos2d &v2= Vertice(2);
    const Pos2d &v3= Vertice(3); const Pos2d &v4= Vertice(4);
    const double &x1= v1.x();
    const double &y1= v1.y();
    const double &x2= v2.x();
    const double &y2= v2.y();
    const double &x3= v3.x();
    const double &y3= v3.y();
    const double &x4= v4.x();
    const double &y4= v4.y();
    // Compute natural coordinates.
    const double xb= x1-x2+x3-x4;
    const double yb= y1-y2+y3-y4;
    const double xcChi= x1+x2-x3-x4;
    const double ycChi= y1+y2-y3-y4;
    const double xcEta= x1-x2-x3+x4;
    const double ycEta= y1-y2-y3+y4;
    const double x0= (x1+x2+x3+x4)/4.0;
    const double y0= (y1+y2+y3+y4)/4.0;
    const double J0= (x3-x1)*(y4-y2)-(x4-x2)*(y3-y1); 
    const double A= J0/2.0;
    const double J1= (x3-x4)*(y1-y2)-(x1-x2)*(y3-y4);
    const double J2= (x2-x3)*(y1-y4)-(x1-x4)*(y2-y3);

    const size_t sz= points.size();
    std::vector<double> retval_i(2,0.0);
    std::vector<std::vector<double> > retval(sz, retval_i);
    size_t count= 0;
    for(std::list<Pos2d>::const_iterator i= points.begin(); i!=points.end(); i++, count++)
      {
	const Pos2d &p= *i;
	const double dx= p.x()-x0;
	const double dy= p.y()-y0;
	const double bChi= A-dx*yb+dy*xb;
	const double bEta= -A-dx*yb+dy*xb;
	const double cChi= dx*ycChi-dy*xcChi;
	const double cEta= dx*ycEta-dy*xcEta;
	retval_i[0]= 2*cChi/(-sqrt(bChi*bChi-2*J1*cChi)-bChi);
	retval_i[1]= 2*cEta/( sqrt(bEta*bEta+2*J2*cEta)-bEta);
	retval[count]= retval_i;
      }
    return retval;
  }

//! @brief Return natural coordinates for points xy (in cartesian coord.)
//! @param points: positions in cartesian coordinates.
boost::python::list Quadrilateral2d::getNaturalCoordinatesPy(const boost::python::list &points) const
  {
    const int sz= len(points);
    std::list<Pos2d> tmp;
    // populate std::list
    for(int i=0; i<sz; i++)
      tmp.push_back(boost::python::extract<Pos2d>(points[i]));
    // call "pure" c++ method.
    std::vector<std::vector<double> > naturalCoord= this->getNaturalCoordinates(tmp);
    // populate returned list.
    boost::python::list retval;
    std::vector<std::vector<double> >::const_iterator i= naturalCoord.begin();
    for(;i!=naturalCoord.end();i++)
      {
	const std::vector<double> &tmp_i= *i;
	boost::python::list retval_i;
	retval_i.append(tmp_i[0]); // r coordinate.
	retval_i.append(tmp_i[1]); // s coordinate.
        retval.append(retval_i);
      }
    return retval;    
  }

//! @brief Return the values of the shape functions for the point argument.
//! @param p: point where the shape functions are evaluated.
std::vector<double> Quadrilateral2d::Ni(const Pos2d &p) const
  {
    // Compute natural coordinates.
    std::vector<double> naturalCoord= this->getNaturalCoordinates(p);
    // Store convenience values.
    const double oneMinusR= 1.0-naturalCoord[0];
    const double onePlusR= 1.0+naturalCoord[0];
    const double oneMinusS= 1.0-naturalCoord[1];
    const double onePlusS= 1.0+naturalCoord[1];
    // Compute values of the shape functions.
    std::vector<double> retval(4,0.0);
    retval[0]= oneMinusR*oneMinusS/4.0;
    retval[1]= onePlusR*oneMinusS/4.0;
    retval[2]= onePlusR*onePlusS/4.0;
    retval[3]= oneMinusR*onePlusS/4.0;
    return retval;
  }

//! @brief Return a Python list containing the values of the shape functions
//! for the point argument.
//! @param p: point where the shape functions are evaluated.
boost::python::list Quadrilateral2d::NiPy(const Pos2d &p) const
  {
    boost::python::list retval;
    std::vector<double> tmp= Ni(p);
    std::vector<double>::const_iterator i= tmp.begin();
    for(;i!=tmp.end();i++)
      retval.append(*i);
    return retval;
  }

//! @brief Return a vector of vectors containing the values of the shape
//! functions for the points in the argument list.
//! @param points: points where the shape functions are evaluated.
std::vector<std::vector<double> > Quadrilateral2d::Ni(const std::list<Pos2d> &points) const
  {
    // Compute natural coordinates.
    std::vector<std::vector<double> > naturalCoord= this->getNaturalCoordinates(points);
    // Prepare return values.
    std::vector<double> retval_i(4,0.0);
    const size_t sz= points.size();
    std::vector<std::vector<double> > retval(sz, retval_i);
    // Iterate through the computed natural coordinates.
    size_t count= 0;
    std::vector<std::vector<double> >::const_iterator i= naturalCoord.begin();
    for(;i!= naturalCoord.end();i++, count++)
      {
	const std::vector<double> &naturalCoord_i= *i; //i-th natural coordinates.
	// Store convenience values.
	const double oneMinusR= 1.0-naturalCoord_i[0];
	const double onePlusR= 1.0+naturalCoord_i[0];
	const double oneMinusS= 1.0-naturalCoord_i[1];
	const double onePlusS= 1.0+naturalCoord_i[1];
	// Compute values of the shape functions.
	retval_i[0]= oneMinusR*oneMinusS/4.0;
	retval_i[1]= onePlusR*oneMinusS/4.0;
	retval_i[2]= onePlusR*onePlusS/4.0;
	retval_i[3]= oneMinusR*onePlusS/4.0;
	retval[count]= retval_i;
      }
    return retval;
  }

//! @brief Return a Python list containing the values of the shape
//! functions for the points in the argument list.
//! @param points: points where the shape functions are evaluated.
boost::python::list Quadrilateral2d::NiPy(const boost::python::list &points) const
  {
    const int sz= len(points);
    std::list<Pos2d> tmp;
    // populate std::list
    for(int i=0; i<sz; i++)
      tmp.push_back(boost::python::extract<Pos2d>(points[i]));
    // call "pure" c++ method.
    std::vector<std::vector<double> > Ni= this->Ni(tmp);
    // populate returned list.
    boost::python::list retval;
    std::vector<std::vector<double> >::const_iterator j= Ni.begin();
    for(;j!=Ni.end();j++)
      {
	const std::vector<double> &Ni_j= *j; //j-th shape functions values.
	boost::python::list retval_i;
	retval_i.append(Ni_j[0]);
	retval_i.append(Ni_j[1]);
	retval_i.append(Ni_j[2]);
	retval_i.append(Ni_j[3]);
        retval.append(retval_i);
      }
    return retval;    
  }

//! @brief Return the intersection between the lines that join the midpoints
//! of the quadrilateral sides.
Pos2d Quadrilateral2d::getMidpoint(void) const
  {
    // Get vertices.
    const Pos2d &v1= Vertice(1); const Pos2d &v2= Vertice(2);
    const Pos2d &v3= Vertice(3); const Pos2d &v4= Vertice(4);

    // Compute midpoints of the sides.
    //            p3
    // v4 +-------x-------+ v3
    //    |               |
    // p4 x               x p2
    //    |               |
    // v1 +-------x-------+ v2
    //          p1
    Pos2d p1= Segment2d(v1,v2).getMidPoint();
    Pos2d p2= Segment2d(v2,v3).getMidPoint();
    Pos2d p3= Segment2d(v3,v4).getMidPoint();
    Pos2d p4= Segment2d(v4,v1).getMidPoint();

    // Compute quadrilateral "axis".
    Segment2d yAxis(p1, p3);
    Segment2d xAxis(p4, p2);
    Pos2d retval;
    GeomObj2d::list_Pos2d tmp= xAxis.getIntersection(yAxis);
    if(!tmp.empty())
      retval= (*tmp.begin());
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< " intersection between quadrilateral \"axis\" not found." << std::endl;
    return retval;
  }

Triangle2d Quadrilateral2d::getFirstTriangle(void) const
  { return Triangle2d(Vertice(1),Vertice(2),Vertice(3)); }
Triangle2d Quadrilateral2d::getSecondTriangle(void) const
  { return Triangle2d(Vertice(1),Vertice(3),Vertice(4)); }

//! @brief Returns a point grid.
Pos2dArray Quadrilateral2d::genMesh(int n1,int n2) const
  {
    Pos2dArray l1(Vertice(1),Vertice(2),n1);
    Pos2dArray l2(Vertice(2),Vertice(3),n2);
    Pos2dArray l3(Vertice(4),Vertice(3),n1);
    Pos2dArray l4(Vertice(1),Vertice(4),n2);
    return Pos2dArray(l1,l2,l3,l4);
  }

//! @brief Returns a point grid (the routine is taken from OpenSees).
Pos2dArray Quadrilateral2d::genBilinMesh(const size_t &nDiv12,const size_t &nDiv23) const
  {
    Pos2dArray retval(nDiv23+1,nDiv12+1);
    if(nDiv12 > 0  && nDiv23 > 0)
      {
        const double deltaXi= 2.0 / nDiv12;
        const double deltaEta= 2.0 / nDiv23;

        double xi, eta, x, y;
	double N[4];
        for(size_t j= 0;j<nDiv23+1;j++)
          for(size_t i= 0;i<nDiv12+1;i++)
            {
              //natural coordinates of the points
              xi= -1.0 + deltaXi  * i;
              eta= -1.0 + deltaEta * j;

              // map to cartesian coordinates using bilinear
              // shape functions
              N[0]= (1.0 - xi)*(1.0 - eta)/4.0;
              N[1]= (1.0 + xi)*(1.0 - eta)/4.0;
              N[2]= (1.0 + xi)*(1.0 + eta)/4.0;
              N[3]= (1.0 - xi)*(1.0 + eta)/4.0;
              x= 0.0; y= 0.0;
              for(size_t s=1;s<=4;s++)
                {
		  const Pos2d &v= Vertice(s);
                  x+= N[s-1] * v.x();
                  y+= N[s-1] * v.y();
                }
              retval(j+1,i+1)= Pos2d(x,y);
            }
      }
    return retval;
  }
