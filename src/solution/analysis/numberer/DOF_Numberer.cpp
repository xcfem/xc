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

//! @brief Create the graph numberer (
void XC::DOF_Numberer::alloc(const std::string &str)
  {
    free_mem();
    if(str=="rcm")
      theGraphNumberer=new RCM(); //Reverse Cuthill-Macgee.
    else if(str=="simple")
      theGraphNumberer=new SimpleNumberer();
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; numerator type: '" << str
                << "' unknown." << std::endl;
  }

//! @brief Copy the graph numberer.
void XC::DOF_Numberer::copy(const GraphNumberer &gn)
  {
    free_mem();
    theGraphNumberer= gn.getCopy();
  }

//! @brief Constructor
void XC::DOF_Numberer::free_mem(void)
  {
    if(theGraphNumberer)
      delete theGraphNumberer;
    theGraphNumberer= nullptr;
  }

//! @brief Constructor
//!
//! @param owr: pointer to the ModelWrapper that ows this object.
//! @param clsTag: class indentifier. 
XC::DOF_Numberer::DOF_Numberer(ModelWrapper *owr, int clsTag) 
  :MovableObject(clsTag), EntCmd(owr), theGraphNumberer(nullptr) {}

//! @brief Copy constructor.
XC::DOF_Numberer::DOF_Numberer(const DOF_Numberer &other)
  : MovableObject(other), EntCmd(other), theGraphNumberer(nullptr)
  {
    if(other.theGraphNumberer)
      copy(*other.theGraphNumberer);
  }
  

XC::DOF_Numberer &XC::DOF_Numberer::operator=(const DOF_Numberer &other)
  {
    MovableObject::operator=(other);
    EntCmd::operator=(other);
    if(other.theGraphNumberer)
      copy(*other.theGraphNumberer);
    return *this;
  }

//! @brief Sets the algorithm to be used for numerating the graph
//! «Reverse Cuthill-Macgee» o simple.
void XC::DOF_Numberer::useAlgorithm(const std::string &nmb)
  { alloc(nmb); }

//! @brief Destructor
XC::DOF_Numberer::~DOF_Numberer(void) 
  { free_mem(); }

//! @brief Virtual constructor.
XC::DOF_Numberer *XC::DOF_Numberer::getCopy(void) const
  { return new DOF_Numberer(*this);  }

//! @brief Invoked to assign the equation numbers to the dofs.
//! 
//! Invoked to assign the equation numbers to the dofs in the DOF\_Groups
//! and the FE\_Elements, ensuring that the dof's in the DOF\_Group whose
//! tag is given by {\em lastDOF\_Group} are numbered last in a \f$-2\f$ or
//! \f$-3\f$ group. The initial values of these equation numbers have been set
//! by the ConstraintHandler object to be \f$-1\f$, \f$-2\f$ or \f$-3\f$, all dofs
//! with a \f$-3\f$ are to be assigned higher equation numbers than those
//! assigned a \f$-2\f$. To set the \p numEqn in the AnalysisModel and to
//! return the number of equations \p numEqn if successful, a negative
//! number if not.  
//
//! This base class performs the ordering by getting an ID containing the
//! ordered DOF\_Group tags, obtained by invoking {\em
//! number(theModel-\f$>\f$getDOFGroupGraph(), lastDOF\_Group)} on the
//! GraphNumberer, \p theGraphNumberer, passed in the constructor. The
//! base class then makes two passes through the DOF\_Group objects in the
//! AnalysisModel by looping through this ID; in the first pass assigning the
//! equation numbers incrementally to any degree-of-freedom marked with a
//! \f$-2\f$ and in the second pass assigning the equation numbers
//! incrementally to any degree-of-freedom marked with a \f$-3\f$. It then
//! iterates through the FE\_Elements in the AnalsisModel invoking {\em
//! setID()} on each object. Finally {\em setNumEqn(numEqn)} is invoked on
//! the AnalysisModel. Return \p numEqn if successful, a warning
//! message and a negative number is returned if an error occurs; \f$-1\f$ is
//! returned if setLinks() has not yet been invoked, \f$-2\f$ if no
//! GraphNumberer was passed in the constructor, \f$-3\f$ if the
//! number of {\em DOF\_Groups} in AnalysisModel and size of ID returned
//! are not the same, and a \f$-4\f$ if there is no DOF\_Group corresponding
//! to one of the tags given in the ID.
int XC::DOF_Numberer::numberDOF(int lastDOF_Group) 
  {
    // check if we have a model and a numberer
    Domain *theDomain= nullptr;
    AnalysisModel *am= getAnalysisModelPtr();
    if(am)
      theDomain= am->getDomainPtr();
    if(theDomain == nullptr)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; WARNING - "
		  << "Pointers are not set\n";
        return -1;
      }
    
    if(!theGraphNumberer)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; WARNING - "
		  << "subclasses must provide own implementation\n";
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
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; WARNING - "
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
            std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; WARNING - DOF_Group "
		      << dofGroupTag << "not in AnalysisModel!\n";
            result= -4;
          }
      }

    // iterate through  the DOFs second time setting -3 values
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
	        std::cerr << getClassName() << "::" << __FUNCTION__
		          << "; code loop through the MRMFreedom_Constraints."
			  << std::endl;
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


