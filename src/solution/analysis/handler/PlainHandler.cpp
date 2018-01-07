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
                                                                        
// $Revision: 1.6 $
// $Date: 2005/11/29 22:04:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/handler/PlainHandler.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the implementation of XC::PlainHandler.
//
// What: "@(#) PlainHandler.C, revA"

#include <solution/analysis/handler/PlainHandler.h>
#include <cstdlib>

#include <solution/analysis/model/AnalysisModel.h>
#include <domain/domain/Domain.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <domain/mesh/node/Node.h>
#include <domain/mesh/element/Element.h>
#include "domain/mesh/node/NodeIter.h"
#include "domain/mesh/element/ElementIter.h"
#include <domain/constraints/SFreedom_ConstraintIter.h>
#include <domain/constraints/SFreedom_Constraint.h>
#include <domain/constraints/MFreedom_ConstraintIter.h>
#include <domain/constraints/MFreedom_Constraint.h>
#include <domain/constraints/MRMFreedom_ConstraintIter.h>
#include <domain/constraints/MRMFreedom_Constraint.h>
#include <solution/analysis/integrator/Integrator.h>
#include <utility/matrix/ID.h>
#include "utility/matrix/Matrix.h"
#include "domain/domain/subdomain/Subdomain.h"

//! @brief Constructor.
//! @param owr: pointer to the model wrapper that owns the handler.
XC::PlainHandler::PlainHandler(ModelWrapper *owr)
  :ConstraintHandler(owr,HANDLER_TAG_PlainHandler) {}

//! @brief Virtual constructor.
XC::ConstraintHandler *XC::PlainHandler::getCopy(void) const
  { return new PlainHandler(*this); }

