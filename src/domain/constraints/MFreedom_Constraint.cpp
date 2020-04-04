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
                                                                        
// $Revision: 1.4 $
// $Date: 2005/12/22 00:35:08 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/constraints/MFreedom_Constraint.cpp,v $
                                                                        
                                                                        
// File: ~/domain/constraints//MFreedom_Constraint.C
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the implementation of class XC::MFreedom_Constraint.
//
// The class XC::MFreedom_Constraint interface:
//

#include <domain/constraints/MFreedom_Constraint.h>

#include <cstdlib>
#include <utility/matrix/ID.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <boost/any.hpp>

#include "xc_utils/src/matrices/m_int.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "utility/actor/actor/CommMetaData.h"
#include "vtkCellType.h"

//! @brief Sets the retained degrees of freedom.
void XC::MFreedom_Constraint::set_retained_dofs(const ID &retainedDOF)
  { retainDOF= retainedDOF; }

//! @brief Sets the retained and constrained degrees of freedom.
void XC::MFreedom_Constraint::set_constrained_retained_dofs(const ID &constrainedDOF,const ID &retainedDOF)
  {
    set_constrained_dofs(constrainedDOF);
    set_retained_dofs(retainedDOF);
  }

//! !@brief Constructor. //Arash
//!
//! @param tag: constraint identifier.
//! @param classTag: constraint class identifier.
XC::MFreedom_Constraint::MFreedom_Constraint(int tag,int classTag)		
 : MFreedom_ConstraintBase(tag,0,classTag),
   retainedNodeTag(0) {}

//! Constructor. // LCPT
//!
//! @param tag: constraint identifier.
XC::MFreedom_Constraint::MFreedom_Constraint(int tag)		
 : MFreedom_ConstraintBase(tag, 0,CNSTRNT_TAG_MFreedom_Constraint),
   retainedNodeTag(0) {}

//! Constructor to be called from subclasses.
//!
//! @param tag: multi-freedom constraint identifier.
//! @param nodeRetain: identifier of the retained node.
//! @param nodeConstr: identifier of the constrained node.
//! @param classTag: constraint class identifier.
XC::MFreedom_Constraint::MFreedom_Constraint(int tag, int nodeRetain, int nodeConstr, int classTag)
 : MFreedom_ConstraintBase(tag, nodeConstr, classTag), retainedNodeTag(nodeRetain) {}

//! Constructor to be called from subclasses.
//!
//! @param tag: identifier for the multi-freedom constraint.
//! @param nodeRetain: identifier of the retained node.
//! @param nodeConstr: identifier of the constrained node.
//! @param constrainedDOF: constrained degrees of freedom.
//! @param retainedDOF: retained degrees of freedom.
//! @param classTag: identifief for the class.
XC::MFreedom_Constraint::MFreedom_Constraint(int tag, int nodeRetain, int nodeConstr,const ID &constrainedDOF,const ID &retainedDOF, int classTag)
 : MFreedom_ConstraintBase(tag, nodeConstr, classTag), retainedNodeTag(nodeRetain)
  { set_constrained_retained_dofs(constrainedDOF,retainedDOF); }

//! Constructor to be called from subclasses.
//!
//! @param tag: identifier for the multi-freedom constraint.
//! @param nodeRetain: identifier of the retained node.
//! @param nodeConstr: identifier of the constrained node.
//! @param constrainedDOF: constrained degrees of freedom.
//! @param retainedDOF: retained degrees of freedom.
XC::MFreedom_Constraint::MFreedom_Constraint(int tag, int nodeRetain, int nodeConstr,const ID &constrainedDOF,const ID &retainedDOF)
 : MFreedom_ConstraintBase(tag, nodeConstr,CNSTRNT_TAG_MFreedom_Constraint), retainedNodeTag(nodeRetain)
  { set_constrained_retained_dofs(constrainedDOF,retainedDOF); }


//! @brief Constructor.
//!
//! @param tag: identifier for the multi-freedom constraint.
//! @param nodeRetain: identifier of the retained node.
//! @param nodeConstr: identifier of the constrained node.
//! @param constr: constraint matrix.
//! @param constrainedDOF: constrained degrees of freedom.
//! @param retainedDOF: retained degrees of freedom.
XC::MFreedom_Constraint::MFreedom_Constraint(int tag, int nodeRetain, int nodeConstr, Matrix &constr,
			     ID &constrainedDOF, ID &retainedDOF)
 : MFreedom_ConstraintBase(tag, nodeConstr, CNSTRNT_TAG_MFreedom_Constraint),
  retainedNodeTag(nodeRetain)
  {
    set_constrained_retained_dofs(constrainedDOF,retainedDOF);    
    set_constraint(constr);
  }


