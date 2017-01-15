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
//RigidBase.cpp


#include "RigidBase.h"
#include "domain/mesh/node/Node.h"
#include "domain/domain/Domain.h"

void XC::RigidBase::setDomain(Domain *theDomain)
  {
    if(theDomain == nullptr)
      {
        std::cerr << "WARNING RigidBase::SetDomain: Specified domain does not exist";
        std::cerr << "Domain = 0\n";
        return;
      }
    MFreedom_Constraint::setDomain(theDomain);

    if(theDomain)
      {
        // get a pointer to the retained and constrained nodes - make sure they exist
        nodeR = theDomain->getNode(getNodeRetained());
        if(!nodeR)
          {
            std::cerr << "XC::RigidBase::setup - retained node " <<  getNodeRetained() <<  " not in domain\n";
          }
        else
          nodeR->connect(this);
        nodeC = theDomain->getNode(getNodeConstrained());
        if(!nodeC)
          {
            std::cerr << "XC::RigidBase::setup - constrained node " <<  getNodeConstrained() <<  " not in domain\n";
          }
        else
          nodeC->connect(this);
      }
  }

XC::RigidBase::RigidBase(int mPtag)
  : MFreedom_Constraint(mPtag), nodeR(nullptr), nodeC(nullptr) {}

XC::RigidBase::RigidBase(int mPtag,const int &masterNode, const int &slaveNode, int classTag)
  : MFreedom_Constraint(mPtag,masterNode,slaveNode,classTag), nodeR(nullptr), nodeC(nullptr) {}

XC::RigidBase::~RigidBase(void)
  {
    if(nodeR)
      nodeR->disconnect(this);
    if(nodeC)
      nodeC->disconnect(this);
  }
