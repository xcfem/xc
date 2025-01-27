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
//BoundaryCondHandler.cc

#include "BoundaryCondHandler.h"
#include "domain/domain/Domain.h"
#include "preprocessor/Preprocessor.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "domain/constraints/SFreedom_Constraint.h"
#include "domain/constraints/skew_constraints/SymmetryConstraint.h"
#include "domain/constraints/skew_constraints/AntiSymmetryConstraint.h"
#include "domain/constraints/MFreedom_Constraint.h"
#include "domain/constraints/MRMFreedom_Constraint.h"
#include "domain/constraints/single_retained_node_constraints/EqualDOF.h"
#include "domain/constraints/single_retained_node_constraints/RigidBeam.h"
#include "domain/constraints/single_retained_node_constraints/RigidRod.h"
#include "domain/constraints/single_retained_node_constraints/RigidDiaphragm.h"
#include "domain/constraints/multiple_retained_node_constraints/GlueNodeToElement.h"
#include "utility/geom/d1/Line2d.h"
#include "utility/geom/d2/Plane.h"
#include "utility/utils/misc_utils/colormod.h"


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
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; could not create constraint."
		<< Color::def << std::endl;
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
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; could not create constraint."
		<< Color::def << std::endl;
    return sp;    
  }

//! @grief Defines a single freedom constraint.
XC::SFreedom_Constraint *XC::BoundaryCondHandler::newSPConstraint(const int &tag_nod,const int &dofId,const double &value)
  { return addSFreedom_Constraint(tag_nod,dofId,value); }

