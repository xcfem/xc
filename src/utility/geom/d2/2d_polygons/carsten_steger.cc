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
//Carsten_Steger.cc

#include "carsten_steger.h"
#include "utility/functions/statistics/combinatorics.h"
#include "utility/geom/d2/2d_polygons/PolygonalSurface2d.h"

inline long long c_p_q_k_l(const int &p,
                                    const int &q,
                                    const int &k,
                                    const int &l)
  { return coeficiente_binomial(k+l,l)*coeficiente_binomial(p+q-k-l,q-l); }

inline double potencias(const double &xi,const double &xi_1, const double &yi,const double &yi_1,
                        const int &p, const int &q,
                        const int &k, const int &l)
  { return pow(xi,k)*pow(xi_1,p-k)*pow(yi,l)*pow(yi_1,q-l); }

inline double sumando_p_q_k_l(const double &xi,const double &xi_1, 
                              const double &yi,const double &yi_1,
                              const int &p, const int &q,
                              const int &k, const int &l)
  { return c_p_q_k_l(p,q,k,l)*potencias(xi,xi_1,yi,yi_1,p,q,k,l); }

double sumatorio_k_l(const double &xi,const double &xi_1, 
                     const double &yi,const double &yi_1,
                     const int &p, const int &q)
  {
    double retval= 0.0;
    for(int k=0;k<=p;k++)
      for(int l=0;l<=q;l++)
        retval+=sumando_p_q_k_l(xi,xi_1,yi,yi_1,p,q,k,l);
    return retval;
  }
double sumatorio_i_p_q(const PolygonalSurface2d &sp,const int &p, const int &q)
  {
    double retval= 0;
    const size_t num_vertices= sp.getNumVertices();
    for(size_t i=1;i<=num_vertices;i++)
      {
        
        const size_t imas1= i%num_vertices+1;

        const Pos2d vi= sp.Vertice(imas1);
        const Pos2d vi_1= sp.Vertice(i);

        const double xi= vi.x();
        const double xi_1= vi_1.x();
        const double yi= vi.y();
        const double yi_1= vi_1.y();
        retval+=(xi_1*yi-xi*yi_1)*sumatorio_k_l(xi,xi_1,yi,yi_1,p,q);
      }
    return retval;
  }
inline long long divisor(const int &p, const int &q)
  { return (p+q+2)*(p+q+1)*coeficiente_binomial(p+q,q); }
GEOM_FT p_q_moment(const PolygonalSurface2d &sp,const int &p, const int &q)
  { return double_to_FT(sumatorio_i_p_q(sp,p,q)/divisor(p,q)); }
