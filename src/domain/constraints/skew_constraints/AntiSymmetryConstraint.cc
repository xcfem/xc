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
//AntiSymmetryConstraint.cpp

#include "domain/constraints/skew_constraints/AntiSymmetryConstraint.h"
#include "utility/matrix/ID.h"
#include "utility/matrix/Matrix.h"
#include <domain/mesh/node/Node.h>
#include <domain/domain/Domain.h>
#include "utility/geom/d1/Line2d.h"
#include "utility/geom/d2/Plane.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Constructor.
XC::AntiSymmetryConstraint::AntiSymmetryConstraint(int tag)
  : SkewPlane(tag, CNSTRNT_TAG_AntiSymmetry_Constraint) {}

//! @brief Constructor.
XC::AntiSymmetryConstraint::AntiSymmetryConstraint(int tag, const int &constrainedNode, const Line2d &l)
  : SkewPlane(tag, constrainedNode, l, 0.0, 0.0, CNSTRNT_TAG_AntiSymmetry_Constraint) {}

//! @brief Constructor.
XC::AntiSymmetryConstraint::AntiSymmetryConstraint(int tag, const int &constrainedNode, const Plane &p)
  : SkewPlane(tag, constrainedNode, p, 0.0, 0.0, CNSTRNT_TAG_AntiSymmetry_Constraint) {}

//! @brief Compute the constrained DOF.
void XC::AntiSymmetryConstraint::compute_constrained_dof(const Node *n)
  {
     std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	       << "; FATAL not implemented yet."
	       << Color::def << std::endl;
     exit(-1);
  }

//! @brief Compute the retained DOFs.
void XC::AntiSymmetryConstraint::compute_retained_dofs(const Node *n)
  {
     std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	       << "; FATAL not implemented yet."
	       << Color::def << std::endl;
     exit(-1);
  }

//! @brief Computes constraint matrix.
void XC::AntiSymmetryConstraint::setup_matrix(void)
  {
     std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	       << "; FATAL not implemented yet."
	       << Color::def << std::endl;
     exit(-1);
  }

//! @brief Constraint setup.
void XC::AntiSymmetryConstraint::setup(Domain *theDomain)
  {
     std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	       << "; FATAL not implemented yet."
	       << Color::def << std::endl;
     exit(-1);
  }
