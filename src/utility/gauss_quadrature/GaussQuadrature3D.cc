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
//GaussQuadrature3D.cc

#include "GaussQuadrature3D.h"
#include "utility/geom/pos_vec/Pos3d.h"

GaussQuadrature3D::GaussQuadrature3D(const Pos3d &p1,const Pos3d &p2,int nx, int ny, int nz): ConjPG3(nx*ny*nz)
  {
    int i= 0,j= 0, k=0,l= 0;
    GaussPoints x(p1.x(),p2.x(),nx);
    GaussPoints y(p1.y(),p2.y(),ny);
    GaussPoints z(p1.z(),p2.z(),nz);
    GaussWeights1D wx, wy, wz;
    num_dim=0;
    if (nx>1) num_dim++;
    if (ny>1) num_dim++;
    if (nz>1) num_dim++;
    if (num_dim==0) num_dim++;
	GaussPoints::const_iterator ii;
    GaussPoints::const_iterator jj;
    GaussPoints::const_iterator kk;
    for(ii=x.begin();ii!=x.end();ii++)
      {
        wx= x.Get(i);
        j= 0;
        for(jj=y.begin();jj!=y.end();jj++)
          {
            wy= y.Get(j);
            k= 0;
            for(kk=z.begin();kk!=z.end();kk++)
              {
                wz= z.Get(k);
                Pos3d c(wx.x,wy.x,wz.x);
                double w=1.0;
                if (nx > 1) w*=wx.w;
                if (ny > 1) w*=wy.w;
                if (nz > 1) w*=wz.w;
                (*this)[l]= GaussWeights3D(wx.x,wy.x,wz.x, w);
                l++;
                k++;
              }
            j++;
          }
        i++;
      }
  }
std::ostream& operator<<(std::ostream& o, const GaussQuadrature3D& gpts)
  {
    GaussQuadrature3D::const_iterator i;
    for(i=gpts.begin();i!=gpts.end();i++)
      o << *i << ", ";
    return o;
  }
