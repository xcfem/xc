// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//SkewPlane.h

#ifndef SkewPlane_h
#define SkewPlane_h

#include "domain/constraints/skew_constraints/Skew_Constraint.h"

class Line2d;
class Plane;

namespace XC {
class Domain;

//! @ingroup CContMP
//
//! @brief Impose the node displacements (and rotations) with respect
//! to a plane (or axis for 2D problems).
class SkewPlane: public Skew_Constraint
  {
    Vector normal;
    
    Vector3d compute_constrained_dof(const Node *);
    void compute_retained_dofs(const Node *);
    void setup_matrix(const Vector3d &normal2normal);
  public:
    SkewPlane(int tag);
    SkewPlane(int tag, const int &constrainedNode, const Line2d &, const double &prescribedDisplacement= 0.0, const double &prescribedRotation= 0.0);
    SkewPlane(int tag, const int &constrainedNode, const Plane &, const double &prescribedDisplacement= 0.0, const double &prescribedRotation= 0.0);
    
    void setup(Domain *theDomain);
    
  };
} // end of XC namespace

#endif
