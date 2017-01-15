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
                                                                        
#ifndef MRMFreedom_Constraint_h
#define MRMFreedom_Constraint_h

// File: ~/domain/constraints/MRMFreedom_Constraint.h
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the class definition for MRMFreedom_Constraint.
// MRMFreedom_Constraint is a class which stores the information for a multiple
// retained nodes constraint. A multipoint constraint relates certain dofs at 
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

//! @ingroup CCont
//! @defgroup CContMP Condiciones de contorno multinodales.
//
//! @ingroup CContMP
//
//! @brief Multiple retained nodes constraint.
class MRMFreedom_Constraint: public MFreedom_ConstraintBase
  {
  protected:
    ID retainedNodeTags; //!< Retained nodes tags.

  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
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
    virtual inline const ID &getRetainedNodeTags(void) const
      { return retainedNodeTags; }
    virtual inline ID &getRetainedNodeTags(void)
      { return retainedNodeTags; }
    inline virtual const ID &getRetainedDOFs(void) const
      { return getConstrainedDOFs(); } //Same as constrained.
    int getNumDofGroups(void) const;         
    int getNumDofs(void) const;           
    int getNumRetainedDofs(void) const;           
    int getNumConstrainedDofs(void) const;           
    int getNumLagrangeDofs(void) const;           
    bool afectaANodo(int tagNodo) const;
    virtual int applyConstraint(double pseudoTime);

    int addResistingForceToNodalReaction(bool inclInertia);

    // methods for output
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    
    int getVtkCellType(void) const;
    int getMEDCellType(void) const;

    virtual void Print(std::ostream &s, int flag =0);

  };
} // end of XC namespace

#endif

