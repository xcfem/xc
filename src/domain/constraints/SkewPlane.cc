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
//SkewPlane.cpp

#include "domain/constraints/SkewPlane.h"
#include "utility/matrix/ID.h"
#include "utility/matrix/Matrix.h"
#include <domain/mesh/node/Node.h>
#include <domain/domain/Domain.h>
#include "utility/geom/d1/Line2d.h"
#include "utility/geom/d2/Plane.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Constructor.
XC::SkewPlane::SkewPlane(int tag)
  : Skew_Constraint(tag, CNSTRNT_TAG_SkewPlane_Constraint) {}

//! @brief Constructor.
XC::SkewPlane::SkewPlane(int tag, const int &constrainedNode, const Line2d &l, const double &prescribedDisplacement, const double &prescribedRotation)
  : Skew_Constraint(tag, constrainedNode, CNSTRNT_TAG_SkewPlane_Constraint),
    normal(l.Normal().getNormalized())
  {
    if(prescribedDisplacement!=0.0)
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; Error: non zero prescribed displacement: "
		  << prescribedDisplacement
		  << " not implemented yet."
		  << Color::def << std::endl;
	exit(-1);
      }
    if(prescribedRotation!=0.0)
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; Error: non zero prescribed rotation: "
		  << prescribedRotation
		  << " not implemented yet."
		  << Color::def << std::endl;
	exit(-1);
      }
  }

//! @brief Constructor.
XC::SkewPlane::SkewPlane(int tag, const int &constrainedNode, const Plane &p, const double &prescribedDisplacement, const double &prescribedRotation)
  : Skew_Constraint(tag, constrainedNode, CNSTRNT_TAG_SkewPlane_Constraint),
    normal(p.Normal().getNormalized())
  {
    if(prescribedDisplacement!=0.0)
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; Error: non zero prescribed displacement: "
		  << prescribedDisplacement
		  << " not implemented yet."
		  << Color::def << std::endl;
	exit(-1);
      }
    if(prescribedRotation!=0.0)
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; Error: non zero prescribed rotation: "
		  << prescribedRotation
		  << " not implemented yet."
		  << Color::def << std::endl;
	exit(-1);
      }
  }

//! @brief Compute the constrained DOF.
void XC::SkewPlane::compute_constrained_dof(void)
  {
    // Compute the index of the maximum component of the
    // normal vector
    int constrainedDOFIndex= -1;
    double maxComponentValue= abs(normal[0]);
    if(maxComponentValue>0.0)
      constrainedDOFIndex= 0;
    const int sz= this->normal.Size();
    for(int i= 1; i<sz; i++)
      {
	const double value= abs(normal[i]);
	if(value>maxComponentValue)
	  {
	    maxComponentValue= value;
	    constrainedDOFIndex= i;
	  }
      }
    if(constrainedDOFIndex<0)
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; Error: normal vector: "
		  << normal
		  << " is null."
		  << Color::def << std::endl;
	exit(-1);
      }
    ID constrained_dofs({constrainedDOFIndex});
    this->set_constrained_dofs(ID({constrainedDOFIndex}));
  }

//! @brief Compute the retained DOFs.
void XC::SkewPlane::compute_retained_dofs(const Node *n)
  {
    std::set<int> ret_dofs; 
    if(n)
      {
	const int &idc= getConstrainedDOFs()[0];
	const int n_dof= n->getNumberDOF();
	for(int i= 0;i<n_dof; i++)
	  {
	    if(i!=idc)
	      ret_dofs.insert(i);
	  }
	this->set_retained_dofs(ID(ret_dofs));
      }
  }

//! @brief Computes constraint matrix.
void XC::SkewPlane::setup_matrix(void)
  {
    std::cout << "normal= " << normal << std::endl;
    std::cout << "constrained node tag: " << this->getNodeConstrained() << std::endl;
    std::cout << "retained node tag: " << this->getNodeRetained() << std::endl;
    // The number of DOF to be coupled
    const int numRetainedDOFs= retainDOF.Size(); //ID of retained DOFs.
    const int numConstrainedDOFs= constrDOF.Size(); //ID of constrained DOFs.

    std::cout << "numRetainedDOFs= " << numRetainedDOFs << std::endl;
    std::cout << "numConstrainedDOFs= " << numConstrainedDOFs << std::endl;

    // The constraint matrix ... U_c = C_cr * U_r
    Matrix Ccr(numConstrainedDOFs, numRetainedDOFs);
    Ccr.Zero();

    for(int i=0;i<numConstrainedDOFs;i++)
      {
	const int constrainedDOFindex= constrDOF[i];
	std::cout << "constrainedDOFindex= " << constrainedDOFindex << std::endl;
	const double &n_i= normal[constrainedDOFindex];
	std::cout << "n_i= " << n_i << std::endl;
	for(int j= 0; j<numRetainedDOFs; j++)
	  {
	    const int retainedDOFindex= retainDOF[j];
	    std::cout << "retaineddDOFindex= " << retainedDOFindex << std::endl;
	    const double &n_j= normal[retainedDOFindex];
	    Ccr(i, j)= -(n_j/n_i);
	  }
      }
    std::cout << "Ccr= " << Ccr << std::endl;
    this->set_constraint(Ccr);
  }

//! @brief Constraint setup.
void XC::SkewPlane::setup(Domain *theDomain)
  {
    std::cout << "Enters: " << getClassName() << "::" << __FUNCTION__ << std::endl;
    // Compute the constrained DOF.
    this->compute_constrained_dof();
    const int constrainedNodeTag= this->getNodeConstrained();
    const Node* theConstrainedNode = theDomain->getNode(constrainedNodeTag);
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
    std::cout << "Exits: " << getClassName() << "::" << __FUNCTION__ << std::endl;
  }