//! @brief Handle the constraints.
//! 
//! Determines the number of FE\_Elements and DOF\_Groups needed from the
//! Domain (a one to one mapping between Elements and FE\_Elements and
//! Nodes and DOF\_Groups) Creates two arrays of pointers to store the
//! FE\_elements and DOF\_Groups, returning a warning message and a \f$-2\f$
//! or \f$-3\f$ if not enough memory is available for these arrays. Then the
//! object will iterate through the Nodes of the Domain, creating a
//! DOF\_Group for each node and setting the initial id for each dof to
//! \f$-2\f$ if no SFreedom\_Constraint exists for the dof, or \f$-1\f$ if a
//! SFreedom\_Constraint exists or \f$-3\f$ if the node identifier is in {\em
//! nodesToBeNumberedLast}. The object then iterates through the Elements
//! of the Domain creating a FE\_Element for each Element, if the Element
//! is a Subdomain setFE\_ElementPtr() is invoked on the Subdomain
//! with the new FE\_Element as the argument. If not enough memory is
//! available for any DOF\_Group or FE\_element a warning message is
//! printed and a \f$-4\f$ or \f$-5\f$ is returned. If any MFreedom\_Constraint
//! objects exist in the Domain a warning message is printed and \f$-6\f$ is
//! returned. If all is successful, the method returns the number of
//! degrees-of-freedom associated with the DOF\_Groups in {\em
//! nodesToBeNumberedLast}.
int XC::PlainHandler::handle(const ID *nodesLast)
  {
    // first check links exist to a Domain and an AnalysisModel object
    Domain *theDomain = this->getDomainPtr();
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    Integrator *theIntegrator = this->getIntegratorPtr();    
    
    if((!theDomain) || (!theModel) || (!theIntegrator))
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; domain, model or integrator was not set.\n";
        return -1;
      }

    // initialse the DOF_Groups and add them to the AnalysisModel.
    //    : must of course set the initial IDs
    NodeIter &theNod= theDomain->getNodes();
    Node *nodPtr= nullptr;
    SFreedom_Constraint *spPtr= nullptr;
    DOF_Group *dofPtr= nullptr;

    int numDOF = 0;
    int count3 = 0;
    int countDOF =0;
    while((nodPtr = theNod()) != nullptr)
      {
        dofPtr= theModel->createDOF_Group(numDOF++, nodPtr);
        // initially set all the ID value to -2
        countDOF+= dofPtr->inicID(-2);

        // loop through the SFreedom_Constraints to see if any of the
        // DOFs are constrained, if so set initial XC::ID value to -1
        int nodeID = nodPtr->getTag();
        SFreedom_ConstraintIter &theSPs = theDomain->getConstraints().getDomainAndLoadPatternSPs();
        while((spPtr = theSPs()) != 0)
            if(spPtr->getNodeTag() == nodeID)
              {
                if(spPtr->isHomogeneous() == false)
                  std::cerr << getClassName() << "::" << __FUNCTION__
                            << ";  non-homogeneos constraint"
                            << " for node " << spPtr->getNodeTag()
                            << " homo assumed\n";
                const ID &id = dofPtr->getID();
                int dof = spPtr->getDOF_Number();                
                if(id(dof) == -2)
                  {
                        dofPtr->setID(spPtr->getDOF_Number(),-1);
                        countDOF--;        
                  }
                else
                  std::cerr << getClassName() << "::" << __FUNCTION__
                            << "; multiple single pointconstraints at DOF "
                            << dof << " for node " << spPtr->getNodeTag()
                            << std::endl;
              }

        // loop through the MFreedom_Constraints to see if any of the
        // DOFs are constrained, note constraint matrix must be diagonal
        // with 1's on the diagonal
        MFreedom_ConstraintIter &theMPs = theDomain->getConstraints().getMPs();
        MFreedom_Constraint *mpPtr;
        while((mpPtr = theMPs()) != 0)
          {
            if(mpPtr->getNodeConstrained() == nodeID)
              {
                if(mpPtr->isTimeVarying() == true)
                  std::cerr << getClassName() << "::" << __FUNCTION__
                            << ";  time-varying constraint"
                            << " for node " << nodeID
                            << " non-varying assumed\n";
                const Matrix &C = mpPtr->getConstraint();
                int numRows = C.noRows();
                int numCols = C.noCols();
                if(numRows != numCols)
                  std::cerr << getClassName() << "::" << __FUNCTION__
                            << " constraint matrix not diagonal,"
                            << " ignoring constraint for node "
                            << nodeID << std::endl;
                else
                  {
                    int ok = 0;
                    for(int i=0; i<numRows; i++)
                      {
                        if(C(i,i) != 1.0) ok = 1;
                        for(int j=0; j<numRows; j++)
                          if(i != j)
                            if(C(i,j) != 0.0)
                          ok = 1;
                      }
                    if(ok != 0)
                      std::cerr << getClassName() << "::" << __FUNCTION__
                                << "; constraint matrix not identity,"
                                << " ignoring constraint for node "
                                << nodeID << std::endl;
                    else
                      {
                        const ID &dofs = mpPtr->getConstrainedDOFs();
                        const ID &id = dofPtr->getID();                                
                        for(int i=0; i<dofs.Size(); i++)
                          {
                            int dof = dofs(i);        
                            if(id(dof) == -2)
                              {
                                dofPtr->setID(dof,-4);
                                countDOF--;        
                              }
                            else
                              std::cerr << getClassName() << "::" << __FUNCTION__
                                        << ";  constraint at dof " << dof
                                        << " already specified for constrained node"
                                        << " in MFreedom_Constraint at node "
                                        << nodeID << std::endl;
                          }
                      }
                  }
              }
          }
        // loop through the MFreedom_Constraints to see if any of the
        // DOFs are constrained, note constraint matrix must be diagonal
        // with 1's on the diagonal
        MRMFreedom_ConstraintIter &theMRMPs = theDomain->getConstraints().getMRMPs();
        MRMFreedom_Constraint *mrmpPtr;
        while((mrmpPtr = theMRMPs()) != 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; loop through the MRMFreedom_Constraints."
		      << std::endl;
          }
      }

    // set the number of eqn in the model
    theModel->setNumEqn(countDOF);

    // now see if we have to set any of the dof's to -3
    //    int numLast = 0;
    if(nodesLast != 0) 
        for(int i=0; i<nodesLast->Size(); i++)
	  {
            int nodeID = (*nodesLast)(i);
            Node *nodPtr = theDomain->getNode(nodeID);
            if(nodPtr != 0)
	      {
                dofPtr = nodPtr->getDOF_GroupPtr();
                
                const ID &id = dofPtr->getID();
                // set all the dof values to -3
                for (int j=0; j < id.Size(); j++) 
                    if(id(j) == -2)
		      {
                        dofPtr->setID(j,-3);
                        count3++;
                      }
		    else
		      std::cerr << getClassName() << "::" << __FUNCTION__
		                << "; boundary sp constraint in subdomain"
		                << " this should not be - results suspect \n";
	      }
	  }
    
    // initialise the FE_Elements and add to the XC::AnalysisModel.
    ElementIter &theEle = theDomain->getElements();
    Element *elePtr;

    int numFe = 0;    
    FE_Element *fePtr;
    while((elePtr = theEle()) != 0)
      { fePtr= theModel->createFE_Element(numFe++, elePtr); }
    return count3;
  }

//! @brief Sends this object through the communicator (not implemented yet).
int XC::PlainHandler::sendSelf(CommParameters &cp)
  { return 0; }

//! @brief Receives this object through the communicator (not implemented yet).
int XC::PlainHandler::recvSelf(const CommParameters &cp)
  { return 0; }
