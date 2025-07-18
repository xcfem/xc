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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.20 $
// $Date: 2005/11/23 22:48:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/mesh/node/Node.cpp,v $


// Written: fmk
// Created: 11/96
// Revision: A
//
// This file contains the implementation of the XC::Node class
//
// What: "@(#) Node.h, revA"

#include <domain/mesh/node/Node.h>
#include "domain/component/Parameter.h"
#include <cstdlib>
#include <domain/component/ContinuaReprComponent.h>
#include <utility/matrix/Vector.h>
#include "utility/actor/actor/MovableVector.h"
#include "utility/actor/actor/MovableMatrix.h"
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <domain/mesh/element/utils/Information.h>
#include "domain/constraints/SFreedom_Constraint.h"


#include "utility/geom/pos_vec/Pos2d.h"
#include "utility/geom/pos_vec/Pos3d.h"
#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/pos_vec/SlidingVectorsSystem3d.h"
#include "utility/geom/d2/GeomObj2d.h"
#include "utility/geom/d3/GeomObj3d.h"
#include "preprocessor/multi_block_topology/trf/TrfGeom.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/prep_handlers/LoadHandler.h"
#include "preprocessor/set_mgmt/SetMeshComp.h"
#include "domain/load/NodalLoad.h"
#include "domain/load/pattern/NodeLocker.h"

#include <domain/domain/Domain.h>
#include "domain/mesh/MeshEdge.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/ElementIter.h"

#include "utility/kernel/python_utils.h"
#include "utility/actor/actor/CommMetaData.h"

#include "utility/tagged/DefaultTag.h"
#include "utility/utils/misc_utils/colormod.h"

std::deque<XC::Matrix> XC::Node::theMatrices;
XC::DefaultTag XC::Node::defaultTag;

//! @brief Default constructor.
//! @param theClassTag: tag of the class.
//!
//! To construct a node which has no data, other than the \p classTag
//! identifier; defaultTag and \p classTag are passed to the DomainComponent
//! constructor. This is the constructor called by an
//! FEM_ObjectBroker. The data can be filled in subsequently by a call
//! to recvSelf().
XC::Node::Node(int theClassTag)
 :MeshComponent(defaultTag++,theClassTag),numberDOF(0), theDOF_GroupPtr(nullptr), 
  disp(), vel(), accel(), unbalLoad(numberDOF), unbalLoadWithInertia(numberDOF),
  reaction(numberDOF), alphaM(0.0), tributary(0.0)
  {
    // for FEM_ObjectBroker, recvSelf() must be invoked on object
    parameterID = 0;
    // AddingSensitivity:END ///////////////////////////////////////////
  }

//! @brief Constructor.
//! @param tag: node identifier.
//! @param theClassTag: tag of the class.
//!
//! To construct a node whose unique integer among nodes in the
//! domain is given by \p tag and whose classTag is given by \p
//! classTag. This constructor can be used by 
//! subclasses who wish to handle their own data management.
XC::Node::Node(int tag, int theClassTag)
  :MeshComponent(tag,theClassTag),
   numberDOF(0), theDOF_GroupPtr(nullptr),
   disp(), vel(), accel(), 
   unbalLoad(numberDOF), unbalLoadWithInertia(numberDOF), reaction(numberDOF),
   alphaM(0.0), tributary(0.0)
  {
    // tag= -1 is reserved for the seed node, don't modify the default tag
    // in this case.
    if(tag>=0) 
        defaultTag= tag+1;
    // AddingSensitivity:BEGIN /////////////////////////////////////////
    parameterID = 0;
    // AddingSensitivity:END ///////////////////////////////////////////
  }

//! @brief Constructor.
//! @param tag: node identifier.
//! @param ndof: number of degrees of freedom for the node.
//! @param Crd1: node position (one single coordinate).
//!
//! To construct a node for 1d problems whose unique integer among nodes in the
//! domain is given by \p tag and whose original position in 1d space
//! is given by (Crd1). With the node is associated \p ndof number
//! of degrees of freedom. The class tag is NOD_TAG_Node (defined in
//! classTags.h). A Vector object is created to hold the coordinates. No
//! storage objects are created to hold the trial and committed response
//! quantities, mass, load quantities; these are created as needed to
//! reduce the memory demands on the system in certain situations.
XC::Node::Node(int tag, int ndof, double Crd1)
  : MeshComponent(tag,NOD_TAG_Node),
    numberDOF(ndof), theDOF_GroupPtr(nullptr),
    Crd(1), disp(), vel(), accel(),
    mass(ndof,ndof), unbalLoad(numberDOF), unbalLoadWithInertia(numberDOF),
    reaction(numberDOF), alphaM(0.0), tributary(0.0)
  {
    // tag= -1 is reserved for the seed node, don't modify the default tag
    // in this case.
    if(tag>=0) 
      defaultTag= tag+1;
    // AddingSensitivity:BEGIN /////////////////////////////////////////
    parameterID = 0;
    // AddingSensitivity:END ///////////////////////////////////////////

    Crd(0) = Crd1;

    index = -1;
    setup_matrices(theMatrices,ndof);
  }


//! @brief Constructor for 2d nodes
//! @param tag: node identifier.
//! @param ndof: number of degrees of freedom for the node.
//! @param Crd1: node position (first coordinate).
//! @param Crd2: node position (second coordinate).
//!
//! To construct a node for 2d problems whose unique integer among nodes in the
//! domain is given by \p tag and whose original position in 2d space
//! is given by (Crd1,Crd2). With the node is associated \p ndof number
//! of degrees of freedom. The class tag is NOD_TAG_Node. A Vector object
//! is created to hold the coordinates. No
//! storage objects are created to hold the trial and committed response
//! quantities, mass, load quantities; these are created as needed to
//! reduce the memory demands on the system in certain situations.
XC::Node::Node(int tag, int ndof, double Crd1, double Crd2)
  :MeshComponent(tag,NOD_TAG_Node),numberDOF(ndof), theDOF_GroupPtr(nullptr),
   Crd(2), disp(), vel(), accel(),
   mass(ndof,ndof), unbalLoad(numberDOF), unbalLoadWithInertia(numberDOF),
   reaction(numberDOF), alphaM(0.0), tributary(0.0)
  {
    // tag= -1 is reserved for the seed node, don't modify the default tag
    // in this case.
    if(tag>=0) 
      defaultTag= tag+1;
    // AddingSensitivity:BEGIN /////////////////////////////////////////
    parameterID = 0;
    // AddingSensitivity:END ///////////////////////////////////////////

    Crd(0)= Crd1;
    Crd(1)= Crd2;

    index = -1;
    setup_matrices(theMatrices,ndof);
  }


//! @brief constructor for 3d nodes
//! @param tag: node identifier.
//! @param ndof: number of degrees of freedom for the node.
//! @param Crd1: node position (first coordinate).
//! @param Crd2: node position (second coordinate).
//! @param Crd3: node position (third coordinate).
//!
//! To construct a node for 3d problems whose unique integer among nodes in the
//! domain is given by \p tag and whose original position in 3d space
//! is given by (Crd1,Crd2,Crd3). With the node is associated \p ndof number
//! of degrees of freedom. The class tag is NOD_TAG_Node. A Vector object
//! is created to hold the coordinates. No
//! storage objects are created to hold the trial and committed response
//! quantities, mass, load quantities; these are created as needed to
//! reduce the memory demands on the system in certain situations.
XC::Node::Node(int tag, int ndof, double Crd1, double Crd2, double Crd3)
  :MeshComponent(tag,NOD_TAG_Node), numberDOF(ndof), theDOF_GroupPtr(nullptr),
   Crd(3), disp(), vel(), accel(),
   mass(ndof,ndof), unbalLoad(numberDOF), unbalLoadWithInertia(numberDOF),
   reaction(numberDOF), alphaM(0.0), tributary(0.0)
  {
    // tag= -1 is reserved for the seed node, don't modify the default tag
    // in this case.
    if(tag>=0) 
      defaultTag= tag+1;
    // AddingSensitivity:BEGIN /////////////////////////////////////////
    parameterID = 0;
    // AddingSensitivity:END ///////////////////////////////////////////

    Crd(0) = Crd1;
    Crd(1) = Crd2;
    Crd(2) = Crd3;

    index= -1;
    setup_matrices(theMatrices,ndof);
  }

//! @brief Constructor.
//! @param tag: node identifier.
//! @param ndof: number of degrees of freedom for the node.
//! @param crds: vector of node coordinates.
XC::Node::Node(int tag, int ndof, const Vector &crds)
  :MeshComponent(tag,NOD_TAG_Node),
   numberDOF(ndof), theDOF_GroupPtr(nullptr),
   Crd(crds), disp(), vel(), accel(),
   mass(ndof,ndof), unbalLoad(numberDOF), unbalLoadWithInertia(numberDOF),
   reaction(numberDOF), alphaM(0.0), tributary(0.0)
  {
    // tag= -1 is reserved for the seed node, don't modify the default tag
    // in this case.
    if(tag>=0) 
      defaultTag= tag+1;
    // AddingSensitivity:BEGIN /////////////////////////////////////////
    parameterID = 0;
    // AddingSensitivity:END ///////////////////////////////////////////

    index= -1;
    setup_matrices(theMatrices,ndof);
  }

//! @brief  used for domain decomposition & external nodes
//!  copy everything but the mass
//!  we should really set the mass to 0.0
XC::Node::Node(const Node &otherNode, bool copyMass)
  :MeshComponent(otherNode), numberDOF(otherNode.numberDOF),
   theDOF_GroupPtr(nullptr), Crd(otherNode.Crd),
   disp(otherNode.disp), vel(otherNode.vel), accel(otherNode.accel),
   R(otherNode.R), unbalLoad(otherNode.unbalLoad),
   unbalLoadWithInertia(otherNode.unbalLoadWithInertia),
   reaction(otherNode.reaction), alphaM(otherNode.alphaM),
   tributary(otherNode.tributary), theEigenvectors(otherNode.theEigenvectors),
   connected(otherNode.connected),
   freeze_constraints(otherNode.freeze_constraints)
  {
    // AddingSensitivity:BEGIN /////////////////////////////////////////
    parameterID = 0;
    // AddingSensitivity:END ///////////////////////////////////////////

    unbalLoad.Zero();

    if(copyMass == true)
      mass= otherNode.mass;

    index = -1;
    setup_matrices(theMatrices,numberDOF);
  }

//! @brief Inserts a component (element, constraint,...) to the connected component list.
void XC::Node::connect(ContinuaReprComponent *el) const
  { 
    if(el)
      connected.insert(el);
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; null argument."
		<< Color::def << std::endl;
  }

//! @brief Removes a component (element, constraint,...) from the connected component list.
void XC::Node::disconnect(ContinuaReprComponent *el) const
  {
    std::set<ContinuaReprComponent *>::const_iterator i= connected.find(el);
    if(i!=connected.end())
      connected.erase(i);
  }

//! @brief Virtual constructor.
XC::Node *XC::Node::getCopy(void) const
  { return new Node(*this,true); }

//! @brief True if node is inactive.
const bool XC::Node::isDead(void) const
  { return !isAlive(); }

//! @brief True if node is active.
//! @param ignoreFreezeConstraints: if true consider freeze constraints as
//!                                 regular constraints.
const bool XC::Node::isAlive(void) const
  {
    bool retval= false;
    if(connected.empty())
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << ";node: " << getTag() << " is free."
		  << Color::def << std::endl;
        retval= true;
      }
    else
      {
        for(std::set<ContinuaReprComponent *>::const_iterator i= connected.begin();i!=connected.end();i++)
          {
            const ContinuaReprComponent *ptr= *i;
            if(ptr)
              {
                if(ptr->isAlive())
                  {
		    retval= true;
		    break;
                  }
              }
            else
	      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			<< "; null pointer in the connected list."
			<< Color::def << std::endl;
          }
      }
    return retval;
  }

//! @brief Return true if the node is fixed only by its freeze
//! constraints (all the other constraints, if any, are dead)
const bool XC::Node::isFixedOnlyByFreezeConstraints(void) const
  {
    bool retval= (this->isAlive() && this->isFrozen());
    if(retval)
      {
        for(std::set<ContinuaReprComponent *>::const_iterator i= connected.begin();i!=connected.end();i++)
          {
            const ContinuaReprComponent *ptr= *i;
            if(ptr)
              {
		if(ptr->isAlive()) // Constraint is alive.
		  {
		    // Check if it is a freeze constraint
		    if(!is_a_freeze_constraint(ptr))
		      {
		        retval= false; //Not a freeze constraint. 
		        break;
		      }
		  }
              }
            else
	      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			<< "; null pointer in the connected list."
			<< Color::def << std::endl;
          }
      }
    return retval;
  }

