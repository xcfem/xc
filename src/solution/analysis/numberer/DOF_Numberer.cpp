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
// $Date: 2005/11/28 21:28:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/numberer/DOF_Numberer.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Created: 9/96
// Revision: A
//
// Description: This file contains the class implementation for XC::DOF_Numberer.
// DOF_Numberer is an abstract base class, i.e. no objects of it's
// type can be created. 
//
// What: "@(#) DOF_Numberer.h, revA"

#include <solution/analysis/numberer/DOF_Numberer.h>
#include "solution/analysis/ModelWrapper.h"
#include <solution/analysis/model/AnalysisModel.h>
#include "solution/graph/numberer/GraphNumberer.h"
#include "solution/graph/numberer/RCM.h"
#include "solution/graph/numberer/SimpleNumberer.h"
#include <utility/matrix/ID.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/analysis/model/FE_EleIter.h>

#include "solution/graph/graph/Graph.h"

#include <domain/domain/Domain.h>
#include <domain/constraints/MFreedom_Constraint.h>
#include <domain/constraints/MRMFreedom_Constraint.h>
#include <domain/mesh/node/Node.h>
#include <domain/constraints/MFreedom_ConstraintIter.h>
#include <domain/constraints/MRMFreedom_ConstraintIter.h>
#include <solution/analysis/model/DOF_GrpIter.h>

//! @brief Crea el numerador de grafos que se le solicita. 
void XC::DOF_Numberer::alloc(const std::string &str)
  {
    libera();
    if(str=="rcm")
      theGraphNumberer=new RCM(); //Reverse Cuthill-Macgee.
    else if(str=="simple")
      theGraphNumberer=new SimpleNumberer();
    else
      std::cerr << "DOF_Numberer::alloc, numerator type: '" << str
                << "' unknown." << std::endl;
  }

//! @brief Copia el numerador de grafos.
void XC::DOF_Numberer::copia(const GraphNumberer &gn)
  {
    libera();
    theGraphNumberer= gn.getCopy();
  }

//! @brief Constructor
void XC::DOF_Numberer::libera(void)
  {
    if(theGraphNumberer)
      delete theGraphNumberer;
    theGraphNumberer= nullptr;
  }

//! @brief Constructor
XC::DOF_Numberer::DOF_Numberer(ModelWrapper *owr, int clsTag) 
  :MovableObject(clsTag), EntCmd(owr), theGraphNumberer(nullptr) {}

XC::DOF_Numberer::DOF_Numberer(ModelWrapper *owr)
  :MovableObject(NUMBERER_TAG_DOF_Numberer), EntCmd(owr), theGraphNumberer(nullptr) {}    

XC::DOF_Numberer::DOF_Numberer(const DOF_Numberer &otro)
  : MovableObject(otro), EntCmd(otro), theGraphNumberer(nullptr)
  {
    if(otro.theGraphNumberer)
      copia(*otro.theGraphNumberer);
  }
  

XC::DOF_Numberer &XC::DOF_Numberer::operator=(const DOF_Numberer &otro)
  {
    MovableObject::operator=(otro);
    EntCmd::operator=(otro);
    if(otro.theGraphNumberer)
      copia(*otro.theGraphNumberer);
    return *this;
  }

//! @brief Sets the algoritmo que se usará para numerar el grafo
//! «Reverse Cuthill-Macgee» o simple.
void XC::DOF_Numberer::useAlgorithm(const std::string &nmb)
  { alloc(nmb); }

//! @brief Destructor
XC::DOF_Numberer::~DOF_Numberer(void) 
  { libera(); }

//! @brief Constructor virtual.
XC::DOF_Numberer *XC::DOF_Numberer::getCopy(void) const
  { return new DOF_Numberer(*this);  }

