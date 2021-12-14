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
//Pos3dArray3d.cc

#include "Pos3dArray3d.h"
#include "Pos3dArray.h"
#include "utility/geom/d1/Segment3d.h"
#include "utility/geom/d3/BND3d.h"
#include "utility/geom/trf/Trf3d.h"
#include "utility/geom/trf/Revolution3d.h"

Pos3dArray3d::Pos3dArray3d(const size_t &iLayers)
  : PosArray3d<Pos3d>(iLayers) {}
Pos3dArray3d::Pos3dArray3d(const size_t &iLayers,const PosArray<Pos3d> &points)
  : PosArray3d<Pos3d>(iLayers,points) {}
Pos3dArray3d::Pos3dArray3d(const PosArray<Pos3d> &l1_points,const PosArray<Pos3d> &l2_points,
                           const PosArray<Pos3d> &l3_points,const PosArray<Pos3d> &l4_points,
                           const size_t &ndiv_12,const size_t &ndiv_13)
  : PosArray3d<Pos3d>(l1_points,l2_points,l3_points,l4_points,ndiv_12,ndiv_13) {}
Pos3dArray3d::Pos3dArray3d(const Pos3dArray &l1_points,const Pos3dArray &l2_points,
                           const Pos3dArray &l3_points,const Pos3dArray &l4_points,
                           const size_t &ndiv_12,const size_t &ndiv_13)
  : PosArray3d<Pos3d>(l1_points,l2_points,l3_points,l4_points,ndiv_12,ndiv_13) {}

Pos3d Pos3dArray3d::getCenter(void) const
  { return get_centro(*this,Segment3d()); }

void Pos3dArray3d::Transform(const Trf3d &trf)
  { trf.Transform(*this); }

Pos3dArray3d Pos3dArray3d::Transform(const Trf3d &trf) const
  {
    Pos3dArray3d retval(*this);
    retval.Transform(trf);
    return retval;
  }

Pos3dArray3d create_uniform_grid(const BND3d &bnd,const size_t &ndiv_x,const size_t &ndiv_y,const size_t &ndiv_z)
  {
    const size_t n_layers= ndiv_z+1;
    Pos3dArray3d retval(n_layers);
    Pos3d pmin= bnd.GetPMin();
    Pos3d pmax= bnd.GetPMax();
    GEOM_FT x= pmin.x();
    GEOM_FT y= pmin.y();
    GEOM_FT z= pmin.z();
    const GEOM_FT Lx= pmax.x()-pmin.x();
    const GEOM_FT Ly= pmax.y()-pmin.y();
    const GEOM_FT Lz= pmax.z()-pmin.z();
    const GEOM_FT inc_x= (ndiv_x > 0 ? Lx/ndiv_x: 0);
    const GEOM_FT inc_y= (ndiv_y > 0 ? Ly/ndiv_y: 0);
    const GEOM_FT inc_z= (ndiv_z > 0 ? Lz/ndiv_z: 0);
    const size_t n_rows= ndiv_x+1;
    const size_t n_columns= ndiv_y+1;
    for(size_t k=1;k<=n_layers;k++) //For each layer.
      {
        retval[k-1]= Pos3dArray(n_rows,n_columns);
        for(size_t i=1;i<=n_rows;i++) //For each row.
          {
            for(size_t j=1;j<=n_columns;j++) //For each column.
              {
                retval(k,i,j)= Pos3d(x,y,z);
                y+= inc_y;
              }
            y= pmin.y();
            x+= inc_x;
          }
        x= pmin.x();
        z+= inc_z;
      }
    return retval;
  }

//! @brief Return the revolution solid obtained by applying
//! to the matrix the transformation argument.
Pos3dArray3d create_revolution_volume(const Revolution3d &r,const Pos3dArray &m)
  {
    if(m.isRow() || m.isColumn()) //Unidimensional.
      {
	std::cerr << __FUNCTION__ << "called with an one-dimensional matrix "
                  << " a degenerated volume will be returned." << std::endl;
      }
    return r.Aplica2d(m);
  }


