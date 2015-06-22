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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
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


