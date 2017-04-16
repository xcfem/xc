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

//
// Written: lcpt 
// Created: 03/2015
// Revision: A
//
// Purpose: This file contains the implementation of class XC::MRMFreedom_Constraint.

#include <domain/constraints/MRMFreedom_Constraint.h>

#include <cstdlib>
#include <utility/matrix/ID.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <boost/any.hpp>

#include "xc_basic/src/matrices/m_int.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/utils/NodePtrsWithIDs.h"
#include "utility/actor/actor/ArrayCommMetaData.h"
#include "utility/actor/actor/MatrixCommMetaData.h"
#include "med.h"
#include "vtkCellType.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"


//! @brief Constructor.	// Arash
//! @param tag: tag for the constraint.
//! @param classTag: tag of the object class.
XC::MRMFreedom_Constraint::MRMFreedom_Constraint(int tag,int clasTag)		
 : MFreedom_ConstraintBase(tag,0,clasTag) {}

//! @brief Constructor. // LCPT
//! @param tag: tag for the constraint.
XC::MRMFreedom_Constraint::MRMFreedom_Constraint(int tag)		
 : MFreedom_ConstraintBase(tag, 0,CNSTRNT_TAG_MRMFreedom_Constraint) {}

//! @brief Constructor to be called from subclasses.
//! @param tag: tag for the constraint.
//! @param retainedNodes: tags of the retained nodes.
//! @param nodeConstr: identifier of the constrained node.
//! @param constrainedDOF: identifiers of the constrained degrees of freedom.
XC::MRMFreedom_Constraint::MRMFreedom_Constraint(int tag, const ID &retainedNodes, int nodeConstr,const ID &constrainedDOF)
  : MFreedom_ConstraintBase(tag, nodeConstr, constrainedDOF, CNSTRNT_TAG_MRMFreedom_Constraint), 
    retainedNodeTags(retainedNodes) {}

//! @brief Constructor to be called from subclasses.
//! @param tag: tag for the constraint.
//! @param retainedNodes: tags of the retained nodes.
//! @param nodeConstr: identifier of the constrained node.
//! @param constrainedDOF: identifiers of the constrained degrees of freedom.
//! @param classTag: tag of the object class.
XC::MRMFreedom_Constraint::MRMFreedom_Constraint(int tag, const ID &retainedNodes, int nodeConstr,const ID &constrainedDOF, int classTag)
  : MFreedom_ConstraintBase(tag, nodeConstr, constrainedDOF, classTag),
   retainedNodeTags(retainedNodes) { }


//! Constructor.
//! @param tag: tag for the constraint.
//! @param retainedNodes: tags of the retained nodes.
//! @param nodeConstr: identifier of the constrained node.
//! @param constr: constraint matrix.
//! @param constrainedDOF: identifiers of the constrained degrees of freedom.
XC::MRMFreedom_Constraint::MRMFreedom_Constraint(int tag, const ID &retainedNodes, int nodeConstr, const Matrix &constr,const ID &constrainedDOF)
  : MFreedom_ConstraintBase(tag, nodeConstr, constr, constrainedDOF, CNSTRNT_TAG_MRMFreedom_Constraint), retainedNodeTags(retainedNodes) {}

XC::MRMFreedom_Constraint::MRMFreedom_Constraint(int tag,const Element &elem, const Node &node, const ID &constrainedDOF)
  : MFreedom_ConstraintBase(tag, node.getTag(), constrainedDOF, CNSTRNT_TAG_MRMFreedom_Constraint)
  {
    retainedNodeTags= elem.getNodePtrs().getExternalNodes();
    const size_t numNodes= retainedNodeTags.Size();
    const size_t numGdls= constrainedDOF.Size();
    constraintMatrix= Matrix(numGdls,numNodes*numGdls);
    Pos3d pos= node.getPosInicial3d(); //XXX Time dependence?
    const Vector interpolationFactors= elem.getInterpolationFactors(pos);
    for(size_t j= 0;j<numNodes;j++)
      {
        const int offset= j*numGdls;
        for(size_t i= 0;i<numGdls;i++)
          constraintMatrix(i,offset+i)= interpolationFactors(j);
      }
  }

//! @brief Returns the number of DOF groups.
int XC::MRMFreedom_Constraint::getNumDofGroups(void) const
  { return retainedNodeTags.Size()+1; }//retained(n)+constrained(1)

//! @brief Number of the degrees of freedom affected by the constraint.
int XC::MRMFreedom_Constraint::getNumDofs(void) const
  { return getNumDofGroups()*getConstrainedDOFs().Size();  }

//! @brief Number of affected DOFs for Lagrange handler
//! numConstrainedDOFs+numRetainedDOFs+numLangrangeDOFs.
//! numLagrangeDOFs= numConstrainedDOFs.
int XC::MRMFreedom_Constraint::getNumLagrangeDofs(void) const
  { return getNumDofs()+getConstrainedDOFs().Size();  }

//! @brief Number of retained DOFs.
int XC::MRMFreedom_Constraint::getNumRetainedDofs(void) const
  { return retainedNodeTags.Size()*getConstrainedDOFs().Size();  }

