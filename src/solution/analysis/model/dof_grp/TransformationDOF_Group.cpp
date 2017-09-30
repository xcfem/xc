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
                                                                        
// $Revision: 1.16 $
// $Date: 2005/11/29 23:31:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/dof_grp/TransformationDOF_Group.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Created: 05/99
// Revision: A
//
// Purpose: This file contains the code for implementing the methods
// of the TransformationDOF_Group class interface.
//
// What: "@(#) TransformationDOF_Group.C, revA"

#include <solution/analysis/model/dof_grp/TransformationDOF_Group.h>
#include <cstdlib>

#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/integrator/TransientIntegrator.h>
#include <domain/constraints/MFreedom_Constraint.h>
#include <domain/constraints/MRMFreedom_Constraint.h>
#include <domain/constraints/SFreedom_Constraint.h>
#include <domain/constraints/SFreedom_ConstraintIter.h>
#include <solution/analysis/handler/TransformationConstraintHandler.h>

const int MAX_NUM_DOF= 16;

// static variables initialisation
XC::UnbalAndTangentStorage XC::TransformationDOF_Group::unbalAndTangentArrayMod(MAX_NUM_DOF+1);
int XC::TransformationDOF_Group::numTransDOFs(0);     // number of objects
XC::TransformationConstraintHandler *XC::TransformationDOF_Group::theHandler= nullptr;     // number of objects

//! @brief Create SFreedom_Constraint pointer array
std::vector<XC::SFreedom_Constraint *> XC::TransformationDOF_Group::getSFreedomConstraintArray(int numNodalDOF) const
  {
    std::vector<SFreedom_Constraint *> retval(numNodalDOF,static_cast<SFreedom_Constraint *>(nullptr));
    for(int ii=0; ii<numNodalDOF; ii++) 
      retval[ii]= nullptr;
    return retval;
  }

//! @brief Initializes object arrays.
void XC::TransformationDOF_Group::arrays_setup(int numNodalDOF, int numConstrainedNodeRetainedDOF, int numRetainedNodeDOF)  
  {
    modNumDOF= numConstrainedNodeRetainedDOF + numRetainedNodeDOF;
    unbalAndTangentMod= UnbalAndTangent(modNumDOF,unbalAndTangentArrayMod);

    // create ID and transformation matrix
    modID= ID(modNumDOF);
    Trans= Matrix(numNodalDOF, modNumDOF);

    // initially set the id values to -2 for any dof still due to constrained node
    for(int i=0; i<numConstrainedNodeRetainedDOF; i++)
      modID(i)= -2;
    
    // // for all the constrained dof values set to -1 LCPT COMMENTED OUT: IS EXACTLY
    // THE SAME THAT THEY DO IN THE LINES "LA" AND "LB" (see below).
    // for(int j=numConstrainedNodeRetainedDOF; j<modNumDOF; j++)
    //   modID(j)= -1;

    // for all the dof corresponding to the retained node set initially to -1
    // we don't initially assign these equation nos. - this is done in doneID()
    for(int k=numConstrainedNodeRetainedDOF; k<modNumDOF; k++) //LINE LA
      modID(k)= -1;                                            //LINE LB
  }

void XC::TransformationDOF_Group::initialize(TransformationConstraintHandler *theTHandler)
  {
    const MFreedom_ConstraintBase *mfc= getMFreedomConstraint();
    // determine the number of DOF 
    const int numNodalDOF= myNode->getNumberDOF();
    const ID &retainedDOF= mfc->getRetainedDOFs();
    const ID &constrainedDOF= mfc->getConstrainedDOFs();    
    const int numNodalDOFConstrained= constrainedDOF.Size();
    const int numConstrainedNodeRetainedDOF= numNodalDOF - numNodalDOFConstrained;
    const int numRetainedNodeDOF= retainedDOF.Size();

    // create SFreedom_Constraint array
    theSPs= getSFreedomConstraintArray(numNodalDOF);
    arrays_setup(numNodalDOF,numConstrainedNodeRetainedDOF,numRetainedNodeDOF);

    /***********************
    // set the XC::SFreedom_Constraint corresponding to the dof in modID
    Domain *theDomain=node->getDomain();
    int nodeTag= node->getTag();    
    SFreedom_ConstraintIter &theSPIter= theDomain->getSPs();
    SFreedom_Constraint *sp;
    while ((sp= theSPIter()) != 0) {
        if(sp->getNodeTag() == nodeTag) {
            int dof= sp->getDOF_Number();
            int loc= 0;
            for(int i=0; i<dof; i++) 
                if(constrainedDOF.getLocation(i) < 0)
                    loc++;
            theSPs[loc]= sp;
        }
    }
    *******************/
     
    numTransDOFs++;
    theHandler= theTHandler;
  }

