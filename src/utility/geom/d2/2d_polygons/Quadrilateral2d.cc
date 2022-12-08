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

//! @brief Return the values of the shape functions for the point argument.
std::vector<double> Quadrilateral2d::Ni(const Pos2d &p) const
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
    const double x12= x1-x2;
    const double x34= x3-x4;
    const double y14= y1-y4;
    const double y23= y2-y3;
    // Get cartesian coordinates of the point.
    const double x= p.x();
    const double y= p.y();
    // Compute values of the shape functions.
    std::vector<double> retval(4,0.0);
    retval[0]= (x-x2)/(x12)*(y-y4)/(y14);
    retval[1]= (x1-x)/(x12)*(y-y3)/(y23);
    retval[2]= (x-x4)/(x34)*(y2-y)/(y23);
    retval[3]= (x3-x)/(x34)*(y1-y)/(y14);
    return retval;
  }
//! @brief Return a Python list containing the values of the shape functions for the point argument.
boost::python::list Quadrilateral2d::NiPy(const Pos2d &p) const
  {
    boost::python::list retval;
    std::vector<double> tmp= Ni(p);
    std::vector<double>::const_iterator i= tmp.begin();
    for(;i!=tmp.end();i++)
      retval.append(*i);
    return retval;
  }

Triangle2d Quadrilateral2d::getFirstTriangle(void) const
  { return Triangle2d(Vertice(1),Vertice(2),Vertice(3)); }
Triangle2d Quadrilateral2d::getSecondTriangle(void) const
  { return Triangle2d(Vertice(1),Vertice(3),Vertice(4)); }


Pos2dArray Quadrilateral2d::genMesh(int n1,int n2) const
  {
    Pos2dArray l1(Vertice(1),Vertice(2),n1);
    Pos2dArray l2(Vertice(2),Vertice(3),n2);
    Pos2dArray l3(Vertice(4),Vertice(3),n1);
    Pos2dArray l4(Vertice(1),Vertice(4),n2);
    return Pos2dArray(l1,l2,l3,l4);
  }

//! @brief Returns a point mesh (la rutina esta tomada de OpenSees).
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
                  x+= N[s-1] * Vertice(s).x();
                  y+= N[s-1] * Vertice(s).y();
                }
              retval(j+1,i+1)= Pos2d(x,y);
            }
      }
    return retval;
  }