//! @brief Imposes zero displacement (zero value for all node DOFs).
void XC::Node::freeze_if_dead(NodeLocker *locker)
  {
    if(isDead())
      {
        const size_t nodeTag= getTag();
        for(int dofId= 0;dofId<numberDOF;dofId++)
          {
            SFreedom_Constraint *sp= locker->addSFreedom_Constraint(nodeTag,dofId,0.0);
            if(sp)
	      {
		const int lockerTag= locker->getTag();
	        if(freeze_constraints.find(lockerTag)==freeze_constraints.end())
		  { freeze_constraints[lockerTag]= std::set<int>({sp->getTag()}); }
	        else
	          { freeze_constraints[lockerTag].insert(sp->getTag()); }
	      }
          }
      }
  }

//! @brief Return the identifiers of the node lockers that affect this node.
XC::ID XC::Node::get_node_lockers_tags(void) const
  {
    const size_t sz= freeze_constraints.size();
    ID retval(sz);
    int cont= 0;
    for(std::map<int, std::set<int> >::const_iterator i= freeze_constraints.begin();i!=freeze_constraints.end();i++, cont++)
      {
	const int tag= i->first;
	retval[cont]= tag;
      }
    return retval;
  }

//! @brief Returns a vector with the constraints identifiers. 
std::vector<XC::ID> XC::Node::get_constraints_tags(void) const
  {
    const size_t sz= freeze_constraints.size();
    std::vector<ID> retval(sz);
    std::deque<int> tmp;
    int conti= 0;
    for(std::map<int, std::set<int> >::const_iterator i= freeze_constraints.begin();i!=freeze_constraints.end();i++, conti++)
      {
	const std::set<int> &intSet= i->second;
	XC::ID tmp(intSet.size());
	int contj= 0;
        for(std::set<int>::const_iterator j= intSet.begin();j!=intSet.end();j++, contj++)
          tmp[contj]= (*j);
	retval[conti]= tmp;
      }
    return retval;
  }

//! @brief Sets the constraints identifiers. 
void XC::Node::set_id_constraints(const ID &nodeLockerTags, const std::vector<ID> &constraintTags)
  {
    const int sz= nodeLockerTags.Size();
    if(sz>0)
      {
	for(int i= 0; i<sz; i++)
	  {
	    int nodeLockerTag= nodeLockerTags[i];
	    const ID &cTags= constraintTags[i];
	    std::set<int> tmp;
	    const int numConstraints= cTags.Size();
	    for(int j= 0; j<numConstraints; j++)
	      tmp.insert(cTags[j]);
	    freeze_constraints[nodeLockerTag]= tmp;
	  }
      }
  }

//! @brief Returns true if the constraint pointer is a single freedom
//! constraint contained in freeze_constraints.
bool XC::Node::is_a_freeze_constraint(const ContinuaReprComponent *ptr) const
  {
    bool retval= false;
    const SFreedom_Constraint *sfConstraint= dynamic_cast<const SFreedom_Constraint *>(ptr);
    if(sfConstraint)
      {
        if(!freeze_constraints.empty())
          {
            for(std::map<int, std::set<int> >::const_iterator i= freeze_constraints.begin();i!=freeze_constraints.end();i++)
	      {
		const int nodeLockerTag= i->first;
		Domain *dom= this->getDomain();
		if(dom)
		  {
		    ConstrContainer &constrContainer= dom->getConstraints();
		    NodeLocker *locker= constrContainer.getNodeLocker(nodeLockerTag, true);
		    if(locker)
		      {
			const std::set<int> &cTags= i->second;
			for(std::set<int>::const_iterator j= cTags.begin(); j!= cTags.end(); j++)
			  {
			    const int constraintTag= sfConstraint->getTag();
			    const bool exists= locker->hasSPWithTag(constraintTag);
			    if(exists)
			      {
			        const SFreedom_Constraint *tmp= locker->getSFreedomConstraint(constraintTag);
				if(tmp==sfConstraint)
				  {
				    retval= true;
				    break;
				  }
			      }
			  }
		      }
		  }
		if(retval)
		  break;
	      }
	  }
      }
    return retval;
  }

//! @brief Deletes the constraint over the node DOFs
//! previously created by the freeze method.
void XC::Node::melt_if_alive(void)
  {
    // Check if fixed by others than freeze constraints.
    if(!this->isFixedOnlyByFreezeConstraints())
      {
	for(std::map<int, std::set<int> >::const_iterator i= freeze_constraints.begin();i!=freeze_constraints.end();i++)
	  {
	    const int nodeLockerTag= i->first;
	    Domain *dom= this->getDomain();
	    if(dom)
	      {
		ConstrContainer &constrContainer= dom->getConstraints();
		NodeLocker *locker= constrContainer.getNodeLocker(nodeLockerTag, true);
		if(locker)
		  {
		    const std::set<int> &cTags= i->second;
		    for(std::set<int>::const_iterator j= cTags.begin(); j!= cTags.end(); j++)
		      {
			const bool exists= locker->hasSPWithTag(*j);
			if(exists)
			  locker->removeSFreedom_Constraint(*j);
		      }
		  }
	      }
	  }
	freeze_constraints.clear();
	reaction.Zero();
      }
  }

//! @brief returns true if the node is frozen.
const bool XC::Node::isFrozen(void) const
  { return !freeze_constraints.empty(); }

//! @brief returns true if the node has no constraints.
const bool XC::Node::isFree(void) const
  { return connected.empty(); }

void XC::Node::kill(void)
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	      << "; must not be called."
	      << Color::def << std::endl;
  }

void XC::Node::alive(void)
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	      << "; must not be called."
	      << Color::def << std::endl;
  }

//! @brief destructor
XC::Node::~Node(void)
  {
    if(theDOF_GroupPtr) theDOF_GroupPtr->resetNodePtr();
  }

//! @brief Returns a default value for node identifier.
XC::DefaultTag &XC::Node::getDefaultTag(void)
  { return defaultTag; }

//! @brief Introduce en the node una constraint
//! como la being passed as parameter.
XC::SFreedom_Constraint *XC::Node::fix(const SFreedom_Constraint &seed)
  { return getPreprocessor()->getBoundaryCondHandler().addSFreedom_Constraint(getTag(),seed); }

//! @brief Sets prescribed displacements on the DOFs being passed as parameter.
void XC::Node::fix(const std::vector<int> &idDOFs,const std::vector<double> &values)
  {
    if(getDomain())
      {
        BoundaryCondHandler &cl= getPreprocessor()->getBoundaryCondHandler();
        const int sz= std::min(idDOFs.size(),values.size());
        if(values.size()<idDOFs.size())
	  std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; vector of prescribed displacements"
                    << " must be of " << idDOFs.size()
                    << " dimension."
		    << Color::def << std::endl;
        if(sz)
          {
            for(int i= 0;i<sz;i++)
              cl.addSFreedom_Constraint(getTag(),idDOFs[i],values[i]);
          }
        else
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; DOFs list is empty."
		    << Color::def << std::endl;
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; domain not defined. Constraint ignored."
		<< Color::def << std::endl;
  }

//! @brief Sets prescribed displacements on the DOFs being passed as parameter.
void XC::Node::fix(const ID &idDOFs,const Vector &values)
  {
    std::vector<double> vs= vector_to_std_vector(values);
    fix(idDOFs,vs);
  }

//! @brief Return the number of node DOFs, associated with
//! the node. .
int XC::Node::getNumberDOF(void) const
  { return numberDOF; }

//! @brief Sets the DOF_Group pointer
//!
//! Each node, when involved with an analysis, will be associated with a
//! DOF_Group object. It is the DOF\_Group that contains the ID of equation
//! numbers. When invoked this method sets the pointer to that DOF_Group
//! object. 
void XC::Node::setDOF_GroupPtr(DOF_Group *theDOF_Grp)
  { theDOF_GroupPtr= theDOF_Grp; }


//! @brief Gets the DOF_Group pointer
//!
//! Method which returns a pointer to the DOF_Group object that was set
//! using setDOF_GroupPtr. If no pointer has been set a nullptr is
//! returned.
XC::DOF_Group *XC::Node::getDOF_GroupPtr(void)
  { return theDOF_GroupPtr; }

//! @brief Gets the equation numbers corresponding to the node DOFS.
const XC::ID &XC::Node::getDOFs(void) const
  {
    if(theDOF_GroupPtr)
      return theDOF_GroupPtr->getID();
    else
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << "; equation numbers not assigned yet."
	          << " Probably analyze has not being called yet."
	          << Color::def << std::endl;
	static ID retval;
	return retval;
      }
  }

//! @brief Return the dimension of the node vector position (1,2 or 3).
size_t XC::Node::getDim(void) const
  { return Crd.Size(); }

//! @brief Return a const reference to the vector of nodal coordinates.
//!
//! Returns the original coordinates in a Vector. The size of the vector
//! is 2 if node object was created for a 2d problem and the size is 3 if
//! created for a 3d problem.
const XC::Vector &XC::Node::getCrds(void) const
  { return Crd; }

//! @brief Return a reference to the vector of nodal coordinates.
//! 
//! Returns the original coordinates in a Vector. The size of the vector
//! is 2 if node object was created for a 2d problem and the size is 3 if
//! created for a 3d problem.
XC::Vector &XC::Node::getCrds(void)
  { return Crd; }

//! @brief Returns the node coordinates in a 3D space.
XC::Vector XC::Node::getCrds3d(void) const
  {
    Vector retval(3,0.0);
    const size_t sz= getDim();
    if(sz>0)
      {
        retval[0]= Crd[0];
        if(sz>1)
          {
            retval[1]= Crd[1];
            if(sz>2)
              retval[2]= Crd[2];
          }
      }
    return retval;
  }

//! @brief Return the initial position of the node in 2D.
Pos2d XC::Node::getInitialPosition2d(void) const
  {
    Pos2d retval;
    const size_t sz= getDim();
    if(sz==1)
      retval= Pos2d(Crd[0],0.0);
    else if(sz>=2)
      retval= Pos2d(Crd[0],Crd[1]);
    return retval;
  }

//! @brief Return the initial position of the node in 3D.
Pos3d XC::Node::getInitialPosition3d(void) const
  {
    Pos3d retval;
    const size_t sz= getDim();
    if(sz==1)
      retval= Pos3d(Crd[0],0.0,0.0);
    else if(sz==2)
      retval= Pos3d(Crd[0],Crd[1],0.0);
    else if(sz==3)
      retval= Pos3d(Crd[0],Crd[1],Crd[2]);
    return retval;
  }

//! @brief Return the 2D position obtained as: initPos+ factor * v.
Pos2d XC::Node::getPosition2d(const Vector &v) const
  {
    Pos2d retval;
    const size_t sz= getDim();
    if(sz==1)
      {
        if(numberDOF==1)
          retval= Pos2d(Crd[0]+v[0],0.0);
        else if(numberDOF==2)
          retval= Pos2d(Crd[0]+v[0],v[1]);
      }
    else if(sz>=2)
      {
        if(numberDOF==1)
          retval= Pos2d(Crd[0]+v[0],Crd[1]);
        else if((numberDOF==2) || (numberDOF==3) || (numberDOF==6))
          retval= Pos2d(Crd[0]+v[0],Crd[1]+v[1]);
      }
    return retval;
  }
  
//! @brief Return the 3D position obtained as: initPos+ factor * v.
Pos3d XC::Node::getPosition3d(const Vector &v) const
  {
    Pos3d retval;
    const size_t sz= getDim();
    if(sz==1)
      {
        if(numberDOF==1)
          retval= Pos3d(Crd[0]+v[0],0.0,0.0);
        else if(numberDOF==2)
          retval= Pos3d(Crd[0]+v[0],v[1],0.0);
        else if((numberDOF==3) || (numberDOF==6))
          retval= Pos3d(Crd[0]+v[0],v[1],v[2]);
      }
    else if(sz==2)
      {
        if(numberDOF==1)
          retval= Pos3d(Crd[0]+v[0],Crd[1],0.0);
        else if((numberDOF==2) || (numberDOF==3))
          retval= Pos3d(Crd[0]+v[0],Crd[1]+v[1],0.0);
      }
    else if(sz==3)
      {
        if(numberDOF==1)
          retval= Pos3d(Crd[0]+v[0],Crd[1],Crd[2]);
        else if(numberDOF==2)
          retval= Pos3d(Crd[0]+v[0],Crd[1]+v[1],Crd[2]);
        else if((numberDOF==3) || (numberDOF==6))
          retval= Pos3d(Crd[0]+v[0],Crd[1]+v[1],Crd[2]+v[2]);
      }
    return retval;
  }