XC::TransformationDOF_Group::TransformationDOF_Group(int tag, Node *node, MFreedom_ConstraintBase *m, TransformationConstraintHandler *theTHandler)  
  :DOF_Group(tag,node), mfc(m), unbalAndTangentMod(0,unbalAndTangentArrayMod), theSPs()
  { initialize(theHandler); }

void XC::TransformationDOF_Group::setID(int dof, int value)
  {
    if(!mfc)
      this->DOF_Group::setID(dof,value);
    else
      modID(dof)= value;
  }
        

XC::TransformationDOF_Group::TransformationDOF_Group(int tag, Node *node, TransformationConstraintHandler *theTHandler)
  :DOF_Group(tag,node), mfc(nullptr), modNumDOF(node->getNumberDOF()),
   unbalAndTangentMod(node->getNumberDOF(),unbalAndTangentArrayMod),theSPs() 
  {
    // create space for the SFreedom_Constraint array
    theSPs= std::vector<SFreedom_Constraint *>(modNumDOF,static_cast<SFreedom_Constraint *>(nullptr));
    for(int i=0; i<modNumDOF; i++) 
      theSPs[i]= 0;

    // set the SFreedom_Constraint corresponding to the dof in myID
    Domain *theDomain=node->getDomain();
    int nodeTag= node->getTag();
    SFreedom_ConstraintIter &theSPIter= theDomain->getConstraints().getSPs();
    SFreedom_Constraint *sp;
    while ((sp= theSPIter()) != 0)
      {
        if(sp->getNodeTag() == nodeTag)
	  {
            int dof= sp->getDOF_Number();
            theSPs[dof]= sp;
          }
      }
    numTransDOFs++;
    theHandler= theTHandler;
  }


//! @brief Destructor.
XC::TransformationDOF_Group::~TransformationDOF_Group()
  {
    numTransDOFs--;
  }    


const XC::ID &XC::TransformationDOF_Group::getID(void) const
  {
    if(!modID.Nulo())
      return modID;
    else
      return this->DOF_Group::getID();
  }

int XC::TransformationDOF_Group::getNumDOF(void) const
  { return modNumDOF; }


int XC::TransformationDOF_Group::getNumFreeDOF(void) const
{
  if(!modID.Nulo())
    {
      int numFreeDOF= modNumDOF;
      for(int i=0; i<modNumDOF; i++)
        if(modID(i) == -1)
          numFreeDOF--;
        return numFreeDOF;        
    }
  else
    return this->DOF_Group::getNumFreeDOF();
  }

int XC::TransformationDOF_Group::getNumConstrainedDOF(void) const
  {   
    if(!modID.Nulo())
      {    
        int numConstr= 0;
        for(int i=0; i<modNumDOF; i++)
          if(modID(i) < 0)
            numConstr++;
        return numConstr;
      }
    else
      return this->DOF_Group::getNumConstrainedDOF();        
  }    


const XC::Matrix &XC::TransformationDOF_Group::getTangent(Integrator *theIntegrator) 
  {
    const Matrix &unmodTangent= this->DOF_Group::getTangent(theIntegrator);
    const Matrix *T= this->getT();
    if(T)
      {
        // unbalAndTangentMod.getTangent()= (*T) ^ unmodTangent * (*T);
        unbalAndTangentMod.getTangent().addMatrixTripleProduct(0.0, *T, unmodTangent, 1.0);
        return unbalAndTangentMod.getTangent();
        
      }
    else 
      return unmodTangent;
  }

