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

//! @ingroup CContMP
//
//! @brief Base class for symmetry and anti-symmetry boundary conditions
class SkewPlane: public Skew_Constraint
  {
  protected:
    Vector normal; //!< Normal to the plane/line defining the symmetry or anti-symmetry.
    
    void setup_matrix(void);
  public:
    SkewPlane(int tag, int classTag= CNSTRNT_TAG_SkewPlane_Constraint);
    SkewPlane(int tag, const int &constrainedNode, const Line2d &, const double &prescribedDisplacement= 0.0, const double &prescribedRotation= 0.0, int classTag= CNSTRNT_TAG_SkewPlane_Constraint);
    SkewPlane(int tag, const int &constrainedNode, const Plane &, const double &prescribedDisplacement= 0.0, const double &prescribedRotation= 0.0, int classTag= CNSTRNT_TAG_SkewPlane_Constraint);

    const Vector &getNormal(void) const;    
  };
} // end of XC namespace

#endif
