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

Quadrilateral2d::Quadrilateral2d(const Pos2d &p1,const Pos2d &p2,const Pos2d &p3,const Pos2d &p4): Polygon2d()
  {
    push_back(p1);
    push_back(p2);
    push_back(p3);
    push_back(p4);
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
