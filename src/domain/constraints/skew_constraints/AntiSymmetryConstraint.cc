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
void XC::AntiSymmetryConstraint::compute_constrained_dofs(const Node *n)
  {
    std::set<int> constrained_dofs_set;
    if(n)
      {
	const ID &idr= getRetainedDOFs();
	const int n_dof= n->getNumberDOF();
	for(int i= 0;i<n_dof; i++)
	  {
	    if(idr.getLocation(i)<0) // not retained.
	      constrained_dofs_set.insert(i);
	  }
	this->set_retained_dofs(ID(constrained_dofs_set));
      }
  }

//! @brief Compute the retained DOFs.
void XC::AntiSymmetryConstraint::compute_retained_dofs(const Node *n)
  {
    // Compute the index of the maximum component of the
    // normal vector
    int retainedDOFIndex= normal.getIndexMaxAbsValue(); // Translational retained DOF.
    if(retainedDOFIndex<0)
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; Error: normal vector: "
		  << normal
		  << " is null."
		  << Color::def << std::endl;
	exit(-1);
      }
    // Translational DOFs.
    std::set<int> retained_dofs_set({retainedDOFIndex});
    // Rotational DOFs.
    const int n_dof= n->getNumberDOF();
    const int dim= normal.Size(); // dimension of the space.
    const int rotational_dofs= n_dof-dim;
    if(rotational_dofs>1) // 3 rotational DOFs.
      {
	if(retainedDOFIndex==0)
	  {
	    retained_dofs_set.insert(1);
	    retained_dofs_set.insert(2);
	  }
	else if(retainedDOFIndex==1)
	  {
	    retained_dofs_set.insert(0);
	    retained_dofs_set.insert(2);
	  }
	else if(retainedDOFIndex==2)
	  {
	    retained_dofs_set.insert(0);
	    retained_dofs_set.insert(1);
	  }
      }
    this->set_retained_dofs(ID(retained_dofs_set));
  }

//! @brief Constraint setup.
void XC::AntiSymmetryConstraint::setup(Domain *theDomain)
  {
    // Compute the retained DOF.
    const int retainedNodeTag= this->getNodeRetained();
    const Node *theRetainedNode= theDomain->getNode(retainedNodeTag);
    this->compute_retained_dofs(theRetainedNode);
    
    if(theRetainedNode == nullptr)
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; FATAL Retained node: "
	          << retainedNodeTag
	          << "does not exist in domain."
		  << Color::def << std::endl;
	exit(-1);
      }
    else
      {
	// Compute the retained DOFs.
	this->compute_constrained_dofs(theRetainedNode);
    
	this->setup_matrix();
      }
  }
