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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/numberer/PlainNumberer.h,v $
                                                                        
                                                                        
// File: ~/analysis/numberer/PlainNumberer.h
// 
// Written: fmk 
// Created: 9/96
// Revision: A
//
// Description: This file contains the class definition for PlainNumberer.
// PlainNumberer is a subclass of DOF_Numberer. The PlainNumberer assigns
// equation numbers to the DOFs on a first come first serve basis; that is 
// it gets the DOF_GrpIter and assigns the equation numbers to the DOFs
// as it iterates through the iter.
//
// What: "@(#) PlainNumberer.h, revA"

#ifndef PlainNumberer_h
#define PlainNumberer_h

#define START_EQN_NUMBER 0

#include <solution/analysis/numberer/DOF_Numberer.h>

namespace XC {

//! @ingroup Numerador
//
//! @brief Trivial DOF numberer.
//!
//! The PlainNumberer class is a DOF\_Numberer. An object of this
//! class assigns the equation numbers to thee DOF\_Groups based on the
//! order in which they are obtained from the DOF\_GroupIter object
//! obtained from the AnalyisModel. The class is useful for situations
//! where the SystemOfEqn or Solver objects will impose a renumbering on
//! the equations, which makes performing a complex numbering of the
//! degrees-of-freedom a waste of computational effort.
class PlainNumberer: public DOF_Numberer
  {
  protected:
    friend class ModelWrapper;
    friend class FEM_ObjectBroker;
    PlainNumberer(ModelWrapper *);
  public:
    virtual DOF_Numberer *getCopy(void) const;

    int numberDOF(int lastDOF = -1);
    int numberDOF(ID &lastDOFs);    

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
  };
} // end of XC namespace

#endif

