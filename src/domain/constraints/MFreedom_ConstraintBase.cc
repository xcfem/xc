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
                                                                        
// File: ~/domain/constraints//MFreedom_ConstraintBase.C
//
// Written: lcpt 
// Created: 03/2015
// Revision: A
//
// Purpose: This file contains the implementation of class MFreedom_ConstraintBase.

#include <domain/constraints/MFreedom_ConstraintBase.h>

#include <cstdlib>
#include <utility/matrix/ID.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "domain/domain/Domain.h"

#include "utility/matrices/m_int.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "utility/actor/actor/CommMetaData.h"
#include "vtkCellType.h"

//! @brief Set the constraint matrix.
void XC::MFreedom_ConstraintBase::set_constraint(const Matrix &c)
  { constraintMatrix= c; }

//! @brief Set the constrained degrees of freedom.
//! @param constrainedDOF: constrained degrees of freedom.
void XC::MFreedom_ConstraintBase::set_constrained_dofs(const ID &constrainedDOF)
  {
    constrDOF= constrainedDOF;
    Uc0.resize(constrDOF.Size());
    Uc0.Zero();
  }

//! @brief Constructor.
//! @param tag: tag for the constraint.
//! @param classTag: tag of the object class.
XC::MFreedom_ConstraintBase::MFreedom_ConstraintBase(int tag,int classTag)		
  :Constraint(tag,0,classTag) {}

//! @brief Constructor to be called from subclasses.
//! @param tag: tag for the constraint.
//! @param nodeConstr: identifier of the constrained node.
//! @param classTag: tag of the object class.
XC::MFreedom_ConstraintBase::MFreedom_ConstraintBase(int tag, int nodeConstr, int classTag)
  :Constraint(tag, nodeConstr, classTag) {}

//! @brief Constructor to be called from subclasses.
//! @param tag: tag for the constraint.
//! @param nodeConstr: identifier of the constrained node.
//! @param constrainedDOF: constrained degrees of freedom.
//! @param classTag: tag of the object class.
XC::MFreedom_ConstraintBase::MFreedom_ConstraintBase(int tag, int nodeConstr,const ID &constrainedDOF, int classTag)
  :Constraint(tag, nodeConstr, classTag)
  {
    set_constrained_dofs(constrainedDOF);    
  }


//! @brief Constructor.
//! @param tag: tag for the constraint.
//! @param nodeConstr: identifier of the constrained node.
//! @param constr: constraint matrix.
//! @param constrainedDOF: constrained degrees of freedom.
//! @param classTag: tag of the object class.
XC::MFreedom_ConstraintBase::MFreedom_ConstraintBase(int tag, int nodeConstr, const Matrix &constr,const ID &constrainedDOF,int classTag)
  :Constraint(tag, nodeConstr, classTag)
  {
    set_constrained_dofs(constrainedDOF);    
    set_constraint(constr);
  }

//! @brief Compute the initial displacement at constrained DOFs. 
void XC::MFreedom_ConstraintBase::initializeUc0(const Domain *model)
  {
    const int constrainedNodeTag= this->getNodeConstrained();
    const Node *theConstrainedNode = model->getNode(constrainedNodeTag);
    if(theConstrainedNode == nullptr)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; FATAL Constrained node: "
		  << constrainedNodeTag
		  << " does not exist in Domain"
		  << std::endl;
	exit(-1);
      }
    const Vector &Uc= theConstrainedNode->getTrialDisp();
    const ID &idc= getConstrainedDOFs();
    for(int i = 0; i < idc.Size(); ++i)
      {
	const int cdof= idc(i);
	if (cdof < 0 || cdof >= Uc.Size())
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; FATAL Error: Constrained DOF "
		      << cdof
		      << " out of bounds [0-" << Uc.Size() << "]"
		      << std::endl;
	    exit(-1);
	  }
	Uc0(i) = Uc(cdof);
      }    
  }

//! @brief Sets the domain for the constraint.
//! @param model: domain in which the constraint is created.
void XC::MFreedom_ConstraintBase::setDomain(Domain *model)
  {
    // store initial state
    if(model)
      {
        if(!initialized)
	  { // don't do it if setDomain called after recvSelf when already initialized!4
	    initializeUc0(model);
            this->initialized = true;
	  }
      }
    // call base class implementation
    Constraint::setDomain(model);    
  }

//! @brief Returns true if the constraints affects the
//! node identified by the tag being passed as parameter.
bool XC::MFreedom_ConstraintBase::affectsNode(int nodeTag) const
  {
    return (nodeTag== getNodeConstrained());
  }

