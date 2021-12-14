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
//Pos2dArray3d.cc

#include "Pos2dArray3d.h"
#include "Pos2dArray.h"
#include "utility/geom/d1/Segment2d.h"
#include "utility/geom/d2/BND2d.h"


Pos2dArray3d::Pos2dArray3d(const size_t iLayers)
  : PosArray3d<Pos2d>(iLayers) {}

Pos2d Pos2dArray3d::getCenter(void) const
  { return get_centro(*this,Segment2d()); }

Pos2dArray3d create_uniform_grid(const BND2d &bnd,const size_t &ndiv_x,const size_t &ndiv_y,const size_t &ndiv_z)
  {
    const size_t n_layers= ndiv_z+1;
    Pos2dArray3d retval(n_layers);
    Pos2d pmin= bnd.GetPMin();
    Pos2d pmax= bnd.GetPMax();
    GEOM_FT x= pmin.x();
    GEOM_FT y= pmin.y();
    const GEOM_FT Lx= pmax.x()-pmin.x();
    const GEOM_FT Ly= pmax.y()-pmin.y();
    const GEOM_FT inc_x= (ndiv_x > 0 ? Lx/ndiv_x: 0);
    const GEOM_FT inc_y= (ndiv_y > 0 ? Ly/ndiv_y: 0);
    const size_t n_rows= ndiv_x+1;
    const size_t n_columns= ndiv_y+1;
    for(size_t k=1;k<=n_layers;k++) //For each layer.
      {
        retval[k-1]= Pos2dArray(n_rows,n_columns);
        for(size_t i=1;i<=n_rows;i++) //For each row.
          {
            for(size_t j=1;j<=n_columns;j++) //For each column.
              {
                retval(k,i,j)= Pos2d(x,y);
                y+= inc_y;
              }
            y= pmin.y();
            x+= inc_x;
          }
        x= pmin.x();
      }
    return retval;
  }