//! @grief Defines a skew constraint.
XC::Skew_Constraint *XC::BoundaryCondHandler::newSkewConstraint(const int &constrainedNode, const ID &constrainedDOF, const ID &retainedDOF)
  {
    Skew_Constraint *skewC= new Skew_Constraint(tag_mp_constraint, constrainedNode,constrainedDOF, retainedDOF);
    tag_mp_constraint++;
    if(skewC)
      {
        getDomain()->addMFreedom_Constraint(skewC);
        getPreprocessor()->updateSets(skewC);
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; could not create constraint."
		<< Color::def << std::endl;
    return skewC;
  }

//! @brief Defines a 2D symmetry constraint.
XC::Skew_Constraint *XC::BoundaryCondHandler::newSymmetryConstraint(const int &constrainedNode, const Line2d &l)
  {
    SymmetryConstraint *skewC= new SymmetryConstraint(tag_mp_constraint, constrainedNode, l);
    tag_mp_constraint++;
    if(skewC)
      {
	Domain *dom= this->getDomain();
	skewC->setup(dom);
        dom->addMFreedom_Constraint(skewC);
        getPreprocessor()->updateSets(skewC);
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; could not create constraint."
		<< Color::def << std::endl;
    return skewC;
  }

//! @brief Defines a 3D symmetry constraint.
XC::Skew_Constraint *XC::BoundaryCondHandler::newSymmetryConstraint(const int &constrainedNode, const Plane &p)
  {
    SymmetryConstraint *skewC= new SymmetryConstraint(tag_mp_constraint, constrainedNode, p);
    tag_mp_constraint++;
    if(skewC)
      {
	Domain *dom= this->getDomain();
	skewC->setup(dom);
        getDomain()->addMFreedom_Constraint(skewC);
        getPreprocessor()->updateSets(skewC);
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; could not create constraint."
		<< Color::def << std::endl;
    return skewC;
  }

//! @brief Defines a 2D anti-symmetry constraint.
XC::Skew_Constraint *XC::BoundaryCondHandler::newAntiSymmetryConstraint(const int &constrainedNode, const Line2d &l)
  {
    AntiSymmetryConstraint *skewC= new AntiSymmetryConstraint(tag_mp_constraint, constrainedNode, l);
    tag_mp_constraint++;
    if(skewC)
      {
	Domain *dom= this->getDomain();
	skewC->setup(dom);
        dom->addMFreedom_Constraint(skewC);
        getPreprocessor()->updateSets(skewC);
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; could not create constraint."
		<< Color::def << std::endl;
    return skewC;
  }
  
//! @brief Defines a 3D anti-symmetry constraint.
XC::Skew_Constraint *XC::BoundaryCondHandler::newAntiSymmetryConstraint(const int &constrainedNode, const Plane &p)
  {
    AntiSymmetryConstraint *skewC= new AntiSymmetryConstraint(tag_mp_constraint, constrainedNode, p);
    tag_mp_constraint++;
    if(skewC)
      {
	Domain *dom= this->getDomain();
	skewC->setup(dom);
        getDomain()->addMFreedom_Constraint(skewC);
        getPreprocessor()->updateSets(skewC);
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; could not create constraint."
		<< Color::def << std::endl;
    return skewC;
  }

//! @grief Appends a multi-freedom constraint to the model.
XC::MFreedom_Constraint *XC::BoundaryCondHandler::newMPConstraint(const int &retainedNode, const int &constrainedNode, const ID &constrainedDOF, const ID &retainedDOF)
  {
    MFreedom_Constraint *mp= new OneRowMFreedom_Constraint(tag_mp_constraint,retainedNode,constrainedNode,constrainedDOF,retainedDOF, CNSTRNT_TAG_MFreedom_Constraint);
    tag_mp_constraint++;
    if(mp)
      {
        getDomain()->addMFreedom_Constraint(mp);
        getPreprocessor()->updateSets(mp);
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; could not create constraint."
		<< Color::def << std::endl;
    return mp;
  }

//! @brief Imposes the same displacements on both nodes for the components
//! specified in the argument.
//! @param retainedNode: tag of the node whose response will be retained in the equations.
//! @param constrainedNode: tag of the node whose response will be removed from the equations.
//! @param dofs: degrees of freedom to impose the constraint on.
XC::MFreedom_Constraint *XC::BoundaryCondHandler::newEqualDOF(const int &retainedNode, const int &constrainedNode, const ID &dofs)
  {
    EqualDOF *mp= new EqualDOF(tag_mp_constraint,retainedNode,constrainedNode,dofs);
    tag_mp_constraint++;
    if(mp)
      {
        mp->setup(getDomain());
        getDomain()->addMFreedom_Constraint(mp);
        getPreprocessor()->updateSets(mp);
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; could not create constraint."
		<< Color::def << std::endl;
    return mp;
  }

XC::MFreedom_Constraint *XC::BoundaryCondHandler::newRigidBeam(const int &retainedNode, const int &constrainedNode)
  {
    RigidBeam *mp= new RigidBeam(tag_mp_constraint,retainedNode,constrainedNode);
    tag_mp_constraint++;
    if(mp)
      {
        mp->setup(getDomain());
        getDomain()->addMFreedom_Constraint(mp);
        getPreprocessor()->updateSets(mp);
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; could not create constraint."
		<< Color::def << std::endl;
    return mp;
  }

XC::MFreedom_Constraint *XC::BoundaryCondHandler::newRigidRod(const int &retainedNode, const int &constrainedNode)
  {
    RigidRod *mp= new RigidRod(tag_mp_constraint,retainedNode,constrainedNode);
    tag_mp_constraint++;
    if(mp)
      {
        mp->setup(getDomain());
        getDomain()->addMFreedom_Constraint(mp);
        getPreprocessor()->updateSets(mp);
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; could not create constraint."
		<< Color::def << std::endl;
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
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; could not create constraint."
		<< Color::def << std::endl;
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
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; could not create constraint."
		<< Color::def << std::endl;
    return mrmp;
  }


//! @brief Removes the constraint from domain.
void XC::BoundaryCondHandler::removeSPConstraint(const int &tagC)
  {
    bool sp= getDomain()->removeSFreedom_Constraint(tagC);
    if(!sp)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; could not remove constraint."
		<< Color::def << std::endl;
  }

XC::BoundaryCondHandler::~BoundaryCondHandler(void)
  { clearAll(); }

//! @brief Clears all the objects.
void XC::BoundaryCondHandler::clearAll(void)
  {
    tag_sp_constraint= 0;
    tag_mp_constraint= 0;
    tag_mrmp_constraint= 0;
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


