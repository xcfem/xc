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
// $Date: 2005/11/22 19:41:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/constraints/SFreedom_Constraint.h,v $
                                                                        
                                                                        
#ifndef SFreedom_Constraint_h
#define SFreedom_Constraint_h

// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the class definition for SFreedom_Constraint.
// SFreedom_Constraint is a class which stores the information for a single
// DOF constraint. Each single point constraint specifies a particular
// degree-of-freedom response (displacement, rotation) at a node.
// The constraint may be time-varying .. time varying constarints however 
// must be implemented using subclasses.
//
// What: "@(#) SFreedom_Constraint, revA"

#include "Constraint.h"

namespace XC {
  class ID;

//! @ingroup CCont
//! @defgroup CContSP Single freedom constraints.
//
//! @ingroup CContSP
//
//! @brief Single freedom constraint.
//!
//! An SFreedom\_Constraint represents a single degree of freedom constraint in
//! the domain. A single point constraint specifies the response of a particular
//! degree-of-freedom at a node. The declaration that all methods are
//! virtual allows for time varying constraints to be introduced.
class SFreedom_Constraint: public Constraint
  {
  protected:
    int dofNumber; //!< identifies which of the nodes dof is constrained 
    double valueR; //!< the reference value
    double valueC; //!< if constant = the reference value, if not constant = the reference value * load factor
    bool isConstant; //!< flag indicating if constant
    int  loadPatternTag;

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public:
    // constructors
    SFreedom_Constraint(int classTag);        
    SFreedom_Constraint(int spTag, int nodeTag);
    SFreedom_Constraint(int spTag, int nodeTag, int ndof, int classTag);
    SFreedom_Constraint(int spTag, int nodeTag, int ndof, double value, bool isConstant= true);
    virtual SFreedom_Constraint *getCopy(void) const;
    SFreedom_Constraint *getCopy(const int &) const;

    virtual int getDOF_Number(void) const;
    virtual int applyConstraint(double loadFactor);    
    virtual double getValue(void) const;
    virtual bool isHomogeneous(void) const;
    virtual void setLoadPatternTag(int loadPaternTag);
    virtual int getLoadPatternTag(void) const;
    
    inline void setIsConstant(const bool &b)
      { isConstant= b; }
    inline bool getIsConstant(void) const
      { return isConstant; }

    int getVtkCellType(void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    virtual void Print(std::ostream &s, int flag =0);
  };
} // end of XC namespace

#endif


