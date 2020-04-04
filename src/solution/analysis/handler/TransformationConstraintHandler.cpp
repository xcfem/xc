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
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO.
//  GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// junto a este programa.
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

// $Revision: 1.12 $
// $Date: 2005/11/29 22:04:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/handler/TransformationConstraintHandler.cpp,v $


// Written: fmk
// Created: May 1998
// Revision: A
//
// What: "@(#) TransformationConstraintHandler.C, revA"

#include <solution/analysis/handler/TransformationConstraintHandler.h>
#include <cstdlib>

#include <solution/analysis/model/AnalysisModel.h>
#include <domain/domain/Domain.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/analysis/model/FE_EleIter.h>
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
#include "utility/matrix/IDVarSize.h"
#include "domain/domain/subdomain/Subdomain.h"
#include <solution/analysis/model/dof_grp/TransformationDOF_Group.h>
#include <solution/analysis/model/fe_ele/transformation/TransformationFE.h>
#include "domain/mesh/element/utils/NodePtrsWithIDs.h"


//! @brief Constructor.
//! @param owr: pointer to the model wrapper that owns the handler.
XC::TransformationConstraintHandler::TransformationConstraintHandler(ModelWrapper *owr)
  :ConstraintHandler(owr,HANDLER_TAG_TransformationConstraintHandler),
 theFEs(), theDOFs(),numDOF(0),numConstrainedNodes(0) {}


//! @brief Virtual constructor.
XC::ConstraintHandler *XC::TransformationConstraintHandler::getCopy(void) const
  { return new TransformationConstraintHandler(*this); }

