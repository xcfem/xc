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
// $Source: /usr/local/cvs/OpenSees/SRC/domain/mesh/node/DummyNode.h,v $
                                                                        
                                                                        
// File: ~/domain/mesh/node/DummyNode.h
//
// Written: fmk 
// Created: Fri Sep 20 15:34:47: 1996
// Revision: A
//
// Purpose: This file contains the class definition for DummyNode.
// DummyNodes are a type of node created and used by Subdomains for their
// exterior nodes. They reference a real node and most methods invoked on
// them are in turn invoked by the dummy node on the real node. The calls
// asking the real node to change its current state are ignored. The
// calls involving DOF\_Group are handled by the dummy node.
//
// 
// What: "@(#) DummyNode.h, revA"

#ifndef DummyNode_h
#define DummyNode_h

#include "domain/mesh/node/Node.h"

namespace XC {
class DOF_Group;
class Element;

//! @ingroup Nod
//
//! @brief DummyNodes are a type of node created and used by
//! Subdomains for their exterior nodes. They reference a real
//! node and most methods invoked on them are in turn invoked
//! by the dummy node on the real node. The calls asking the
//! real node to change its current state are ignored. The
//! calls involving DOF_Group are handled by the dummy node.
class DummyNode: public Node
  {
  private:
    Node *theRealNode;
    DOF_Group *theDOFGroup;
  public:
    DummyNode(void);    
    DummyNode(Node &theRealNode);
    
    Node *getNode(void) const;
    
    void setDOF_GroupPtr(DOF_Group *theDOF_Grp);
    DOF_Group *getDOF_GroupPtr(void);
    int  getNumberDOF(void) const;

    const Matrix &getMass(void);
    int setMass(const Matrix &);
    
    const Vector &getCrds(void) const;

    const Vector &getDisp(void) const;
    const Vector &getVel(void) const;
    const Vector &getAccel(void) const;   

    const Vector &getTrialDisp(void) const;    
    const Vector &getTrialVel(void) const;    
    const Vector &getTrialAccel(void) const;    
    
    int setTrialDisp(const Vector &);
    int setTrialVel(const Vector &);   
    int setTrialAccel(const Vector &);        

    int setIncrTrialDisp(const Vector &);    
    int setIncrTrialVel(const Vector &);    
    int setIncrTrialAccel(const Vector &);        
    
    void addUnbalancedLoad(const Vector &);        
    const Vector &getUnbalancedLoad(void) const;    
    void zeroUnbalancedLoad(void);
    int commitState();

    void Print(std::ostream &s) const;
    friend std::ostream &operator<<(std::ostream &s, const DummyNode &N);

    int addElementPtr(Element *);

    void setColor(int newColor);
    int getColor(void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);    
  };
} // end of XC namespace

#endif