const XC::Vector &XC::TransformationDOF_Group::getUnbalance(Integrator *theIntegrator)
  {
    const Vector &unmodUnbalance= this->DOF_Group::getUnbalance(theIntegrator);

    const Matrix *T= this->getT();
    if(T)
      {
        // unbalAndTangentMod.getResidual()= (*T) ^ unmodUnbalance;
        unbalAndTangentMod.getResidual().addMatrixTransposeVector(0.0, *T, unmodUnbalance, 1.0);
        return unbalAndTangentMod.getResidual();    
      }
    else
      return unmodUnbalance;
  }

//! @brief Returns a pointer to the multi-freedom constraint.
const XC::MFreedom_ConstraintBase *XC::TransformationDOF_Group::getMFreedomConstraint(void) const
  { return mfc; }

//! @brief Returns a pointer to the multi-freedom constraint.
XC::MFreedom_ConstraintBase *XC::TransformationDOF_Group::getMFreedomConstraint(void)
  {
    const TransformationDOF_Group *const_this= static_cast<const TransformationDOF_Group*>(this);
    return const_cast<MFreedom_ConstraintBase *>(const_this->getMFreedomConstraint());
  }

//! @brief Computes the residual vector and returns it.
const XC::Vector &XC::TransformationDOF_Group::setupResidual(int numCNodeDOF, const ID &constrainedDOF,const ID &retainedDOF, const Vector &responseC, const std::vector<Node *> &ptrsToRetainedNodes,const Vector &(Node::*response)(void) const)
  {
    int loc= 0;
    for(int i=0; i<numCNodeDOF; i++)
      {
        if(constrainedDOF.getLocation(i) < 0)
          {
            (unbalAndTangentMod.getResidual())(loc)= responseC(i);
            loc++;
          }
      }
    const int numRetainedNodeDOF= retainedDOF.Size();
    for(std::vector<Node *>::const_iterator nIter= ptrsToRetainedNodes.begin(); nIter!= ptrsToRetainedNodes.end(); nIter++)
      {
        const Node *retainedNodePtr= *nIter;
        const Vector &responseR= ((*retainedNodePtr).*response)(); //Displacement, velocity or acceleration.
        for(int j=0; j<numRetainedNodeDOF; j++)
          {
            const int dof= retainedDOF(j);
            (unbalAndTangentMod.getResidual())(loc)= responseR(dof);
            loc++;
          }
      }
    return unbalAndTangentMod.getResidual();
  }

//! @brief Returns the number of retained nodes.
size_t XC::TransformationDOF_Group::getNumRetainedNodes(void) const
  { return mfc->getNumRetainedNodes(); }

//! @brief Returns the number of retained degrees of freedom (retained nodes x retained DOFs on each node).
size_t XC::TransformationDOF_Group::getNumRetainedNodeDOFs(void) const
  {
    size_t retval= 0;
    const size_t nrn= getNumRetainedNodes();
    if(nrn) //nrn!=0 => mfc!=nullptr
      {
        const MFreedom_ConstraintBase *mfc= getMFreedomConstraint();
        retval= nrn*mfc->getRetainedDOFs().Size();
      }
    return retval;
  }

//! @brief Returns a vector with the pointers to the retained nodes.
std::vector<XC::Node *> XC::TransformationDOF_Group::getPointersToRetainedNodes(void)
  {
    std::vector<Node *> retval(1,nullptr);   

    const MFreedom_ConstraintBase *mfc= getMFreedomConstraint();
    if(mfc)
      retval= mfc->getPointersToRetainedNodes();
    return retval;
  }

//! @brief Returns the commited value for the response.
const XC::Vector &XC::TransformationDOF_Group::getCommittedResponse(const Vector &(Node::*response)(void) const)
  {
    const Vector &responseC= ((*myNode).*response)(); //Displacement, velocity or acceleration.

    const MFreedom_ConstraintBase *mfc= getMFreedomConstraint();
    if(!mfc)
      return responseC;
    else
      {
        int numCNodeDOF= myNode->getNumberDOF();
        const ID &constrainedDOF= mfc->getConstrainedDOFs();            
        const ID &retainedDOF= mfc->getRetainedDOFs();
	const std::vector<Node *> ptrsToRetainedNodes= getPointersToRetainedNodes();
        return setupResidual(numCNodeDOF,constrainedDOF,retainedDOF,responseC,ptrsToRetainedNodes,response);
      }
  }