//! @brief Returns true if the constraint affects the node and DOF arguments.
bool XC::MFreedom_ConstraintBase::affectsNodeAndDOF(int nodeTag, int theDOF) const
  {
    bool retval= false;
    if(nodeTag== getNodeConstrained())
      {
	int loc= getConstrainedDOFs().getLocation(theDOF);
	if(loc>=0)
	  retval= true;
      }
    return retval;
  }

//! @brief Returns the identifiers of the constrained degrees of fredom.
const XC::ID &XC::MFreedom_ConstraintBase::getConstrainedDOFs(void) const
  {
    // return the ID corresponding to constrained DOF of Ccr
    return constrDOF;
  }


//! @brief Returns true if the constraint varies with time.
bool XC::MFreedom_ConstraintBase::isTimeVarying(void) const
  { return false; }


//! @brief Add reactions to nodes.
int XC::MFreedom_ConstraintBase::addResistingForceToNodalReaction(bool inclInertia)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility."
              << std::endl;
    return 0;
  }

//! @brief Returns the constraint matrix.
//!
//! Returns the current constraint Matrix, that determined in the last
//! call to applyConstraint(). For the MFreedom\_Constraint class, \f$C_{cr}\f$
//! determined in the constructor is returned.
const XC::Matrix &XC::MFreedom_ConstraintBase::getConstraint(void) const
  { return constraintMatrix; }

const XC::Vector &XC::MFreedom_ConstraintBase::getConstrainedDOFsInitialDisplacement(void) const
  {
    return Uc0;
  }

//! @brief Send data through the communicator argument.
int XC::MFreedom_ConstraintBase::sendData(Communicator &comm)
  {
    int res= Constraint::sendData(comm);
    res+= comm.sendMatrix(constraintMatrix,getDbTagData(),CommMetaData(4));
    res+= comm.sendID(constrDOF,getDbTagData(),CommMetaData(5));
    res+= comm.sendVector(Uc0, CommMetaData(6));
    return res;
  }

//! @brief Receive data through the communicator argument.
int XC::MFreedom_ConstraintBase::recvData(const Communicator &comm)
  {
    int res= Constraint::recvData(comm);
    res+= comm.receiveMatrix(constraintMatrix,getDbTagData(),CommMetaData(4));
    res+= comm.receiveID(constrDOF,getDbTagData(),CommMetaData(5));
    res+= comm.receiveVector(Uc0,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::MFreedom_ConstraintBase::getPyDict(void) const
  {
    boost::python::dict retval= Constraint::getPyDict();
    retval["constraintMatrix"]= this->constraintMatrix.getPyList();
    retval["constrDOF"]= this->constrDOF.getPyList();
    retval["Uc0"]= this->Uc0;
    return retval;
  }
//! @brief Set the values of the object members from a Python dictionary.
void XC::MFreedom_ConstraintBase::setPyDict(const boost::python::dict &d)
  {
    Constraint::setPyDict(d);
    this->constraintMatrix= Matrix(boost::python::extract<boost::python::list>(d["constraintMatrix"]));
    this->constrDOF= ID(boost::python::extract<boost::python::list>(d["constrDOF"]));
    this->Uc0= Vector(boost::python::extract<boost::python::list>(d["Uc0"]));
  }

//! @brief Printing.
//!
//! Prints out the MFreedom\_Constraints tag, then the tags of the constrained
//! and retained nodes, then the two ID's and finally the constraint Matrix.
void XC::MFreedom_ConstraintBase::Print(std::ostream &s, int flag) const
  {     
    s << "MFreedom_ConstraintBase: " << this->getTag() << std::endl
      << "\tNode Constrained: " << getNodeConstrained()
      << " constrained dof: " << constrDOF
      << " constraint matrix: " << constraintMatrix
      << " constrained initial displacement: " << Uc0
      << std::endl;
  }

//! @brief Return the indices of the nodes (for VTK).
std::vector<int> XC::MFreedom_ConstraintBase::getIdxNodes(void) const
  {
    
    Domain *theDomain= getDomain();
    const int constrainedNodeTag= getNodeConstrained();
    const Node *constrainedNode= theDomain->getNode(constrainedNodeTag);   
    std::vector<const Node *> retainedNodes= getPointersToRetainedNodes();   

    const size_t sz= retainedNodes.size()+1;
    std::vector<int> retval(sz,-1);
    if(constrainedNode)
      retval[0]= constrainedNode->getIdx();
    for(size_t i=1; i<sz; i++)
      {
	const Node *retainedNode= retainedNodes[i-1];
        if(retainedNode)
          retval[i]= retainedNode->getIdx();
      }
    return retval;     
  }