//! @brief Handle the constraints.
int XC::TransformationConstraintHandler::handle(const ID *nodesLast)
  {
    // first check links exist to a Domain and an AnalysisModel object
    Domain *theDomain= this->getDomainPtr();
    if(!theDomain)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; null pointer to domain." << std::endl;
    AnalysisModel *theModel= this->getAnalysisModelPtr();
    if(!theModel)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< " pointer to analysis model is null." << std::endl;
    Integrator *theIntegrator= this->getIntegratorPtr();
    if(!theIntegrator)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << " pointer to integrator is null." << std::endl;

    // get number of elements and nodes in the domain
    // and init the theFEs and theDOFs arrays

    //    int numSPConstraints= theDomain->getConstraints().getNumSPs();
    int numSPConstraints= 0;
    SFreedom_ConstraintIter &theSP1s= theDomain->getConstraints().getDomainAndLoadPatternSPs();
    SFreedom_Constraint *theSP1;
    while((theSP1= theSP1s()) != nullptr)
      numSPConstraints++;
    numDOF= 0;

    //int i= -1;

    // create an ID of constrained node tags in MFreedom_Constraints
    const int numMPConstraints= theDomain->getConstraints().getNumMPs();
    IDVarSize constrainedNodesMP(0, numMPConstraints, -1); //defaultValue= -1 otherwise we get a fase positive for
                                                           //a node with tag 0. LCPT 22/01/2018. 
    std::vector<MFreedom_Constraint *> mfs(numMPConstraints,static_cast<MFreedom_Constraint *>(nullptr));
    if(numMPConstraints != 0)
      {
        MFreedom_ConstraintIter &theMPs= theDomain->getConstraints().getMPs();
        MFreedom_Constraint *theMP= nullptr;
        int index= 0;
        while((theMP= theMPs()) != nullptr)
          {
            const int nodeConstrained= theMP->getNodeConstrained();
            numDOF++;
            constrainedNodesMP[index]= nodeConstrained;
            mfs[index]= theMP;
            index++;
          }
      }

    // create an ID of constrained node tags in MRMFreedom_Constraints
    const int numMRMPConstraints= theDomain->getConstraints().getNumMRMPs();
    IDVarSize constrainedNodesMRMP(0, numMRMPConstraints, -1); //defaultValue= -1 otherwise we get a fase positive for
                                                               //a node with tag 0. LCPT 22/01/2018.
    std::vector<MRMFreedom_Constraint *> mrmfs(numMRMPConstraints,static_cast<MRMFreedom_Constraint *>(nullptr));
    if(numMRMPConstraints != 0)
      {
        MRMFreedom_ConstraintIter &theMRMPs= theDomain->getConstraints().getMRMPs();
        MRMFreedom_Constraint *theMRMP= nullptr;
        int index= 0;
        while((theMRMP= theMRMPs()) != nullptr)
          {
            const int nodeConstrained= theMRMP->getNodeConstrained();
            numDOF++;
            constrainedNodesMRMP[index]= nodeConstrained;
            mrmfs[index]= theMRMP;
            index++;
          }
      }

    // create an ID of constrained node tags in SFreedom_Constraints
    IDVarSize constrainedNodesSP(0, numSPConstraints, -1); //defaultValue= -1 otherwise we get a fase positive for
                                                           //a node with tag 0. LCPT 22/01/2018.
    std::vector<SFreedom_Constraint *> sFreedomConstraints(numSPConstraints,static_cast<SFreedom_Constraint *>(nullptr));
    if(numSPConstraints != 0)
      {
        SFreedom_ConstraintIter &theSPs= theDomain->getConstraints().getDomainAndLoadPatternSPs();
        SFreedom_Constraint *theSP;
        int index= 0;
        while((theSP= theSPs()) != nullptr)
          {
            const int constrainedNodeTag= theSP->getNodeTag();
            numDOF++;
            constrainedNodesSP[index]= constrainedNodeTag;
            sFreedomConstraints[index]= theSP;
            index++;
          }
      }

    // create an array for the DOF_Groups and zero it
    if(numDOF <= 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << " DOF_Groups array of size " << numDOF << std::endl;
        return -3;
      }
    theDOFs= std::vector<DOF_Group *>(numDOF,static_cast<DOF_Group *>(nullptr));


    //create a DOF_Group for each node and add it to the analysis model.
    //    :must of course set the initial IDs
    NodeIter &theNod= theDomain->getNodes();
    Node *nodPtr= nullptr;

    int numDofGrp= 0;
    int count3= 0;
    int countDOF =0;

    numConstrainedNodes= 0;
    numDOF= 0;
    while((nodPtr= theNod()) != nullptr) //For each node.
      {

        DOF_Group *dofPtr= nullptr;

        const int nodeTag= nodPtr->getTag();
        const int numNodalDOF= nodPtr->getNumberDOF();
        int loc= -1;
        bool createdDOF= false;

	//Multi-freedom constraints.
        loc= constrainedNodesMP.getLocation(nodeTag);
        if(loc >= 0)
          {
            TransformationDOF_Group *tDofPtr= theModel->createTransformationDOF_Group(numDofGrp++, nodPtr, mfs[loc], this);
            createdDOF= true;
            dofPtr= tDofPtr;

            // add any SPs
            if(numSPConstraints != 0)
              {
                loc= constrainedNodesSP.getLocation(nodeTag);
                if(loc >= 0)
                  {
                    tDofPtr->addSFreedom_Constraint(*(sFreedomConstraints[loc]));
                    for(int i= loc+1; i<numSPConstraints; i++)
                      {
                        if(constrainedNodesSP(i) == nodeTag)
                          tDofPtr->addSFreedom_Constraint(*(sFreedomConstraints[i]));
                      }
                  }
                // add the DOF to the array
                theDOFs[numDOF++]= dofPtr;
                numConstrainedNodes++;
              }
          }

	//Multi retained node, multi-freedom constraints.
        if(!createdDOF)
          {
            loc= constrainedNodesMRMP.getLocation(nodeTag);
            if(loc >= 0)
              {
                TransformationDOF_Group *tDofPtr= theModel->createTransformationDOF_Group(numDofGrp++, nodPtr, mrmfs[loc], this);
                createdDOF= true;
                dofPtr= tDofPtr;
                // add any SPs
                if(numSPConstraints != 0)
                  {
                    loc= constrainedNodesSP.getLocation(nodeTag);
                    if(loc >= 0)
                      {
                        tDofPtr->addSFreedom_Constraint(*(sFreedomConstraints[loc]));
                        for(int i= loc+1; i<numSPConstraints; i++)
                          {
                            if(constrainedNodesSP(i) == nodeTag)
                              tDofPtr->addSFreedom_Constraint(*(sFreedomConstraints[i]));
                          }
                      }
                    // add the DOF to the array
                    theDOFs[numDOF++]= dofPtr;
                    numConstrainedNodes++;
                  }
              }
          }

	//Single freedom constraints.
        if(!createdDOF)
          {
            loc= constrainedNodesSP.getLocation(nodeTag);
            if(loc >= 0)
              {
                TransformationDOF_Group *tDofPtr= theModel->createTransformationDOF_Group(numDofGrp++, nodPtr, this);
                int numSPs= 1;
                createdDOF= true;
                dofPtr= tDofPtr;
                tDofPtr->addSFreedom_Constraint(*(sFreedomConstraints[loc]));

                // check for more SFreedom_constraints acting on node and add them
                for(int i= loc+1; i<numSPConstraints; i++)
                  {
                    if(constrainedNodesSP(i) == nodeTag)
                      {
                        tDofPtr->addSFreedom_Constraint(*(sFreedomConstraints[i]));
                        numSPs++;
                      }
                  }
                // add the DOF to the array
                theDOFs[numDOF++]= dofPtr;
                numConstrainedNodes++;
                countDOF+= numNodalDOF - numSPs;
              }
          }

        // create an ordinary DOF_Group object if no dof constrained
        if(!createdDOF)
          {
            dofPtr= theModel->createDOF_Group(numDofGrp++, nodPtr);
            countDOF+= numNodalDOF;
          }

        if(dofPtr == nullptr)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; error in logic.\n";

      }

    // create the FE_Elements for the elements and add them to the analysis model
    ElementIter &theEle= theDomain->getElements();
    Element *elePtr;
    FE_Element *fePtr= nullptr;

    int numFE= 0;
    std::set<int> transformedEle;
    while((elePtr= theEle()) != nullptr)
      {
        int flag= 0;
        if(elePtr->isSubdomain() == true)
          {
            Subdomain *theSub= (Subdomain *)elePtr;
            if(theSub->doesIndependentAnalysis() == true)
              flag= 1;
          }

        if(flag == 0)
          {
            const ID &nodes= elePtr->getNodePtrs().getExternalNodes();
            const size_t nodesSize= nodes.Size();
            int isConstrainedNode= 0;
            for(size_t i=0; i<nodesSize; i++)
              {
                const int nodeTag= nodes(i);
                if(numMPConstraints != 0)
                  {
                    int loc= constrainedNodesMP.getLocation(nodeTag);
                    if(loc >= 0)
                      {
                        isConstrainedNode= 1;
                        i= nodesSize;
                      }
                  }
                if(numMRMPConstraints != 0)
                  {
                    int loc= constrainedNodesMRMP.getLocation(nodeTag);
                    if(loc >= 0)
                      {
                        isConstrainedNode= 1;
                        i= nodesSize;
                      }
		  }
                if(numSPConstraints != 0 && isConstrainedNode == 0)
                  {
                    int loc= constrainedNodesSP.getLocation(nodeTag);
                    if(loc >= 0)
                      {
                        isConstrainedNode= 1;
                        i= nodesSize;
                      }
                  }
              }
            if(isConstrainedNode == 1)
              {
                numFE++;
                transformedEle.insert(elePtr->getTag());
	      }
          }
      }

    const size_t numberOfElements= theDomain->getNumElements();

    // create an array for the FE_elements and zero it
    if(numberOfElements>0 && numFE<=0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; FE_Element array of size " << numFE << std::endl;
        return -2;
      }

    ElementIter &theEle1= theDomain->getElements();

    // int numConstraints= numMRMPConstraints+numMPConstraints+numSPConstraints;
    int numFeEle= 0;

    while((elePtr= theEle1()) != 0)
      {
        fePtr= theModel->createTransformationFE(numFeEle, elePtr,transformedEle,theFEs);
        numFeEle++;
      }

    theModel->setNumEqn(countDOF);

    // set the number of equations in the model
    // now see if we have to set any of the dof's to -3
    //    int numLast= 0;
    if(nodesLast)
      for(int i=0; i<nodesLast->Size(); i++)
        {
          int nodeID= (*nodesLast)(i);
          Node *nodPtr= theDomain->getNode(nodeID);
          if(nodPtr != 0)
            {
              DOF_Group *dofPtr= nodPtr->getDOF_GroupPtr();
              const ID &id= dofPtr->getID();
              // set all the dof values to -3
              for(int j=0; j < id.Size(); j++)
                {
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
	}
    return count3;
  }

void XC::TransformationConstraintHandler::clearAll(void)
  {
    // delete the arrays
    theFEs.clear();
    theDOFs.clear();

    // reset the numbers
    numDOF= 0;
    ConstraintHandler::clearAll();
  }

int XC::TransformationConstraintHandler::sendSelf(Communicator &comm)
  { return 0; }

int XC::TransformationConstraintHandler::recvSelf(const Communicator &comm)
  { return 0; }


int XC::TransformationConstraintHandler::applyLoad(void)
  { return this->enforceSPs(); }

int XC::TransformationConstraintHandler::enforceSPs(void)
  {
    for(int i=1; i<=numConstrainedNodes; i++)
      {
        // upward cast - safe as i put it in this location
        TransformationDOF_Group *theDof= dynamic_cast<TransformationDOF_Group *>(theDOFs[numDOF-i]);
        theDof->enforceSPs();
      }
    for(std::set<FE_Element *>::iterator j=theFEs.begin(); j!=theFEs.end(); j++)
      (*j)->updateElement();
    return 0;
  }

int XC::TransformationConstraintHandler::doneNumberingDOF(void)
  {
    for(int i=1; i<=numConstrainedNodes; i++)
      {
        // upward cast - safe as i put it in this location
        TransformationDOF_Group *theDof= dynamic_cast<TransformationDOF_Group *>(theDOFs[numDOF-i]);
        theDof->doneID();
      }

    // iterate through the XC::FE_Element getting them to set their IDs
    AnalysisModel *theModel=this->getAnalysisModelPtr();
    FE_EleIter &theEle= theModel->getFEs();
    FE_Element *elePtr;
    while ((elePtr= theEle()) != nullptr)
      elePtr->setID();

    return 0;
  }
