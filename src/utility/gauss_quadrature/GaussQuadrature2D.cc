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
//GaussQuadrature2D.cc

#include "GaussQuadrature2D.h"


GaussQuadrature2D::GaussQuadrature2D( double x1, double x2,double y1, double y2,int nx, int ny) : ConjPG2(nx*ny)
  {
    int i= 1, j= 1;
    GaussPoints x(x1,x2,nx);
    GaussPoints y(y1,y2,ny);
    GaussPoints::const_iterator ii,jj;
    for(ii=x.begin();ii!=x.end();ii++)
      {
        j= 1;
        for(jj=y.begin();jj!=y.end();jj++)
          {
            (*this)[(i-1)*ny+j-1] = GaussWeights2D(x[i].x, y[j].x, x[i].w*y[j].w);
            j++;
          }
        i++;
      }
  }
std::ostream& operator<<(std::ostream& o, const GaussQuadrature2D& gpts)
  {
    GaussQuadrature2D::const_iterator i;
    for(i=gpts.begin();i!=gpts.end();i++)
      o << *i << ", ";
    return o;
  }