//! @brief Returns true ifafecta to the node cuyo tag being passed as parameter.
bool XC::MFreedom_Constraint::affectsNode(int nodeTag) const
  { return ( (nodeTag== getNodeConstrained()) || (nodeTag== getNodeRetained()));}

//! @brief Returns true if the constraint affects the node and DOF arguments.
bool XC::MFreedom_Constraint::affectsNodeAndDOF(int nodeTag, int theDOF) const
  {
    bool retval= MFreedom_ConstraintBase::affectsNodeAndDOF(nodeTag,theDOF);
    if(!retval)
      if(nodeTag== getNodeRetained())
	{
	  int loc= getRetainedDOFs().getLocation(theDOF);
	  if(loc>=0)
	    retval= true;
	}
    return retval;
  }

//! @brief Returns the identifiers of the retained degrees of freedom.
const XC::ID &XC::MFreedom_Constraint::getRetainedDOFs(void) const
  {
    // return the XC::ID corresponding to retained DOF of Ccr
    return retainDOF;    
  }

//! @brief Returns a vector with the pointers to the retained nodes.
std::vector<XC::Node *> XC::MFreedom_Constraint::getPointersToRetainedNodes(void) const
  {
    std::vector<Node *> retval(1,nullptr);   

    Domain *theDomain= getDomain();
    const int retainedNode= getNodeRetained();
    retval[0]= theDomain->getNode(retainedNode);   
    return retval;
  }


//! @brief Applies the constraint at the pseudo-time being passed as parameter.
//!
//! A method to invoked to inform the MFreedom\_Constraint to determine
//! \f$C_{cr}\f$, for the time \p timeStamp. {\bf The base class will do
//! nothing, as Matrix is assumed to be constant.}
int XC::MFreedom_Constraint::applyConstraint(double timeStamp)
  {
    // does nothing MFreedom_Constraint objects are time invariant
    return 0;
  }

//! @brief Sends data through the communicator argument.
int XC::MFreedom_Constraint::sendData(Communicator &comm)
  {
    int res= MFreedom_ConstraintBase::sendData(comm);
    res+= comm.sendID(retainDOF,getDbTagData(),CommMetaData(5));
    res+= comm.sendInt(retainedNodeTag,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receives data through the communicator argument.
int XC::MFreedom_Constraint::recvData(const Communicator &comm)
  {
    int res= MFreedom_ConstraintBase::recvData(comm);
    res+= comm.receiveID(retainDOF,getDbTagData(),CommMetaData(5));
    res+= comm.receiveInt(retainedNodeTag,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::MFreedom_Constraint::sendSelf(Communicator &comm)
  {
    inicComm(14);
    int result= sendData(comm);
    const int dataTag= getDbTag();
    result = comm.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; WARNING - error sending ID data\n";
    return result;
  }


//! @brief Receives object through the communicator argument.
int XC::MFreedom_Constraint::recvSelf(const Communicator &comm)
  {
    inicComm(14);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; data could not be received.\n" ;
    else
      res+= recvData(comm);
    return res;
  }

//! @brief Add to nodes the actions due to this constraint.
//! See "Calculation within MSC/Nastran of the forces transmitted by
//! multifreedom constraints (MFC) and the forces generated in support
//! constraints" by E, de la Fuente and J. San Millán (INTA), Spain.
int XC::MFreedom_Constraint::addResistingForceToNodalReaction(bool inclInertia)
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

    //Add reaction to retained node.
    Vector Rn(numConstrainedDOFs);
    //R_n = -G^T R_m (see page 4).
    Rn.addMatrixTransposeVector(1.0,constraintMatrix,Rm,-1.0);
    Vector RnNod(numConstrainedDOFs);
    RnNod.Extract(Rn,0,1.0);
    tmp.Zero();
    tmp.putComponents(RnNod,constrDOF);
    Node *rN= dom->getNode(getNodeRetained());
    rN->addReactionForce(tmp,1.0);

    if(inclInertia)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << " ERROR;  inertia effects not yet implemented." << std::endl;
    return 0;
  }

//! @brief Printing.
void XC::MFreedom_Constraint::Print(std::ostream &s, int flag) const
  {     
    s << "MFreedom_Constraint: " << this->getTag() << "\n";
    s << "\tNode Constrained: " << getNodeConstrained();
    s << " node Retained: " << getNodeRetained() ;
    s << " constrained dof: " << constrDOF;    
    s << " retained dof: " << retainDOF;        
    s << " constraint matrix: " << constraintMatrix << "\n";
  }


//! @brief Interfaz con VTK.
int XC::MFreedom_Constraint::getVtkCellType(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return VTK_EMPTY_CELL;
  }