//! @brief Invoked to assign the equation numbers to the dofs.
//!
//! Invoked to assign the equation numbers to the dofs in the DOF\_Groups
//! and the FE\_Elements, ensuring that the dof's in the DOF\_Groups whose
//! tag is given in {\em lastDOF\_Groups} are numbered last in a \f$-2\f$ or
//! \f$-3\f$ group. The initial values of these equation numbers have been set
//! by the ConstraintHandler object to be \f$-1\f$, \f$-2\f$ or \f$-3\f$, all dofs
//! with a \f$-3\f$ are to be assigned higher equation numbers than those
//! assigned a \f$-2\f$. To set the \p numEqn in the AnalysisModel and to
//! return the number of equations \p numEqn if successful, a negative
//! number if not.  
//! 
//! This method in the base class is almost identical to the one just
//! described. The only difference is that the ID identifying the order of
//! the DOF\_Groups is obtained by invoking {\em
//! number(theModel-\f$>\f$getDOFGroupGraph(), lastDOF\_Groups)} on the
//! GraphNumberer.
int XC::DOF_Numberer::numberDOF(ID &lastDOFs) 
  {
    // check we have a model and a numberer
    Domain *theDomain= nullptr;
    AnalysisModel *am= getAnalysisModelPtr();

    if(am)
      theDomain= am->getDomainPtr();
    if(!theDomain)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; WARNING - pointers are not set\n";
        return -1;
      }
    
    if((theGraphNumberer == nullptr))
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; WARNING - subclasses must provide own implementation\n";
        return -2;
      }

    // check we cant do quick return
    if(am->getNumDOF_Groups() == 0)
      return 0;

    // we first number the dofs using the dof group graph
        
    const ID &orderedRefs= theGraphNumberer->number(am->getDOFGroupGraph(), lastDOFs);     

    // we now iterate through the DOFs first time setting -2 values

    int eqnNumber= 0;
    if(orderedRefs.Size() != am->getNumDOF_Groups())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; WARNING - incompatable Sizes\n";
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
            std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; WARNING - DOF_Group "
		      << dofGroupTag << "not in AnalysisModel!\n";
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
		std::cerr << getClassName() << "::" << __FUNCTION__
		          << "; code loop through the MRMFreedom_Constraints."
			  << std::endl;
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


//! @brief Send object members through the channel passed as parameter.
//!
//! The DOF\_Numberer sends the class identifier and database tag of the
//! GraphNumberer in a ID to the channel, if no GraphNumberer is
//! associated a \f$-1\f$ is sent as the class tag. The object then invokes
//! sendSelf() on the GraphNumberer. 
int XC::DOF_Numberer::sendData(CommParameters &cp)
  {
    //setDbTagDataPos(0,getTag());
    // XXX arreglar.
    int res= cp.sendMovablePtr(theGraphNumberer,getDbTagData(),PtrCommMetaData(1,2));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
//! 
//! The DOF\_Numberer receives the class identifier and database tag of
//! the GraphNumberer in an ID from the channel, if no GraphNumberer is
//! associated a \f$-1\f$ is received. The DOF\_Numberer will then ask {\em
//! theBroker} for a GraphNumberer with that class identifier, it sets the
//! database tag for the GraphNumberer and it then invokes {\em
//! recvSelf()} on that GraphNumberer.  
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
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::DOF_Numberer::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName()  << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
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

//! @brief Return a const pointer to the GraphNumberer object associated with
//! the DOF\_Numberer, \p theGraphNumberer.
const XC::GraphNumberer *XC::DOF_Numberer::getGraphNumbererPtr(void) const
  { return theGraphNumberer; }

//! @brief Return a pointer to the GraphNumberer object associated with
//! the DOF\_Numberer, \p theGraphNumberer.
XC::GraphNumberer *XC::DOF_Numberer::getGraphNumbererPtr(void)
  { return theGraphNumberer; }
