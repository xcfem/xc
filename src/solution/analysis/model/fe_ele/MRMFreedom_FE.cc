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
//MRMFreedom_FE.cpp

#include "MRMFreedom_FE.h"
#include <domain/mesh/node/Node.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <domain/constraints/MRMFreedom_Constraint.h>
#include <domain/domain/Domain.h>
#include "utility/utils/misc_utils/colormod.h"

//! @brief Constructor.
XC::MRMFreedom_FE::MRMFreedom_FE(int tag, int numDOF_Group, int ndof, MRMFreedom_Constraint &TheMP,const double &Alpha)
  : MPBase_FE(tag,numDOF_Group,ndof,Alpha), theMRMP(&TheMP)
  {}

//! @brief Put the constradned DOFs displacements in the given vector.
void XC::MRMFreedom_FE::assemble_constrained_DOF_displacements(Vector &uu) const
  {
    const ID &id1= theMRMP->getConstrainedDOFs();
    const int id1Sz= id1.Size();
    const Vector &Uc = theConstrainedNode->getTrialDisp();
    const Vector &Uc0 = theMRMP->getConstrainedDOFsInitialDisplacement();
    for(int i = 0; i < id1Sz; ++i)
      {
        const int cdof= id1(i);
        if(cdof < 0 || cdof >= Uc.Size())
	  {
	    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		      << "; FATAL Error: Constrained DOF "
		      << cdof << " out of bounds [0-" << Uc.Size() << "]"
		  << Color::def << std::endl;
            exit(-1);
	  }
        uu(i) = Uc(cdof) - Uc0(i);
      }
  }

//! @brief Put the retained DOFs displacements in the given vector.
void XC::MRMFreedom_FE::assemble_retained_DOF_displacements(Vector &uu, const int &offset) const
  {
    int local_offset= offset;
    const ID &id2= theMRMP->getRetainedDOFs();
    const int id2Sz= id2.Size();
    size_t jj= 0;
    for(std::vector<Node *>::const_iterator j= this->theRetainedNodes.begin();
	j!= theRetainedNodes.end(); j++, jj++)
      {
	const Node *theRetainedNode= *j;
	const Vector &Ur = theRetainedNode->getTrialDisp();
	const Vector &Ur0 = theMRMP->getRetainedDOFsInitialDisplacement(jj);
	for(int i = 0; i < id2Sz; ++i)
	  {
	    const int rdof = id2(i);
	    if(rdof < 0 || rdof >= Ur.Size())
	      {
		std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			  << "; FATAL Error: Retained DOF "
			  << rdof
			  << " out of bounds [0-" << Ur.Size()
			  << "]"
			  << Color::def << std::endl;
		exit(-1);
	      }
	    uu(i + local_offset) = Ur(rdof) - Ur0(i);
	  }
	local_offset+= id2Sz;
      }
  }

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
	    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		      << "; WARNING - no DOF_Group with retained Node"
		      << Color::def << std::endl;
	    return -2;
          }

        const ID &theRetainedNodesID = theRetainedNodesDOFs->getID();    

        int size2 = RetainedDOFs.Size();
        for(int j=0; j<size2; j++)
          {
	    const int retained = RetainedDOFs(j);
	    if(retained < 0 || retained >= theRetainedNode->getNumberDOF())
              {
		std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			  << "; WARNING unknown DOF: "
			  << retained << " at retained node: "
			  << theRetainedNode->getTag() << Color::def << std::endl;
	        myID(conta) = -1; // modify so nothing will be added
	        retval = -3;
	      }    	
	    else
              {
	        if(retained >= theRetainedNodesID.Size())
                  {
		    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			      << "; WARNING - nodes DOF_Group too small."
			      << Color::def << std::endl;
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
    if(retval>=0)
      retval+= conta-RetainedDOFs.Size();
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
	    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		      << "; FATAL - Retained node does not exist in Domain"
		      << Color::def << std::endl;
	    std::cerr << retainedNodeTags(i) << std::endl;
	    exit(-1);
          }	


        dofGrpPtr = theRetainedNode->getDOF_GroupPtr();
        if(dofGrpPtr!= 0)
          myDOF_Groups(i+offset) = dofGrpPtr->getTag();
        else 
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; WARNING - node no Group yet?"
		    << Color::def << std::endl; 
      }
    return offset+sz;
  }