int XC::DOF_Numberer::numberDOF(int lastDOF_Group) 
  {
    // check we have a model and a numberer
    Domain *theDomain= nullptr;
    AnalysisModel *am= getAnalysisModelPtr();
    if(am)
      theDomain= am->getDomainPtr();
    if(theDomain == 0)
      {
        std::cerr << "WARNING DOF_Numberer::numberDOF - ";
        std::cerr << "Pointers are not set\n";
        return -1;
      }
    
    if(!theGraphNumberer)
      {
        std::cerr << "WARNING DOF_Numberer::numberDOF - ";
        std::cerr << "subclasses must provide own implementation\n";
        return -2;
      }

    // check we cant do quick return
    if(am->getNumDOF_Groups() == 0)
      return 0;

    // we first number the dofs using the dof group graph
    const ID &orderedRefs= theGraphNumberer->number(am->getDOFGroupGraph(), lastDOF_Group);

    // we now iterate through the DOFs first time setting -2 values  
    if(orderedRefs.Size() != am->getNumDOF_Groups())
      {
        std::cerr << "WARNING DOF_Numberer::numberDOF - "
                  << "incompatible sizes; orderedRefs "
                  << orderedRefs.Size() << " numDOF_Groups "
                  << am->getNumDOF_Groups() << std::endl;
        return -3;
      }
    int result= 0;

    int eqnNumber= 0;
    int size= orderedRefs.Size();
    for(int i=0; i<size; i++)
      {
        const int dofGroupTag= orderedRefs(i);
        DOF_Group *dofGroupPtr= am->getDOF_GroupPtr(dofGroupTag);
        if(dofGroupPtr)
          {
            const ID &theID= dofGroupPtr->getID();
            const int idSize= theID.Size();
            for(int j=0; j<idSize; j++)
              if(theID(j) == -2) dofGroupPtr->setID(j,eqnNumber++);
          }
        else
          {
            std::cerr << "WARNING DOF_Numberer::numberDOF - ";
            std::cerr << "DOF_Group " << dofGroupTag << "not in AnalysisModel!\n";
            result= -4;
          }
      }

    // iterate throgh  the DOFs second time setting -3 values
    for(int k=0; k<size; k++)
      {
        const int dofGroupTag= orderedRefs(k);
        DOF_Group *dofGroupPtr= am->getDOF_GroupPtr(dofGroupTag);
        if(dofGroupPtr)
          {
            const ID &theID= dofGroupPtr->getID();
            int idSize= theID.Size();
            for(int j=0; j<idSize; j++)
                if(theID(j) == -3) dofGroupPtr->setID(j,eqnNumber++);
          }
      }

    // iterate through the DOFs one last time setting any -4 values
    // iterate throgh the DOFs second time setting -3 values
    DOF_GrpIter &tDOFs= am->getDOFGroups();
    DOF_Group *dofGroupPtr= nullptr;
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
            MFreedom_Constraint *mpPtr;
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
	        std::cerr << "DOF_Numberer::numberDOF(int) code loop through the MRMFreedom_Constraints." << std::endl;
              }
          }        
      }

    const int numEqn= eqnNumber;

    // iterate through the FE_Element getting them to set their IDs
    FE_EleIter &theEle= am->getFEs();
    FE_Element *elePtr= nullptr;
    while((elePtr= theEle()) != 0)
      elePtr->setID();


    // set the numOfEquation in the Model
    am->setNumEqn(numEqn);


    if(result == 0)
      return numEqn;
    return result;
  }


