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
                                                                        
// $Revision: 1.7 $
// $Date: 2005/11/07 21:36:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/UniformExcitation.cpp,v $
                                                                        
                                                                        
// File: ~/domain/load/UniformExcitation.h
//
// Written: fmk 11/98
// Revised:
//
// Purpose: This file contains the class definition for XC::UniformExcitation.
// UniformExcitation is an abstract class.

#include <domain/load/pattern/load_patterns/UniformExcitation.h>
#include <domain/load/groundMotion/GroundMotion.h>
#include <domain/load/groundMotion/GroundMotionRecord.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <domain/mesh/element/Element.h>
#include "domain/mesh/node/NodeIter.h"
#include "domain/mesh/element/ElementIter.h"
#include "domain/constraints/SFreedom_Constraint.h"
#include "domain/constraints/SFreedom_ConstraintIter.h"
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>


//! @brief Constructor.
XC::UniformExcitation::UniformExcitation(int tag)
  :EarthquakePattern(tag, PATTERN_TAG_UniformExcitation),
   theMotion(nullptr), theDof(0), vel0(0.0), fact(1.0) {}

//! @brief Constructor.
XC::UniformExcitation::UniformExcitation(GroundMotion &_theMotion, int dof, int tag, double velZero, const double &f)
  :EarthquakePattern(tag, PATTERN_TAG_UniformExcitation), theMotion(&_theMotion), theDof(dof), vel0(velZero), fact(f)
  {
    // add the motion to the list of ground motions
    addMotion(*theMotion);
  }

//! @brief Return the ground motion record.
XC::GroundMotion &XC::UniformExcitation::getGroundMotionRecord(void)
  {
    if(!theMotion)
      {
        theMotion= new GroundMotionRecord();
        assert(theMotion);
        addMotion(*theMotion);
      }
    return *theMotion;
  }

//! @brief Assigns a domain to this load.
void XC::UniformExcitation::setDomain(Domain *theDomain) 
  {
    EarthquakePattern::setDomain(theDomain);

    // now we go through and set all the node velocities to be vel0
    // for those nodes not fixed in the dirn!
    if(vel0 != 0.0)
      {
	std::set<int> constrainedNodes= theDomain->getConstraints().getTagsNodesffectedBySPs(theDof);

        NodeIter &theNodes = theDomain->getNodes();
        Node *theNode= nullptr;
        Vector newVel(1);
        int currentSize = 1;
        while ((theNode = theNodes()) != 0)
          {
	    const int tag= theNode->getTag();
	    if(constrainedNodes.count(tag)==0)
	      {
		const int numDOF = theNode->getNumberDOF();

		if(numDOF != currentSize) 
		  newVel.resize(numDOF);

		newVel = theNode->getVel();
		newVel(theDof) = vel0;
		theNode->setTrialVel(newVel);
		theNode->commitState();
	      }
          }
      }
  }

//! @brief Applies the load.
//!
//! @param time: instant to calculate the value of the load.
//!
//! Checks to see if the number of nodes in the domain has changed, if
//! there has been a change it invokes {\em setNumColR(1)} and then 
//! {\em setR(theDof, 0, 1.0)} on each Node. It then invokes the base classes
//! applyLoad() method. THIS SHOULD BE CHANGED TO USE LATEST domainChanged().
void XC::UniformExcitation::applyLoad(double time)
  {
    Domain *theDomain = getDomain();
    if(theDomain)
      {
	NodeIter &theNodes = theDomain->getNodes();
	Node *theNode= nullptr;
	while ((theNode = theNodes()) != 0)
	  {
	    theNode->setNumColR(1);
	    const Vector &crds=theNode->getCrds();
	    const int ndm= crds.Size();

	    if(ndm == 1)
	      {	theNode->setR(theDof, 0, fact); }
	    else if(ndm == 2)
	      {
		if(theDof < 2)
		  { theNode->setR(theDof, 0, fact); }
		else if(theDof == 2)
		  {
		    const double xCrd = crds(0);
		    const double yCrd = crds(1);
		    theNode->setR(0, 0, -fact*yCrd);
		    theNode->setR(1, 0, fact*xCrd);
		    theNode->setR(2, 0, fact);
		  }
	      }
	    else if(ndm == 3)
	      {
		if(theDof < 3)
		  {
		    theNode->setR(theDof, 0, fact);
		  }
		else if(theDof == 3)
		  {
		    const double yCrd = crds(1);
		    const double zCrd = crds(2);
		    theNode->setR(1, 0, -fact*zCrd);
		    theNode->setR(2, 0, fact*yCrd);
		    theNode->setR(3, 0, fact);
		  }
		else if(theDof == 4)
		  {
		    const double xCrd = crds(0);
		    const double zCrd = crds(2);
		    theNode->setR(0, 0, fact*zCrd);
		    theNode->setR(2, 0, -fact*xCrd);
		    theNode->setR(4, 0, fact);
		  }
		else if(theDof == 5)
		  {
		    const double xCrd = crds(0);
		    const double yCrd = crds(1);
		    theNode->setR(0, 0, -fact*yCrd);
		    theNode->setR(1, 0, fact*xCrd);
		    theNode->setR(5, 0, fact);
		  }
	      }
	  }
        EarthquakePattern::applyLoad(time);
      }
    return;
  }

//! @brief Applies load sensitivity.
void XC::UniformExcitation::applyLoadSensitivity(double time)
  {
    Domain *theDomain = this->getDomain();
    if(theDomain)
      {

        //if(numNodes != theDomain->getNumNodes()) {
        NodeIter &theNodes = theDomain->getNodes();
        Node *theNode;
        while((theNode = theNodes()) != 0)
          {
            theNode->setNumColR(1);
            theNode->setR(theDof, 0, 1.0);
          }
        //  }
        EarthquakePattern::applyLoadSensitivity(time);
      }
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::UniformExcitation::sendData(Communicator &comm)
  {
    int res= EarthquakePattern::sendData(comm);
    res+= comm.sendInt(theDof,getDbTagData(),CommMetaData(23));
    res+= comm.sendDoubles(vel0,fact,getDbTagData(),CommMetaData(24));
    res+= sendGroundMotionPtr(theMotion,getDbTagData(),comm,BrokedPtrCommMetaData(25,26,27));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::UniformExcitation::recvData(const Communicator &comm)
  {
    int res= EarthquakePattern::recvData(comm);
    res+= comm.receiveInt(theDof,getDbTagData(),CommMetaData(23));
    res+= comm.receiveDoubles(vel0,fact,getDbTagData(),CommMetaData(24));
    theMotion= receiveGroundMotionPtr(theMotion,getDbTagData(),comm,BrokedPtrCommMetaData(25,26,27));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::UniformExcitation::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << ";failed to send data\n";
    return res;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::UniformExcitation::recvSelf(const Communicator &comm)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << ";failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
                    << ";failed to receive data\n";
      }
    return res;
  }

//! @brief Prints stuff.
void XC::UniformExcitation::Print(std::ostream &s, int flag) const
  {
    s << getClassName() << "::" << __FUNCTION__
      << "; " << this->getTag() 
      << " - Not Printing the GroundMotion.\n";
  }