//! @brief Return the current position of the node scaled by
//! factor: return initPos+ factor * nodDisplacement.
Pos2d XC::Node::getCurrentPosition2d(const double &factor) const
  {
    Vector fd= factor*getDisp();
    if(fd.isnan()) //Something went wrong.
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << " displacement vector for node: "
	          << getTag() << " is NOT VALID"
	          << " returning original position."
	          << Color::def << std::endl;
        fd.Zero();
      }
    return getPosition2d(fd);
  }

//! @brief Return the current position of the node scaled by
//! factor: return initPos+ factor * nodDisplacement.
Pos3d XC::Node::getCurrentPosition3d(const double &factor) const
  {
    Vector fd= factor*getDisp();
    if(fd.isnan()) //Something went wrong.
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << " displacement vector for node: "
	          << getTag() << " is NOT VALID"
	          << " returning original position."
	          << Color::def << std::endl;
        fd.Zero();
      }
    return getPosition3d(fd);
  }

//! @brief Returns true if the current position of the node scaled by
//! factor: return initPos+ factor * nodDisplacement lies insiof the
//! geometric object.
bool XC::Node::In(const GeomObj3d &geomObj,const double &factor, const double &tol) const
  {
    const Pos3d pos= getCurrentPosition3d(factor);
    return geomObj.In(pos,tol);
  }

//! @brief Returns true if the current position of the node scaled by
//! factor: return initPos+ factor * nodDisplacement lies insiof the
//! geometric object.
bool XC::Node::In(const GeomObj2d &geomObj,const double &factor, const double &tol) const
  {
    const Pos2d pos= getCurrentPosition2d(factor);
    return geomObj.In(pos,tol);
  }

//! @brief Returns true if the current position of the node scaled by
//! factor: return initPos+ factor * nodDisplacement lies outsiof the
//! geometric object.
bool XC::Node::Out(const GeomObj3d &geomObj,const double &factor, const double &tol) const
  {
    const Pos3d pos= getCurrentPosition3d(factor);
    return !geomObj.In(pos,tol);
  }

//! @brief Returns true if the current position of the node scaled by
//! factor: return initPos+ factor * nodDisplacement lies outsiof the
//! geometric object.
bool XC::Node::Out(const GeomObj2d &geomObj,const double &factor, const double &tol) const
  {
    const Pos2d pos= getCurrentPosition2d(factor);
    return geomObj.Out(pos,tol);
  }

//! @brief Extracts translational components from d vector.
//! @param d: displacement (or velocity or acceleration) vector
XC::Vector extract_translation(const XC::Vector &d,const size_t &dim,const size_t numDOF)
  {
    XC::Vector retval(3,0.0);
    if(dim==1)
      {
        if(numDOF==1) //Elast 1D.
          { retval[0]= d[0]; }
        else if(numDOF==2)
          { retval[0]= d[0]; retval[1]=d[1]; }
        else if((numDOF==3) || (numDOF==6)) //Elast 3D o RM 3D.
          { retval[0]= d[0]; retval[1]=d[1]; retval[2]=d[2]; }
      }
    else if(dim==2)
      {
        if(numDOF==1) //Elast 1D.
          { retval[0]= d[0]; }
        else if((numDOF==2) || (numDOF==3)) //Elast 2D o RM 2D.
          { retval[0]= d[0]; retval[1]=d[1]; }
      }
    else if(dim==3)
      {
        if(numDOF==1) //Elast 1D.
          { retval[0]= d[0]; }
        else if(numDOF==2) //Elast 2D.
          { retval[0]= d[0]; retval[1]=d[1]; }
        else if((numDOF==3) || (numDOF==6)) //Elast 3D o RM 3D.
          { retval[0]= d[0]; retval[1]=d[1]; retval[2]=d[2]; }
      }
    return retval;
  }

//! @brief Extracts rotational components from d vector.
//! @param d: displacement (or velocity or acceleration) vector
XC::Vector extract_rotation(const XC::Vector &d,const size_t &dim,const size_t numDOF)
  {
    XC::Vector retval(3,0.0);
    if((dim==2) && (numDOF==3)) //RM 2D.
      {retval[3]= d[2];}
    else if ((dim==3) && (numDOF==6)) //RM 3D.
      {retval[0]= d[3]; retval[1]=d[4]; retval[2]=d[5];}
    return retval;
  }

//! @brief Returns the XYZ components of node displacement.
XC::Vector XC::Node::getDispXYZ(void) const
  { return extract_translation(getDisp(),getDim(),numberDOF); }

//! @brief Returns the XYZ components of node rotation.
XC::Vector XC::Node::getRotXYZ(void) const
  { return extract_rotation(getDisp(),getDim(),numberDOF); }

//! @brief Returns the XYZ components of the translational velocity of the node.
XC::Vector XC::Node::getVelXYZ(void) const
  { return extract_translation(getVel(),getDim(),numberDOF); }

//! @brief Returns the XYZ components of the angular velocity of the node.
XC::Vector XC::Node::getOmegaXYZ(void) const
  { return extract_rotation(getVel(),getDim(),numberDOF); }

//! @brief Returns the XYZ components of the translational acceleration of the node.
XC::Vector XC::Node::getAccelXYZ(void) const
  { return extract_translation(getAccel(),getDim(),numberDOF); }

//! @brief Returns the XYZ components of the angular acceleration of the node.
XC::Vector XC::Node::getAlphaXYZ(void) const
  { return extract_rotation(getAccel(),getDim(),numberDOF); }

//! @brief Returns the square of the distance from the node to the point
//! being passed as parameter.
//! @param p: 2D point to calculate the distance.
//! @param initialGeometry: if true initial position of the node is considered.
double XC::Node::getDist2(const Pos2d &p,bool initialGeometry) const
  { return getDist2(To3dXY2d(p),initialGeometry); }

//! @brief Returns the distance from the node to the point
//! being passed as parameter.
//! @param p: 2D point to calculate the distance.
//! @param initialGeometry: if true initial position of the node is considered.
double XC::Node::getDist(const Pos2d &p,bool initialGeometry) const
  { return sqrt(getDist2(p,initialGeometry)); }

//! @brief Returns the square of the distance from the node to the point
//! being passed as parameter.
//! @param p: 3D point to calculate the distance.
//! @param initialGeometry: if true initial position of the node is considered.
double XC::Node::getDist2(const Pos3d &p,bool initialGeometry) const
  {
    if(initialGeometry)
      return ::dist2(getInitialPosition3d(),p);
    else
      return ::dist2(getCurrentPosition3d(),p);
  }

//! @brief Returns the distance from the node to the point
//! being passed as parameter.
//! @param p: 3D point to calculate the distance.
//! @param initialGeometry: if true initial position of the node is considered.
double XC::Node::getDist(const Pos3d &p,bool initialGeometry) const
  { return sqrt(getDist2(p,initialGeometry)); }

//! @brief Sets the node position.
void XC::Node::setPos(const Pos3d &p)
  {
    const size_t sz= getDim();
    if(sz==1)
      Crd[0]= p.x();
    else if(sz==2)
      {
        Crd[0]= p.x();
        Crd[1]= p.y();
      }
    else if(sz==3)
      {
        Crd[0]= p.x();
        Crd[1]= p.y();
        Crd[2]= p.z();
      }
  }

//! @brief Applies to the node position the transformation being passed as parameter.
void XC::Node::Transform(const TrfGeom &trf)
  {
    static Pos3d p;
    setPos(trf.Transform(getInitialPosition3d()));
  }

//! @brief Return the 3D position of the node.
//! @param initialGeometry: if true initial position of the node is considered.
Pos3d XC::pos_node(const Node &nod,bool initialGeometry)
  {
    if(initialGeometry)
      return nod.getInitialPosition3d();
    else
      return nod.getCurrentPosition3d(1.0);
  }


//! @brief Returns the displacement of the node.
//!
//! Returns the last committed displacement as a Vector, the vector of
//! size ndof. If no Vector has yet been allocated, two Vector
//! objects are created to store the committed and trial response
//! quantities created.
const XC::Vector &XC::Node::getDisp(void) const
  { return disp.getData(numberDOF); }

//! @brief Returns the velocity of the node.
//!
//! Returns the last committed velocity as a Vector, the vector of size
//! ndof. If no Vector has yet been allocated, two Vector
//! objects are created to store the committed and trial response
//! quantities created.
const XC::Vector &XC::Node::getVel(void) const
  { return vel.getData(numberDOF); }

//! @brief Returns the acceleration of the node.
//!
//! Returns the last committed acceleration as a Vector, the vector of
//! size ndof. If no Vector has yet been allocated, two Vector 
//! objects are created to store the committed and trial response
//! quantities created.
const XC::Vector &XC::Node::getAccel(void) const
  { return accel.getData(numberDOF); }


/* *********************************************************************
**
**   Methods to return the trial response quantities similar to committed
**
** *********************************************************************/

//! @brief Returns the trial value of the displacement of the node.
//!
//! Returns the current trial displacements as a Vector, the vector of
//! size ndof. If no Vector has yet been allocated, a new Vector is
//! created and returned.
const XC::Vector &XC::Node::getTrialDisp(void) const
  { return disp.getTrialData(numberDOF); }

//! @brief Returns the trial value of the velocity of the node.
//!
//! Returns the current trial velocities as a Vector, the vector of size
//! ndof. If no Vector has yet been allocated, a new Vector is created
//! and returned.
const XC::Vector &XC::Node::getTrialVel(void) const
  { return vel.getTrialData(numberDOF); }

//! @brief Returns the trial value of the acceleration of the node.
//!
//! Returns the current trial accelerations as a Vector, the vector of size
//! ndof. If no Vector has yet been allocated, a new Vector is created
//! and returned.
const XC::Vector &XC::Node::getTrialAccel(void) const
  { return accel.getTrialData(numberDOF); }

//! @brief Returns the displacement increment.
const XC::Vector &XC::Node::getIncrDisp(void) const
  { return disp.getIncrDisp(numberDOF); }

//! @brief Return the incremental displacement.
//!
//! Returns the incremental displacement as a Vector. The incremental
//! displacement is equal to the difference between the current trial
//! displacement and committed displacement (trial - committed).
//! If no Vector has yet been allocated, three Vector 
//! objects are created to store the committed, trial and incremental response
//! quantities.
const XC::Vector &XC::Node::getIncrDeltaDisp(void) const
  { return disp.getIncrDeltaDisp(numberDOF); }

int XC::Node::setTrialDispComponent(double value, int dof)
  { return disp.setTrialDispComponent(numberDOF,value,dof); }

//! @brief Set the current trial displacement.
//!
//! Sets the current trial displacement to be that given by
//! \p newTrialDisp. Sets the incremental displacement to be trial-committtd.
//! Returns 0 if successful, an error message is printed and
//! a $-2$ is returned if \p newTrialDisp is not of size ndof.
int XC::Node::setTrialDisp(const Vector &newTrialDisp)
  { return disp.setTrialDisp(numberDOF,newTrialDisp); }

//! @brief Set the current trial velocity.
//!
//! Sets the current trial velocity to be that given by
//! \p newTrialVel.
//! Returns 0 if successful, an error message is printed
//! and a $-2$ is returned if \p newTrialVel is not of size ndof.
int XC::Node::setTrialVel(const Vector &newTrialVel)
  { return vel.setTrialData(numberDOF,newTrialVel); }

//! @brief Set the current trial acceleration.
//!
//! Sets the current trial acceleration to be that given by
//! \p newTrialAccel. Returns 0 if successful,
//! an error message is printed and a -2 is returned if \p
//! newTrialAccel is not of size ndof. 
int XC::Node::setTrialAccel(const Vector &newTrialAccel)
  { return accel.setTrialData(numberDOF,newTrialAccel); }

//! @brief Increments the current trial displacement.
//! 
//! Sets the current trial displacement to be that given by the addition
//! of the last trial displacement, assumed 0 if not yet set, and 
//! trialIncrDisp. Increments the incremental displacement by \p trialIncrDisp.
//! Returns 0 if successful, an error message is printed and a -2 is
//! returned if trialIncrDisp is not of size ndof.
int XC::Node::incrTrialDisp(const Vector &incrDispl)
  { return disp.incrTrialDisp(numberDOF,incrDispl); }

//! @brief Increments the current trial velocity.
//! 
//! Sets the current trial velocity to be that given by the addition
//! of the last trial velocity, assumed 0 if not yet set, and 
//! trialIncrVel.
//! Returns 0 if successful, an error message is printed and a -2
//! is returned if trialIncrVel is not of size ndof.
int XC::Node::incrTrialVel(const Vector &incrVel)
  { return vel.incrTrialData(numberDOF,incrVel); }

