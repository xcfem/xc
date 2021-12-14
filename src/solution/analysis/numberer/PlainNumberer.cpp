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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/14 23:00:51 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/numberer/PlainNumberer.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/numberer/PlainNumberer.C
// 
// Written: fmk 
// Created: 9/96
// Revision: A
//
// Description: This file contains the class definition for XC::PlainNumberer.
// PlainNumberer is a subclass of XC::DOF_Numberer. The XC::PlainNumberer assigns
// equation numbers to the DOFs on a first come first serve basis; that is 
// it gets the XC::DOF_GrpIter and assigns the equation numbers to the DOFs
// as it iterates through the iter.
//
// What: "@(#) PlainNumberer.C, revA"

#include <solution/analysis/numberer/PlainNumberer.h>
#include <solution/analysis/model/AnalysisModel.h>

#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/analysis/model/FE_EleIter.h>
#include <utility/matrix/ID.h>

#include <domain/domain/Domain.h>
#include <domain/constraints/MFreedom_Constraint.h>
#include <domain/constraints/MRMFreedom_Constraint.h>
#include <domain/mesh/node/Node.h>
#include <domain/constraints/MFreedom_ConstraintIter.h>
#include <domain/constraints/MRMFreedom_ConstraintIter.h>

//! @brief Constructor.
//!
//! @param owr: object owner.
XC::PlainNumberer::PlainNumberer(ModelWrapper *owr) 
  :DOF_Numberer(owr,NUMBERER_TAG_PlainNumberer) {}

//! @brief Virtual constructor.
XC::DOF_Numberer *XC::PlainNumberer::getCopy(void) const
  { return new PlainNumberer(*this);  }


//! @brief Method to number the unnumbered DOFs in the DOF Groups.
//!
//! The PlainNumberer will twice obtain the DOF\_GroupIter from the
//! AnalysisModel. It iterates twice through the DOF\_Groups first
//! assigning the dofs with eqn numbers assigned -2 a number and 
//! then on the next pass the dofs assigned -3. The PlainNumberer then
//! invokes setID() on each FE\_Element in the
//! AnalysisModel. Finally it invokes {\em setEqnNum(numEqn)} on the
//! AnalyisModel. Returns a positive integer equal to the last equation
//! number set if successful, a negative number if not; the value of 
//! which depends on the type of the PlainNumberer. A PlainNumberer will
//! not use the {\em lastDOF\_Group} integer, if one is supplied a warning
//! message is printed. 
int XC::PlainNumberer::numberDOF(int lastDOF)
  {
    int eqnNumber= 0; // start equation number= 0

    // get a pointer to the model & check its not null
    AnalysisModel *theModel= this->getAnalysisModelPtr();
    Domain *theDomain= nullptr;
    if(theModel != 0) theDomain= theModel->getDomainPtr();

    if(theModel == 0 || theDomain == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING: no AnalysisModel.\n";
        return -1;
      }
    
    if(lastDOF != -1)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING: does not use the lastDOF as requested.\n";
      }
    
    // iterate through  the DOFs first time setting -2 values
    DOF_GrpIter &theDOFGroups= theModel->getDOFGroups();
    DOF_Group *dofGroupPtr= nullptr;
    while((dofGroupPtr= theDOFGroups()) != 0)
      {
        const ID &theID= dofGroupPtr->getID();
        for(int i=0; i<theID.Size(); i++)
          if(theID(i) == -2) 
            dofGroupPtr->setID(i,eqnNumber++);
      }


    // iterate through  the DOFs second time setting -3 values
    DOF_GrpIter &moreDOFs= theModel->getDOFGroups();
    
    while((dofGroupPtr= moreDOFs()) != 0)
      {
        const ID &theID= dofGroupPtr->getID();
        for(int i=0; i<theID.Size(); i++)
          if(theID(i) == -3)
            dofGroupPtr->setID(i,eqnNumber++);
      }

    // iterate through the DOFs one last time setting any -4 values
    DOF_GrpIter &tDOFs= theModel->getDOFGroups();
    while((dofGroupPtr= tDOFs()) != 0)
      {
        const ID &theID= dofGroupPtr->getID();
        bool have4s= false;
        for(int i=0; i<theID.Size(); i++)
          if(theID(i) == -4)
            {
              have4s= true;
              break;
            }

        if(have4s)
          {
            const int nodeID= dofGroupPtr->getNodeTag();
            // loop through the MFreedom_Constraints to see if any of the
            // DOFs are constrained, note constraint matrix must be diagonal
            // with 1's on the diagonal
            MFreedom_ConstraintIter &theMPs= theDomain->getConstraints().getMPs();
            MFreedom_Constraint *mpPtr= nullptr;
            while((mpPtr= theMPs()) != 0 )
              {
                // note keep looping over all in case multiple constraints
                // are used to constrain a node -- can't assume intelli user
                if(mpPtr->getNodeConstrained() == nodeID)
                  {
                    int nodeRetained= mpPtr->getNodeRetained();
                    Node *nodeRetainedPtr= theDomain->getNode(nodeRetained);
                    DOF_Group *retainedDOF= nodeRetainedPtr->getDOF_GroupPtr();
                    const ID &retainedDOFIDs= retainedDOF->getID();
                    const ID &constrainedDOFs= mpPtr->getConstrainedDOFs();
                    const ID &retainedDOFs= mpPtr->getRetainedDOFs();
                    for(int i=0; i<constrainedDOFs.Size(); i++)
                      {
                        const int dofC= constrainedDOFs(i);
                        const int dofR= retainedDOFs(i);
                        const int dofID= retainedDOFIDs(dofR);
                        dofGroupPtr->setID(dofC, dofID);
                      }
                  }
              }
            MRMFreedom_ConstraintIter &theMRMPs= theDomain->getConstraints().getMRMPs();
            MRMFreedom_Constraint *mrmpPtr;
            while((mrmpPtr= theMRMPs()) != 0 )
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
		          << "; write code loop through the "
		          << theDomain->getConstraints().getClassName()
			  << std::endl;
              }
          }        
      }
    eqnNumber--;
    const int numEqn= eqnNumber - START_EQN_NUMBER +1;
        
    // iterate through the FE_Element getting them to set their IDs
    FE_EleIter &theEle= theModel->getFEs();
    FE_Element *elePtr= nullptr;
    while((elePtr= theEle()) != 0)
      elePtr->setID();

    // set the numOfEquation in the Model
    theModel->setNumEqn(numEqn);

    return numEqn;
  }

