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
//Revolution3d.cc

#include "Revolution3d.h"
#include "../pos_vec/Pos3dArray.h"
#include "../pos_vec/Pos3dArray3d.h"
#include "Rotation3d.h"

//! @brief Constructor.
Revolution3d::Revolution3d(const Line3d &e,const GEOM_FT &th,const size_t &nd)
  : axis(e), theta(th), ndiv(nd)
  {}

//! @brief Return the point row that results from apply the revolution
//! to the argument.
Pos3dArray Revolution3d::Aplica0d(const Pos3d &p) const
  {
    Pos3dArray retval(1,ndiv+1);
    retval(1,1)= p;
    const GEOM_FT angle_inc= theta/ndiv;
    GEOM_FT ang(angle_inc);
    for(size_t i=2;i<=ndiv+1;i++)
      {
        const Rotation3d trf(axis,ang);
        retval(1,i)= trf(p);
        ang+= angle_inc;
      }
    return retval;
  }

//! @brief Return the point matrix that results from apply the revolution
//! to the argument.
Pos3dArray Revolution3d::Aplica1d(const Pos3dArray &m) const
  {
    const size_t nPoints= m.size();
    if(m.isColumn()) //Column matrix, put points by rows.
      {
        Pos3dArray retval(nPoints,ndiv+1);
        for(size_t i=1;i<=nPoints;i++) //Points of the first column.
          retval(i,1)= m(i,1);
        const GEOM_FT angle_inc= theta/ndiv;
        GEOM_FT ang(angle_inc);
        for(size_t i=2;i<=ndiv+1;i++)
          {
            const Rotation3d trf(axis,ang);
            for(size_t j=1;j<=nPoints;j++)
              retval(i,j)= trf(m(i,1));
            ang+= angle_inc;
          }
        return retval;
      }
    else if(m.isRow()) //Column matrix, put points by columns.
      {
        Pos3dArray retval(ndiv+1,nPoints);
        for(size_t j=1;j<=nPoints;j++) //Points of the first row.
          retval(1,j)= m(1,j);
        const GEOM_FT angle_inc= theta/ndiv;
        GEOM_FT ang(angle_inc);
        for(size_t i=2;i<=ndiv+1;i++)
          {
            const Rotation3d trf(axis,ang);
            for(size_t j=1;j<=nPoints;j++)
              retval(i,j)= trf(m(1,i));
            ang+= angle_inc;
          }
        return retval;
      }
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; bi-dimensional matrix,"
                  << " the result is not a point matrix" 
                  << " use " << getClassName() << "::Aplica2d."
		  << std::endl;
        return Pos3dArray();
      }
  }

//! @brief Return the point matrix that results from apply the revolution
//! to the bi-dimensional matrix passed as argument.
Pos3dArray3d Revolution3d::Aplica2d(const Pos3dArray &m) const
  {
    if(!m.isRow() && !m.isColumn()) //bi-dimensional matrix.
      {
        const size_t n_layers= ndiv+1;
	Pos3dArray3d retval(n_layers,m);
        const GEOM_FT angle_inc= theta/ndiv;
        GEOM_FT ang(angle_inc);
        for(size_t i=2;i<=n_layers;i++)
          {
            const Rotation3d trf(axis,ang);
            retval(i)= trf(m);
            ang+= angle_inc;
	  }
        return retval;
      }
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; uni-dimensional matrix,"
                  << " the result is a point matrix" 
                  << " use " << getClassName() << "::Aplica1d."
		  << std::endl;
        return Pos3dArray3d();
      }
  }

Pos3dArray Revolution3d::operator()(const Pos3d &p) const
  { return Aplica0d(p); }

Pos3dArray Revolution3d::operator()(const Pos3dArray &m) const
  { return Aplica1d(m); }
