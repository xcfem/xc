// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
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
//Linear3d.h

#ifndef LINEAR3D_H
#define LINEAR3D_H

#include "../d3/GeomObj3d.h"


//! @ingroup GEOM
//
//! @brief Base class for the objects involving a single dimension
//! in a three-dimensional space.
class Linear3d : public GeomObj3d
  {
  public:
    Linear3d(void): GeomObj3d(){}
    ~Linear3d(void) {}
    inline virtual unsigned short int Dimension(void) const
      { return 1; }
    //! @brief Return the object area.
    virtual GEOM_FT getArea(void) const
      { return 0.0; }
    //! @brief Return the volume of the object.
    virtual GEOM_FT getVolume(void) const
      { return 0.0; }
    //@brief Return true if the points is on sobre la Linea.
    virtual bool In(const Pos3d &p, const double &tol= 0.0) const= 0;
    
    //Return the intersection of the line with the plane
    // defined by the equation coord_i= d.
/*     virtual list_Pos3d Int(unsigned short int i, const double &d) const= 0; */
/*     inline list_Pos3d intx(const double &x) */
/*     //Return the intersection of the line with a plane x=cte. */
/*       { return Int(1,x); } */
/*     inline list_Pos3d inty(const double &y) */
/*     //Return the intersection of the line with a plane y=cte. */
/*       { return Int(2,y); } */
/*     inline list_Pos3d intz(const double &z) */
/*     //Return the intersection of the line with a plane z=cte. */
/*       { return Int(3,z); } */
  };
#endif