//! @brief Does nothing.
int XC::PlainNumberer::sendSelf(Communicator &comm)
  { return 0; }

//! @brief Does nothing.
int XC::PlainNumberer::recvSelf(const Communicator &comm)
  { return 0; }


//! The method is identical to that outlined above. A PlainNumberer will
//! not use the {\em lastDOF\_Groups} ID, if this method is invoked a warning
//! message is printed. 
int XC::PlainNumberer::numberDOF(ID &lastDOFs)
  {
    int eqnNumber= 0; // start equation number= 0
    
    // get a pointer to the model & check its not null
    AnalysisModel *theModel= this->getAnalysisModelPtr();
    Domain *theDomain =0;
    if(theModel != 0) theDomain= theModel->getDomainPtr();

    if(theModel == 0 || theDomain == 0)
      {
        std::cerr << "WARNING PlainNumberer::numberDOF(int) -";
        std::cerr << " - no AnalysisModel.\n";
        return -1;
      }
    
    std::cerr << "WARNING PlainNumberer::numberDOF(ID):";
    std::cerr << " does not use the lastDOFs as requested\n";
    
    // iterate through  the DOFs first time setting -2 values
    DOF_GrpIter &theDOFGroups= theModel->getDOFGroups();
    DOF_Group *dofGroupPtr= nullptr;
    
    while((dofGroupPtr= theDOFGroups()) != 0)
      {
        const ID &theID= dofGroupPtr->getID();
        for(int i=0; i<theID.Size(); i++)
          if(theID(i) == -2) dofGroupPtr->setID(i,eqnNumber++);
      }

    // iterate through  the DOFs first time setting -3 values
    DOF_GrpIter &moreDOFs= theModel->getDOFGroups();
    
    while((dofGroupPtr= moreDOFs()) != 0)
      {
        const ID &theID= dofGroupPtr->getID();
        for(int i=0; i<theID.Size(); i++)
          if(theID(i) == -3) dofGroupPtr->setID(i,eqnNumber++);
      }
    // iterate through the DOFs one last time setting any -4 values
    // iterate through the DOFs one last time setting any -4 values
    DOF_GrpIter &tDOFs= theModel->getDOFGroups();
    while((dofGroupPtr= tDOFs()) != 0)
      {
        const ID &theID= dofGroupPtr->getID();
        bool have4s= false;
        for(int i=0; i<theID.Size(); i++)
          if(theID(i) == -4)
            {
              have4s= true;
              break;
            }

        if(have4s)
          {
            int nodeID= dofGroupPtr->getNodeTag();
            // loop through the MFreedom_Constraints to see if any of the
            // DOFs are constrained, note constraint matrix must be diagonal
            // with 1's on the diagonal
            MFreedom_ConstraintIter &theMPs= theDomain->getConstraints().getMPs();
            MFreedom_Constraint *mpPtr= nullptr;
            while((mpPtr= theMPs()) != 0 )
              {
                // note keep looping over all in case multiple constraints
                // are used to constrain a node -- can't assume intelli user
                if(mpPtr->getNodeConstrained() == nodeID)
                  {
                    int nodeRetained= mpPtr->getNodeRetained();
                    Node *nodeRetainedPtr= theDomain->getNode(nodeRetained);
                    DOF_Group *retainedDOF= nodeRetainedPtr->getDOF_GroupPtr();
                    const ID &retainedDOFIDs= retainedDOF->getID();
                    const ID &constrainedDOFs= mpPtr->getConstrainedDOFs();
                    const ID &retainedDOFs= mpPtr->getRetainedDOFs();
                    for(int i=0; i<constrainedDOFs.Size(); i++)
                      {
                        int dofC= constrainedDOFs(i);
                        int dofR= retainedDOFs(i);
                        int dofID= retainedDOFIDs(dofR);
                        dofGroupPtr->setID(dofC, dofID);
                      }
                  }
              }
            MRMFreedom_ConstraintIter &theMRMPs= theDomain->getConstraints().getMRMPs();
            MRMFreedom_Constraint *mrmpPtr;
            while((mrmpPtr= theMRMPs()) != 0 )
              {
                 std::cerr << "PlainNumberer::numberDOF(ID) write code loop through the MRMFreedom_Constraints." << std::endl;
              }

          }        
      }
    eqnNumber--;
    int numEqn= eqnNumber - START_EQN_NUMBER +1;
        
    // iterate through the FE_Element getting them to set their IDs
    FE_EleIter &theEle= theModel->getFEs();
    FE_Element *elePtr;
    while((elePtr= theEle()) != 0)
      elePtr->setID();

    // set the numOfEquation in the Model
    theModel->setNumEqn(numEqn);

    return numEqn;
  }
