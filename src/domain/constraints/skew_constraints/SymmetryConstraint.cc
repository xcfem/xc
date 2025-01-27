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
//SymmetryConstraint.cpp

#include "domain/constraints/skew_constraints/SymmetryConstraint.h"
#include "utility/matrix/ID.h"
#include "utility/matrix/Matrix.h"
#include <domain/mesh/node/Node.h>
#include <domain/domain/Domain.h>
#include "utility/geom/d1/Line2d.h"
#include "utility/geom/d2/Plane.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Constructor.
XC::SymmetryConstraint::SymmetryConstraint(int tag)
  : SkewPlane(tag, CNSTRNT_TAG_Symmetry_Constraint) {}

//! @brief Constructor.
XC::SymmetryConstraint::SymmetryConstraint(int tag, const int &constrainedNode, const Line2d &l)
  : SkewPlane(tag, constrainedNode, l, 0.0, 0.0, CNSTRNT_TAG_Symmetry_Constraint) {}

//! @brief Constructor.
XC::SymmetryConstraint::SymmetryConstraint(int tag, const int &constrainedNode, const Plane &p)
  : SkewPlane(tag, constrainedNode, p, 0.0, 0.0, CNSTRNT_TAG_Symmetry_Constraint) {}

//! @brief Compute the constrained DOF.
void XC::SymmetryConstraint::compute_constrained_dofs(const Node *n)
  {
    // Compute the index of the maximum component of the
    // normal vector
    int constrainedDOFIndex= normal.getIndexMaxAbsValue();
    if(constrainedDOFIndex<0)
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; Error: normal vector: "
		  << normal
		  << " is null."
		  << Color::def << std::endl;
	exit(-1);
      }
    std::set<int> constrained_dofs_set({constrainedDOFIndex});
    if(n)
      {
	const int n_dof= n->getNumberDOF();
	const int dim= n->getDim();
	const int rotational_dofs= n_dof-dim;
	if(rotational_dofs>0)
          {
	    if(rotational_dofs==1)
	      constrained_dofs_set.insert(n_dof-1);
	    else // 3 rotational DOFs.
	      {
		if(constrainedDOFIndex==0)
		  {
		    constrained_dofs_set.insert(1+dim);
		    constrained_dofs_set.insert(2+dim);
		  }
		else if(constrainedDOFIndex==1)
		  {
		    constrained_dofs_set.insert(0+dim);
		    constrained_dofs_set.insert(2+dim);
		  }
		else if(constrainedDOFIndex==2)
		  {
		    constrained_dofs_set.insert(0+dim);
		    constrained_dofs_set.insert(1+dim);
		  }
	      }
	  }
      }
    ID constrained_dofs(constrained_dofs_set.begin(), constrained_dofs_set.end());
    this->set_constrained_dofs(constrained_dofs);
  }

//! @brief Compute the retained DOFs.
void XC::SymmetryConstraint::compute_retained_dofs(const Node *n)
  {
    std::set<int> ret_dofs;
    if(n)
      {
	const ID &idc= getConstrainedDOFs();
	const int n_dof= n->getNumberDOF();
	for(int i= 0;i<n_dof; i++)
	  {
	    if(idc.getLocation(i)<0) // not constrained.
	      ret_dofs.insert(i);
	  }
	this->set_retained_dofs(ID(ret_dofs));
      }
  }

//! @brief Constraint setup.
void XC::SymmetryConstraint::setup(Domain *theDomain)
  {
    // Compute the constrained DOF.
    const int constrainedNodeTag= this->getNodeConstrained();
    const Node* theConstrainedNode = theDomain->getNode(constrainedNodeTag);
    this->compute_constrained_dofs(theConstrainedNode);
    
    if(theConstrainedNode == nullptr)
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; FATAL Constrained node: "
	          << constrainedNodeTag
	          << "does not exist in domain."
		  << Color::def << std::endl;
	exit(-1);
      }
    else
      {
	// Compute the retained DOFs.
	this->compute_retained_dofs(theConstrainedNode);
    
	this->setup_matrix();
      }
  }
