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
// $Source: /usr/local/cvs/OpenSees/SRC/domain/constraints/Skew_Constraint.h,v $
                                                                        
                                                                        
#ifndef Skew_Constraint_h
#define Skew_Constraint_h

// File: ~/domain/constraints/Skew_Constraint.h
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the class definition for Skew_Constraint.
// This class implements skew type constraints as defined in section 4.2
// of the book: Finite Element Procedures Klaus-Jurgen Bathe, 2006
// ISBN	097900490X, 9780979004902
//
// What: "@(#) Skew_Constraint, revA"

#include "MFreedom_Constraint.h"

namespace XC {

/**

 @brief Skew constraint.
 
 Objects of this class store the information for a skew constraint as defined
 in section 4.2 of the book: Finite Element Procedures Klaus-Jurgen Bathe, 2006
  ISBN 097900490X, 9780979004902. Page 190 et seq.
 @ingroup CContMP
*/
class Skew_Constraint: public MFreedom_Constraint
  {
  public:
    // constructors        
    Skew_Constraint(int tag, int classTag= CNSTRNT_TAG_Skew_Constraint);

    Skew_Constraint(int tag, int nodeConstr, int classTag);    

    Skew_Constraint(int tag, int nodeConstr, const ID &constrainedDOF, const ID &retainedDOF, int classTag= CNSTRNT_TAG_Skew_Constraint);    

    Skew_Constraint(int tag, int nodeConstr, Matrix &constrnt, ID &constrainedDOF,ID &retainedDOF, int classTag= CNSTRNT_TAG_Skew_Constraint);

    // methods to get information about the constraint
    //! @brief Returns the tag of the retained (or primary) node
    //! in this type of constraint is the same as the constrained node.
    virtual inline const int &getNodeRetained(void) const
      { return this->getNodeConstrained(); }

    int addResistingForceToNodalReaction(bool inclInertia);
    
    int getVtkCellType(void) const;

    virtual void Print(std::ostream &s, int flag =0) const;

  };
} // end of XC namespace

#endif

