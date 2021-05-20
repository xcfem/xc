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
//enriched_polyedron.cc

#include "enriched_polyhedron.h"
#include "utility/geom/pos_vec/Pos3d.h"

class ConvPosicion
  {
  public:
   inline CGPoint_3 operator()(const CGPoint_3 &p) const
     { return p; }
  };

EPolyhedron enriquece(const CGPolyhedron_3 &pol)
  {
    typedef EPolyhedron::HalfedgeDS HalfedgeDS;
    EPolyhedron retval;
    Build_tdest_polyhedron<CGPolyhedron_3,HalfedgeDS,ConvPosicion> bpoly(pol);
    retval.delegate(bpoly);
    return retval;
  }


CGPolyhedron_3 empobrece(const EPolyhedron &pol)
  {
    
    typedef CGPolyhedron_3::HalfedgeDS HalfedgeDS;
    CGPolyhedron_3 retval;
    Build_tdest_polyhedron<EPolyhedron,HalfedgeDS,ConvPosicion> bpoly(pol);
    retval.delegate(bpoly);
    return retval;
  }
