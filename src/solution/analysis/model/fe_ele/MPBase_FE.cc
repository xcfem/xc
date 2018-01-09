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
//MPBase_FE.cpp

#include "MPBase_FE.h"
#include <domain/mesh/node/Node.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <domain/constraints/MFreedom_ConstraintBase.h>

//! @brief Constructor.
XC::MPBase_FE::MPBase_FE(int tag, int numDOF_Group, int ndof,const double &Alpha)
  : MPSPBaseFE(tag,numDOF_Group,ndof,Alpha), theConstrainedNode(nullptr)
  {}

//! @brief determine the IDs in myID for those DOFs marked
//! as constrained DOFs, this is obtained from the DOF_Group
//! associated with the constrained node.
int XC::MPBase_FE::determineConstrainedDOFsIDs(const MFreedom_ConstraintBase &theMFreedom,const int &offset)
  {
    int retval= offset;
    DOF_Group *theConstrainedNodesDOFs = theConstrainedNode->getDOF_GroupPtr();
    if(theConstrainedNodesDOFs == 0)
      {
	std::cerr << "WARNING MPBase_FE::setID(void)";
	std::cerr << " - no DOF_Group with Constrained XC::Node\n";
	return -2;
      }
    else
      myDOF_Groups(0) = theConstrainedNodesDOFs->getTag();

    const ID &constrainedDOFs = theMFreedom.getConstrainedDOFs();
    const ID &theConstrainedNodesID = theConstrainedNodesDOFs->getID();    
    
    const int size1 = constrainedDOFs.Size();
    retval+= size1;
    for(int i=0; i<size1; i++)
      {
	int constrained = constrainedDOFs(i);
	if(constrained < 0 || constrained >= theConstrainedNode->getNumberDOF())
          {	    
	    std::cerr << "WARNING MPBase_FE::setID(void) - unknown DOF ";
	    std::cerr << constrained << " at Node\n";
	    myID(i+offset) = -1; // modify so nothing will be added to equations
	    retval = -3;
	  }    	
	else
          {
	    if(constrained >= theConstrainedNodesID.Size())
              {
		std::cerr << "WARNING XC::MPBase_FE::setID(void) - ";
		std::cerr << " Nodes XC::DOF_Group too small\n";
		myID(i+offset) = -1; // modify so nothing will be added to equations
		retval = -4;
	      }
	    else
	      myID(i+offset) = theConstrainedNodesID(constrained);
	  }
      }
    return retval;
  }


//! @brief get the DOF_Group attached to the constrained node.
XC::DOF_Group *XC::MPBase_FE::determineConstrainedNodeDofGrpPtr(void)
  {
    DOF_Group *retval= nullptr;
    if(theConstrainedNode == nullptr)
      {
	std::cerr << "FATAL MPBase_FE::determineConstrainedNodeDofGrpPtr() - Constrained";
	std::cerr << " node does not exist in domain.\n";
	exit(-1);
      }
    else // set up the dof groups tags
      {
        DOF_Group *dofGrpPtr= theConstrainedNode->getDOF_GroupPtr();
       if(dofGrpPtr != nullptr) 
         retval= dofGrpPtr;	        
       else
        std::cerr << "WARNING MPBase_FE::determineConstrainedNodeDofGrpPtr() - node no Group yet?\n";
      }
    return retval;
  }


