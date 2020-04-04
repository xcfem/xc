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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:00:58 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/mesh/node/DummyNode.cpp,v $
                                                                        
                                                                        
// File: ~/model/DummyNode.C
//
// Written: fmk 11/95
// Revised:
//
// This file contains the methods required for class XC::DummyNode.
   
#include "DummyNode.h"
#include "classTags.h"

XC::DummyNode::DummyNode(void)
  : Node(0, NOD_TAG_DummyNode), theRealNode(0), theDOFGroup(0) {}

XC::DummyNode::DummyNode(Node &theNode)
  : Node(theNode.getTag(), NOD_TAG_DummyNode), theRealNode(&theNode), theDOFGroup(0)
  { theRealNode = &theNode; }


//! @brief Return a pointer to the associated node.
XC::Node *XC::DummyNode::getNode(void) const
  { return theRealNode; }

//! Each DummyNode, when involved with a StaticCondensationAnalysis
//! analysis, will be associated with a DOF\_Group object. It is the
//! DOF\_Group that contains the ID of equation numbers. When invoked this
//! method sets that link for the DummyNode object. 
void XC::DummyNode::setDOF_GroupPtr(DOF_Group *theDOF_Grp)
  { theDOFGroup = theDOF_Grp; }

//! Method which returns a pointer to the DOF\_Group object that was set
//! using {\em setDOF\_GroupPtr}. If no pointer has been set a \f$0\f$ is
//! returned.
XC::DOF_Group *XC::DummyNode::getDOF_GroupPtr(void)
  { return theDOFGroup; }


//! @brief Invokes getNumberDOF() on its associated
//! node object. 
int XC::DummyNode::getNumberDOF(void) const
  { return theRealNode->getNumberDOF(); }


//! @brief Returns the result of invoking getMass() on its associated
//! node object. 
const XC::Matrix &XC::DummyNode::getMass(void) const
  { return theRealNode->getMass(); }

//! @brief Returns the result of invoking setMass() on its associated
//! node object. 
int XC::DummyNode::setMass(const XC::Matrix &m)
  { return theRealNode->setMass(m); }



//! @brief Returns the result of invoking getCrds() on its associated
//! node object. 
const XC::Vector &XC::DummyNode::getCrds() const
  { return theRealNode->getCrds(); }


//! @brief Returns the result of invoking getDisp() on its associated
//! Node object. 
const XC::Vector &XC::DummyNode::getDisp(void) const
  { return theRealNode->getDisp(); }


//! @brief Returns the result of invoking getVel() on its associated
//! node object. 
const XC::Vector &XC::DummyNode::getVel(void) const
  { return theRealNode->getVel(); }


//! @brief Returns the result of invoking getAccel() on its associated
//! Node object. 
const XC::Vector &XC::DummyNode::getAccel(void)  const
  { return theRealNode->getAccel(); }


//! @brief Returns the result of invoking getTrialDisp() on its associated
//! node object. 
const XC::Vector &XC::DummyNode::getTrialDisp(void) const
  { return theRealNode->getTrialDisp(); }

//! @brief Returns the result of invoking getTrialVel() on its associated
//! Node object. 
const XC::Vector &XC::DummyNode::getTrialVel(void) const
  { return theRealNode->getTrialVel(); }

//! @brief Returns the result of invoking getTrialAccel() on its associated
//! node object. 
const XC::Vector &XC::DummyNode::getTrialAccel(void) const
  { return theRealNode->getTrialAccel(); }

//! @brief Does nothing.
int XC::DummyNode::setTrialDisp(const Vector &)
  { return 0; }

//! @brief Does nothing.
int XC::DummyNode::setTrialVel(const Vector &)
  { return 0; }


//! @brief Does nothing.
int XC::DummyNode::setTrialAccel(const Vector &)
  { return 0; }


//! @brief Does nothing.
int XC::DummyNode::setIncrTrialDisp(const Vector &)
  { return 0; }


//! @brief Does nothing.
int XC::DummyNode::setIncrTrialVel(const Vector &)
  { return 0; }

//! @brief Does nothing.
int XC::DummyNode::setIncrTrialAccel(const Vector &)
  { return 0; }

//! @brief Does nothing.
int XC::DummyNode::commitState()
  { return 0; }


//! @brief Returns the result of invoking addUnbalancedLoad() on its associated
//! node object. 
void XC::DummyNode::addUnbalancedLoad(const Vector &load)
  { theRealNode->addUnbalancedLoad(load); }

//! @brief Returns the result of invoking getUnbalancedLoad() on its associated
//! node object. 
const XC::Vector &XC::DummyNode::getUnbalancedLoad(void) const
  { return theRealNode->getUnbalancedLoad(); }

//! @brief Calls zeroUnbalancedLoad() on its associated node.
void XC::DummyNode::zeroUnbalancedLoad(void)
  { theRealNode->zeroUnbalancedLoad(); }

//! @brief Does nothing.
int XC::DummyNode::addElementPtr(Element *elePtr)
  {
//    return theRealNode->addElementPtr(elePtr);
    return 0;
  }


//! @brief Does nothing.
void XC::DummyNode::setColor(int newColor)
  {
//    theRealNode->setColor(newColor);
  }

//! @brief Does nothing.
int XC::DummyNode::getColor(void) const
  {
//    return theRealNode->getColor();
    return 0;
  }

int XC::DummyNode::sendSelf(Communicator &comm)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << ";should never be called\n"
	      << "sending actual node\n";    
    
    return theRealNode->sendSelf(comm);
  }

int XC::DummyNode::recvSelf(const Communicator &comm)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; should never be called\n";
    return 0;
  }    

//! @brief Print stuff.
void XC::DummyNode::Print(std::ostream &s) const
  { theRealNode->Print(s); }
  
std::ostream &XC::operator<<(std::ostream &s, const XC::DummyNode &N)
  {
    N.Print(s);
    return s;
  }


