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
//FrenetTrihedron.h
//Coordinate systems base class.

#ifndef FRENET_TRIHEDRON_H
#define FRENET_TRIHEDRON_H

#include "../ProtoGeom.h"
#include "utility/geom/d1/Polyline3d.h"


//!  @brief Coordinate systems base class.
//!  @ingroup CooSys
class FrenetTrihedron: public ProtoGeom
  {
    Polyline3d path; //!< Reference path).
  public:
    FrenetTrihedron(void);
    FrenetTrihedron(const Polyline3d &);
    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);
    virtual void Print(std::ostream &os) const;
    friend std::ostream &operator<<(std::ostream &os,const FrenetTrihedron &sc);
  };

#endif
