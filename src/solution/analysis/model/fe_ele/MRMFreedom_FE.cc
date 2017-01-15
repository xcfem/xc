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
//MRMFreedom_FE.cpp

#include "MRMFreedom_FE.h"
#include <domain/mesh/node/Node.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <domain/constraints/MRMFreedom_Constraint.h>
#include <domain/domain/Domain.h>

//! @brief Constructor.
XC::MRMFreedom_FE::MRMFreedom_FE(int tag, int numDOF_Group, int ndof, MRMFreedom_Constraint &TheMP,const double &Alpha)
  : MPBase_FE(tag,numDOF_Group,ndof,Alpha), theMRMP(&TheMP)
  {}


//! @brief determine the IDs in myID for those DOFs marked
//! as constrained DOFs, this is obtained from the DOF_Group
//! associated with the constrained node.
int XC::MRMFreedom_FE::determineConstrainedDOFsIDs(const int &offset)
  { return MPBase_FE::determineConstrainedDOFsIDs(*theMRMP,offset); }

//! @brief determine determine the IDs for the retained dof's
int XC::MRMFreedom_FE::determineRetainedDOFsIDs(const int &offset)
  {
    int retval= offset;
    const ID &retainedNodeTags= theMRMP->getRetainedNodeTags();
    const size_t sz= retainedNodeTags.Size();
    int conta= offset;
    const ID &RetainedDOFs = theMRMP->getRetainedDOFs();
    for(size_t i= 0;i<sz;i++)
      {
        Node *theRetainedNode= theRetainedNodes[i];

        DOF_Group *theRetainedNodesDOFs = theRetainedNode->getDOF_GroupPtr();
        if(theRetainedNodesDOFs == 0)
          {
	    std::cerr << "WARNING XC::MRMFreedom_FE::setID(void)";
	    std::cerr << " - no XC::DOF_Group with Retained XC::Node\n";
	    return -2;
          }    
    
        const ID &theRetainedNodesID = theRetainedNodesDOFs->getID();    

        int size2 = RetainedDOFs.Size();
        for(int j=0; j<size2; j++)
          {
	    const int retained = RetainedDOFs(j);
	    if(retained < 0 || retained >= theRetainedNode->getNumberDOF())
              {
	        std::cerr << "WARNING XC::MRMFreedom_FE::setID(void) - unknown DOF ";
	        std::cerr << retained << " at XC::Node\n";
	        myID(conta) = -1; // modify so nothing will be added
	        retval = -3;
	      }    	
	    else
              {
	        if(retained >= theRetainedNodesID.Size())
                  {
		    std::cerr << "WARNING XC::MRMFreedom_FE::setID(void) - ";
		    std::cerr << " Nodes DOF_Group too small\n";
		    myID(conta) = -1; // modify so nothing will be added 
		    retval = -4;
	          }
	        else
		  myID(conta) = theRetainedNodesID(retained);
	      }
            conta++;
          }
        myDOF_Groups(i+1)= theRetainedNodesDOFs->getTag();
      }
    if(retval>=0) retval+= conta-RetainedDOFs.Size();
    return retval;
  }

//! @brief get the DOF_Group attached to the retained nodes.
int XC::MRMFreedom_FE::determineRetainedNodesDofGrpPtr(Domain &theDomain, const int &offset)
  {
    const ID &retainedNodeTags= theMRMP->getRetainedNodeTags();
    const size_t sz= retainedNodeTags.Size();
    theRetainedNodes= std::vector<Node *>(sz,nullptr);   
    DOF_Group *dofGrpPtr= nullptr;
    for(size_t i= 0;i<sz;i++)
      {
        Node *theRetainedNode= theDomain.getNode(retainedNodeTags(i));
        theRetainedNodes[i]= theRetainedNode;

        if(theRetainedNode == 0)
          {
	    std::cerr << "FATAL MRMFreedom_FE::determineRetainedNodesDofGrpPtr() - Retained";
	    std::cerr << " node does not exist in Domain\n";
	    std::cerr << retainedNodeTags(i) << std::endl;
	    exit(-1);
          }	


        dofGrpPtr = theRetainedNode->getDOF_GroupPtr();
        if(dofGrpPtr!= 0)
          myDOF_Groups(i+offset) = dofGrpPtr->getTag();
        else 
          std::cerr << "WARNING MRMFreedom_FE::MRMFreedom_FE() - node no Group yet?\n"; 
      }
    return offset+sz;
  }

