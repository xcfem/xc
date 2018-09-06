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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/14 23:00:55 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/constraints/MFreedom_Constraint.h,v $
                                                                        
                                                                        
#ifndef MFreedom_Constraint_h
#define MFreedom_Constraint_h

// File: ~/domain/constraints/MFreedom_Constraint.h
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the class definition for MFreedom_Constraint.
// MFreedom_Constraint is a class 
//
// What: "@(#) MFreedom_Constraint, revA"

#include "MFreedom_ConstraintBase.h"
#include <utility/matrix/Matrix.h>


namespace XC {
class ID;

//! @ingroup CContMP
//
//! @brief Multi-freedom constraint.
//! 
//! Objects of this class store the information for a multifreedom constraint.
//! A multifreedom constraint imposes a relationship between the displacement
//! for certain dof at two nodes in the model, typically called the \p retained
//! node and the \p constrained node: \f$U_c = C_{cr} U_r\f$
//!
//! The MFreedom_Constraint class assumes time invariant constraints, i.e. the
//! constraint matrix does not change over time. All the methods are declared
//! as pure virtual, which will allow subclasses for time varying constraints.
//!
//! An MFreedom\_Constraint is responsible for providing information on the
//! relationship between the dof, this is in the form of a constraint
//! Matrix, \f$C_{cr}\f$, and two ID objects, \p retainedID and {\em
//! constrainedID} indicating the dof's at the nodes 
//! represented by \f$C_{cr}\f$. For example, for the following constraint
//! imposing a relationship between the displacements at node \f$1\f$, the 
//! constrained node, with the displacements at node \f$2\f$, the retained
//! node in a problem where the x,y,z components are identified as the
//! 0,1,2 degrees-of-freedom:
//! 
//! \f$\f$ u_{1,x} = 2 u_{2,x} + u_{2,z} \f$\f$
//! \f$\f$ u_{1,y} = 3 u_{2,z}\f$\f$
//! 
//! the constraint matrix is:
//! \f$\f$ C_{cr} =
//! \left[
//! \begin{array}{cc}
//! 2 & 1
//! 0 & 3
//! \end{array}
//! \right] 
//! \f$\f$
//! 
//! \p constrainedID = \f$[0\f$ \f$1]\f$ and \p retainedID \f$= [0\f$ \f$2]\f$.
class MFreedom_Constraint: public MFreedom_ConstraintBase
  {
  protected:
    int retainedNodeTag; //!< Tag of the retained node.
    ID retainDOF;  //!< ID of retained DOF at retained node
    
    void set_retained_dofs(const ID &);
    void set_constrained_retained_dofs(const ID &constrainedDOF,const ID &retainedDOF);
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    // constructors        
    MFreedom_Constraint(int tag , int classTag ); // Arash
    MFreedom_Constraint(int tag); // LCPT

    MFreedom_Constraint(int tag,int nodeRetain, int nodeConstr, int classTag);    

    MFreedom_Constraint(int tag,int nodeRetain, int nodeConstr, const ID &constrainedDOF, const ID &retainedDOF);    
    MFreedom_Constraint(int tag,int nodeRetain, int nodeConstr, const ID &constrainedDOF, const ID &retainedDOF,int classTag);    

    MFreedom_Constraint(int tag,int nodeRetain, int nodeConstr, Matrix &constrnt,ID &constrainedDOF,ID &retainedDOF);

    // methods to get information about the constraint
    //! @brief Returns the tag of the retained (or master) node.
    virtual inline const int &getNodeRetained(void) const
      { return retainedNodeTag; }
    //! @brief Returns the tag of the retained (or master) node.
    virtual inline int &getNodeRetained(void) 
      { return retainedNodeTag; }
    //! @brief Returns the number of retained nodes.
    virtual size_t getNumRetainedNodes(void) const
      { return 1; }
    std::vector<XC::Node *> getPointersToRetainedNodes(void) const;
    bool affectsNode(int ) const;
    bool affectsNodeAndDOF(int , int ) const;
    virtual const ID &getRetainedDOFs(void) const;            
    virtual int applyConstraint(double pseudoTime);

    // methods for output
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    
    int getVtkCellType(void) const;

    int addResistingForceToNodalReaction(bool inclInertia);

    virtual void Print(std::ostream &s, int flag =0);

  };
} // end of XC namespace

#endif

