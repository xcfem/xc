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
**    OpenSeess - Open System for Earthquake Engineering Simulation    **
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
// $Date: 2005/11/30 23:47:00 $
// $Source: domain/domain/subdomain/ActorSubdomain.cpp,v $
                                                                        
                                                                        

#include "ActorSubdomain.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"
#include "domain/constraints/SFreedom_Constraint.h"
#include "domain/constraints/MFreedom_Constraint.h"
#include "domain/load/ElementalLoad.h"
#include "domain/load/NodalLoad.h"
#include "domain/load/pattern/LoadPattern.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/Vector.h"
#include "solution/analysis/analysis/DomainDecompositionAnalysis.h"
#include "domain/domain/subdomain/modelbuilder/PartitionedModelBuilder.h"
#include "solution/analysis/convergenceTest/ConvergenceTest.h"

#include "solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h"
#include "solution/analysis/integrator/IncrementalIntegrator.h"
#include "solution/system_of_eqn/linearSOE/LinearSOE.h"
#include "solution/system_of_eqn/linearSOE/LinearSOESolver.h"
#include "utility/recorder/Recorder.h"

#include "domain/domain/subdomain/ShadowActorSubdomain.h"

XC::ActorSubdomain::ActorSubdomain(Channel &theCh,FEM_ObjectBroker &theBroker,DataOutputHandler::map_output_handlers *oh,CommandEntity *owr)
  :Subdomain(0,oh,owr), Actor(theCh,theBroker,0), msgData(4)
  {}
    

