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
//MFreedom_FE.cpp

#include "MFreedom_FE.h"
#include <domain/mesh/node/Node.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <domain/constraints/MFreedom_Constraint.h>
#include "utility/utils/misc_utils/colormod.h"


//! @brief Constructor.
XC::MFreedom_FE::MFreedom_FE(int tag, int numDOF_Group, int ndof, MFreedom_Constraint &TheMP,const double &Alpha)
  : MPBase_FE(tag,numDOF_Group,ndof,Alpha), theMFreedom(&TheMP), theRetainedNode(nullptr)
  {}

//! @brief Put the constradned DOFs displacements in the given vector.
void XC::MFreedom_FE::assemble_constrained_DOF_displacements(Vector &uu) const
  {
    const ID &id1= theMFreedom->getConstrainedDOFs();
    const int id1Sz= id1.Size();
    const Vector &Uc = theConstrainedNode->getTrialDisp();
    const Vector &Uc0 = theMFreedom->getConstrainedDOFsInitialDisplacement();
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
void XC::MFreedom_FE::assemble_retained_DOF_displacements(Vector &uu, const int &offset) const
  {
    const ID &id2= theMFreedom->getRetainedDOFs();
    const int id2Sz= id2.Size();
    const Vector &Ur = theRetainedNode->getTrialDisp();
    const Vector &Ur0 = theMFreedom->getRetainedDOFsInitialDisplacement();
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
        uu(i + offset) = Ur(rdof) - Ur0(i);
      }
  }

//! @brief determine the IDs in myID for those DOFs marked
//! as constrained DOFs, this is obtained from the DOF_Group
//! associated with the constrained node.
int XC::MFreedom_FE::determineConstrainedDOFsIDs(const int &offset)
  { return MPBase_FE::determineConstrainedDOFsIDs(*theMFreedom,offset); }

//! @brief determine determine the IDs for the retained dof's
int XC::MFreedom_FE::determineRetainedDOFsIDs(const int &offset)
  {
    int retval= offset;
    DOF_Group *theRetainedNodesDOFs = theRetainedNode->getDOF_GroupPtr();
    if(theRetainedNodesDOFs == 0)
     {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; WARNING - no DOF_Group with Retained node."
	          << Color::def << std::endl;
	return -2;
      }    
    
    const ID &RetainedDOFs = theMFreedom->getRetainedDOFs();
    const ID &theRetainedNodesID = theRetainedNodesDOFs->getID();    

    const int size2 = RetainedDOFs.Size();
    retval+= size2;
    for(int j=0; j<size2; j++)
      {
	int retained = RetainedDOFs(j);
	if(retained < 0 || retained >= theRetainedNode->getNumberDOF())
          {
	    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	              << "; WARNING unknown DOF: "
		      << retained << " at retained node: "
		      << theRetainedNode->getTag()
		      << Color::def << std::endl;
	    myID(j+offset) = -1; // modify so nothing will be added
	    retval= -3;
	  }    	
	else
          {
	    if(retained >= theRetainedNodesID.Size())
              {
		std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			  << "; WARNING - nodes DOF_Group too small."
		          << Color::def << std::endl;
		myID(j+offset) = -1; // modify so nothing will be added 
		retval= -4;
	      }
	    else
	      myID(j+offset)= theRetainedNodesID(retained);
	  }
      }

     myDOF_Groups(1)= theRetainedNodesDOFs->getTag();
    return retval;
  }

//! @brief get the DOF_Group attached to the retained node.
XC::DOF_Group *XC::MFreedom_FE::determineRetainedNodeDofGrpPtr(void)
  {
    DOF_Group *retval= nullptr;
    if(theRetainedNode == nullptr)
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; FATAL - Retained"
		  << " node does not exist in domain."
		  << Color::def << std::endl;
	exit(-1);
      }
    else // set up the dof groups tags
      {
        DOF_Group *dofGrpPtr= theRetainedNode->getDOF_GroupPtr();
        if(dofGrpPtr != nullptr) 
          retval= dofGrpPtr;	        
        else
         std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		   << "; WARNING - node no Group yet?"
		  << Color::def << std::endl;
      }
    return retval;
  }

