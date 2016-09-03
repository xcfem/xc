//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo
//  bajo los términos de la Licencia Pública General GNU publicada
//  por la Fundación para el Software Libre, ya sea la versión 3
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO.
//  Consulte los detalles de la Licencia Pública General GNU para obtener
//  una información más detallada.
//
// Debería haber recibido una copia de la Licencia Pública General GNU
// junto a este programa.
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
#include <domain/constraints/SP_ConstraintIter.h>
#include <domain/constraints/SP_Constraint.h>
#include <domain/constraints/MP_ConstraintIter.h>
#include <domain/constraints/MP_Constraint.h>
#include <domain/constraints/MRMP_ConstraintIter.h>
#include <domain/constraints/MRMP_Constraint.h>
#include <solution/analysis/integrator/Integrator.h>
#include <utility/matrix/ID.h>
#include "domain/domain/subdomain/Subdomain.h"
#include <solution/analysis/model/dof_grp/TransformationDOF_Group.h>
#include <solution/analysis/model/fe_ele/transformation/TransformationFE.h>
#include "domain/mesh/element/NodePtrsWithIDs.h"


//! @brief Constructor.
XC::TransformationConstraintHandler::TransformationConstraintHandler(ModelWrapper *owr)
  :ConstraintHandler(owr,HANDLER_TAG_TransformationConstraintHandler),
 theFEs(), theDOFs(),numDOF(0),numConstrainedNodes(0) {}


//! @brief Constructor virtual.
XC::ConstraintHandler *XC::TransformationConstraintHandler::getCopy(void) const
  { return new TransformationConstraintHandler(*this); }

