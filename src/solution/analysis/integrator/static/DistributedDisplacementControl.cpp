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
                                                                        
// $Revision: 1.1 $
// $Date: 2005/11/29 21:59:49 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/DistributedDisplacementControl.cpp,v $
                                                                        
// Written: fmk 
// Created: 07/98
//
// Description: This file contains the class definition for XC::DistributedDisplacementControl.
// DistributedDisplacementControl is an algorithmic class for performing a static analysis
// using the arc length scheme, that is within a load step the following
// constraint is enforced: dU^TdU + alpha^2*dLambda^2 = DistributedDisplacementControl^2
// where dU is change in nodal displacements for step, dLambda is
// change in applied load and XC::DistributedDisplacementControl is a control parameter.
//
// What: "@(#) DistributedDisplacementControl.C, revA"


#include <solution/analysis/integrator/static/DistributedDisplacementControl.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <utility/matrix/Vector.h>
#include <utility/actor/channel/Channel.h>
#include <cmath>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <utility/matrix/ID.h>

//! @brief Constructor.
XC::DistributedDisplacementControl::DistributedDisplacementControl(AnalysisAggregation *owr)
  :DisplacementControl(owr) {}

//! @brief Constructor.
XC::DistributedDisplacementControl::DistributedDisplacementControl(AnalysisAggregation *owr,int node, int dof, double increment, int numIncr, double min, double max)
  :DisplacementControl(owr,node,dof,increment,numIncr,min,max) {}




int XC::DistributedDisplacementControl::newStep(void)
  {
    // get pointers to XC::AnalysisModel and XC::LinearSOE
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();    
    if(theModel == 0 || theLinSOE == 0)
      {
	std::cerr << "WARNING XC::DistributedDisplacementControl::newStep() ";
	std::cerr << "No XC::AnalysisModel or XC::LinearSOE has been set\n";
	return -1;
      }

    // determine increment for this iteration
    theIncrement*=factor();

    if(theIncrement < minIncrement)
      theIncrement = minIncrement;
    else if(theIncrement > maxIncrement)
      theIncrement = maxIncrement;


    // get the current load factor
    vectors.setCurrentLambda(getCurrentModelTime());

    // determine dUhat
    this->formTangent();
    vectors.distribDetermineUhat(processID,*theLinSOE);


    const Vector &dUhat= vectors.getDeltaUhat();

    const double dUahat= dUhat(theDofID);
    if(dUahat == 0.0)
      {
	std::cerr << "WARNING XC::DistributedDisplacementControl::newStep() ";
	std::cerr << "dUahat is zero -- zero reference displacement at control node DOF\n";
	return -1;
      }


    // determine delta lambda(1) == dlambda    
    const double dLambda = theIncrement/dUahat;

    vectors.newStep(dLambda,vectors.getDeltaUhat());

    // update model with delta lambda and delta U
    theModel->incrDisp(vectors.getDeltaU());    
    applyLoadModel(vectors.getCurrentLambda());    
    if(updateModel() < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; update failed for the new dU\n";
        return -1;
      }
    numIncrLastStep = 0;
    return 0;
  }

int XC::DistributedDisplacementControl::update(const XC::Vector &dU)
  {

    AnalysisModel *theModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();    
    if(theModel == 0 || theLinSOE == 0)
      {
	std::cerr << "WARNING XC::DistributedDisplacementControl::update() ";
	std::cerr << "No XC::AnalysisModel or XC::LinearSOE has been set\n";
	return -1;
      }

    vectors.setDeltaUbar(dU); // have to do this as the SOE is gonna change
    const double dUabar= vectors.getDeltaUbar()(theDofID);
    
    vectors.distribDetermineUhat(processID,*theLinSOE);

    const double dUahat=  vectors.getDeltaUhat()(theDofID);

    if(dUahat == 0.0)
      {
	std::cerr << "WARNING XC::DistributedDisplacementControl::update() ";
	std::cerr << "dUahat is zero -- zero reference displacement at control node DOF\n";
	return -1;
      }

    // determine delta lambda(1) == dlambda    
    const double dLambda = -dUabar/dUahat;
    
    vectors.update(dLambda);

    // update the model
    theModel->incrDisp(vectors.getDeltaU());    
    
    applyLoadModel(vectors.getCurrentLambda());    
    if(updateModel() < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; update failed for the new dU\n";
        return -1;
      }

    // set the X soln in linearSOE to be deltaU for convergence Test
    theLinSOE->setX(vectors.getDeltaU());

    numIncrLastStep++;

    return 0;
  }