//! @brief Returns the commited value for the displacement.
const XC::Vector &XC::TransformationDOF_Group::getCommittedDisp(void)
  { return getCommittedResponse(&Node::getDisp); }

//! @brief Returns the commited value for the velocity.
const XC::Vector &XC::TransformationDOF_Group::getCommittedVel(void)
  { return getCommittedResponse(&Node::getVel); }

//! @brief Returns the commited value for the acceleration.
const XC::Vector &XC::TransformationDOF_Group::getCommittedAccel(void)
  { return getCommittedResponse(&Node::getAccel); }

void XC::TransformationDOF_Group::setupResidual(const Vector &u,int (Node::*setTrial)(const Vector &))
  {
    const ID &theID= this->getID();
    for(int i=0; i<modNumDOF; i++)
      {
        const int loc= theID(i);
        if(loc >= 0)
          (unbalAndTangentMod.getResidual())(i)= u(loc);
        else
          (unbalAndTangentMod.getResidual())(i)= 0.0;        
      }    
    const Matrix *T= this->getT();
    if(T)
      {

        // unbalAndTangent.getResidual()= (*T) * (unbalAndTangentMod.getResidual());
        unbalAndTangent.getResidual().addMatrixVector(0.0, *T, unbalAndTangentMod.getResidual(), 1.0);
        ((*myNode).*setTrial)(unbalAndTangent.getResidual());
      }
    else
      ((*myNode).*setTrial)(unbalAndTangentMod.getResidual());
  }

//! @brief Method to set the corresponding nodes displacements to the
//! values in u, components identified by myID
void XC::TransformationDOF_Group::setNodeDisp(const Vector &u)
  {
    const MFreedom_ConstraintBase *mfc= getMFreedomConstraint();
    // call base class method and return if no MFreedom_Constraint
    if(!mfc)
      {
        this->DOF_Group::setNodeDisp(u);
        return;
      }
    else
      setupResidual(u,&Node::setTrialDisp);
  }

//! @brief Method to set the corresponding nodes velocities to the
//! values in u, components identified by myID
void XC::TransformationDOF_Group::setNodeVel(const Vector &u)
  {
    const MFreedom_ConstraintBase *mfc= getMFreedomConstraint();
    // call base class method and return if no MFreedom_Constraint
    if(!mfc)
      {
        this->DOF_Group::setNodeVel(u);
        return;
      }
    else
      setupResidual(u,&Node::setTrialVel);    
  }


//! @brief Method to set the corresponding nodes accelerations to the
//! values in u, components identified by myID
void XC::TransformationDOF_Group::setNodeAccel(const Vector &u)
  {
    const MFreedom_ConstraintBase *mfc= getMFreedomConstraint();
    // call base class method and return if no MFreedom_Constraint
    if(!mfc)
      {
        this->DOF_Group::setNodeAccel(u);
        return;
      }
    else
      setupResidual(u,&Node::setTrialAccel);    
  }

//! @brief Method to set the corresponding nodes displacements increments to the
//! values in u, components identified by myID.
void XC::TransformationDOF_Group::incrNodeDisp(const Vector &u)
  {
    const MFreedom_ConstraintBase *mfc= getMFreedomConstraint();
    // call base class method and return if no MFreedom_Constraint
    if(!mfc)
      {
        this->DOF_Group::incrNodeDisp(u);
        return;
      }
    else
      setupResidual(u,&Node::incrTrialDisp);
  }
        
//! @brief Method to set the corresponding nodes velocities increments to the
//! values in u, components identified by myID.
void XC::TransformationDOF_Group::incrNodeVel(const Vector &u)
  {
    const MFreedom_ConstraintBase *mfc= getMFreedomConstraint();
    // call base class method and return if no MFreedom_Constraint
    if(!mfc)
      {
        this->DOF_Group::incrNodeVel(u);
        return;
      }
    else
      setupResidual(u,&Node::incrTrialVel);
  }

