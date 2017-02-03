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
//ImposedMotionBase.h

#ifndef ImposedMotionBase_h
#define ImposedMotionBase_h

#include <domain/constraints/SFreedom_Constraint.h>
#include <utility/matrix/Vector.h>

namespace XC {
class GroundMotion;
class Node;
class LoadPattern;

//! @ingroup CContSP
//
//! @brief Base class for prescribed displacements at the nodes.
class ImposedMotionBase : public SFreedom_Constraint
  {
  private:
    int groundMotionTag;
    int patternTag;

  protected:
    GroundMotion *theGroundMotion;  // pointer to ground motion
    Node *theNode; //!< pointer to node being constrained
    Vector theGroundMotionResponse; // the ground motions response
  public:
    // constructors    
    ImposedMotionBase(int classTag);        
    ImposedMotionBase(int classTag,int spTag, int nodeTag, int ndof, int patternTag, int theGroundMotionTag);

    // destructor
    ~ImposedMotionBase(void);

    void setDomain(Domain *theDomain);

    double getValue(void) const;
    bool isHomogeneous(void) const;

    int getMotion(void);

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace

#endif