int XC::ActorSubdomain::run(void)
  {
    Vector theVect(4);
    bool exitYet = false;

    while(exitYet == false)
      {
  	this->recvID(msgData);
	int action = msgData(0);

	int theType, theOtherType, tag, dbTag, loadPatternTag;
	Element *theEle= nullptr;
	Node *theNod= nullptr;
	MFreedom_Constraint *theMP= nullptr;
	SFreedom_Constraint *theSP= nullptr;
	LoadPattern *theLoadPattern= nullptr;
	NodalLoad *theNodalLoad= nullptr;
	ElementalLoad *theElementalLoad= nullptr;
	DomainDecompositionAnalysis *theDDAnalysis= nullptr;
	const Matrix *theMatrix= nullptr;
	const Vector *theVector= nullptr;
	Matrix *theMtx= nullptr;
	Vector *theVct= nullptr;
	PartitionedModelBuilder *theBuilder= nullptr;
	IncrementalIntegrator *theIntegrator= nullptr;
	EquiSolnAlgo *theAlgorithm= nullptr;
	LinearSOE *theSOE= nullptr;
	LinearSOESolver *theSolver= nullptr;
	ConvergenceTest *theTest= nullptr;
	Recorder *theRecorder= nullptr;
	bool res= false;
	double doubleRes= 0.0;
	int intRes;


	const ID *theID;
	
	switch (action)
	  {
	  case ShadowActorSubdomain_setTag:
	    tag = msgData(1); // subdomain tag
	    this->setTag(tag);
	    this->Actor::setCommitTag(tag);
	    break;

	  case ShadowActorSubdomain_newStep:
	    this->recvVector(theVect);
	    this->newStep(theVect(0));
	    break;

	  case ShadowActorSubdomain_buildSubdomain:
	    theType = msgData(1);
	    tag = msgData(3); // subdomain tag
	    this->setTag(tag);
	    tag = msgData(2); // numSubdomains
	    theBuilder = theBroker->getPtrNewPartitionedModelBuilder(*this, theType);
	    this->recvObject(*theBuilder);
	    this->buildSubdomain(tag, *theBuilder);
	    break;

	case ShadowActorSubdomain_getRemoteData:
	    theID = &(this->getExternalNodes());
	    msgData(0) = theID->Size();
	    msgData(1) = this->getNumDOF();
	    this->sendID(msgData);
	    if(theID->Size() != 0)
	      this->sendID(*theID);
	    break;

	  case ShadowActorSubdomain_getCost:
       	    theVect(0) = this->getCost(); // have to use [] for Sun's CC!
	    this->sendVector(theVect);
	    break;	    

 	  case ShadowActorSubdomain_addElement:
	    theType = msgData(1);
	    dbTag = msgData(2);
	    theEle = theBroker->getNewElement(theType);
	    if(theEle != 0)
	      {
		theEle->setDbTag(dbTag);		
		this->recvObject(*theEle);
		bool result = this->addElement(theEle);
		if(result == true)
		    msgData(0) = 0;
		else
		    msgData(0) = -1;
	      }
	    else
	      msgData(0) = -1;

	    /*
	    this->recvID(msgData);	    
	    std::cerr << "XC::ActorSubdomain::addElement() : " << msgData;
	    
	    msgData(0) = 1;
	    msgData(1) = 2;
	    msgData(2) = 3;
	    msgData(3) = 4;
	    this->sendID(msgData);	    
	    */

	    break;
	    
	  case ShadowActorSubdomain_hasNode:
	    theType = msgData(1);
	    res = this->hasNode(theType);
	    if(res == true)
	      msgData(0) = 0;
	    else
	      msgData(0) = -1;
	    this->sendID(msgData);
	    break;

	  case ShadowActorSubdomain_hasElement:
	    theType = msgData(1);
	    res = this->hasElement(theType);
	    if(res == true)
	      msgData(0) = 0;
	    else
	      msgData(0) = -1;
	    this->sendID(msgData);
             break;


	  case ShadowActorSubdomain_addNode:
	    theType = msgData(1);
	    dbTag = msgData(2);
	    theNod = theBroker->getNewNode(theType);
	    if(theNod != 0)
	      {
		theNod->setDbTag(dbTag);		
		this->recvObject(*theNod); 
		bool result = this->addNode(theNod);
		if(result == true)
		  msgData(0) = 0;
		else
		  msgData(0) = -1;
	      }
	    else
	      msgData(0) = -1;
	    //	    std::cerr << "XC::ActorSubdomain::add node: " << *theNod;
	    break;

	  case ShadowActorSubdomain_addExternalNode:
	    theType = msgData(1);
	    dbTag = msgData(2);
	    theNod = theBroker->getNewNode(theType);

	    if(theNod != 0)
	      {
		theNod->setDbTag(dbTag);
		this->recvObject(*theNod);
		bool result = this->XC::Subdomain::addExternalNode(theNod);
		delete theNod;
		/*
		Node *dummy = new Node(*theNod);
		delete theNod;
		std::cerr << *dummy;
		std::cerr << dummy->getMass();
		*/

		if(result == true)
		    msgData(0) = 0;
		else
		    msgData(0) = -1;
	      }
	    else
	      msgData(0) = -1;
	    break;	    

	  case ShadowActorSubdomain_addSFreedom_Constraint:
	    theType = msgData(1);
	    dbTag = msgData(2);	    
	    theSP= theBroker->getNewSP(theType);
	    if(theSP)
              {
		theSP->setDbTag(dbTag);
		this->recvObject(*theSP);
		bool result = this->addSFreedom_Constraint(theSP);
		if(result == true)
		    msgData(0) = 0;
		else
		    msgData(0) = -1;
	      }
            else
	      msgData(0) = -1;
	    break;	    
	    
	  case ShadowActorSubdomain_addMFreedom_Constraint:
	    theType = msgData(1);
	    dbTag = msgData(2);
	    theMP = theBroker->getNewMP(theType);

	    if(theMP != 0) {
		theMP->setDbTag(dbTag);
		this->recvObject(*theMP);
		bool result = this->addMFreedom_Constraint(theMP);
		if(result == true)
		    msgData(0) = 0;
		else
		    msgData(0) = -1;
	    } else
		msgData(0) = -1;

	    break;	    
	    
	    
	  case ShadowActorSubdomain_addLoadPattern:
	    theType = msgData(1);
	    dbTag = msgData(2);
	    
	    theLoadPattern = theBroker->getNewLoadPattern(theType);

	    if(theLoadPattern != 0) {
		theLoadPattern->setDbTag(dbTag);
		this->recvObject(*theLoadPattern);
		bool result = this->addLoadPattern(theLoadPattern);
		if(result == true)
		    msgData(0) = 0;
		else
		    msgData(0) = -1;
	    } else
		msgData(0) = -1;

	    break;	    	    

	  case ShadowActorSubdomain_addNodalLoadToPattern:
 	    theType = msgData(1);
	    dbTag = msgData(2);
	    loadPatternTag = msgData(3);
	    
	    theNodalLoad = theBroker->getNewNodalLoad(theType);

	    if(theNodalLoad != 0) {
		theNodalLoad->setDbTag(dbTag);
		this->recvObject(*theNodalLoad);
		bool result = this->addNodalLoad(theNodalLoad, loadPatternTag);
		if(result == true)
		    msgData(0) = 0;
		else
		    msgData(0) = -1;
	    } else
		msgData(0) = -1;

	    break;	    
	    
	    
	  case ShadowActorSubdomain_addElementalLoadToPattern:
	    theType = msgData(1);
	    dbTag = msgData(2);
	    loadPatternTag = msgData(3);
	    
	    theElementalLoad = theBroker->getNewElementalLoad(theType);

	    if(theElementalLoad != 0) {
		theElementalLoad->setDbTag(dbTag);
		this->recvObject(*theElementalLoad);
		bool result = this->addElementalLoad(theElementalLoad, 
						     loadPatternTag);
		if(result == true)
		    msgData(0) = 0;
		else
		    msgData(0) = -1;
	    } else
		msgData(0) = -1;

	    break;	    	    
	    
	  case ShadowActorSubdomain_addSFreedom_ConstraintToPattern:
	    theType = msgData(1);
	    dbTag = msgData(2);
	    loadPatternTag = msgData(3);
	    theSP = theBroker->getNewSP(theType);
	    if(theSP)
              {
		theSP->setDbTag(dbTag);
		this->recvObject(*theSP);
		bool result = this->addSFreedom_Constraint(theSP, loadPatternTag);

		if(result == true)
		    msgData(0) = 0;
		else
		    msgData(0) = -1;
	      }
            else
	      msgData(0) = -1;
	    break;	    	    	    

	  case ShadowActorSubdomain_removeElement:
	    tag = msgData(1);

	    this->removeElement(tag);

	    if(theEle != 0) 
		msgData(0) = theEle->getClassTag();
	    else
		msgData(0) = -1;

	    this->sendID(msgData);
	    if(theEle != 0) {
		this->sendObject(*theEle);
		delete theEle;
	    }

	    msgData(0) = 0;

	    break;	    	    	    


	  case ShadowActorSubdomain_removeNode:
	    tag = msgData(1);

	    this->removeNode(tag);

	    if(theNod != 0) 
		msgData(0) = theNod->getClassTag();
	    else
		msgData(0) = -1;

	    this->sendID(msgData);
	    if(theNod != 0) {
		this->sendObject(*theNod);
		delete theNod;
	    }

	    msgData(0) = 0;

	    break;

	  case ShadowActorSubdomain_removeSFreedom_Constraint:
	    tag = msgData(1);

	    this->removeSFreedom_Constraint(tag);

	    break;	    
	    
	  case ShadowActorSubdomain_removeMFreedom_Constraint:
	    tag = msgData(1);

	    this->removeMFreedom_Constraint(tag);

	    break;	    	    

	  case ShadowActorSubdomain_removeLoadPattern:
	    tag = msgData(1);

	    this->removeLoadPattern(tag);

	    break;	    	    
	    
	  case ShadowActorSubdomain_removeNodalLoadFromPattern:
	    tag = msgData(1);
	    theType = msgData(2);

	    removeNodalLoad(tag, theType);

	    break;	    	    	    

	  case ShadowActorSubdomain_removeElementalLoadFromPattern:
	    tag = msgData(1);
	    theType = msgData(2);

	    removeElementalLoad(tag, theType);

	    break;
   	    	    
	  case ShadowActorSubdomain_removeSFreedom_ConstraintFromPattern:
	    tag = msgData(1);
	    theType = msgData(2);

	    this->removeSFreedom_Constraint(tag, theType);

	    break;	    	    	    
	    
	    
	    
	  case ShadowActorSubdomain_getElement:
	    tag = msgData(1);

	    theEle = this->getElement(tag);

	    if(theEle != 0) 
		msgData(0) = theEle->getClassTag();
	    else
		msgData(0) = -1;

	    this->sendID(msgData);
	    if(theEle != 0) {
		this->sendObject(*theEle);
	    }

	    msgData(0) = 0;

	    break;	    	    	    	    


	  case ShadowActorSubdomain_Print:
	    this->Print(std::cerr);
	    this->sendID(msgData);

	    break;	    	    	    	    

	  case ShadowActorSubdomain_applyLoad:
	    this->recvVector(theVect);	    
	    this->applyLoad(theVect(0));
	    break;

	  case ShadowActorSubdomain_setCommittedTime:
	    this->recvVector(theVect);	    
	    this->setCommittedTime(theVect(0));
	    this->setCurrentTime(theVect(0));
	    break;	    
	    
	  case ShadowActorSubdomain_setLoadConstant:
	    this->setLoadConstant();
	    break;	    
	    
	  case ShadowActorSubdomain_update:
	    this->update();
	    break;

	  case ShadowActorSubdomain_updateTimeDt:
	    this->updateTimeDt();
	    break;

	  case ShadowActorSubdomain_computeNodalResponse:
	    tag = msgData(1);
	    lastResponse= Vector(tag);
	    this->recvVector(lastResponse);
	    this->computeNodalResponse();


	    
	  case ShadowActorSubdomain_commit:
	    this->commit();
	    break;
	    
	  case ShadowActorSubdomain_revertToLastCommit:
	    this->revertToLastCommit();
	    break;	    
	    
	  case ShadowActorSubdomain_revertToStart:
	    this->revertToStart();
	    this->sendID(msgData);
	    #ifdef _PARALLEL_PROCESSING
	    this->barrierCheck(0);
	    #endif
	    break;	    	    

	  case ShadowActorSubdomain_addRecorder:
	    theType = msgData(1);
	    theRecorder = theBroker->getPtrNewRecorder(theType);
	    if(theRecorder != 0) {
	      this->recvObject(*theRecorder);	      
	      this->addRecorder(*theRecorder);
	    }
	    break;	    	    

	  case ShadowActorSubdomain_removeRecorders:
	    this->removeRecorders();
	    break;	    	    
	    

	case ShadowActorSubdomain_wipeAnalysis:
	    this->wipeAnalysis();	    
	    break;

	  case ShadowActorSubdomain_setDomainDecompAnalysis:
	    theType = msgData(1);
	    theDDAnalysis = 
		theBroker->getNewDomainDecompAnalysis(theType, *this);

	    if(theDDAnalysis != 0) {
		this->recvObject(*theDDAnalysis);
		this->setDomainDecompAnalysis(*theDDAnalysis);
		msgData(0) = 0;
	    } else
		msgData(0) = -1;
	    
	    break;

	case ShadowActorSubdomain_setAnalysisAlgorithm:
	  theType = msgData(1);
	  theAlgorithm = theBroker->getNewEquiSolnAlgo(theType);
	  if(theAlgorithm != 0) {
	    this->recvObject(*theAlgorithm);
	    this->setAnalysisAlgorithm(*theAlgorithm);
	    msgData(0) = 0;
	  } else
	    msgData(0) = -1;
	    
	  break;
	  
	case ShadowActorSubdomain_setAnalysisIntegrator:
	  theType = msgData(1);
	  theIntegrator = theBroker->getNewIncrementalIntegrator(theType);
	  if(theIntegrator != 0) {
	    this->recvObject(*theIntegrator);
	    this->setAnalysisIntegrator(*theIntegrator);
	    msgData(0) = 0;
	  } else
	    msgData(0) = -1;
	    
	  break;

	case ShadowActorSubdomain_setAnalysisLinearSOE:
	  theType = msgData(1);
	  theOtherType = msgData(2);
	  theSOE = theBroker->getNewLinearSOE(theType, theOtherType);
	  
	  if(theSOE != 0) {
	    this->recvObject(*theSOE);
	    theSolver = theSOE->getSolver();
	    this->recvObject(*theSolver);
	    this->setAnalysisLinearSOE(*theSOE);
	    msgData(0) = 0;
	  } else
	    msgData(0) = -1;
	    
	  break;

	case ShadowActorSubdomain_setAnalysisConvergenceTest:
	  theType = msgData(1);
	  theTest = theBroker->getNewConvergenceTest(dynamic_cast<Element *>(this),theType);
	  
	  if(theTest != 0) {
	    this->recvObject(*theTest);
	    this->setAnalysisConvergenceTest(*theTest);
	    msgData(0) = 0;
	  } else
	    msgData(0) = -1;

	  break;
	    
	  case ShadowActorSubdomain_domainChange:
	    this->domainChange();

	    tag = this->getNumDOF();
            lastResponse= Vector(tag);
	    break;

	  case ShadowActorSubdomain_clearAnalysis:
//	    this->clearAnalysis();
	    break;
	  /*
	  case 50:
	    const XC::Matrix *theMatrix1 = &(this->getStiff());
	    this->sendMatrix(*theMatrix1);
	    break;

	  case 51:
	    const XC::Matrix *theMatrix2 = &(this->getDamp());
	    this->sendMatrix(*theMatrix2);
	    break;
	    
	  case 52:
	    const XC::Matrix *theMatrix3 = &(this->getMass());
	    this->sendMatrix(*theMatrix3);
	    break;	    
	    */
	  case  XC::ShadowActorSubdomain_getTang:
	    theMatrix = &(this->getTang());
	    this->sendMatrix(*theMatrix);
	    break;	    
	    
	  case ShadowActorSubdomain_getResistingForce:
	    theVector = &(this->getResistingForce());
	    this->sendVector(*theVector);
	    break;	    	    

	  case ShadowActorSubdomain_computeTang:
	    tag = msgData(1);
	    this->setTag(tag);
	    this->computeTang();
	    break;


	  case ShadowActorSubdomain_computeResidual:
	    this->computeResidual();
	    break;

	  case ShadowActorSubdomain_clearAll:
	    this->clearAll();
	    this->sendID(msgData);
	    break;


	  case ShadowActorSubdomain_getNodeDisp:
	    tag = msgData(1);  // nodeTag
	    dbTag = msgData(2); // dof
	    doubleRes = this->getNodeDisp(tag, dbTag, intRes);
	    msgData(0) = intRes;
	    this->sendID(msgData);
	    if(intRes == 0)
	      {
	        theVct = new Vector(1);
	        (*theVct)(0) = doubleRes;
	        this->sendVector(*theVct);
	        delete theVct;
		theVct= nullptr;
	      }
	    break;

	  case ShadowActorSubdomain_setMass:
	    tag = msgData(1);  // nodeTag
	    dbTag = msgData(2); // noRows
	    theOtherType = msgData(3); // noRows
	    theMtx = new Matrix(dbTag, theOtherType);
	    this->recvMatrix(*theMtx);
	    intRes = this->setMass(*theMtx, tag);
	    
	    delete theMtx;
	    theMtx= nullptr;
	    msgData(0) = intRes;
	    this->sendID(msgData);
	    break;

         case ShadowActorSubdomain_setRayleighDampingFactors:
	   theVct= new Vector(4);
	   this->recvVector(*theVct);
	   intRes= this->XC::Subdomain::setRayleighDampingFactors(RayleighDampingFactors(*theVct));
	   delete theVct;
	   theVct= nullptr;
	   break;

	  case ShadowActorSubdomain_DIE:
	    exitYet = true;
	    break;

	  default:
	    std::cerr << Domain::getClassName() << "::" << __FUNCTION__
	              << ";invalid action " << action << "received\n";
	    msgData(0) = -1;
	}
    }

    this->sendID(msgData);
    return 0;
  }


const XC::Vector &XC::ActorSubdomain::getLastExternalSysResponse(void)
  {
    int numDOF = this->getNumDOF();
    numDOF = this->getNumDOF();

    lastResponse= Vector(numDOF);
    
    if(mapBuilt == false)
      this->buildMap();

    ID &theMap = *map;
    Vector &localResponse= lastResponse;
    int numberDOF = this->getNumDOF();
    for(int i=0; i<numberDOF; i++)
      (*mappedVect)(theMap(i)) = localResponse(i);

    return *mappedVect;

  }

int XC::ActorSubdomain::update(void)
  {
    int res = this->XC::Domain::update();
    this->barrierCheck(res);
    return res;
  }

int XC::ActorSubdomain::updateTimeDt(void)
  {
    static XC::Vector data(2);
    this->recvVector(data);
    double newTime = data(0);
    double dT = data(1);
    int res = this->XC::Domain::update(newTime, dT);
    return this->barrierCheck(res);
  }

int XC::ActorSubdomain::barrierCheck(int myResult)
  {
    static XC::ID data(1);
    data(0) = myResult;
    this->sendID(data);
    this->recvID(data);
    return data(0);
  }