//! @brief Increments the current trial acceleration.
//!
//! Sets the current trial acceleration to be that given by the addition
//! of the last trial Acceleration, assumed 0 if not yet set, and
//! \p trialIncrAccel.  Returns 0 if successful,
//! an error message is printed and a -2 is returned if 
//! trialIncrAccel is not of size \p ndof.
int XC::Node::incrTrialAccel(const Vector &incrAccel)
  { return accel.incrTrialData(numberDOF,incrAccel); }

//! @brief Create a new load on the node and put it in the current load pattern.
const XC::NodalLoad *XC::Node::newLoad(const Vector &v)
  {
    NodalLoad *retval= nullptr;
    LoadPattern *lp= getCurrentLoadPattern();
    const int nodeTag= getTag(); //Load over this node.

    const size_t sz= v.Size();
    if(sz>0)
      {
        if(lp)
          retval= lp->newNodalLoad(nodeTag,v);
        else
	  std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; there is no current load pattern."
		    << " Nodal load ignored."
		    << Color::def << std::endl; 
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
                << "; a vector of dimension greater than zero was expected."
		<< Color::def << std::endl;
    return retval;
  }

//! @brief Creates the inertia load that corresponds to the
//! acceleration argument.
void XC::Node::createInertiaLoad(const Vector &accel)
  {
    Vector v= unbalLoad;
    v.Zero();
    const size_t sz= accel.Size();
    for(size_t i= 0;i<sz;i++)
      v[i]-= mass(i,i)*accel(i); //Like Ansys.
    newLoad(v); //Put the load in the current load pattern.
  }

//! @brief Causes the node to zero out its unbalanced load vector.
void XC::Node::zeroUnbalancedLoad(void)
  { unbalLoad.Zero(); }

//! @brief Adds vector to the node unbalanced load.
//! @param add: vector to add.
//! @param fact: multiplying factor.
//!
//! The Node is responsible for adding \p fact times 
//! add to the current unbalanced load at the Node. If {\em
//! add is not of size ndof no load is added, an error
//! message is printed and a -1 is returned. Returns 0 if successful.
int XC::Node::addUnbalancedLoad(const Vector &add, double fact)
  {
    int retval= -1;
    // check vector arg is of correct size
    if(add.Size() != numberDOF)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; load to add of incorrect size "
		  << add.Size() << " should be " <<  numberDOF
		  << Color::def << std::endl;
      }
    else
      {
        if(isDead())
          {
            std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		      << "; load over dead node: "
                      << getTag() << Color::def << std::endl;
          }
        else // add fact*add to the unbalanced load
          {
            unbalLoad.addVector(1.0, add,fact);
            retval= 0;
          }
      }
    return retval;
  }

//! @brief Adds inertial loads to the node unbalanced load vector.
//!
//! To add MINUS \p fact times the product M * R * accel to the
//! current unbalanced load. Nothing is done if no mass or R matrix have
//! been set. Prints a warning and returns a -1 if the size of accel and
//! the number of columns in R are not the same. Returns 0 if successful.
int XC::Node::addInertiaLoadToUnbalance(const Vector &accelG, double fact)
  {
    // simply return if node has no mass or R matrix
    if(R.isEmpty() || isDead())
      return 0;

    // otherwise we must determine MR accelG
    if(accelG.Size() != R.noCols())
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << "; accelG not of correct dimension"
		  << Color::def << std::endl;
        return -1;
      }

    // form - fact * M*R*accelG and add it to the unbalanced load
    //unbalLoad-= ((mass) * R * accelG)*fact;
    Matrix MR(mass.noRows(), R.noCols());
    MR.addMatrixProduct(0.0, mass, R, 1.0);
    unbalLoad.addMatrixVector(1.0, MR, accelG, -fact);
    return 0;
  }

//! @brief ??
int XC::Node::addInertiaLoadSensitivityToUnbalance(const XC::Vector &accelG, double fact, bool somethingRandomInMotions)
  {
    // simply return if node has no R matrix
    if(R.isEmpty())
      return 0;

    // otherwise we must determine MR accelG
    if(accelG.Size() != R.noCols())
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; accelG not of correct dimension"
	          << Color::def << std::endl;
        return -1;
      }


    // form - fact * M*R*accelG and add it to the unbalanced load
    //unbalLoad-= ((mass) * R * accelG)*fact;

    Matrix massSens(mass.noRows(),mass.noCols());
    if(parameterID != 0)
      { massSens(parameterID-1,parameterID-1) = 1.0; }

    Matrix MR(mass.noRows(), R.noCols());

    if(somethingRandomInMotions)
      { MR.addMatrixProduct(0.0,mass,R, 1.0); }
    else
      { MR.addMatrixProduct(0.0, massSens, R, 1.0); }
    unbalLoad.addMatrixVector(1.0, MR, accelG, -fact);
    return 0;
  }


//! @brief Returns unbalanced load vector.
//!
//! Returns the current unbalanced load. 
const XC::Vector &XC::Node::getUnbalancedLoad(void) const
  { return unbalLoad; }


//! @brief Return unbalanced load vector including inertial forces.
//!
//! Returns the current unbalanced load Vector, as defined above,
//! MINUS the product of the nodes mass matrix and the trial nodal
//! accelerations. The result is saved in another vector which is returned. 
const XC::Vector &XC::Node::getUnbalancedLoadIncInertia(void) const
  {
    unbalLoadWithInertia= this->getUnbalancedLoad();

    if(!mass.isEmpty())
      {
        const Vector &theAccel= getTrialAccel(); // in case accel not created
        unbalLoadWithInertia.addMatrixVector(1.0, mass, theAccel, -1.0);

        if(alphaM != 0.0)
          {
            const Vector &theVel= getTrialVel(); // in case vel not created
            unbalLoadWithInertia.addMatrixVector(1.0, mass, theVel, -alphaM);
          }
      }
    return unbalLoadWithInertia;
  }

//! @brief Commits the state of the node.
//!
//! Causes the node to set the committed model displacements, velocities and
//! accelerations to be equal to the current trial displacements, velocities and
//! accelerations. The incremental displacement is set to 0. No assignment
//! is done for any of the quantities for which no memory has been
//! allocated. Returns 0.
int XC::Node::commitState(void)
  {
    disp.commitState(numberDOF);
    vel.commitState(numberDOF);
    accel.commitState(numberDOF);
    return 0;
  }


//! @brief Returns to the last committed state.
//!
//! Causes the node to set the trial nodal displacements, velocities and
//! accelerations to be equal to the current committed displacements,
//! velocities and accelerations. The incremental displacement is set to 0.
//! No assignment is done for any of the trial quantities for which
//! no memory has been allocated. Returns 0.
int XC::Node::revertToLastCommit(void)
  {
    disp.revertToLastCommit(numberDOF);
    vel.revertToLastCommit(numberDOF);
    accel.revertToLastCommit(numberDOF);
    reaction.Zero();
    return 0;
  }


//! @brief Returns to the initial state.
//!
//! Causes the node to set the trial and committed nodal displacements,
//! velocities and accelerations to zero. No assignment is done for any of
//! the trial quantities for which no memory has been allocated. Returns 0.
int XC::Node::revertToStart(void)
  {
    disp.revertToStart(numberDOF);
    vel.revertToStart(numberDOF);
    accel.revertToStart(numberDOF);

    unbalLoad.Zero();
    reaction.Zero();

    // AddingSensitivity: BEGIN /////////////////////////////////
    dispSensitivity.Zero();
    velSensitivity.Zero();
    accSensitivity.Zero();
    // AddingSensitivity: END ///////////////////////////////////

    // if we get here we are done
    return 0;
  }

//! @brief Return the mass matrix of the node.
//!
//! Returns the mass matrix set for the node, which is a matrix of size
//! ndof,ndof. This matrix is equal to that set in setMass()
//! or zero if setMass() has not been called. If no storage space
//! has been allocated for the mass, a matrix is now created. An error
//! message is printed and the program terminated if no space is available
//! on the heap for this matrix.
const XC::Matrix &XC::Node::getMass(void) const
  { return mass; }

//! @brief Return the mass matrix component for the DOF argument.
double XC::Node::getMassComponent(const int &dof) const
  {
    const size_t sz= mass.noRows();
    Vector J(sz);
    J(dof)= 1.0;
    Vector tmp(sz);
    tmp.addMatrixVector(1.0, mass, J, 1.0);
    const double retval= dot(J,tmp);
    return retval;
  }

//! @brief Sets the Rayleigh dumping factor.
int XC::Node::setRayleighDampingFactor(double alpham)
  {
    alphaM = alpham;
    return 0;
  }


//! @brief Return the damping matrix of the node.
const XC::Matrix &XC::Node::getDamp(void) const
  {
    // make sure it was created before we return it
    if(alphaM == 0.0)
      {
        theMatrices[index].Zero();
        return theMatrices[index];
      }
    else
      {
        Matrix &result= theMatrices[index];
        result= mass;
        result*= alphaM;
        return result;
      }
  }

//! @brief Adds to tributary the lentgth,
//! area o volume being passed as parameter.
void XC::Node::addTributary(const double &t) const
  { tributary+= t; }

//! @brief Zeroes tributary (length, area or volume).
void XC::Node::resetTributary(void) const
  { tributary= 0; }

//! @brief Return tributary value (length, area or volume).
const double &XC::Node::getTributary(void) const
  { return tributary; }

const XC::Matrix &XC::Node::getDampSensitivity(void) const
  {
    // make sure it was created before we return it
    if(alphaM == 0.0)
      {
        theMatrices[index].Zero();
        return theMatrices[index];
      }
    else
      {
        Matrix &result= theMatrices[index];
        result.Zero();
        //result = *mass;
        //result *= alphaM;
        return result;
      }
  }

//! @brief Set the mass matrix of the node.
//!
//! Sets the value of the mass at the node. A check is made to ensure that
//! the \p mass has the same dimensions of the mass matrix associated with the
//! Node; if incompatible size an error message is printed and -1 returned.
//! Returns 0 if successful. 
int XC::Node::setMass(const Matrix &newMass)
  {
    // check right size
    if(newMass.noRows() != numberDOF || newMass.noCols() != numberDOF)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; incompatible matrices."
                  << " A matrix with dimensions: "
		  << numberDOF << 'x' << numberDOF
	          << " was expected."
		  << Color::def << std::endl;
        return -1;
      }
    mass= newMass;
    return 0;
  }

//! Creates a Matrix to store the R matrix.
//! 
//! Creates a Matrix to store the R matrix. The matrix is of dimension
//! \p ndof and \p numCol. Zeroes the
//! matrix R and returns 0 if successful.
int XC::Node::setNumColR(int numCol)
  {
    if(R.noCols() != numCol)
      R= Matrix(numberDOF, numCol);
    R.Zero();
    return 0;
  }

//! Sets the (row,col) entry of R to be equal to \p Value. If
//! no matrix R has been specified or the position in R is out of range a
//! warning message is printed and a -1 is returned. Returns 0 if
//! successful.
int XC::Node::setR(int row, int col, double Value)
  {
    // ensure row, col in range (matrix assignment will catch this - extra work)
    if(row < 0 || row > numberDOF || col < 0 || col > R.noCols())
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << "; row, col index out of range."
	          << Color::def << std::endl;
        return -1;
      }
    // do the assignment
    R(row,col)= Value;
    return 0;
  }


//! @brief This is a method provided for Element objects, the Node object returns
//! the product of the matrix R and the vector V. If the matrix 
//! and vector are of inappropriate size a warning message is printed and
//! a zero vector is returned. 
const XC::Vector &XC::Node::getRV(const Vector &V)
  {
    // we store the product of RV in unbalLoadWithInertia

    // see if quick return , i.e. R == 0
    if(R.isEmpty())
      unbalLoadWithInertia.Zero();
    else if(R.noCols() != V.Size()) // check dimensions of R and V
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << "; R and V of incompatible dimensions.\n"
                  << "R: " << R << "V: " << V
	          << Color::def << std::endl;
        unbalLoadWithInertia.Zero();
      }
    else // determine the product
      unbalLoadWithInertia.addMatrixVector(0.0, R, V, 1.0);
    return unbalLoadWithInertia;
  }


//! @brief Set the dimensions of the matrix that
//! constrains the eigenvectors.
int XC::Node::setNumEigenvectors(int numVectorsToStore)
  {
    // ensure a positive number of vectors
    if(numVectorsToStore <= 0)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; " << numVectorsToStore << " < 0"
	          << Color::def << std::endl;
        return -1;
      }

    // if matrix not yet assigned or not of correct size
    // delete old and create new
    if(getNumModes() != numVectorsToStore)
      theEigenvectors= Matrix(numberDOF, numVectorsToStore);
    else // zero the eigenvector matrix
      theEigenvectors.Zero();
    return 0;
  }