//! @brief Number of constrained DOFs.
int XC::MRMFreedom_Constraint::getNumConstrainedDofs(void) const
  { return getConstrainedDOFs().Size();  }


//! @brief Returns true if the constraint affect the node
//! identified by the tag being passed as parameter.
bool XC::MRMFreedom_Constraint::affectsNode(int nodeTag) const
  {
    bool retval= false;
    if (nodeTag== getNodeConstrained())
      retval= true;
    else
      {
        const ID &rNodes= getRetainedNodeTags();
        const size_t sz= rNodes.Size();
        for(size_t i= 0;i<sz;i++)
          if(nodeTag==rNodes(i))
            {
 	      retval= true;
              break;
            }
      }
    return retval;
  }

//! @brief Applies the constraint.
int XC::MRMFreedom_Constraint::applyConstraint(double timeStamp)
  {
    // does nothing MRMFreedom_Constraint objects are time invariant
    return 0;
  }

//! @brief Add reactions to nodes.
//! See "Calculation within MSC/Nastran of the forces transmitted by
//! multipoint constraints (MPC) and the forces generated in support
//! constraints" by E, de la Fuente and J. San Millán (INTA), Spain.
int XC::MRMFreedom_Constraint::addResistingForceToNodalReaction(bool inclInertia)
  {
    const size_t numConstrainedDOFs= constrDOF.Size(); //ID of constrained DOFs at constrained node
    Domain *dom= getDomain();
    Node *cN= dom->getNode(getNodeConstrained());
    const int numNodalDOF = cN->getNumberDOF();

    //Add reaction to constrained node.
    //R_m = F_m= K_mm u_m - P_m (see page 3).
    const Vector cNReaction= cN->getReaction().getComponents(constrDOF);
    Vector Rm(numConstrainedDOFs);
    for(size_t i= 0;i<numConstrainedDOFs;i++)
      Rm(i)= -cNReaction(i);

    Vector tmp(numNodalDOF);
    tmp.putComponents(Rm,constrDOF);
    cN->addReactionForce(tmp,1.0);

    //Add reaction to retained nodes.
    const size_t numRetainedNodes= retainedNodeTags.Size();
    Vector Rn(numConstrainedDOFs*numRetainedNodes);
    //R_n = -G^T R_m (see page 4).
    Rn.addMatrixTransposeVector(1.0,constraintMatrix,Rm,-1.0);
    Vector RnNod(numConstrainedDOFs);
    for(size_t j= 0;j<numRetainedNodes;j++)
      {
        const int offset= j*numConstrainedDOFs;
        RnNod.Extract(Rn,offset,1.0);
        tmp.Zero();
        tmp.putComponents(RnNod,constrDOF);
        Node *rN= dom->getNode(retainedNodeTags(j));
        rN->addReactionForce(tmp,1.0);
      }
    if(inclInertia)
      std::cerr << "ERROR; MRMFreedom_Constraint::addResistingForceToNodalReaction"
                << " inertia effects not yet implemented." << std::endl;
    return 0;
  }


//! @brief Send data through the channel being passed as parameter.
int XC::MRMFreedom_Constraint::sendData(CommParameters &cp)
  {
    int res= MFreedom_ConstraintBase::sendData(cp);
    res+= cp.sendID(retainedNodeTags,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Receive data through the channel being passed as parameter.
int XC::MRMFreedom_Constraint::recvData(const CommParameters &cp)
  {
    int res= MFreedom_ConstraintBase::recvData(cp);
    res+= cp.receiveID(retainedNodeTags,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::MRMFreedom_Constraint::sendSelf(CommParameters &cp)
  {
    inicComm(10);
    int result= sendData(cp);
    const int dataTag= getDbTag();
    result = cp.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << nombre_clase() << "::" << __FUNCTION__
	        << "; error sending ID data\n";
    return result;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::MRMFreedom_Constraint::recvSelf(const CommParameters &cp)
  {
    inicComm(10);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << nombre_clase() << "::" << __FUNCTION__
	        << "; error receiving ID data\n";
    else
      res+= recvData(cp);
    return res;
  }

//! @brief Printing.
void XC::MRMFreedom_Constraint::Print(std::ostream &s, int flag)
  {     
    s << "MRMFreedom_Constraint: " << this->getTag() << "\n";
    s << "\tNode Constrained: " << getNodeConstrained();
    s << " retained nodes: " << getRetainedNodeTags() ;
    s << " constrained dof: " << constrDOF;    
    s << " constraint matrix: " << constraintMatrix << "\n";
  }


//! @brief Interfaz con VTK.
int XC::MRMFreedom_Constraint::getVtkCellType(void) const
  {
    std::cerr << nombre_clase() << "::" << __FUNCTION__
              << "; not implemented." << std::endl;
    return VTK_EMPTY_CELL;
  }

//! @brief Interfce with Salome MED format.
int XC::MRMFreedom_Constraint::getMEDCellType(void) const
  { 
    std::cerr << "MRMFreedom_Constraint::getMEDCellType; not implemented." << std::endl;
    return MED_SEG2; 
  }

