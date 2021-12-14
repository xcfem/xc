// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
                                                                        
#ifndef MRMFreedom_Constraint_h
#define MRMFreedom_Constraint_h

// File: ~/domain/constraints/MRMFreedom_Constraint.h
//
//
// Written: lcpt 
// Created: 03/2015
//
// Purpose: This file contains the class definition for MRMFreedom_Constraint.
// MRMFreedom_Constraint is a class which stores the information for a multiple
// retained nodes constraint. A multifreedom constraint relates certain dofs at 
// a constrained node to be related to certains dofs at multiple retained nodes:

// The MRMFreedom_Constraint class assumes time invariant constraints, i.e. the
// constraint matrix does not change over time. All the methods are declared
// as pure virtual, which will allow subclasses for time varying constraints.
//

#include "MFreedom_ConstraintBase.h"
#include <utility/matrix/Matrix.h>


namespace XC {
class Matrix;
class ID;

//! @ingroup CContMP
//
//! @brief Multiple retained nodes constraint.
class MRMFreedom_Constraint: public MFreedom_ConstraintBase
  {
  protected:
    ID retainedNodeTags; //!< Retained nodes tags.

  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);
  public:
    // constructors        
    MRMFreedom_Constraint(int tag , int classTag ); // Arash
    MRMFreedom_Constraint(int tag); // LCPT

    MRMFreedom_Constraint(int tag,const ID &, int nodeConstr, int classTag);    

    MRMFreedom_Constraint(int tag,const ID &, int nodeConstr, const ID &constrainedDOF);    
    MRMFreedom_Constraint(int tag,const ID &, int nodeConstr, const ID &constrainedDOF,int classTag);    

    MRMFreedom_Constraint(int tag,const ID &, int nodeConstr, const Matrix &, const ID &constrainedDOF);
    MRMFreedom_Constraint(int tag,const Element &, const Node &, const ID &);


    // method to get information about the constraint
    //! @brief Returns the tags of the retained nodes.
    virtual inline const ID &getRetainedNodeTags(void) const
      { return retainedNodeTags; }
    //! @brief Returns the tags of the retained nodes.
    virtual inline ID &getRetainedNodeTags(void)
      { return retainedNodeTags; }
    //! @brief Returns the indexes of the degrees of freedom.
    inline virtual const ID &getRetainedDOFs(void) const
      { return getConstrainedDOFs(); } //Same as constrained.
    virtual size_t getNumRetainedNodes(void) const
      { return retainedNodeTags.size(); }
    std::vector<XC::Node *> getPointersToRetainedNodes(void) const;
    int getNumDofGroups(void) const;         
    int getNumDofs(void) const;           
    int getNumRetainedDofs(void) const;           
    int getNumConstrainedDofs(void) const;           
    int getNumLagrangeDofs(void) const;
    bool affectsNode(int ) const;
    bool affectsNodeAndDOF(int , int ) const;
    virtual int applyConstraint(double pseudoTime);

    int addResistingForceToNodalReaction(bool inclInertia);

    // methods for output
    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);
    
    int getVtkCellType(void) const;

    virtual void Print(std::ostream &s, int flag =0) const;

  };
} // end of XC namespace

#endif