//! @brief Method to set the corresponding nodes accelerations increments to the
//! values in u, components identified by myID.
void XC::TransformationDOF_Group::incrNodeAccel(const XC::Vector &u)
  {
    const MFreedom_ConstraintBase *mfc= getMFreedomConstraint();
    // call base class method and return if no MFreedom_Constraint
    if(!mfc)
      {
        this->DOF_Group::incrNodeAccel(u);
        return;
      }
    else
      setupResidual(u,&Node::incrTrialAccel);
  }

void XC::TransformationDOF_Group::setEigenvector(int mode, const Vector &u)
  {
    const MFreedom_ConstraintBase *mfc= getMFreedomConstraint();
    // call base class method and return if no MFreedom_Constraint
    if(!mfc)
      this->DOF_Group::setEigenvector(mode, u);
    else
      {
        const ID &theID= this->getID();
        for(int i=0; i<modNumDOF; i++)
          {
            int loc= theID(i);
            if(loc >= 0)
              (unbalAndTangentMod.getResidual())(i)= u(loc);
            // DO THE SP STUFF
          }    
        const Matrix *T= this->getT();
        if(T)
          {
            // unbalAndTangent.getResidual()= (*T) * (unbalAndTangentMod.getResidual());
            unbalAndTangent.getResidual().addMatrixVector(0.0, *T, unbalAndTangentMod.getResidual(), 1.0);
            myNode->setEigenvector(mode, unbalAndTangent.getResidual());
          }
        else
          myNode->setEigenvector(mode, unbalAndTangentMod.getResidual());
      }
  }


XC::Matrix *XC::TransformationDOF_Group::getT(void)
  {
    const MFreedom_ConstraintBase *mfc= getMFreedomConstraint();
    Matrix *retval= nullptr;

    if(mfc)
      {
        if(mfc->isTimeVarying())
	  {
            const int numNodalDOF= myNode->getNumberDOF();
            const ID &retainedDOF= mfc->getRetainedDOFs();
            const ID &constrainedDOF= mfc->getConstrainedDOFs();    
            int numNodalDOFConstrained= constrainedDOF.Size();
            int numRetainedDOF= numNodalDOF - numNodalDOFConstrained;
            int numRetainedNodeDOF= retainedDOF.Size();

            Trans.Zero();
            const Matrix &Ccr= mfc->getConstraint();
            int col= 0;
            for(int i=0; i<numNodalDOF; i++)
              {
                const int loc= constrainedDOF.getLocation(i);
                if(loc < 0)
                  {
                    Trans(i,col)= 1.0;
                    col++;
                  }
                else
                  {
                    for(int j=0; j<numRetainedNodeDOF; j++)
                      Trans(i,j+numRetainedDOF)= Ccr(loc,j);
                  }
              }
	  }
	retval= &Trans;
      }
    return retval;    
  }


int XC::TransformationDOF_Group::doneID(void)
  {
    const MFreedom_ConstraintBase *mfc= getMFreedomConstraint();
    if(mfc)
      {
        // get number of DOF & verify valid
        int numNodalDOF= myNode->getNumberDOF();
        const ID &retainedDOF= mfc->getRetainedDOFs();
        const ID &constrainedDOF= mfc->getConstrainedDOFs();    
        int numNodalDOFConstrained= constrainedDOF.Size();
        int numRetainedDOF= numNodalDOF - numNodalDOFConstrained;
        int numRetainedNodeDOF= retainedDOF.Size();

	const std::vector<Node *> ptrsToRetainedNodes= getPointersToRetainedNodes();
	for(std::vector<Node *>::const_iterator i= ptrsToRetainedNodes.begin();i!= ptrsToRetainedNodes.end(); i++)
	  {
            Node *retainedNodePtr= *i;
            DOF_Group *retainedGroup= retainedNodePtr->getDOF_GroupPtr();
            const ID &otherID= retainedGroup->getID();
    
            // set the ID for those dof corresponding to dof at another node
            for(int i=0; i<numRetainedNodeDOF; i++)
              {
                const int dof= retainedDOF(i);
                modID(i+numRetainedDOF)= otherID(dof);
              }
    
            // if constraint is not time-varying determine the transformation matrix
            if(mfc->isTimeVarying() == false)
              {
                Trans.Zero();
                const Matrix &Ccr= mfc->getConstraint();
                int col= 0;
                for(int i=0; i<numNodalDOF; i++)
                  {
                    const int loc= constrainedDOF.getLocation(i);
                    if(loc < 0)
                      {
                        Trans(i,col)= 1.0;
                        col++;
                      }
                    else
                      {
                        for(int j=0; j<numRetainedNodeDOF; j++)
                          Trans(i,j+numRetainedDOF)= Ccr(loc,j);
                      }
                  }
              }
          }
      }
    return 0;
  }

