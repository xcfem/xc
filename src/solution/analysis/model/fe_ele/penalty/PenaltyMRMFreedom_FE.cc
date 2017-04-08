//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
                                                                        
// Written: lcpt
// Created: 03/2015
// Revision: A

#include <solution/analysis/model/fe_ele/penalty/PenaltyMRMFreedom_FE.h>
#include <cstdlib>

#include <domain/mesh/element/Element.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/integrator/Integrator.h>
#include "domain/domain/subdomain/Subdomain.h"
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <domain/mesh/node/Node.h>
#include <domain/constraints/MRMFreedom_Constraint.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>

XC::PenaltyMRMFreedom_FE::PenaltyMRMFreedom_FE(int tag, Domain &theDomain, 
			   MRMFreedom_Constraint &TheMRMP, double Alpha)
  :MRMFreedom_FE(tag, TheMRMP.getNumDofGroups(),TheMRMP.getNumDofs(), TheMRMP,Alpha)
  {
    const ID &id1 = theMRMP->getConstrainedDOFs();
    const int nGdls= id1.Size();
    const int numNodes= 1+theMRMP->getRetainedNodeTags().Size();//1 constrained node + numb. of retained nodes
    const int size= nGdls*numNodes;

    tang= Matrix(size,size);
    resid= Vector(size);
    C= Matrix(nGdls,size);

    theConstrainedNode = theDomain.getNode(theMRMP->getNodeConstrained());
    myDOF_Groups(0)= determineConstrainedNodeDofGrpPtr()->getTag();

    determineRetainedNodesDofGrpPtr(theDomain, 1);

    if(theMRMP->isTimeVarying() == false)
      this->determineTangent();
  }

//! @brief Method to set the correMPonding index of the ID to value.
int XC::PenaltyMRMFreedom_FE::setID(void)
  {
    // first determine the IDs in myID for those DOFs marked
    // as constrained DOFs, this is obtained from the XC::DOF_Group
    // associated with the constrained node
    const int size1= determineConstrainedDOFsIDs(0);
    // then determine the IDs for the retained dof's
    int result= determineRetainedDOFsIDs(size1);
    return result;
  }

const XC::Matrix &XC::PenaltyMRMFreedom_FE::getTangent(Integrator *theNewIntegrator)
  {
    if(theMRMP->isTimeVarying() == true)
	this->determineTangent();    
    return tang;
  }

const XC::Vector &XC::PenaltyMRMFreedom_FE::getResidual(Integrator *theNewIntegrator)
  {
    // zero residual, CD = 0
    return resid;
  }



const XC::Vector &XC::PenaltyMRMFreedom_FE::getTangForce(const Vector &disp, double fact)
  {
    std::cerr << "WARNING PenaltyMRMFreedom_FE::getTangForce() - not yet implemented.";
    resid.Zero(); //Added by LCPT.
    return resid;
  }

const XC::Vector &XC::PenaltyMRMFreedom_FE::getK_Force(const Vector &disp, double fact)
  {
    std::cerr << "WARNING PenaltyMRMFreedom_FE::getK_Force() - not yet implemented\n";
    resid.Zero(); //Added by LCPT.
    return resid;
  }

const XC::Vector &XC::PenaltyMRMFreedom_FE::getC_Force(const Vector &disp, double fact)
  {
    std::cerr << "WARNING PenaltyMRMFreedom_FE::getC_Force() - not yet implemented\n";
    resid.Zero(); //Added by LCPT.
    return resid;
  }

const XC::Vector &XC::PenaltyMRMFreedom_FE::getM_Force(const Vector &disp, double fact)
  {
    std::cerr << "WARNING PenaltyMRMFreedom_FE::getM_Force() - not yet implemented.\n";
    resid.Zero(); //Added by LCPT.
    return resid;
  }

void XC::PenaltyMRMFreedom_FE::determineTangent(void)
  {
    // first determine [C] = [-I [Ccr]]
    C.Zero();
    const Matrix &constraint = theMRMP->getConstraint();
    const int noRows = constraint.noRows();
    const int noCols = constraint.noCols();

    for(int j=0; j<noRows; j++)
      C(j,j) = -1.0;
    
    for(int i=0; i<noRows; i++)
      for(int j=0; j<noCols; j++)
	C(i,j+noRows) = constraint(i,j);
    
    // now form the tangent: [K] = alpha * [C]^t[C]
    // *(tang) = C^C;
    // *(tang) *= alpha;

    // THIS IS A WORKAROUND UNTIL WE GET addMatrixTransposeProduct() IN
    // THE Matrix CLASS OR UNROLL THIS COMPUTATION
    Matrix CT= C.getTrn();
    const Matrix &Cref= C;
    // Compute alpha*(C^C)
    tang.addMatrixProduct(0.0, CT, Cref, alpha);
  }


