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
#include "utility/geom/pos_vec/Vector3d.h"
#include "Rect3d3dCooSys.h"
#include "utility/geom/coo_sys/ref_sys/Ref3d3d.h"
#include <map>

//!  @brief Coordinate systems base class.
//!  @ingroup CooSys
class FrenetTrihedron: public ProtoGeom
  {
    Polyline3d path; //!< Reference path.
    typedef std::map<double, Polyline3d::const_iterator> IntervalMap;
    
    IntervalMap interval_map;
    std::vector<Vector3d> tangent_vectors; //!< Tangent vectors at each vertex.
    std::vector<Vector3d> normal_vectors; //!< Normal vectors at each vertex.
    std::vector<Vector3d> binormal_vectors; //!< Binormal vectors at each vertex.
    
    IntervalMap compute_interval_map(void);
    IntervalMap::const_iterator get_interval_end(const double &) const;
    void compute_tangent_vectors(void);   
    void compute_vectors(void);
  public:
    FrenetTrihedron(void);
    FrenetTrihedron(const Polyline3d &);

    const Polyline3d &getPath(void) const;

    Vector3d getTangent(const double &) const;
    Vector3d getNormal(const double &) const;
    Vector3d getBinormal(const double &) const;
    Rect3d3dCooSys getCooSys(const double &) const;
    Ref3d3d getRefSys(const double &) const;    
    
    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);
    
    virtual void Print(std::ostream &os) const;
    friend std::ostream &operator<<(std::ostream &os,const FrenetTrihedron &sc);
  };

#endif
