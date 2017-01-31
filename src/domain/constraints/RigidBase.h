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
//RigidBase.h

#ifndef RigidBase_h
#define RigidBase_h

#include "MFreedom_Constraint.h"

namespace XC {
class Domain;
class Node;

//! @ingroup CContMP
//
//! @brief Base class for the "rigid body motion" conditions.
class RigidBase: public MFreedom_Constraint
  {
  protected:
    Node *nodeR; //!< Pointer to retained node.
    Node *nodeC; //!< Pointer to constrained node.
  public:
    RigidBase(int tag);
    RigidBase(int tag,const int &, const int &, int classTag);
    ~RigidBase(void);
    void setDomain(Domain *theDomain);
  };
} // end of XC namespace

#endif
