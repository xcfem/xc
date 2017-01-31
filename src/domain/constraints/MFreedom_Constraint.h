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
// MFreedom_Constraint is a class which stores the information for a multi
// point constraint. A multipoint constraint relates certain dof at 
// a constrained node to be related to certain dof at a retained node: 
//                      {Uc} = [Ccr] {Ur}
//
// The MFreedom_Constraint class assumes time invariant constraints, i.e. the
// constraint matrix does not change over time. All the methods are declared
// as pure virtual, which will allow subclasses for time varying constraints.
//
// What: "@(#) MFreedom_Constraint, revA"

#include "MFreedom_ConstraintBase.h"
#include <utility/matrix/Matrix.h>


namespace XC {
class Matrix;
class ID;

//! @ingroup CContMP
//
//! @brief Multi-freedom constraint.
class MFreedom_Constraint: public MFreedom_ConstraintBase
  {
  protected:
    int retainedNodeTag; //!< Tag del nodo retenido.

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
    MFreedom_Constraint(const MFreedom_Constraint &otro);
    MFreedom_Constraint &operator=(const MFreedom_Constraint &otro);

    // method to get information about the constraint
    virtual inline const int &getNodeRetained(void) const //Tag del nodo retenido o master.
      { return retainedNodeTag; }
    virtual inline int &getNodeRetained(void) //Tag del nodo retenido o master.
      { return retainedNodeTag; }
    bool afectaANodo(int tagNodo) const;
    virtual const ID &getRetainedDOFs(void) const;            
    virtual int applyConstraint(double pseudoTime);

    // methods for output
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    
    int getVtkCellType(void) const;
    int getMEDCellType(void) const;

    virtual void Print(std::ostream &s, int flag =0);

  };
} // end of XC namespace

#endif