int XC::TransformationDOF_Group::addSFreedom_Constraint(SFreedom_Constraint &theSP)
  {
    // add the SFreedom_Constraint
    int dof= theSP.getDOF_Number();
    theSPs[dof]= &theSP;

    const MFreedom_ConstraintBase *mfc= getMFreedomConstraint();
    if(!mfc)
      this->setID(dof,-1); // set a -1 in the correct ID location
    else
      {
        const ID &constrainedDOF= mfc->getConstrainedDOFs();
        int loc= 0;
        for(int i=0; i<dof; i++) 
          if(constrainedDOF.getLocation(i) < 0)
            loc++;
        this->setID(loc,-1);
      }
    return 0;
  }

int XC::TransformationDOF_Group::enforceSPs(void)
  {
    int numDof= myNode->getNumberDOF();
    for(int i=0; i<numDof; i++)
      if(theSPs[i] != 0)
        {
          const double value= theSPs[i]->getValue();
          myNode->setTrialDispComponent(value, i);
        }
    return 0;
  }

void XC::TransformationDOF_Group::addM_Force(const Vector &Udotdot, double fact)
  {
    const MFreedom_ConstraintBase *mfc= getMFreedomConstraint();
    // call base class method and return if no MFreedom_Constraint
    if(!mfc || modID.Nulo())
      this->DOF_Group::addM_Force(Udotdot, fact);
    else
      {
        for(int i=0; i<modNumDOF; i++)
          {
            const int loc= modID(i);
            if(loc >= 0)
              (unbalAndTangentMod.getResidual())(i)= Udotdot(loc);
            else         // DO THE SP STUFF
              (unbalAndTangentMod.getResidual())(i)= 0.0;            
          }    
        Vector unmod(Trans.noRows());
        //unmod= Trans * (unbalAndTangentMod.getResidual());
        unmod.addMatrixVector(0.0, Trans, unbalAndTangentMod.getResidual(), 1.0);
        this->addLocalM_Force(unmod, fact);
      }
  }


const XC::Vector &XC::TransformationDOF_Group::getM_Force(const XC::Vector &Udotdot, double fact)
  {
    const MFreedom_ConstraintBase *mfc= getMFreedomConstraint();
    // call base class method and return if no MFreedom_Constraint
    if(!mfc || modID.Nulo())
      { return this->DOF_Group::getM_Force(Udotdot, fact); }
    else
      {
        this->DOF_Group::zeroTangent();    
        this->DOF_Group::addMtoTang();    
        const Matrix &unmodTangent= this->DOF_Group::getTangent(0);

        Vector data(modNumDOF);
        for(int i=0; i<modNumDOF; i++)
          {
            const int loc= modID(i);
            if(loc >= 0)
              data(i)= Udotdot(loc);
            else // DO THE SP STUFF
              data(i)= 0.0;            
          }    

        const Matrix *T= this->getT();
        if(T)
          {
            // unbalAndTangentMod.getTangent()= (*T) ^ unmodTangent * (*T);
            unbalAndTangentMod.getTangent().addMatrixTripleProduct(0.0, *T, unmodTangent, 1.0);
            unbalAndTangentMod.getResidual().addMatrixVector(0.0, unbalAndTangentMod.getTangent(), data, 1.0);
          }
        else
          unbalAndTangentMod.getResidual().addMatrixVector(0.0, unmodTangent, data, 1.0);
        return unbalAndTangentMod.getResidual();
      }
  }