int XC::DOF_Numberer::numberDOF(ID &lastDOFs) 
  {
    // check we have a model and a numberer
    Domain *theDomain= nullptr;
    AnalysisModel *am= getAnalysisModelPtr();

    if(am)
      theDomain= am->getDomainPtr();
    if(!theDomain)
      {
        std::cerr << "WARNING XC::DOF_Numberer::numberDOF - ";
        std::cerr << "Pointers are not set\n";
        return -1;
      }
    
    if((theGraphNumberer == 0))
      {
        std::cerr << "WARNING XC::DOF_Numberer::numberDOF - ";
        std::cerr << "subclasses must provide own implementation\n";
        return -2;
      }

    // check we cant do XC::quick return
    if(am->getNumDOF_Groups() == 0)
      return 0;

    // we first number the dofs using the dof group graph
        
    const ID &orderedRefs= theGraphNumberer->number(am->getDOFGroupGraph(), lastDOFs);     

    // we now iterate through the DOFs first time setting -2 values

    int eqnNumber= 0;
    if(orderedRefs.Size() != am->getNumDOF_Groups())
      {
        std::cerr << "WARNING XC::DOF_Numberer::numberDOF - ";
        std::cerr << "Incompatable Sizes\n";
        return -3;
      }

    int result =0;
    int size= orderedRefs.Size();
    for(int i=0; i<size; i++)
      {
        const int dofGroupTag= orderedRefs(i);
        DOF_Group *dofGroupPtr= am->getDOF_GroupPtr(dofGroupTag);
        if(!dofGroupPtr)
          {
            std::cerr << "WARNING XC::DOF_Numberer::numberDOF - ";
            std::cerr << "DOF_Group " << dofGroupTag << "not in XC::AnalysisModel!\n";
            result= -4;
          } 
        else
         {
            const ID &theID= dofGroupPtr->getID();
            const int idSize= theID.Size();
            for(int j=0; j<idSize; j++)
              if(theID(j) == -2) dofGroupPtr->setID(j,eqnNumber++);
         }        
      }

    // iterate throgh  the DOFs first time setting -3 values
    
    for(int k=0; k<size; k++)
      {
        const int dofGroupTag= orderedRefs(k);
        DOF_Group *dofGroupPtr= am->getDOF_GroupPtr(dofGroupTag);
        if(dofGroupPtr)
          {
            const ID &theID= dofGroupPtr->getID();
            const int idSize= theID.Size();
            for(int j=0; j<idSize; j++)
                if(theID(j) == -3) dofGroupPtr->setID(j,eqnNumber++);
          }
      }

    // iterate through the DOFs one last time setting any -4 values
    // iterate throgh  the DOFs second time setting -3 values
    DOF_GrpIter &tDOFs= am->getDOFGroups();
    DOF_Group *dofGroupPtr= nullptr;
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
            MFreedom_Constraint *mpPtr;
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
                 std::cerr << "DOF_Numberer::numberDOF(ID) code loop through the MRMFreedom_Constraints." << std::endl;
              }
	  }
      }

    int numEqn= eqnNumber;

    // iterate through the FE_Element getting them to set their IDs
    FE_EleIter &theEle= am->getFEs();
    FE_Element *elePtr;
    while((elePtr= theEle()) != 0)
        elePtr->setID();

    // set the numOfEquation in the Model
    am->setNumEqn(numEqn);
    
    if(result == 0)
        return numEqn;
    
    return result;

}


//! @brief Send members del objeto through the channel being passed as parameter.
int XC::DOF_Numberer::sendData(CommParameters &cp)
  {
    //setDbTagDataPos(0,getTag());
    // XXX arreglar.
    int res= cp.sendMovablePtr(theGraphNumberer,getDbTagData(),PtrCommMetaData(1,2));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::DOF_Numberer::recvData(const CommParameters &cp)
  {
    //setTag(getDbTagDataPos(0));
    // XXX arreglar.
    //theGraphNumberer= cp.receiveMovablePtr(theGraphNumberer,getDbTagData(),PtrCommMetaData(1,2));
    return 0;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::DOF_Numberer::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::DOF_Numberer::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

//! @brief Returns a pointer to the model wrapper that owns this object.
XC::ModelWrapper *XC::DOF_Numberer::getModelWrapper(void)
  { return dynamic_cast<ModelWrapper *>(Owner()); }

//! @brief Returns a pointer to the model wrapper that owns this object.
const XC::ModelWrapper *XC::DOF_Numberer::getModelWrapper(void) const
  { return dynamic_cast<const ModelWrapper *>(Owner()); }

//! @brief Returns a pointer to the analysis model.
const XC::AnalysisModel *XC::DOF_Numberer::getAnalysisModelPtr(void) const
  {
    const ModelWrapper *sm= getModelWrapper();
    assert(sm);
    return sm->getAnalysisModelPtr();
  }

//! @brief Returns a pointer to the analysis model.
XC::AnalysisModel *XC::DOF_Numberer::getAnalysisModelPtr(void)
  {
    ModelWrapper *sm= getModelWrapper();
    assert(sm);
    return sm->getAnalysisModelPtr();
  }

const XC::GraphNumberer *XC::DOF_Numberer::getGraphNumbererPtr(void) const
  { return theGraphNumberer; }

XC::GraphNumberer *XC::DOF_Numberer::getGraphNumbererPtr(void)
  { return theGraphNumberer; }
