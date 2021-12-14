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
/***************************************************************************
quad-triangle.h  -  quad/triangle subdivision
----------------------------------------------------------------------------
begin                : march 2003
copyright            : (C) 2003 by Pierre Alliez - INRIA
email                : pierre.alliez@sophia.inria.fr
***************************************************************************/

#ifndef QUAD_TRIANGLE
#define QUAD_TRIANGLE

#include "modifier_quad_triangle.h"

// Implemented from:
// Stam and Loop. Quad/Triangle Subdivision. 
// Computer Graphics Forum.
// Volume 22, Issue 1, March 2003.

// Another reference is:
// Levin. Polynomial Generation and Quasi-Interpolation 
// in Stationary Non-Uniform Subdivision.
// Computer Aided Geometric Design.
// pages 41-60, volume 20(1), 2003.


template <class Polyhedron,class kernel>
class CSubdivider_quad_triangle
  {
  public:
    typedef typename Polyhedron::HalfedgeDS HalfedgeDS;
    CSubdivider_quad_triangle() {}
    ~CSubdivider_quad_triangle() {}

  public:
    void subdivide_no_smooth(Polyhedron &OriginalMesh,Polyhedron &NewMesh)
      {
        //subdivide
        CModifierQuadTriangle<HalfedgeDS,Polyhedron,kernel> builder(&OriginalMesh);
        NewMesh.delegate(builder);
      }
    void subdivide(Polyhedron &OriginalMesh,
                   Polyhedron &NewMesh,
                   const bool smooth_boundary = true)
      {
        // subdivide, then smooth
        CModifierQuadTriangle<HalfedgeDS,Polyhedron,kernel> builder(&OriginalMesh);
        NewMesh.delegate(builder);
        builder.smooth(&NewMesh,smooth_boundary);
      }
  };


#endif // QUAD_TRIANGLE