const XC::Vector &XC::TransformationDOF_Group::getC_Force(const XC::Vector &Udotdot, double fact)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not yet implemented\n";
    return unbalAndTangentMod.getResidual();
  }

const XC::Vector &XC::TransformationDOF_Group::getTangForce(const XC::Vector &Udotdot, double fact)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not yet implemented\n";
    return unbalAndTangentMod.getResidual();
  }



// AddingSensitivity:BEGIN ////////////////////////////////////////
const XC::Vector &XC::TransformationDOF_Group::getDispSensitivity(int gradNumber)
  {
    const Vector &result= this->DOF_Group::getDispSensitivity(gradNumber);

    const Matrix *T= this->getT();
    if(T)
      {
        // unbalAndTangentMod.getResidual()= (*T) ^ unmodUnbalance;
        unbalAndTangentMod.getResidual().addMatrixTransposeVector(0.0, *T, result, 1.0);
        return unbalAndTangentMod.getResidual();    
      }
    else
      return result;
  }

const XC::Vector &XC::TransformationDOF_Group::getVelSensitivity(int gradNumber)
  {
    const Vector &result= this->DOF_Group::getVelSensitivity(gradNumber);

    const Matrix *T= this->getT();
    if(T)
      {
        // unbalAndTangentMod.getResidual()= (*T) ^ unmodUnbalance;
        unbalAndTangentMod.getResidual().addMatrixTransposeVector(0.0, *T, result, 1.0);
        return unbalAndTangentMod.getResidual();    
      }
    else
      return result;
  }

const XC::Vector &XC::TransformationDOF_Group::getAccSensitivity(int gradNumber)
  {
    const Vector &result= this->DOF_Group::getAccSensitivity(gradNumber);

    const Matrix *T= this->getT();
    if(T)
      {
        // unbalAndTangentMod.getResidual()= (*T) ^ unmodUnbalance;
        unbalAndTangentMod.getResidual().addMatrixTransposeVector(0.0, *T, result, 1.0);
        return unbalAndTangentMod.getResidual();    
      }
    else
      return result;
  }
        
        

int XC::TransformationDOF_Group::saveSensitivity(Vector *u,Vector *udot,Vector *udotdot, int gradNum,int numGrads)
  {
    int retval= 0;
    const MFreedom_ConstraintBase *mfc= getMFreedomConstraint();
    // call base class method and return if no MFreedom_Constraint
    if(!mfc)
      {
        retval= this->DOF_Group::saveSensitivity(u, udot, udotdot, gradNum, numGrads);
      }
    else
      {
        // Get sensitivities for my dof out of vectors
        Vector myV(modNumDOF);
        
        const ID &theID= this->getID();
        for(int i=0; i<modNumDOF; i++)
          {
            int loc= theID(i);
            if(loc >= 0)
              (unbalAndTangentMod.getResidual())(i)= (*u)(loc);
            // DO THE SP STUFF
          }    
        const Matrix *T= this->getT();
        if(T)
          {
            // unbalAndTangent.getResidual()= (*T) * (unbalAndTangentMod.getResidual());
            myV.addMatrixVector(0.0, *T, unbalAndTangentMod.getResidual(), 1.0);
          }
        else
          myV= unbalAndTangentMod.getResidual();


        // Vel and Acc sensitivities only if they are being delivered
        if((udot != 0) && (udotdot != 0))
          {
            Vector myVdot(modNumDOF);
            Vector myVdotdot(modNumDOF);
            for(int i=0; i<modNumDOF; i++)
              {
                int loc= theID(i);
                if(loc >= 0)
                  (unbalAndTangentMod.getResidual())(i)= (*udot)(loc);
                // DO THE SP STUFF
              }    

            if(T)
              {
                // unbalAndTangent.getResidual()= (*T) * (unbalAndTangentMod.getResidual());
                myVdot.addMatrixVector(0.0, *T, unbalAndTangentMod.getResidual(), 1.0);
              }
            else
              myVdot= unbalAndTangentMod.getResidual();

            for(int j=0; j<modNumDOF; j++)
              {
                int loc= theID(j);
                if(loc >= 0)
                  (unbalAndTangentMod.getResidual())(j)= (*udotdot)(loc);
                // DO THE SP STUFF
              }
            if(T)
              {
                // unbalAndTangent.getResidual()= (*T) * (unbalAndTangentMod.getResidual());
                myVdotdot.addMatrixVector(0.0, *T, unbalAndTangentMod.getResidual(), 1.0);
              }
            else
              myVdotdot= unbalAndTangentMod.getResidual();
            myNode->saveSensitivity(&myV, &myVdot, &myVdotdot, gradNum, numGrads);
          }
        else
          myNode->saveSensitivity(&myV,nullptr,nullptr,gradNum,numGrads);
      }
    return retval;
  }

