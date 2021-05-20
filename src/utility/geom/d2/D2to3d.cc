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
//D2to3d.cc

#include "D2to3d.h"
#include "utility/geom/d2/Plane.h"
#include "utility/geom/d1/Line2d.h"
#include "utility/geom/d1/Segment2d.h"
#include "utility/geom/d1/Segment3d.h"
#include "utility/geom/d1/Line3d.h"
#include "utility/geom/d3/GeomGroup3d.h"
#include "utility/geom/d3/HalfSpace3d.h"
#include "utility/geom/lists/auxiliary.h"
#include "utility/geom/pos_vec/Pos2dArray.h"
#include "utility/geom/pos_vec/Pos3dArray.h"



//! @brief Constructor.
D2to3d::D2to3d(void)
  : Surface3d(), ref() {}

//! @brief Constructor.
D2to3d::D2to3d(const Pos3d &o,const Pos3d &p,const Pos3d &q)
  : Surface3d(), ref(o,p,q)
  {}

//! @brief Constructor.
D2to3d::D2to3d(const Pos3d &o,const Vector3d &n,const Vector3d &i)
  : Surface3d(), ref(o,o+i,o+(n^i))
  {
    const Vector3d j= n^i;
    if(Abs2(j)>1e-3)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; error, vectors n= " << n
                << " and i= " << i << " are parallel." << std::endl;
  }

//! @brief Constructor.
D2to3d::D2to3d(const Pos3d &o)
  : Surface3d(), ref(o)
  {}

//! @brief Constructor.
D2to3d::D2to3d(const Ref2d3d &rf)
  : ref(rf) {}

//! @brief Constructor.
D2to3d::D2to3d(const Ref3d3d &rf)
  : ref(rf) {}

//! @brief Comparison operator.
bool D2to3d::operator==(const D2to3d &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= Surface3d::operator==(other);
        if(retval)
          retval= (ref==other.ref);
       }
    return retval;
  }

//! @brief Return the unary vector I expressed in global coordinates.
Vector3d D2to3d::getIVector(void) const
  { return ref.getIVector(); }

//! @brief Return the unary vector J expressed in global coordinates.
Vector3d D2to3d::getJVector(void) const
  { return ref.getJVector(); }

//! @brief Return the unary vector K expressed in global coordinates.
Vector3d D2to3d::getKVector(void) const
  { return ref.getKVector(); }


Pos3d D2to3d::to_3d(const Pos2d &p2d) const
  { return ref.GetPosGlobal(p2d); }

Segment3d D2to3d::to_3d(const Segment2d &sg2d) const
  { return Segment3d(to_3d(sg2d.getFromPoint()),to_3d(sg2d.getToPoint())); }

Pos3dArray D2to3d::to_3d(const Pos2dArray &p2d) const
  {
    const size_t n_rows= p2d.getNumberOfRows();
    const size_t n_columns= p2d.getNumberOfColumns();
    Pos3dArray retval(n_rows,n_columns);
    for(size_t i=1;i<=n_rows;i++)
      for(size_t j=1;j<=n_columns;j++)
        retval(i,j)= to_3d(p2d(i,j));
    return retval;
  }


Pos2d D2to3d::to_2d(const Pos3d &p3d) const
  { return ref.GetPosLocal(p3d); }

void D2to3d::ThreePoints(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3)
  {
    ref= Ref2d3d(p1,p2,p3);
  }
Plane D2to3d::getPlane(void) const
  { return ref.getXYPlane(); }

void D2to3d::Print(std::ostream &os) const
  { os << "Ref2d3d: " << ref << std::endl; }
