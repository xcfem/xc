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

#include "domain/constraints/skew_constraints/SkewPlane.h"
#include "utility/geom/d1/Line2d.h"
#include "utility/geom/d2/Plane.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Constructor.
XC::SkewPlane::SkewPlane(int tag, int classTag)
  : Skew_Constraint(tag, classTag) {}

//! @brief Constructor.
XC::SkewPlane::SkewPlane(int tag, const int &constrainedNode, const Line2d &l, const double &prescribedDisplacement, const double &prescribedRotation, int classTag)
  : Skew_Constraint(tag, constrainedNode, classTag),
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
XC::SkewPlane::SkewPlane(int tag, const int &constrainedNode, const Plane &p, const double &prescribedDisplacement, const double &prescribedRotation, int classTag)
  : Skew_Constraint(tag, constrainedNode, classTag),
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

const XC::Vector &XC::SkewPlane::getNormal(void) const
  { return normal; }

//! @brief Computes constraint matrix.
void XC::SkewPlane::setup_matrix(void)
  {
    // The number of DOF to be coupled
    const int numConstrainedDOFs= constrDOF.Size(); //ID of constrained DOFs.
    const int numRetainedDOFs= retainDOF.Size(); //ID of retained DOFs.

    // The constraint matrix ... U_c = C_cr * U_r
    Matrix Ccr(numConstrainedDOFs, numRetainedDOFs);
    Ccr.Zero();

    const int dim= normal.Size(); // dimension of the space.

    for(int i=0;i<numConstrainedDOFs;i++)
      {
	const int constrainedDOFindex= constrDOF[i];
	if(constrainedDOFindex<dim) // tranlational constrained DOF.
	  {
	    const double &n_i= normal[constrainedDOFindex];
	    for(int j= 0; j<numRetainedDOFs; j++)
	      {
		const int retainedDOFindex= retainDOF[j];
		if(retainedDOFindex<dim) // translational retained DOF.
		  {
		    const double &n_j= normal[retainedDOFindex];
		    Ccr(i, j)= -(n_j/n_i);
		  }
	      }
	  }
	else // rotational constrained DOF.
	  {
	    const double &n_j= normal[constrainedDOFindex-dim];
	    for(int j= 0; j<numRetainedDOFs; j++)
	      {
		const int retainedDOFindex= retainDOF[j];
		const double &n_i= normal[retainedDOFindex-dim];
		if(retainedDOFindex>=dim) // rotational retained DOF.
		  { Ccr(i, j)= n_j/n_i; }
	      }
	  }
      }
    this->set_constraint(Ccr);
  }
