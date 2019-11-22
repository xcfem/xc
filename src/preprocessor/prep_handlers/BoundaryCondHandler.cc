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
//BoundaryCondHandler.cc

#include "BoundaryCondHandler.h"
#include "domain/domain/Domain.h"
#include "domain/constraints/SFreedom_Constraint.h"
#include "domain/constraints/MFreedom_Constraint.h"
#include "domain/constraints/MRMFreedom_Constraint.h"
#include "domain/constraints/EqualDOF.h"
#include "domain/constraints/RigidBeam.h"
#include "domain/constraints/RigidRod.h"
#include "domain/constraints/RigidDiaphragm.h"
#include "domain/constraints/GlueNodeToElement.h"


#include "preprocessor/Preprocessor.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"


//! @brief Default constructor.
XC::BoundaryCondHandler::BoundaryCondHandler(Preprocessor *owr)
  : PrepHandler(owr), tag_sp_constraint(0), tag_mp_constraint(0) {}

//! @grief Appends a single freedom constraint to the model.
XC::SFreedom_Constraint *XC::BoundaryCondHandler::addSFreedom_Constraint(const int &tag_nod,const SFreedom_Constraint &seed)
  {
    SFreedom_Constraint *sp= seed.getCopy(tag_sp_constraint);
    tag_sp_constraint++;
    if(sp)
      {
        sp->setNodeTag(tag_nod);
        getDomain()->addSFreedom_Constraint(sp);
        getPreprocessor()->updateSets(sp);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; could not create constraint." << std::endl;
    return sp;    
  }

//! @grief Appends a single freedom constraint to the model.
XC::SFreedom_Constraint *XC::BoundaryCondHandler::addSFreedom_Constraint(const int &tag_nod,const int &dofId,const double &value)
  {
    SFreedom_Constraint *sp= new SFreedom_Constraint(tag_sp_constraint,tag_nod,dofId, value);
    tag_sp_constraint++;
    if(sp)
      {
        getDomain()->addSFreedom_Constraint(sp);
        getPreprocessor()->updateSets(sp);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; could not create constraint." << std::endl;
    return sp;    
  }

//! @grief Defines a single freedom constraint.
XC::SFreedom_Constraint *XC::BoundaryCondHandler::newSPConstraint(const int &tag_nod,const int &dofId,const double &value)
  { return addSFreedom_Constraint(tag_nod,dofId,value); }

//! @grief Appends a multi-freedom constraint to the model.
XC::MFreedom_Constraint *XC::BoundaryCondHandler::newMPConstraint(const int &masterNode, const int &slaveNode, const ID &constrainedDOF, const ID &retainedDOF)
  {
    MFreedom_Constraint *mp= new MFreedom_Constraint(tag_mp_constraint,masterNode,slaveNode,constrainedDOF,retainedDOF);
    tag_mp_constraint++;
    if(mp)
      {
        getDomain()->addMFreedom_Constraint(mp);
        getPreprocessor()->updateSets(mp);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; could not create constraint." << std::endl;
    return mp;
  }

//! @brief Imposes the same displacements on both nodes for the components
//! specified in the argument.
//! @param masterNode: tag of the master node.
//! @param slaveNode: tag of the slave node.
//! @param dofs: degrees of freedom to impose the constraint on.
XC::MFreedom_Constraint *XC::BoundaryCondHandler::newEqualDOF(const int &masterNode, const int &slaveNode, const ID &dofs)
  {
    EqualDOF *mp= new EqualDOF(tag_mp_constraint,masterNode,slaveNode,dofs);
    tag_mp_constraint++;
    if(mp)
      {
        mp->setup(getDomain());
        getDomain()->addMFreedom_Constraint(mp);
        getPreprocessor()->updateSets(mp);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; could not create constraint." << std::endl;
    return mp;
  }

XC::MFreedom_Constraint *XC::BoundaryCondHandler::newRigidBeam(const int &masterNode, const int &slaveNode)
  {
    RigidBeam *mp= new RigidBeam(tag_mp_constraint,masterNode,slaveNode);
    tag_mp_constraint++;
    if(mp)
      {
        mp->setup(getDomain());
        getDomain()->addMFreedom_Constraint(mp);
        getPreprocessor()->updateSets(mp);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; could not create constraint." << std::endl;
    return mp;
  }

XC::MFreedom_Constraint *XC::BoundaryCondHandler::newRigidRod(const int &masterNode, const int &slaveNode)
  {
    RigidRod *mp= new RigidRod(tag_mp_constraint,masterNode,slaveNode);
    tag_mp_constraint++;
    if(mp)
      {
        mp->setup(getDomain());
        getDomain()->addMFreedom_Constraint(mp);
        getPreprocessor()->updateSets(mp);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; could not create constraint." << std::endl;
    return mp;
  }

//! @grief Appends a multi-row, multi-freedom constraint to the model.
XC::MRMFreedom_Constraint *XC::BoundaryCondHandler::newMRMPConstraint(const ID &retainedNodes, const int &constrainedNode, const ID &constrainedDOF)
  {
    MRMFreedom_Constraint *mrmp= new MRMFreedom_Constraint(tag_mrmp_constraint,retainedNodes,constrainedNode,constrainedDOF);
    tag_mrmp_constraint++;
    if(mrmp)
      {
        getDomain()->addMRMFreedom_Constraint(mrmp);
        getPreprocessor()->updateSets(mrmp);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; could not create constraint." << std::endl;
    return mrmp;
  }

XC::MRMFreedom_Constraint *XC::BoundaryCondHandler::newGlueNodeToElement(const Node &constrainedNode, const Element &e, const ID &constrainedDOF)
  {
    GlueNodeToElement *mrmp= new GlueNodeToElement(tag_mrmp_constraint,constrainedNode,e,constrainedDOF);
    tag_mrmp_constraint++;
    if(mrmp)
      {
        getDomain()->addMRMFreedom_Constraint(mrmp);
        getPreprocessor()->updateSets(mrmp);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; could not create constraint." << std::endl;
    return mrmp;
  }


//! @brief Removes the constraint from domain.
void XC::BoundaryCondHandler::removeSPConstraint(const int &tagC)
  {
    bool sp= getDomain()->removeSFreedom_Constraint(tagC);
    if(!sp)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; could not remove constraint." << std::endl;
  }

XC::BoundaryCondHandler::~BoundaryCondHandler(void)
  { clearAll(); }

//! @brief Clears all the objects.
void XC::BoundaryCondHandler::clearAll(void)
  {
    tag_sp_constraint= 0;
    tag_mp_constraint= 0;
  }

//! @brief returns number of single node constraints.
int XC::BoundaryCondHandler::getNumSPs(void) const
  { return getDomain()->getConstraints().getNumSPs(); }

//! @brief returns numbr of multiple node constraints.
int XC::BoundaryCondHandler::getNumMPs(void) const
  { return getDomain()->getConstraints().getNumMPs(); }

//! @brief returns numbr of multiple retained node constraints.
int XC::BoundaryCondHandler::getNumMRMPs(void) const
  { return getDomain()->getConstraints().getNumMRMPs(); }

//! @brief returns number of load patterns.
int XC::BoundaryCondHandler::getNumLPs(void) const
  { return getDomain()->getConstraints().getNumLoadPatterns(); }