//! @brief Set el eigenvector eigenvector al modo mode.
int XC::Node::setEigenvector(int mode, const Vector &eigenvector)
  {
    int retval= 0;
    if(getNumModes() < mode)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; mode " << mode << " invalid"
	          << Color::def << std::endl;
        retval= -1;
      }
    else if(eigenvector.Size() != numberDOF)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; eigenvector of incorrect size"
	          << Color::def << std::endl;
        retval= -2;
      }
    else
      {
	// set the values
	for(int i=0; i<numberDOF; i++)
	  theEigenvectors(i, mode-1)= eigenvector(i);
      }
    return retval;
  }

//! @brief Return the angular frequency corresponding to the i-th mode.
double XC::Node::getAngularFrequency(int i) const
  {
    const Domain *dom= getDomain();
    assert(dom);
    return dom->getAngularFrequency(i);
  }

//! @brief Returns the angular frequencies from the modal analysis.
XC::Vector XC::Node::getAngularFrequencies(void) const
  {
    const Domain *dom= getDomain();
    assert(dom);
    const int nm= getNumModes();
    Vector retval(nm);
    for(int i= 1;i<=nm;i++)
      retval[i-1]= dom->getAngularFrequency(i);
    return retval;
  }

//! @brief Returns the eigenvectors that correspond to the node.
const XC::Matrix &XC::Node::getEigenvectors(void) const
  { return theEigenvectors; }

//! @brief Returns the eigenvector that corresponds to i-th mode.
XC::Vector XC::Node::getEigenvector(int mode) const
  { return theEigenvectors.getCol(mode-1); }

//! @brief Returns the eigenvector that corresponds to i-th mode
//! normalized so the maximum values of the components
//! is 1 (infinite norm).
XC::Vector XC::Node::getNormalizedEigenvector(int mode) const
  { return normalize_inf(getEigenvector(mode)); }

//! @brief Returns a matrix with the eigenvectors as columns.
XC::Matrix XC::Node::getNormalizedEigenvectors(void) const
  {
    const int n_rows= theEigenvectors.noRows();
    const int nm= getNumModes();
    Matrix retval(n_rows,nm);
    for(int j= 0;j<nm;j++)
      {
        const Vector eigenvector= getNormalizedEigenvector(j+1);
        for(int i= 0;i<n_rows;i++)
          retval(i,j)= eigenvector(i);
      }
    return retval;
  }

//! @brief Remove the stored eigenvectors.
void XC::Node::clearEigenvectors(void)
  {
    theEigenvectors.resize(0,0);
  }

//! @brief Return the "modal" position of the node scaled by
//! factor: return initPos+ factor * getNormalizedEigenvector(mode).
Pos2d XC::Node::getEigenPosition2d(const double &factor, int mode) const
  {
    const Vector fd= factor*getNormalizedEigenvector(mode);
    return getPosition2d(fd);
  }

//! @brief Return the "modal" position of the node scaled by
//! factor: return initPos+ factor * getNormalizedEigenvector(mode).
Pos3d XC::Node::getEigenPosition3d(const double &factor, int mode) const
  {
    const Vector fd= factor*getNormalizedEigenvector(mode);
    return getPosition3d(fd);
  }

//! @brief Return the eigenvector displacement components in a 3D vector.
Vector3d XC::Node::getEigenvectorDisp3dComponents(int mode) const
  { return get3dForceComponents(getEigenvector(mode)); }

//! @brief Return the eigenvector rotation components in a 3D vector.
Vector3d XC::Node::getEigenvectorRot3dComponents(int mode) const
  { return get3dMomentComponents(getEigenvector(mode)); }

//! @brief Returns the modal participation factor
//! corresponding to i-th mode.
double XC::Node::getModalParticipationFactor(int i) const
  {
    const Domain *dom= getDomain();
    assert(dom);
    return dom->getModalParticipationFactor(i);
  }

//! @brief Returns the modal participation factors.
XC::Vector XC::Node::getModalParticipationFactors(void) const
  {
    const int nm= getNumModes();
    Vector retval(nm);
    for(int i= 1;i<=nm;i++)
      retval[i-1]= getModalParticipationFactor(i);
    return retval;
  }

//! @brief Returns the modal participation factor
//! corresponding to i-th mode. If the dofs argument
//! is not empty it "projects" the i-th mode over
//! the selected DOFs in the argument. It's written to allow
//! expressing the participation with respect to a DOF
//! (see verification test: test_cqc_01.xc).
//! THE RESULTS OF THIS FUNCTION DOESN'T MATCH WITH THOSE
//! OBTAINED FROM THE EIGENVALUE PROBLEM
//! @param mode: index of the mode.
//! @param dofs: degrees of freedom to project on.
double XC::Node::getModalParticipationFactor(int mode,const std::set<int> &dofs) const
  {
    double retval= 0;
    if(dofs.empty())
      { retval= getModalParticipationFactor(mode); }
    else
      {
        const Vector ev= getEigenvector(mode);
        const int sz= ev.Size();
        double num= 0;
        if((mass.noRows()!=sz) || (mass.noCols()!=sz))
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; ERROR. The eigenvector has dimension " << sz
                    << " and the mass matrix has dimension " << mass.noRows()
                    << "x" << mass.noCols() << "."
	            << Color::def << std::endl;
        Vector J(sz,0.0);
        for(std::set<int>::const_iterator i= dofs.begin();i!=dofs.end();i++)
          J[*i]= 1;
        num= dot(ev,(mass*J));
        if(num!=0)
          {
            const double denom= dot(ev,(mass*ev));
            retval= num/denom;
          }
      }
    return retval;
  }

//! @brief Returns the modal participation factors. If dofs argument
//! is not empty it "projects" the i-th mode over
//! the selected DOFs in the argument.
//! @param dofs: degrees of freedom to project on.
XC::Vector XC::Node::getModalParticipationFactors(const std::set<int> &dofs) const
  {
    const int nm= getNumModes();
    Vector retval(nm);
    for(int i= 1;i<=nm;i++)
      retval[i-1]= getModalParticipationFactor(i,dofs);
    return retval;
  }

//! @brief Returns the modal participation factors. If dofs argument
//! is not empty it "projects" the i-th mode over
//! the selected DOFs in the argument.
//! @param l: degrees of freedom to project on.
XC::Vector XC::Node::getModalParticipationFactorsForDOFs(const boost::python::list &l) const
  {
    std::set<int> tmp= set_int_from_py_list(l);
    return getModalParticipationFactors(tmp);
  }

//! @brief Returns the distribution factor corresponding to the i-th mode.
XC::Vector XC::Node::getDistributionFactor(int i) const
  { return getModalParticipationFactor(i)*getEigenvector(i); }

//! @brief Returns the distribution factor corresponding to the mode
//! being passed as parameter. If dofs argument
//! is not it "projects" the i-th mode over
//! the selected DOFs in the argument.
//! @param mode: index of the mode.
//! @param dofs: degrees of freedom to project on.
XC::Vector XC::Node::getDistributionFactor(int mode,const std::set<int> &dofs) const
  { return getModalParticipationFactor(mode,dofs)*getEigenvector(mode); }

//! @brief Returns the matrix with the computed distribution factors
//! placed by columns.
XC::Matrix XC::Node::getDistributionFactors(void) const
  {
    Matrix retval;
    const int nm= getNumModes();
    if(nm>0)
      {
        Vector distribFactor= getDistributionFactor(1);
        const int n_rows= distribFactor.Size();
        retval= Matrix(n_rows,nm);
        for(int i= 0;i<n_rows;i++)
          retval(i,0)= distribFactor(i);
        for(int j= 2;j<=nm;j++)
          {
            distribFactor= getDistributionFactor(j);
            for(int i= 0;i<n_rows;i++)
              retval(i,j-1)= distribFactor(i);
          }
      }
    return retval;
  }

//! @brief Return the effective modal mass
//! that corresponds to i mode.
double XC::Node::getEffectiveModalMass(int mode) const
  {
    double retval= 0;
    const Vector ev= getEigenvector(mode);
    const int sz= ev.Size();
    const double tau= getModalParticipationFactor(mode);
    const Vector J(sz,1.0);
    retval= tau*dot(ev,(mass*J));
    return retval;
  }

//! @brief Returns the effective modal masses.
XC::Vector XC::Node::getEffectiveModalMasses(void) const
  {
    const int nm= getNumModes();
    Vector retval(nm);
    for(int i= 1;i<=nm;i++)
      retval[i-1]= getEffectiveModalMass(i);
    return retval;
  }

//! @brief Return the equivalent static load for the mode
//! being passed as parameter and the acceleration corresponding to that mode.
XC::Vector XC::Node::getEquivalentStaticLoad(int mode,const double &accel_mode) const
  {
    Vector retval= mass*getDistributionFactor(mode);
    retval*=(accel_mode);
    return retval;
  }

//! @brief Returns the maximal modal displacement for the node
//! being passed as parameter and the acceleration corresponding to that mode.
XC::Vector XC::Node::getMaxModalDisplacement(int mode,const double &accel_mode) const
  {
    return getMaxModalAcceleration(mode,accel_mode)/sqr(getAngularFrequency(mode));
  }

//! @brief Return the maximal modal velocity for the mode
//! being passed as parameter and the acceleration corresponding to that mode.
XC::Vector XC::Node::getMaxModalVelocity(int mode,const double &accel_mode) const
  {
    return getMaxModalAcceleration(mode,accel_mode)/getAngularFrequency(mode);
  }

//! @brief Return the maximal modal acceleration for the mode
//! being passed as parameter and the acceleration corresponding to that mode.
XC::Vector XC::Node::getMaxModalAcceleration(int mode,const double &accel_mode) const
  {
    return accel_mode*getDistributionFactor(mode);
  }

//! @brief Returns the maximal modal displacement on the DOFs and mode being passed
//! as parameter and the acceleration corresponding to that mode.
//! @param dofs: degrees of freedom to deal with.
XC::Vector XC::Node::getMaxModalDisplacementForDOFs(int mode,const double &accel_mode,const std::set<int> &dofs) const
  {
    return getMaxModalAccelerationForDOFs(mode,accel_mode,dofs)/sqr(getAngularFrequency(mode));
  }

//! @brief Returns the maximal modal displacement on the DOFs and mode being passed
//! as parameter and the acceleration corresponding to that mode.
//! @param dofs: degrees of freedom to deal with.
XC::Vector XC::Node::getMaxModalDisplacementForDOFs(int mode,const double &accel_mode,const boost::python::list &dofs) const
  {
    std::set<int> tmp= set_int_from_py_list(dofs);
    return getMaxModalDisplacementForDOFs(mode,accel_mode,tmp);
  }


//! @brief Returns the maximum modal velocity on the DOFs and mode being passed
//! as parameter and the acceleration corresponding to that mode.
//! @param dofs: degrees of freedom to deal with.
XC::Vector XC::Node::getMaxModalVelocityForDOFs(int mode,const double &accel_mode,const std::set<int> &dofs) const
  {
    return getMaxModalAccelerationForDOFs(mode,accel_mode,dofs)/getAngularFrequency(mode);
  }

//! @brief Returns the maximum modal velocity on the DOFs and mode being passed
//! as parameter and the acceleration corresponding to that mode.
//! @param dofs: degrees of freedom to deal with.
XC::Vector XC::Node::getMaxModalVelocityForDOFs(int mode,const double &accel_mode,const boost::python::list &dofs) const
  {
    std::set<int> tmp= set_int_from_py_list(dofs);
    return getMaxModalVelocityForDOFs(mode,accel_mode,tmp);
  }

//! @brief Return the maximal modal acceleration on the DOFs and mode being passed
//! as parameter and the acceleration corresponding to that mode.
//! @param dofs: degrees of freedom to deal with.
XC::Vector XC::Node::getMaxModalAccelerationForDOFs(int mode,const double &accel_mode,const std::set<int> &dofs) const
  {
    return accel_mode*getDistributionFactor(mode,dofs);
  }

//! @brief Return the maximal modal acceleration on the DOFs and mode being passed
//! as parameter and the acceleration corresponding to that mode.
//! @param dofs: degrees of freedom to deal with.
XC::Vector XC::Node::getMaxModalAccelerationForDOFs(int mode,const double &accel_mode,const boost::python::list &dofs) const
  {
    std::set<int> tmp= set_int_from_py_list(dofs);
    return getMaxModalAccelerationForDOFs(mode,accel_mode,tmp);
  }


