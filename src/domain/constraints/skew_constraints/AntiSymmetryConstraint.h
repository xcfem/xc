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
//AntiSymmetryConstraint.h

#ifndef AntiSymmetryConstraint_h
#define AntiSymmetryConstraint_h

#include "domain/constraints/skew_constraints/SkewPlane.h"


namespace XC {

//! @ingroup CContMP
//
//! @brief Impose the node displacements (and rotations) with respect
//! to a plane (or axis for 2D problems).
class AntiSymmetryConstraint: public SkewPlane
  {
  protected:
    void compute_constrained_dofs(const Node *);
    void compute_retained_dofs(const Node *);
  public:
    AntiSymmetryConstraint(int tag);
    AntiSymmetryConstraint(int tag, const int &constrainedNode, const Line2d &);
    AntiSymmetryConstraint(int tag, const int &constrainedNode, const Plane &);

    void setup(Domain *theDomain);
    
  };
} // end of XC namespace

#endif
