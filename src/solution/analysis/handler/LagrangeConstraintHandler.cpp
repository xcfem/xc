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

// $Revision: 1.5 $
// $Date: 2005/11/29 22:04:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/handler/LagrangeConstraintHandler.cpp,v $

// Written: fmk
// Created: May 1998
// Revision: A
//
// What: "@(#) LagrangeConstraintHandler.C, revA"

#include <solution/analysis/handler/LagrangeConstraintHandler.h>
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
#include "domain/domain/subdomain/Subdomain.h"
#include <solution/analysis/model/dof_grp/LagrangeDOF_Group.h>
#include <solution/analysis/model/fe_ele/lagrange/LagrangeSFreedom_FE.h>
#include <solution/analysis/model/fe_ele/lagrange/LagrangeMFreedom_FE.h>
#include <solution/analysis/model/fe_ele/lagrange/LagrangeMRMFreedom_FE.h>


//! @brief Constructor.
//! @param owr: pointer to the model wrapper that owns the handler.
//! @param sp: factor to be used with the single freedom constraints.
//! @param mp: factor to be used with the multi-freedom constraints.
XC::LagrangeConstraintHandler::LagrangeConstraintHandler(ModelWrapper *owr,const double &sp,const double &mp)
  :FactorsConstraintHandler(owr,HANDLER_TAG_LagrangeConstraintHandler,sp,mp) {}

//! @brief Virtual constructor.
XC::ConstraintHandler *XC::LagrangeConstraintHandler::getCopy(void) const
  { return new LagrangeConstraintHandler(*this); }

//! @brief Handle the constraints.
int XC::LagrangeConstraintHandler::handle(const ID *nodesLast)
  {
    // first check links exist to a Domain and an XC::AnalysisModel object
    Domain *theDomain = this->getDomainPtr();
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    Integrator *theIntegrator = this->getIntegratorPtr();

    if((!theDomain) || (!theModel) || (!theIntegrator))
      {
        std::cerr << nombre_clase() << "::" << __FUNCTION__
	          << "; domain, model or integrator was not set.\n";
        return -1;
      }

    // get number of elements and nodes in the domain
    // and init the theFEs and theDOFs arrays

    int numConstraints = 0;
    SFreedom_ConstraintIter &theSPss= theDomain->getConstraints().getDomainAndLoadPatternSPs();
    SFreedom_Constraint *spPtr;
    while((spPtr = theSPss()) != 0)
      numConstraints++;

    numConstraints+= theDomain->getConstraints().getNumMPs();

    //create a DOF_Group for each Node and add it to the AnalysisModel.
    //    : must of course set the initial IDs
    NodeIter &theNod = theDomain->getNodes();
    Node *nodPtr= nullptr;
    DOF_Group *dofPtr= nullptr;

    int numDofGrp= 0;
    int count3= 0;
    int countDOF=0;
    while((nodPtr = theNod()) != nullptr)
      {
        dofPtr= theModel->createDOF_Group(numDofGrp++, nodPtr);
	// initially set all the ID value to -2
        countDOF+= dofPtr->inicID(-2);
      }

    // create the FE_Elements for the Elements and add to the AnalysisModel
    ElementIter &theEle = theDomain->getElements();
    Element *elePtr= nullptr;

    int numFeEle = 0;
    FE_Element *fePtr= nullptr;
    while((elePtr = theEle()) != nullptr)
      { fePtr= theModel->createFE_Element(numFeEle++, elePtr); }

    // create the LagrangeSFreedom_FE for the SFreedom_Constraints and
    // add to the AnalysisModel
    SFreedom_ConstraintIter &theSPs = theDomain->getConstraints().getDomainAndLoadPatternSPs();
    while((spPtr = theSPs()) != nullptr)
      {
        dofPtr= theModel->createLagrangeDOF_Group(numDofGrp++, spPtr);
	// initially set all the ID value to -2
        countDOF+= dofPtr->inicID(-2);
        fePtr= theModel->createLagrangeSFreedom_FE(numFeEle++, *spPtr,*dofPtr, alphaSP);
      }

    // create the LagrangeMFreedom_FE for the MFreedom_Constraints and
    // add to the AnalysisModel
    MFreedom_ConstraintIter &theMPs = theDomain->getConstraints().getMPs();
    MFreedom_Constraint *mpPtr;
    while((mpPtr = theMPs()) != nullptr)
      {
        dofPtr= theModel->createLagrangeDOF_Group(numDofGrp++, mpPtr);
	// initially set all the ID value to -2
        countDOF+= dofPtr->inicID(-2);
        fePtr= theModel->createLagrangeMFreedom_FE(numFeEle++, *mpPtr, *dofPtr, alphaMP);
      }

    // create the LagrangeMRMFreedom_FE for the MRMFreedom_Constraints and
    // add to the AnalysisModel
    MRMFreedom_ConstraintIter &theMRMPs = theDomain->getConstraints().getMRMPs();
    MRMFreedom_Constraint *mrmpPtr;
    while((mrmpPtr = theMRMPs()) != nullptr)
      {
        dofPtr= theModel->createLagrangeDOF_Group(numDofGrp++, mrmpPtr);
	// initially set all the ID value to -2
        countDOF+= dofPtr->inicID(-2);
        fePtr= theModel->createLagrangeMRMFreedom_FE(numFeEle++, *mrmpPtr, *dofPtr, alphaMP);
      }

    theModel->setNumEqn(countDOF); // set the number of eqn in the model
    
    // now see if we have to set any of the dof's to -3
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
              for(int j=0; j < id.Size(); j++)
                if(id(j) == -2)
                  {
                    dofPtr->setID(j,-3);
                    count3++;
                  }
                else
                  {
                    std::cerr << nombre_clase() << "::" << __FUNCTION__
		              << "; boundary sp constraint in subdomain"
                              << " this should not be - results suspect \n";
                  }
            }
        }
    return count3;
  }