int XC::DistributedDisplacementControl::domainChanged(void)
  {

    // we first create the Vectors needed
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();    
    if(theModel == 0 || theLinSOE == 0)
      {
	std::cerr << "WARNING XC::DistributedDisplacementControl::update() ";
	std::cerr << "No XC::AnalysisModel or XC::LinearSOE has been set\n";
	return -1;
      }
    
    //int size = theModel->getNumEqn(); // ask model in case N+1 space
    const Vector &b = theLinSOE->getB(); // ask model in case N+1 space
    const size_t size = b.Size();

    // first we determine the id of the nodal dof
    Domain *theDomain = theModel->getDomainPtr();
    if(!theDomain)
      {
        std::cerr << "BUG WARNING DistributedDisplacementControl::domainChanged() - no XC::Domain associated!!";
        return -1;
      }

    theDofID = -1;
    Node *theNodePtr= theDomain->getNode(theNodeTag);
    if(theNodePtr)
      {
        DOF_Group *theGroup = theNodePtr->getDOF_GroupPtr();
        if(!theGroup)
          {
	    std::cerr << "BUG DistributedDisplacementControl::domainChanged() - no XC::DOF_Group associated with the node!!\n";
	    return -1;
          }
        const ID &theID = theGroup->getID();
        if(theDof < 0 || theDof >= theID.Size())
          {
	    std::cerr << "DistributedDisplacementControl::domainChanged() - not a valid dof " << theDof << std::endl;
	    return -1;
          }
        theDofID = theID(theDof);
        if(theDofID < 0)
          {
	    std::cerr << "DistributedDisplacementControl::domainChanged() - constrained dof not a valid a dof\n";;
	    return -1;
          }
      }

    static ID data(1);    

    if(processID != 0)
      {
        CommParameters cp(0,*theChannels[0]);
        cp.sendInt(theDofID,DistributedObj::getDbTagData(),CommMetaData(0)); 
        cp.receiveInt(theDofID,DistributedObj::getDbTagData(),CommMetaData(0)); //??
      } 
    else
      {
        // if main domain, collect all theDofID if not -1 then this is the value & send value out
        const size_t numChannels= theChannels.size();
        for(size_t j=0; j<numChannels; j++)
          {
            CommParameters cp(0,*theChannels[j]);
            cp.receiveInt(theDofID,DistributedObj::getDbTagData(),CommMetaData(0));
          }
        for(size_t k=0; k<numChannels; k++)
          {
            CommParameters cp(0,*theChannels[k]);
            cp.sendInt(theDofID,DistributedObj::getDbTagData(),CommMetaData(0));
          }
      }
    vectors.domainChanged(size,*this,*theLinSOE);

    if(theDofID == -1)
      {
        std::cerr << "DistributedDisplacementControl::setSize() - failed to find valid dof - are the node tag and dof values correct?\n";
        return -1;
      }
    return 0;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::DistributedDisplacementControl::sendData(CommParameters &cp)
  {
    int res= DispBase::sendData(cp);
    res+= cp.sendBool(allHaveDofID,DistributedObj::getDbTagData(),CommMetaData(17));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::DistributedDisplacementControl::recvData(const CommParameters &cp)
  {
    int res= DispBase::recvData(cp);
    res+= cp.receiveBool(allHaveDofID,DistributedObj::getDbTagData(),CommMetaData(17));
    return res;
  }

int XC::DistributedDisplacementControl::sendSelf(CommParameters &cp)
  {					 
    int sendID =0;

    // if P0 check if already sent. If already sent use old processID; if not allocate a new_ process 
    // id for remote part of object, enlarge channel * to hold a channel * for this remote object.

    // if not P0, send current processID

    if(processID == 0)
      {
        // check if already using this object
        bool found= buscaCanal(cp,sendID);

        // if new_ object, enlarge XC::Channel pointers to hold new_ channel * & allocate new_ ID
        if(found == false)
          {
            assert(cp.getChannel());
            theChannels.push_back(cp.getChannel());
            const int numChannels= theChannels.size();
            // allocate new_ processID for remote object
            sendID = numChannels;
          }

      }
    else 
      sendID = processID;

    // send remotes processID & info about node, dof and numIncr
    int res= cp.sendInts(sendID,theNodeTag,theDof,DistributedObj::getDbTagData(),CommMetaData(18));
    if(res < 0)
      {
        std::cerr <<"WARNING DistributedDisplacementControl::sendSelf() - failed to send data\n";
        return -1;
      }
    res+= cp.sendDoubles(theIncrement,minIncrement,maxIncrement,specNumIncrStep,numIncrLastStep,DistributedObj::getDbTagData(),CommMetaData(19));
    if(res < 0)
      {
        std::cerr <<"WARNING DistributedDisplacementControl::recvSelf() - failed to recv vector data\n";
        return -1;
      }	        
    return 0;
  }


int XC::DistributedDisplacementControl::recvSelf(const CommParameters &cp)
  {
    int sendID =0;
    int res= cp.receiveInts(sendID,theNodeTag,theDof,DistributedObj::getDbTagData(),CommMetaData(18));
    if(res < 0)
      {
        std::cerr <<"WARNING XC::DistributedDisplacementControl::recvSelf() - failed to recv id data\n";
        return -1;
      }	      
    res+= cp.receiveDoubles(theIncrement,minIncrement,maxIncrement,specNumIncrStep,numIncrLastStep,DistributedObj::getDbTagData(),CommMetaData(19));

    if(res < 0)
      {
        std::cerr <<"WARNING XC::DistributedDisplacementControl::recvSelf() - failed to recv vector data\n";
        return -1;
      }	        

    theChannels.clear();
    theChannels.push_back(const_cast<CommParameters &>(cp).getChannel());
    return res;
  }

void XC::DistributedDisplacementControl::Print(std::ostream &s, int flag)
  {
    // TO FINISH    
  }