//! @brief Returns a vector to store the dbTags
//! de los miembros of the clase.
XC::DbTagData &XC::Node::getDbTagData(void) const
  {
    static DbTagData retval(22);
    return retval;
  }

//! @brief Sends object members through the communicator argument.
int XC::Node::sendData(Communicator &comm)
  {
    int res= MeshComponent::sendData(comm);
    res+= comm.sendInt(numberDOF,getDbTagData(),CommMetaData(4));
    res+= comm.sendMatrix(mass,getDbTagData(),CommMetaData(5));
    res+= comm.sendVector(reaction,getDbTagData(),CommMetaData(6));
    res+= comm.sendVector(unbalLoad,getDbTagData(),CommMetaData(7));
    res+= comm.sendVector(unbalLoadWithInertia,getDbTagData(),CommMetaData(8));
    res+= comm.sendVector(Crd,getDbTagData(),CommMetaData(9));
    res+= comm.sendMatrix(R,getDbTagData(),CommMetaData(10));
    res+= comm.sendDoubles(alphaM,tributary,getDbTagData(),CommMetaData(11)); //Arreglar.
    res+= comm.sendMatrix(theEigenvectors,getDbTagData(),CommMetaData(12));
    res+=comm.sendMovable(disp,getDbTagData(),CommMetaData(13));
    res+=comm.sendMovable(vel,getDbTagData(),CommMetaData(14));
    res+=comm.sendMovable(accel,getDbTagData(),CommMetaData(15));
    const ID nodeLockersTags= get_node_lockers_tags();
    res+= comm.sendID(nodeLockersTags,getDbTagData(),CommMetaData(16));
    std::vector<ID> constraintsTags= get_constraints_tags();
    res+= comm.sendIDs(constraintsTags, getDbTagData(),CommMetaData(17));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::Node::recvData(const Communicator &comm)
  {
    int res= MeshComponent::recvData(comm);
    res+= comm.receiveInt(numberDOF,getDbTagData(),CommMetaData(4));
    res+= comm.receiveMatrix(mass,getDbTagData(),CommMetaData(5));
    res+= comm.receiveVector(reaction,getDbTagData(),CommMetaData(6));
    res+= comm.receiveVector(unbalLoad,getDbTagData(),CommMetaData(7));
    res+= comm.receiveVector(unbalLoadWithInertia,getDbTagData(),CommMetaData(8));
    res+= comm.receiveVector(Crd,getDbTagData(),CommMetaData(9));
    res+= comm.receiveMatrix(R,getDbTagData(),CommMetaData(10));
    res+= comm.receiveDoubles(alphaM,tributary,getDbTagData(),CommMetaData(11));
    res+= comm.receiveMatrix(theEigenvectors,getDbTagData(),CommMetaData(12));
    this->index= -1;
    res+= comm.receiveMovable(disp,getDbTagData(),CommMetaData(13));
    res+= comm.receiveMovable(vel,getDbTagData(),CommMetaData(14));
    res+= comm.receiveMovable(accel,getDbTagData(),CommMetaData(15));
    ID nodeLockersTags;
    res+= comm.receiveID(nodeLockersTags,getDbTagData(),CommMetaData(16));
    std::vector<ID> constraintsTags;
    res+= comm.receiveIDs(constraintsTags, getDbTagData(),CommMetaData(17));
    set_id_constraints(nodeLockersTags, constraintsTags);
    setup_matrices(theMatrices,numberDOF);
    return res;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::Node::getPyDict(void) const
  {
    boost::python::dict retval= DomainComponent::getPyDict();
    retval["numberDOF"]= numberDOF;
    retval["mass"]= mass.getPyList();
    retval["reaction"]= reaction.getPyList();
    retval["unbalLoad"]= unbalLoad.getPyList();
    retval["unbalLoadWithInertia"]= unbalLoadWithInertia.getPyList();
    retval["Crd"]= Crd.getPyList();
    retval["R"]= R.getPyList();
    retval["alphaM"]= alphaM;
    retval["tributary"]= tributary;
    retval["theEigenvectors"]= theEigenvectors.getPyList();
    retval["disp"]= disp.getPyDict();
    retval["vel"]= vel.getPyDict();
    retval["accel"]= accel.getPyDict();
    const ID nodeLockersTags= get_node_lockers_tags();
    retval["node_loackers_tags"]= nodeLockersTags.getPyList(); 
    std::vector<ID> constraintsTags= get_constraints_tags();
    boost::python::list pyList;
    for( std::vector<ID>::const_iterator i= constraintsTags.begin(); i!= constraintsTags.end(); i++)
      pyList.append((*i).getPyList());
    retval["constraintsTags"]= pyList; 
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::Node::setPyDict(const boost::python::dict &d)
  {
    DomainComponent::setPyDict(d);
    this->numberDOF= boost::python::extract<int>(d["numberDOF"]);
    mass= Matrix(boost::python::extract<boost::python::list>(d["mass"]));
    reaction= Vector(boost::python::extract<boost::python::list>(d["reaction"]));
    unbalLoad= Vector(boost::python::extract<boost::python::list>(d["unbalLoad"]));
    unbalLoadWithInertia= Vector(boost::python::extract<boost::python::list>(d["unbalLoadWithInertia"]));
    Crd= Vector(boost::python::extract<boost::python::list>(d["Crd"]));
    R= Matrix(boost::python::extract<boost::python::list>(d["R"]));
    this->alphaM= boost::python::extract<double>(d["alphaM"]);
    this->tributary= boost::python::extract<double>(d["tributary"]);
    theEigenvectors= Matrix(boost::python::extract<boost::python::list>(d["theEigenvectors"]));
    this->index= -1;
    disp.setPyDict(boost::python::extract<boost::python::dict>(d["disp"]));
    vel.setPyDict(boost::python::extract<boost::python::dict>(d["vel"]));
    accel.setPyDict(boost::python::extract<boost::python::dict>(d["accel"]));
    ID nodeLockersTags= ID(boost::python::extract<boost::python::list>(d["node_loackers_tags"]));
    boost::python::list pyList= boost::python::extract<boost::python::list>(d["constraintsTags"]);
    const size_t sz= boost::python::len(pyList);
    std::vector<ID> constraintsTags(sz);
    for(size_t i= 0;i<sz; i++)
      {
	constraintsTags[i]= ID(boost::python::extract<boost::python::list>(pyList[i]));
      }
    this->set_id_constraints(nodeLockersTags, constraintsTags);
    setup_matrices(theMatrices,numberDOF);    
  }

//! @brief Send the object through the communicator argument.
//!
//! Causes the Node object to send the data needed to init itself on a
//! remote machine to the Communicator object \p cp. 
//! The data sent includes the tag, number of dof, coordinates, committed
//! response quantities, unbalanced load, mass and participation matrix. 
//! To do this the Node creates an ID object into which it stores its tag,
//! the \p ndof and a flag indicating whether any additional
//! information, i.e. mass, response quantities also need to be sent. In
//! addition four database tags are also included in this ID object. The
//! database tags, if not already obtained, are requested from the cp
//! object (these are needed as each object can only store a single object
//! of a particular size using it's own database tags -- additional tags
//! are needed when multiple objects of the same size are needed.
//! The objects that have been created are then sent.
int XC::Node::sendSelf(Communicator &comm)
  {
    inicComm(22);
    int res= sendData(comm);

    const int dataTag= getDbTag();
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to send ID data."
	        << Color::def << std::endl;
    return res;
  }

//! @brief Receives object through the communicator argument.
//!
//! Invoked on a remote machine to read its data that was sent by a node
//! object in another actor when sendSelf() was invoked. As in {\em
//! sendSelf()}, the Node object creates an ID object. It asks the Channel
//! object to fill this object with data. Based on the data it creates
//! Matrix and Vector objects to store the Nodes data and asks the cp
//! object to fill these with data. The data placed here by the cp
//! object correspond to the data put there by the sending Node object.
int XC::Node::recvSelf(const Communicator &comm)
  {
    const int dataTag= getDbTag();
    inicComm(22);
    int res = comm.receiveIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ID data"
	        << Color::def << std::endl;
    else
      res+= recvData(comm);
    return res;
  }

//! @brief Returns the sets that include this node.
std::set<XC::SetBase *> XC::Node::get_sets(void) const
  {
    std::set<SetBase *> retval;
    const Preprocessor *preprocessor= getPreprocessor();
    if(preprocessor)
      {
        MapSet &sets= const_cast<MapSet &>(preprocessor->get_sets());
        retval= sets.get_sets(this);
      }
    else
      std::cerr << Color::red << getClassName() << __FUNCTION__
	        << "; preprocessor needed."
		<< Color::def << std::endl;
    return retval;
  }

//! @brief Adds the node to the sets being passed as parameters.
void XC::Node::add_to_sets(std::set<SetBase *> &other_sets)
  {
    std::set<SetBase *> this_sets= this->get_sets();
    for(std::set<SetBase *>::iterator i= other_sets.begin();i!= other_sets.end();i++)
      {
	SetBase *other_set= *i;
	if(this_sets.count(other_set)==0) // element not already in set.
	  {
	    SetMeshComp *other_smc= dynamic_cast<SetMeshComp *>(other_set);
	    if(other_smc)
	      other_smc->addNode(this);
	  }
      }
  }

//! @brief Add this element to all the sets containing the given one.
void XC::Node::copySetsFrom(const Node &n)
  {
    std::set<SetBase *> other_sets= n.get_sets();
    this->add_to_sets(other_sets);
  }

//! @brief Prints node data.
//!
//! Causes the node to print out its tag, mass matrix, and committed
//! response quantities. 
void XC::Node::Print(std::ostream &s, int flag) const
  {
    if(flag == 0)
      { // print out everything
        s << "\n "<< getClassName()<< ": " << getTag() << std::endl;
        s << "\tCoordinates  : " << Crd;
        disp.Print(s,flag);
        vel.Print(s,flag);
        accel.Print(s,flag);
        s << "\t unbalanced load: " << unbalLoad;
        s << "\t reaction: " << reaction;
        s << "\tMass : " << mass;
        s << "\t Eigenvectors: " << theEigenvectors;
        if(theDOF_GroupPtr != 0)
          s << "\tID : " << theDOF_GroupPtr->getID();
        s << "\n";
      }
  }


// AddingSensitivity:BEGIN ///////////////////////////////////////

XC::Matrix XC::Node::getMassSensitivity(void) const
  {
    Matrix massSens(mass.noRows(),mass.noCols());
    if( (parameterID == 1) || (parameterID == 2) || (parameterID == 3) )
     { massSens(parameterID-1,parameterID-1) = 1.0; }
    return massSens;
  }


int XC::Node::getCrdsSensitivity(void) const
  {
    if( (parameterID == 4) || (parameterID == 5) || (parameterID == 6) )
      { return (parameterID-3);        }
    else
      { return 0; }
  }


int XC::Node::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    // The following parameterID map is being used:
    // 1: nodal mass in direction 1
    // 2: nodal mass in direction 2
    // 3: nodal mass in direction 3
    // 4: coordinate in direction 1
    // 5: coordinate in direction 2
    // 6: coordinate in direction 3


    //const int argc= argv.size();
    if(argv.size() < 2)
      return -1;

    if((strstr(argv[0].c_str(),"mass") != 0) || (strstr(argv[0].c_str(),"-mass") != 0))
      { 
        int direction = 0; // atoi(argv[1]);
        if((argv[1] == "x")||(argv[1] == "X")||(argv[1] == "1"))
          direction = 1;
        else if((argv[1] == "y")||(argv[1] == "Y")||(argv[1] == "2"))
          direction = 2;
        else if((argv[1] == "z")||(argv[1] == "Z")||(argv[1] == "3"))                                        
          direction = 3;
        else if((argv[1] == "xy")||(argv[1] == "XY"))
          direction = 7;
        else if((argv[1] == "xyz")||(argv[1] == "XYZ"))
          direction = 8;
        if((direction >= 1 && direction <= 3) || direction == 7 || direction == 8)
          return param.addObject(direction, this);
      }
    else if(strstr(argv[0].c_str(),"coord") != 0)
      {
        int direction = atoi(argv[1]);
        if(direction >= 1 && direction <= 3)
          return param.addObject(direction+3, this);
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; could not set parameter in node: "
		<< getTag()
		<< Color::def << std::endl;
    return -1;
  }


int XC::Node::updateParameter(int pparameterID, Information &info)
  {
    if( (pparameterID == 1) || (pparameterID == 2) || (pparameterID == 3) )
      { mass(pparameterID-1,pparameterID-1) = info.theDouble; }
    else
      if( (pparameterID == 4) || (pparameterID == 5) || (pparameterID == 6) )
        {
          if(Crd(pparameterID-4) != info.theDouble)
            {
              //Set the coordinate value.
              Crd(pparameterID-4) = info.theDouble;

              // Need to "setDomain" to make the change take effect.
              Domain *theDomain = this->getDomain();
              ElementIter &theElements = theDomain->getElements();
              Element *theElement;
              while((theElement = theElements()) != 0)
                { theElement->setDomain(theDomain); }
            }
          else
            {
            // No change in nodal coordinate
            }
        }
    return 0;
  }


int XC::Node::activateParameter(int passedParameterID)
  {
    parameterID = passedParameterID;
    return 0;
  }


int XC::Node::saveSensitivity(Vector *v,Vector *vdot,Vector *vdotdot, int gradNum, int numGrads)
  {
    // Initial declarations
    int i;

    // If the sensitivity matrices are not already created:
    if(dispSensitivity.isEmpty())
      { dispSensitivity= Matrix(numberDOF,numGrads ); }
    if((vdot!=0) && (vdotdot!=0) )
      {
        if(velSensitivity.isEmpty())
          { velSensitivity= Matrix(numberDOF,numGrads ); }
        if(accSensitivity.isEmpty())
          { accSensitivity= Matrix(numberDOF,numGrads ); }
      }


    // Put GRADIENT VECTORS into COLUMNS of matrices
    for(i=0; i<numberDOF; i++ )
      { dispSensitivity(i,gradNum-1) = (*v)(i); }
    if( (vdot!=0) && (vdotdot!=0) )
      {
        for(i=0; i<numberDOF; i++ )
          { velSensitivity(i,gradNum-1) = (*vdot)(i); }
        for(i=0; i<numberDOF; i++ )
          { accSensitivity(i,gradNum-1) = (*vdotdot)(i); }
      }
    return 0;
  }

//! @brief Return displacement sensitivity.
double XC::Node::getDispSensitivity(int dof, int gradNum) const
  {
    double result= 0.0;
    if(!dispSensitivity.isEmpty())
      result= dispSensitivity(dof-1,gradNum-1);
    return result;
  }

//! @brief Return velocity sensitivity.
double XC::Node::getVelSensitivity(int dof, int gradNum) const
  {
    double result= 0.0;
    if(!velSensitivity.isEmpty())
      result= velSensitivity(dof-1,gradNum-1);
    return result;
  }

//! @brief Return acceleration sensitivity.
double XC::Node::getAccSensitivity(int dof, int gradNum) const
  {
    double result= 0.0;
    if(!accSensitivity.isEmpty())
      return accSensitivity(dof-1,gradNum-1);
    return result;
  }
// AddingSensitivity:END /////////////////////////////////////////

//! @brief Return the nunber of connected constraints.
size_t XC::Node::getNumberOfConnectedConstraints(void) const
  {
    size_t retval= 0;
    for(std::set<ContinuaReprComponent *>::const_iterator i= connected.begin();i!=connected.end();i++)
      {
        const Constraint *ptr= dynamic_cast<const Constraint *>(*i);
        if(ptr)
          retval++;
      }
    return retval;    
  }

//! @brief Return the nunber of connected constraints.
size_t XC::Node::getNumberOfConnectedConstraints(const SetBase *s) const
  {
    size_t retval= 0;
    for(std::set<ContinuaReprComponent *>::const_iterator i= connected.begin();i!=connected.end();i++)
      {
        const Constraint *ptr= dynamic_cast<const Constraint *>(*i);
        if(ptr)
	  if(s->In(ptr))
	    retval++;
      }
    return retval;    
  }

//! @brief Return a set of const pointers the constraints connected to the node.
std::set<const XC::Constraint *> XC::Node::getConnectedConstraints(void) const
  {
    std::set<const Constraint *> retval;
    for(std::set<ContinuaReprComponent *>::const_iterator i= connected.begin();i!=connected.end();i++)
      {
        const Constraint *ptr= dynamic_cast<const Constraint *>(*i);
        if(ptr)
          retval.insert(ptr);
      }
    return retval;    
  }

//! @brief Return a set of const pointers the constraints of the set that are
//! connected to the node.
std::set<const XC::Constraint *> XC::Node::getConnectedConstraints(const SetBase *s) const
  {
    std::set<const Constraint *> retval;
    const std::set<const Constraint *> tmp= this->getConnectedConstraints();
    if(s)
      {
	for(std::set<const Constraint *>::const_iterator i= tmp.begin(); i!= tmp.end(); i++)
	  {
	    const Constraint *pC= *i;
	    if(s->In(pC))
	      retval.insert(pC);
	  }
      }
    else
      retval= tmp;
    return retval;    
  }

//! @brief Return a set of pointers the constraints connected to the node.
std::set<XC::Constraint *> XC::Node::getConnectedConstraints(void)
  {
    std::set<Constraint *> retval;
    for(std::set<ContinuaReprComponent *>::iterator i= connected.begin();i!=connected.end();i++)
      {
        Constraint *ptr= dynamic_cast<Constraint *>(*i);
        if(ptr)
          retval.insert(ptr);
      }
    return retval;    
  }

//! @brief Return a set of pointers the constraints of the given set connected
//! to this node.
std::set<XC::Constraint *> XC::Node::getConnectedConstraints(const SetBase *s)
  {
    std::set<Constraint *> retval;
    const std::set<Constraint *> tmp= this->getConnectedConstraints();
    if(s)
      {
	for(std::set<Constraint *>::const_iterator i= tmp.begin(); i!= tmp.end(); i++)
	  {
	    Constraint *pC= *i;
	    if(s->In(pC))
	      retval.insert(pC);
	  }
      }
    else
      retval= tmp;
    return retval;    
  }

//! @brief Return a Python list of pointers to the constraints connected to the node.
boost::python::list XC::Node::getConnectedConstraintsPy(void)
  {
    boost::python::list retval;
    std::set<Constraint *> constraints= getConnectedConstraints();
    for(std::set<ContinuaReprComponent *>::iterator i= connected.begin();i!=connected.end();i++)
      {
        Constraint *ptrConstraint= dynamic_cast<Constraint *>(*i);
	if(ptrConstraint)
	  {
	    boost::python::object pyObj(boost::ref(*ptrConstraint));
	    retval.append(pyObj);
	  }
      }
    return retval;
  }

//! @brief Return a Python list of pointers to the constraints connected to the node.
boost::python::list XC::Node::getConnectedConstraintsPy(const SetBase *s)
  {
    boost::python::list retval;
    if(s)
      {
	std::set<Constraint *> constraints= getConnectedConstraints();
	for(std::set<ContinuaReprComponent *>::iterator i= connected.begin();i!=connected.end();i++)
	  {
	    Constraint *ptrConstraint= dynamic_cast<Constraint *>(*i);
	    if(ptrConstraint)
	      if(s->In(ptrConstraint))
		{
		  boost::python::object pyObj(boost::ref(*ptrConstraint));
		  retval.append(pyObj);
		}
	  }
      }
    else
      retval= this->getConnectedConstraintsPy();
    return retval;
  }

//! @brief Return a Python list with the tags of the constraints connected to the node.
boost::python::list XC::Node::getConnectedConstraintTags(void) const
  {
    boost::python::list retval;
    std::set<const Constraint *> constraints= getConnectedConstraints();
    for(std::set<ContinuaReprComponent *>::const_iterator i= connected.begin();i!=connected.end();i++)
      {
        const Constraint *ptrConstraint= dynamic_cast<const Constraint *>(*i);
	if(ptrConstraint)
	  {
	    const int tag= ptrConstraint->getTag();
	    retval.append(tag);
	  }
      }
    return retval;
  }

//! @brief Return a Python list with the tags of the constraints of the
//! given set that are connected to this node.
boost::python::list XC::Node::getConnectedConstraintTags(const SetBase *s) const
  {
    boost::python::list retval;
    if(s)
      {
	std::set<const Constraint *> constraints= getConnectedConstraints();
	for(std::set<ContinuaReprComponent *>::const_iterator i= connected.begin();i!=connected.end();i++)
	  {
	    const Constraint *ptrConstraint= dynamic_cast<const Constraint *>(*i);
	    if(ptrConstraint)
	      if(s->In(ptrConstraint))
		{
		  const int tag= ptrConstraint->getTag();
		  retval.append(tag);
		}
	  }
      }
    else
      retval= this->getConnectedConstraintTags();
    return retval;
  }

//! @brief Return the number of elements that are connected to this node.
size_t XC::Node::getNumberOfConnectedElements(void) const
  {
    size_t retval= 0;
    for(std::set<ContinuaReprComponent *>::const_iterator i= connected.begin();i!=connected.end();i++)
      {
        const Element *ptrElem= dynamic_cast<const Element *>(*i);
        if(ptrElem)
          retval++;
      }
    return retval;
  }

//! @brief Return the number of elements from the given set that are connected
// to this node.
size_t XC::Node::getNumberOfConnectedElements(const SetBase *s) const
  {
    size_t retval= 0;
    for(std::set<ContinuaReprComponent *>::const_iterator i= connected.begin();i!=connected.end();i++)
      {
        const Element *ptrElem= dynamic_cast<const Element *>(*i);
        if(ptrElem)
	  if(s->In(ptrElem))
	    retval++;
      }
    return retval;
  }

//! @brief Return a set of pointers to the elements that are connected with this node.
XC::Node::ElementConstPtrSet XC::Node::getConnectedElements(void) const
  {
    XC::Node::ElementConstPtrSet retval;
    for(std::set<ContinuaReprComponent *>::const_iterator i= connected.begin();i!=connected.end();i++)
      {
        const Element *ptrElem= dynamic_cast<const Element *>(*i);
        if(ptrElem)
          retval.insert(ptrElem);
      }
    return retval;
  }

//! @brief Return a set of pointers to the elements of the given set that are
//! connected with this node.
XC::Node::ElementConstPtrSet XC::Node::getConnectedElements(const SetBase *s) const
  {
    XC::Node::ElementConstPtrSet retval;
    const std::set<const Element *> tmp= this->getConnectedElements();
    if(s)
      {
	for(std::set<const Element *>::const_iterator i= tmp.begin();i!=tmp.end();i++)
	  {
	    const Element *ptrElem= *i;
	    if(ptrElem)
	      if(s->In(ptrElem))
		retval.insert(ptrElem);
	  }
      }
    else
      retval= tmp;
    return retval;
  }

//! @brief Return a set of pointers to the elements that are connected with this node.
XC::Node::ElementPtrSet XC::Node::getConnectedElements(void)
  {
    XC::Node::ElementPtrSet retval;
    for(std::set<ContinuaReprComponent *>::const_iterator i= connected.begin();i!=connected.end();i++)
      {
        Element *ptrElem= dynamic_cast<Element *>(*i);
        if(ptrElem)
          retval.insert(ptrElem);
      }
    return retval;
  }

//! @brief Return a set of pointers to the elements of the given set that are
//! connected with this node.
XC::Node::ElementPtrSet XC::Node::getConnectedElements(const SetBase *s)
  {
    XC::Node::ElementPtrSet retval;
    const XC::Node::ElementPtrSet tmp= this->getConnectedElements();
    if(s)
      {
	for(XC::Node::ElementPtrSet::const_iterator i= tmp.begin();i!=tmp.end();i++)
	  {
	    Element *ptrElem= *i;
	    if(ptrElem)
	      if(s->In(ptrElem))
		retval.insert(ptrElem);
	  }
      }
    else
      retval= tmp;
    return retval;
  }

//! @brief Return a python list of pointers to the elements that
//! are connected with this node.
boost::python::list XC::Node::getConnectedElementsPy(void)
  {
    boost::python::list retval;
    XC::Node::ElementPtrSet elements= getConnectedElements();
    for(XC::Node::ElementPtrSet::iterator i= elements.begin(); i!= elements.end(); i++)
      {
        Element *ptrElem= *i;
	boost::python::object pyObj(boost::ref(*ptrElem));
	retval.append(pyObj);
      }
    return retval;
  }

//! @brief Return a python list of pointers to the elements from the give set
//! that are connected with this node.
boost::python::list XC::Node::getConnectedElementsPy(const SetBase *s)
  {
    boost::python::list retval;
    XC::Node::ElementPtrSet elements= getConnectedElements(s);
    for(XC::Node::ElementPtrSet::iterator i= elements.begin(); i!= elements.end(); i++)
      {
        Element *ptrElem= *i;
	boost::python::object pyObj(boost::ref(*ptrElem));
	retval.append(pyObj);
      }
    return retval;
  }

//! @brief Return a python list containing the tags of the elements that
//! are connected with this node.
boost::python::list XC::Node::getConnectedElementTags(void) const
  {
    boost::python::list retval;
    std::set<const Element *> elements= this->getConnectedElements();
    for(std::set<const Element *>::iterator i= elements.begin(); i!= elements.end(); i++)
      {
        const Element *ptrElem= *i;
        retval.append(ptrElem->getTag());
      }
    return retval;
  }

//! @brief Return a python list containing the tags of the elements from the
//! given set that are connected with this node.
boost::python::list XC::Node::getConnectedElementTags(const SetBase *s) const
  {
    boost::python::list retval;
    std::set<const Element *> elements= this->getConnectedElements(s);
    for(std::set<const Element *>::iterator i= elements.begin(); i!= elements.end(); i++)
      {
        const Element *ptrElem= *i;
        retval.append(ptrElem->getTag());
      }
    return retval;
  }

//! @brief Returns an edge that has its origin in this node (and is not in visited).
const XC::MeshEdge *XC::Node::next(const std::deque<MeshEdge> &edges, const std::set<const MeshEdge *> &visited) const
  {
    const MeshEdge *retval= nullptr;
    for(std::deque<MeshEdge>::const_iterator i= edges.begin();i!=edges.end();i++)
      {
        const MeshEdge &edge= *i;
        if(visited.find(&edge)==visited.end()) //Not already visited.
          if(edge.getFirstNode()==this) //Edge starts in this node.
            {
              retval= &edge;
              break;
            }
      }
    return retval;
  }


//! @brief Return the action of the elements from the set over this node.
const XC::Vector &XC::Node::getResistingForce(const std::set<const Element *> &elements,const bool &inc_inertia) const
  {
    static Vector retval;
    retval.resize(numberDOF);
    retval.Zero();
    if(isAlive())
      {
        for(std::set<ContinuaReprComponent *>::const_iterator i= connected.begin();i!=connected.end();i++)
          if((*i)->isAlive())
            {
              const Element *ptrElem= dynamic_cast<const Element *>(*i);
              if(ptrElem)
                {
                  if(elements.count(ptrElem)>0)
                    {
                      if(!inc_inertia)
                        retval+= ptrElem->getNodeResistingForce(this);
                      else
                        retval+= ptrElem->getNodeResistingForceIncInertia(this);
                    }
                }
              else
		std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		          << "; node: " << getTag()
                          << " has a constraint."
			  << Color::def << std::endl;
            }
      }
    return retval; 
  }

//! @brief Returns the sliding vector system that represents
//! the action of the elements of the set over the node.
SlidingVectorsSystem3d XC::Node::getResistingSlidingVectorsSystem3d(const std::set<const Element *> &elements,const bool &inc_inertia) const
  {
    SlidingVectorsSystem3d retval;
    const Pos3d o= getInitialPosition3d();
    const Vector &v= getResistingForce(elements,inc_inertia);
    if(numberDOF==2)
      retval= SlidingVectorsSystem3d(o,Vector3d(v[0],v[1],0));
    else if(numberDOF==3)
      retval= SlidingVectorsSystem3d(o,Vector3d(v[0],v[1],0),Vector3d(0,0,v[2]));
    else if(numberDOF==6)
      retval= SlidingVectorsSystem3d(o,Vector3d(v[0],v[1],v[2]),Vector3d(v[3],v[4],v[5]));
    else
      {
        retval= SlidingVectorsSystem3d(o);
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; dof number: "
                  << numberDOF << " not covered."
                  << Color::def << std::endl;
      }
    return retval; 
  }

//! @brief Return the sum of the stiffness matrix
//! of a subset of the connected elements.
//!
//! @param elements: subset of elements.
//! @param initial: if true initial stiffness is requested, otherwise it returns the
//!                 tangent stiffness.
XC::Matrix XC::Node::get_element_stiff(const ElementConstPtrSet &elements, bool initial) const
  {
    Matrix retval(numberDOF,numberDOF);
    if(isAlive())
      {
        for(std::set<ContinuaReprComponent *>::const_iterator i= connected.begin();i!=connected.end();i++)
          if((*i)->isAlive())
            {
              const Element *ptrElem= dynamic_cast<const Element *>(*i);
              if(ptrElem) // connected element.
                {
                  if(elements.count(ptrElem)>0)
                    {
		      if(initial)
		        retval+= ptrElem->getNodeMatrixComponents(this,ptrElem->getInitialStiff());
		      else
		        retval+= ptrElem->getNodeMatrixComponents(this,ptrElem->getTangentStiff());
		    }
		}
            }
      }
    return retval;
  }

//! @brief Tries to return a stiffness matrix that
//! represent the stiffness of the constraints connected
//! to the node. It's used in getTangentStiff and
//! getInitialStiff methods.
XC::Matrix XC::Node::get_constraints_stiff(void) const
  {
    Matrix retval(numberDOF,numberDOF);
    const int nodeTag= getTag();
    if(isAlive())
      {
        for(std::set<ContinuaReprComponent *>::const_iterator i= connected.begin();i!=connected.end();i++)
          if((*i)->isAlive())
            {
	      const SFreedom_Constraint *constraint= dynamic_cast<const SFreedom_Constraint *>(*i);
	      if(constraint)
		{
		  for(int i= 0;i<numberDOF;i++)
		    {
		      if(constraint->affectsNodeAndDOF(nodeTag, i))
			{ retval(i,i)+= 1.0e15; } //Tries to represent the constraint stiffness.
		    }
		}
	      else
		{
		  std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			    << "; constraint of type: " << (*i)->getClassName()
			    << " not implemented yet."
			    << Color::def << std::endl;
		}
            }
      }
    return retval; 
  }

//! @brief Return the tangent stiffness contribution of the elements
//! argument.
//!
//! Each of the elements connected to the node contributes to the stiffness
//! of the model in that node. This methods return the contribution (if any)
//! of the elements being passed as parameter.
XC::Matrix XC::Node::getTangentStiff(const ElementConstPtrSet &elements) const
  { return get_element_stiff(elements,false)+get_constraints_stiff(); }

//! @brief Return the initial stiffness contribution of the elements
//! argument.
//!
//! Each of the elements connected to the node contributes to the stiffness
//! of the model in that node. This methods return the contribution (if any)
//! of the elements being passed as parameter.
XC::Matrix XC::Node::getInitialStiff(const ElementConstPtrSet &elements) const
  { return get_element_stiff(elements,true)+get_constraints_stiff(); }

//! @brief Return the node reaction
const XC::Vector &XC::Node::getReaction(void) const
  { return reaction; }

//! @brief Set the node reaction
void XC::Node::setReaction(const Vector &r)
  {
    std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
	      << "; setting reaction value is not a standard"
              << " procedure (you're at your own)"
	      << Color::def << std::endl;
    const int sz= reaction.Size();
    if(r.Size()!=sz)
      std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
	        << "; vector argument has a size of: "
	        << r.Size() << " a vector of size: "
	        << sz << " was expected."
		<< Color::def << std::endl;
    reaction= r;
  }

//! @brief Return the reaction force in a 3D vector.
Vector3d XC::Node::getReactionForce3d(void) const
  { return get3dForceComponents(reaction); }

//! @brief Return the reaction moment in a 3D vector.
Vector3d XC::Node::getReactionMoment3d(void) const
  { return get3dMomentComponents(reaction); }

//! @brief Return the "force/displacement" components in a 3d vector.
Vector3d XC::Node::get3dForceComponents(const Vector &v) const
  {
    Vector3d retval(0.0,0.0,0.0);
    const size_t dim= getDim();
    if(numberDOF== 3)
      {
	if(dim==2) // 2D structural
	  retval= Vector3d(v[0],v[1],0.0);
	else if(dim==3) // 3D solid mechanics
	  retval= Vector3d(v[0],v[1],v[2]);
	else
	  std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	            << " not implemented for numDOFs= "	
                    << numberDOF << " and spaceDim= "
                    << dim
		    << Color::def << std::endl;
      }
    else if(numberDOF==2)
      {
	if(dim==2) // 2D solid mechanics
	  retval= Vector3d(v[0],v[1],0.0);
	else
	  std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	            << " not implemented for numDOFs= "	
                    << numberDOF << " and spaceDim= "
                    << dim
		    << Color::def << std::endl;
      }
    else if(numberDOF== 6) // 3D structural
      retval= Vector3d(v[0],v[1],v[2]);
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << " not implemented for numDOFs= "	
                << numberDOF << " and spaceDim= "
                << dim
		<< Color::def << std::endl;
    return retval;
  }
  
//! @brief Return the "moment/rotation" components in a 3d vector.
Vector3d XC::Node::get3dMomentComponents(const Vector &v) const
  {
    Vector3d retval(0.0,0.0,0.0);
    const size_t dim= getDim();
    if(numberDOF== 3)
      {
	if(dim==2) // 2D structural
	  retval= Vector3d(0.0, 0.0, v[2]);
	else if(dim!=3) // NOT 3D solid mechanics => error.
	  std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	            << " not implemented for numDOFs= "	
                    << numberDOF << " and spaceDim= "
                    << dim
		    << Color::def << std::endl;
      }
    else if(numberDOF==2)
      {
	if(dim!=2) // NOT 2D solid mechanics => error.
	  std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	            << " not implemented for numDOFs= "	
                    << numberDOF << " and spaceDim= "
                    << dim
		    << Color::def << std::endl;
      }
    else if(numberDOF== 6) // 3D structural
      retval= Vector3d(v[3],v[4],v[5]);
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << " not implemented for numDOFs= "	
                << numberDOF << " and spaceDim= "
                << dim
		<< Color::def << std::endl;
    return retval;
  }
 
//! @brief Increments the node reaction.
int XC::Node::addReactionForce(const Vector &add, double factor)
  {
    // check vector of appropraie size.
    const int addSz= add.Size();
    if(addSz != numberDOF)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; vector not of correct size in node: "
	          << this->getTag()
	          << " size is: "
	          << addSz
	          << " and should be: "
	          << numberDOF
	          << Color::def << std::endl;
        return -1;
      }

    if(factor == 1.0)
      reaction+= add;
    else if(factor == -1.0)
      reaction-= add;
    else
      reaction+= add*factor;
    return 0;
  }

//! @brief Checks that reactions on the node correspond to
//! constrained degrees of freedom.
bool XC::Node::checkReactionForce(const double &tol) const
  {
    bool retval= true;
    const ConstrContainer &cc= getDomain()->getConstraints();
    const double norm= reaction.Norm();
    if(norm>tol)
      {
	const std::string errHeader= getClassName() + "::" + __FUNCTION__;
	if(!isFrozen())
	  {
	    const int nTag= getTag();
	    if(!cc.nodeAffectedByConstraints(nTag))
	      {
		std::clog << Color::yellow << errHeader
			  << "; the node: " << this->getTag()
		          << " at position: " << this->getInitialPosition3d()
			  << " has not constraints and however"
			  << " it has a reaction with value: " << reaction 
			  << " and norm: " << norm
			  << " it seems that the solution method"
			  << " is not well suited to the problem."
			  << " (tol= " << tol << ")."
			  << Color::def << std::endl;
		retval= false;
	      }
	    else
	      {
		const size_t sz= reaction.Size();
                for(size_t nDOF= 0;nDOF<sz;nDOF++)
		  {
		    const double r= fabs(reaction[nDOF]);
		    if(r>tol)
		      {
			const bool affectedDOF= cc.isDOFAffectedByConstraints(nTag,nDOF);
			if(!affectedDOF)
			  {
			    std::clog << Color::yellow << errHeader
				      << "; the node: " << getTag()
		                      << " at position: "
				      << this->getInitialPosition3d()
				      << " has not constraints on DOF: "
				      << nDOF << " and, however,"
				      << " it has a reaction with value: "
				      << r 
				      << " on this degree of freedom, it seems "
				      << "that the solution method and/or "
				      << "tolerances are not well suited"
				      << " to the problem."
				      << " (tol= " << tol << ")."
				      << Color::def << std::endl;
			    retval= false;
			  }
		      }
		  }// for nDOF
	      }
	  }
      }
    return retval;
  }

//! @brief Calculate the reactions in this node
//! (used in Domain::calculateNodalReactions).
int XC::Node::resetReactionForce(bool inclInertia)
  {
    reaction.Zero();

    // add unbalance, the negative of applied forces hence the -=
    if(!inclInertia)
      { reaction-= getUnbalancedLoad(); }
    else
      { reaction-= getUnbalancedLoadIncInertia(); }
    return 0;
  }

//! @brief Moves the node (intended only for its use from XC::Set).
void XC::Node::Move(const Vector3d &desplaz)
  {
    Crd(0)+= desplaz.x();
    Crd(1)+= desplaz.y();
    Crd(2)+= desplaz.z();
  }