void XC::TransformationDOF_Group::addM_ForceSensitivity(const XC::Vector &Udotdot, double fact)
  {
    const MFreedom_ConstraintBase *mfc= getMFreedomConstraint();
    // call base class method and return if no MFreedom_Constraint
    if(!mfc || modID.Nulo())
      { this->DOF_Group::addM_ForceSensitivity(Udotdot, fact); }
    else
      {
        for(int i=0; i<modNumDOF; i++)
          {
            const int loc= modID(i);
            if(loc >= 0)
              (unbalAndTangentMod.getResidual())(i)= Udotdot(loc);
            else         // DO THE SP STUFF
              (unbalAndTangentMod.getResidual())(i)= 0.0;            
          }    

        Vector unmod(Trans.noRows());
        //unmod= Trans * (unbalAndTangentMod.getResidual());
        unmod.addMatrixVector(0.0, Trans, unbalAndTangentMod.getResidual(), 1.0);
        this->DOF_Group::addM_ForceSensitivity(unmod, fact);
      }
  }

void XC::TransformationDOF_Group::addD_Force(const XC::Vector &Udot, double fact)
  {
    const MFreedom_ConstraintBase *mfc= getMFreedomConstraint();
    // call base class method and return if no MFreedom_Constraint
    if(!mfc || modID.Nulo())
      { this->DOF_Group::addD_Force(Udot, fact); }
    else
      {
        for(int i=0; i<modNumDOF; i++)
          {
            const int loc= modID(i);
            if(loc >= 0)
              (unbalAndTangentMod.getResidual())(i)= Udot(loc);
            else         // DO THE SP STUFF
              (unbalAndTangentMod.getResidual())(i)= 0.0;            
          }    
        Vector unmod(Trans.noRows());
        //unmod= Trans * (unbalAndTangentMod.getResidual());
        unmod.addMatrixVector(0.0, Trans, unbalAndTangentMod.getResidual(), 1.0);
        this->DOF_Group::addD_Force(unmod, fact);
      }
  }

void XC::TransformationDOF_Group::addD_ForceSensitivity(const XC::Vector &Udot, double fact)
  {
    const MFreedom_ConstraintBase *mfc= getMFreedomConstraint();
    // call base class method and return if no MFreedom_Constraint
    if(!mfc || modID.Nulo())
      { this->DOF_Group::addD_ForceSensitivity(Udot, fact); }
    else
      {
        for(int i=0; i<modNumDOF; i++)
          {
            const int loc= modID(i);
            if(loc >= 0)
              (unbalAndTangentMod.getResidual())(i)= Udot(loc);
            else         // DO THE SP STUFF
              (unbalAndTangentMod.getResidual())(i)= 0.0;    
          }    

        Vector unmod(Trans.noRows());
        //unmod= Trans * (unbalAndTangentMod.getResidual());
        unmod.addMatrixVector(0.0, Trans, unbalAndTangentMod.getResidual(), 1.0);
        this->DOF_Group::addD_ForceSensitivity(unmod, fact);
      }
  }

// AddingSensitivity:END //////////////////////////////////////////
