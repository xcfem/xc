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
                                                                        
#ifndef MFreedom_ConstraintBase_h
#define MFreedom_ConstraintBase_h

// File: ~/domain/constraints/MFreedom_ConstraintBase.h
//
// Written: lcpt 
// Created: 03/2015
// Revision: A
//
// Purpose: This file contains the class definition for MFreedom_ConstraintBase.
// MFreedom_ConstraintBase is a base class for multi node constraints.
// A multifreedom constraint relates certain dofs at 
// a constrained node to be related to certain dosf at one or more
// retained nodes

#include "Constraint.h"
#include <utility/matrix/Matrix.h>


namespace XC {
class Matrix;
class ID;

//! @ingroup CCont
//! @defgroup CContMP Multi-freedom constraints.
//
//! @ingroup CContMP
//
//! @brief Base class for mult-freedom constraints.
class MFreedom_ConstraintBase: public Constraint
  {
  protected:
    Matrix constraintMatrix;  //!< Constraint matrix.
    ID constrDOF;  //!< ID of constrained DOF at constrained node
    
    void set_constraint(const Matrix &c);
    void set_constrained_dofs(const ID &);
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    // constructors        
    MFreedom_ConstraintBase(int tag , int classTag ); // Arash

    MFreedom_ConstraintBase(int tag, int nodeConstr, int classTag);    

    MFreedom_ConstraintBase(int tag, int nodeConstr, const ID &,int classTag);    

    MFreedom_ConstraintBase(int tag, int nodeConstr, const Matrix &, const ID &, int classTag);

    // method to get information about the constraint
    //! @brief Return the identifier of the constrained node.
    virtual inline const int &getNodeConstrained(void) const
      { return constrNodeTag; }
    //! @brief Return the identifier of the constrained node.
    virtual inline int &getNodeConstrained(void)
      { return constrNodeTag; }
    virtual const ID &getRetainedDOFs(void) const= 0;            
    virtual const ID &getConstrainedDOFs(void) const;        
    virtual const Matrix &getConstraint(void) const;
    virtual size_t getNumRetainedNodes(void) const= 0;
    virtual std::vector<XC::Node *> getPointersToRetainedNodes(void) const= 0;
    std::vector<int> getIdxNodes(void) const;

    bool isTimeVarying(void) const;

    bool affectsNode(int ) const;
    bool affectsNodeAndDOF(int theNode, int theDOF) const;
    int addResistingForceToNodalReaction(bool inclInertia);

    virtual void Print(std::ostream &s, int flag =0) const;

  };
} // end of XC namespace

#endif

