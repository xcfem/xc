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



XC::Node *XC::DummyNode::getNode() const
  { return theRealNode; }

void XC::DummyNode::setDOF_GroupPtr(DOF_Group *theDOF_Grp)
  { theDOFGroup = theDOF_Grp; }

 XC::DOF_Group *XC::DummyNode::getDOF_GroupPtr(void)
  { return theDOFGroup; }


int XC::DummyNode::getNumberDOF(void) const
  { return theRealNode->getNumberDOF(); }





const XC::Matrix &XC::DummyNode::getMass(void) 
  { return theRealNode->getMass(); }




int XC::DummyNode::setMass(const XC::Matrix &m)
  { return theRealNode->setMass(m); }



const XC::Vector &XC::DummyNode::getCrds() const
  { return theRealNode->getCrds(); }


const XC::Vector &XC::DummyNode::getDisp(void) const
  { return theRealNode->getDisp(); }


const XC::Vector &XC::DummyNode::getVel(void) const
  { return theRealNode->getVel(); }


const XC::Vector &XC::DummyNode::getAccel(void)  const
  { return theRealNode->getAccel(); }


const XC::Vector &XC::DummyNode::getTrialDisp(void) const
  { return theRealNode->getTrialDisp(); }



const XC::Vector &XC::DummyNode::getTrialVel(void) const
  { return theRealNode->getTrialVel(); }



const XC::Vector &XC::DummyNode::getTrialAccel(void) const
  { return theRealNode->getTrialAccel(); }





int XC::DummyNode::setTrialDisp(const XC::Vector &displ)
  { return 0; }

int XC::DummyNode::setTrialVel(const XC::Vector &vel)
  { return 0; }


int XC::DummyNode::setTrialAccel(const XC::Vector &accel)
  { return 0; }


int XC::DummyNode::setIncrTrialDisp(const XC::Vector &incrDispl)
  { return 0; }


int XC::DummyNode::setIncrTrialVel(const XC::Vector &incrVel)
  { return 0; }


int XC::DummyNode::setIncrTrialAccel(const XC::Vector &incrAccel)
  { return 0; }


int XC::DummyNode::commitState()
  { return 0; }


void XC::DummyNode::addUnbalancedLoad(const XC::Vector &add)
  { theRealNode->addUnbalancedLoad(add); }



const XC::Vector &XC::DummyNode::getUnbalancedLoad(void) const
  { return theRealNode->getUnbalancedLoad(); }


void XC::DummyNode::zeroUnbalancedLoad(void)
  { theRealNode->zeroUnbalancedLoad(); }

int XC::DummyNode::addElementPtr(Element *elePtr)
  {
//    return theRealNode->addElementPtr(elePtr);
    return 0;
  }



void XC::DummyNode::setColor(int newColor)
  {
//    theRealNode->setColor(newColor);
  }

int XC::DummyNode::getColor(void) const
  {
//    return theRealNode->getColor();
    return 0;
  }

int XC::DummyNode::sendSelf(CommParameters &cp)
  {
    std::cerr << "XC::DummyNode::sendSelf - should never be called\n";
    std::cerr << "sending actual node\n";    
    
    return theRealNode->sendSelf(cp);
  }

int XC::DummyNode::recvSelf(const CommParameters &cp)
  {
    std::cerr << "XC::DummyNode::recvSelf - should never be called\n";
    return 0;
  }    

void XC::DummyNode::Print(std::ostream &s) const
  { theRealNode->Print(s); }
  
std::ostream &XC::operator<<(std::ostream &s, const XC::DummyNode &N)
  {
    N.Print(s);
    return s;
  }