int XC::TransformationConstraintHandler::handle(const ID *nodesLast)
  {
    // first check links exist to a Domain and an AnalysisModel object
    Domain *theDomain= this->getDomainPtr();
    if(!theDomain)
      std::cerr << "WARNING TransformationConstraintHandler::handle() - "
                << " puntero a dominio nulo." << std::endl;
    AnalysisModel *theModel= this->getAnalysisModelPtr();
    if(!theModel)
      std::cerr << "WARNING TransformationConstraintHandler::handle() - "
                << " puntero a modelo de análisis nulo." << std::endl;
    Integrator *theIntegrator= this->getIntegratorPtr();
    if(!theIntegrator)
      std::cerr << "WARNING TransformationConstraintHandler::handle() - "
                << " puntero a integrador nulo." << std::endl;

    // get number of elements and nodes in the domain
    // and init the theFEs and theDOFs arrays
    int numMPConstraints= theDomain->getConstraints().getNumMPs();

    //    int numSPConstraints= theDomain->getConstraints().getNumSPs();
    int numSPConstraints= 0;
    SP_ConstraintIter &theSP1s= theDomain->getConstraints().getDomainAndLoadPatternSPs();
    SP_Constraint *theSP1;
    while((theSP1= theSP1s()) != 0)
        numSPConstraints++;


    numDOF= 0;

    int i= -1;

    // create an ID of constrained node tags in MP_Constraints
    ID constrainedNodesMP(0, numMPConstraints);
    std::vector<MP_Constraint *> mps(numMPConstraints,static_cast<MP_Constraint *>(nullptr));
    if(numMPConstraints != 0)
      {
        MP_ConstraintIter &theMPs= theDomain->getConstraints().getMPs();
        MP_Constraint *theMP= nullptr;
        int index= 0;
        while((theMP= theMPs()) != 0)
          {
            int nodeConstrained= theMP->getNodeConstrained();
            numDOF++;
            constrainedNodesMP[index]= nodeConstrained;
            mps[index]= theMP;
            index++;
          }
      }

    int numMRMPConstraints= theDomain->getConstraints().getNumMRMPs();
    // create an ID of constrained node tags in MRMP_Constraints
    ID constrainedNodesMRMP(0, numMRMPConstraints);
    std::vector<MRMP_Constraint *> mrmps(numMRMPConstraints,static_cast<MRMP_Constraint *>(nullptr));
    if(numMRMPConstraints != 0)
      {
        MRMP_ConstraintIter &theMRMPs= theDomain->getConstraints().getMRMPs();
        MRMP_Constraint *theMRMP= nullptr;
        int index= 0;
        while((theMRMP= theMRMPs()) != 0)
          {
            int nodeConstrained= theMRMP->getNodeConstrained();
            numDOF++;
            constrainedNodesMRMP[index]= nodeConstrained;
            mrmps[index]= theMRMP;
            index++;
          }
      }

    // create an ID of constrained node tags in SP_Constraints
    ID constrainedNodesSP(0, numSPConstraints);
    std::vector<SP_Constraint *> sps(numSPConstraints,static_cast<SP_Constraint *>(nullptr));
    if(numSPConstraints != 0)
      {
        SP_ConstraintIter &theSPs= theDomain->getConstraints().getDomainAndLoadPatternSPs();
        SP_Constraint *theSP;
        int index= 0;
        while((theSP= theSPs()) != 0)
          {
            int constrainedNode= theSP->getNodeTag();
            numDOF++;
            constrainedNodesSP[index]= constrainedNode;
            sps[index]= theSP;
            index++;
          }
      }

    // create an array for the DOF_Groups and zero it
    if(numDOF <= 0)
      {
        std::cerr << " DOF_Groups array of size " << numDOF << std::endl;
        return -3;
      }
    theDOFs= std::vector<DOF_Group *>(numDOF,static_cast<DOF_Group *>(nullptr));


    //create a DOF_Group for each XC::Node and add it to the XC::AnalysisModel.
    //    :must of course set the initial IDs
    NodeIter &theNod= theDomain->getNodes();
    Node *nodPtr;

    int numDofGrp= 0;
    int count3= 0;
    int countDOF =0;

    numConstrainedNodes= 0;
    numDOF= 0;
    while((nodPtr= theNod()) != 0)
      {

        DOF_Group *dofPtr= 0;

        const int nodeTag= nodPtr->getTag();
        int numNodalDOF= nodPtr->getNumberDOF();
        int loc= -1;
        int createdDOF= 0;

        loc= constrainedNodesMP.getLocation(nodeTag);
        if(loc >= 0)
          {
            TransformationDOF_Group *tDofPtr= theModel->createTransformationDOF_Group(numDofGrp++, nodPtr, mps[loc], this);
            createdDOF= 1;
            dofPtr= tDofPtr;

            // add any SPs
            if(numSPConstraints != 0)
              {
                loc= constrainedNodesSP.getLocation(nodeTag);
                if(loc >= 0)
                  {
                    tDofPtr->addSP_Constraint(*(sps[loc]));
                    for(int i= loc+1; i<numSPConstraints; i++)
                      {
                        if(constrainedNodesSP(i) == nodeTag)
                          tDofPtr->addSP_Constraint(*(sps[i]));
                      }
                  }
                // add the DOF to the array
                theDOFs[numDOF++]= dofPtr;
                numConstrainedNodes++;
              }
          }

        if(createdDOF == 0)
          {
            loc= constrainedNodesSP.getLocation(nodeTag);
            if(loc >= 0)
              {
                TransformationDOF_Group *tDofPtr= theModel->createTransformationDOF_Group(numDofGrp++, nodPtr, this);
                int numSPs= 1;
                createdDOF= 1;
                dofPtr= tDofPtr;
                tDofPtr->addSP_Constraint(*(sps[loc]));

                // check for more SP_constraints acting on node and add them
                for(int i= loc+1; i<numSPConstraints; i++)
                  {
                    if(constrainedNodesSP(i) == nodeTag)
                      {
                        tDofPtr->addSP_Constraint(*(sps[i]));
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
        if(createdDOF == 0)
          {
            dofPtr= theModel->createDOF_Group(numDofGrp++, nodPtr);
            countDOF+= numNodalDOF;
          }

        if(dofPtr == 0)
          std::cerr << "TransformationConstraintHandler::handle() - error in logic\n";

      }

    // create the FE_Elements for the Elements and add to the XC::AnalysisModel
    ElementIter &theEle= theDomain->getElements();
    Element *elePtr;
    FE_Element *fePtr= nullptr;

    int numFE= 0;
    std::set<int> transformedEle;

    while((elePtr= theEle()) != 0)
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
            int nodesSize= nodes.Size();
            int isConstrainedNode= 0;
            for(int i=0; i<nodesSize; i++)
              {
                int nodeTag= nodes(i);
                if(numMPConstraints != 0)
                  {
                    int loc= constrainedNodesMP.getLocation(nodeTag);
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


    // create an array for the FE_elements and zero it
    if(numFE<=0)
      {
        std::cerr << " FE_Element array of size " << numFE << std::endl;
        return -2;
      }

    ElementIter &theEle1= theDomain->getElements();

    // int numConstraints= numMPConstraints+numSPConstraints;
    int numFeEle= 0;

    while((elePtr= theEle1()) != 0)
      {
        fePtr= theModel->createTransformationFE(numFeEle, elePtr,transformedEle,theFEs);
        numFeEle++;
      }
    theModel->setNumEqn(countDOF);

    // set the number of eqn in the model
    // now see if we have to set any of the dof's to -3
    //    int numLast= 0;
    if(nodesLast != 0)
      for(i=0; i<nodesLast->Size(); i++)
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
                    {
                      std::cerr << "WARNING TransformationConstraintHandler::handle() ";
                      std::cerr << " - boundary sp constraint in subdomain";
                      std::cerr << " this should not be - results suspect \n";
                    }
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

int XC::TransformationConstraintHandler::sendSelf(CommParameters &cp)
  { return 0; }

int XC::TransformationConstraintHandler::recvSelf(const CommParameters &cp)
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
    while ((elePtr= theEle()) != 0)
      elePtr->setID();

    return 0;
}
