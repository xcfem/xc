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
                                                                        
#ifndef MFreedom_Joint_h
#define MFreedom_Joint_h

// Written: Arash Altoontash, Gregory Deierlein
// Created: 08/01
// Revision: Arash

// Purpose: This file contains the class definition for MFreedom_Joint.
// It is a sub class for MFreedom_Constraint specialized to be used for simple joint 
// connection element. MFreedom_Joint defines a nonlinear, time dependent multi 
// point constraint.
//

#include <domain/constraints/MFreedom_Constraint.h>

namespace XC {

class Node;
class Matrix;
class ID;


//! @ingroup CContMP
//
//! @brief Base class for joint constraints.
class MFreedom_Joint: public MFreedom_Constraint
  {
  protected:
    Node *RetainedNode; //!<  to identify the retained node
    Node *ConstrainedNode; //!<  to identify  the constrained node
    int LargeDisplacement; //!<  flag for large displacements enabled; LrgDsp=0 means large displacement is not enabled
    // 0 for constant constraint matrix(small deformations)
    // 1 for time varying constraint matrix(large deformations)
    // 2 for large deformations with length correction
    double Length0;
  public:
    // constructors        
    MFreedom_Joint(int tag, int classTag);
    MFreedom_Joint(Domain *theDomain, int tag, int classTag, int nodeRetain, int nodeConstr, int LrgDsp);
    ~MFreedom_Joint(void);

    // method to get information about the constraint
    bool isTimeVarying(void) const;
    void setDomain(Domain *theDomain);

    // methods for output
    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif

