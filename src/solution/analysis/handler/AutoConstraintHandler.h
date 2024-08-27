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
                                                                        
// $Revision: 1.4 $
// $Date: 2005-11-28 21:35:54 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/handler/AutoConstraintHandler.h,v $
                                                                        
                                                                        
// Written: Massimo Petracca
// Created: June 2024.
// Revision: A
//
// Description: This file contains the class definition for 
// AutoConstraintHandler. AutoConstraintHandler is a 
// constraint handler for handling constraints using the different methods.
//
// For each element and degree-of-freedom at a node it constructs:
//
// 1) regular FE_Element and DOF_Groups if there is no SP_Constraint or MP_Constraint;
// 2) TransformationDOF_Group for SP constraints (as in the Transformation method)
// 3) PenaltyMP_FE for MP constraints
//
// Notes:
// 1) For each PenaltyMP_FE, by default it automatically selects a proper penalty
//    value based on the stiffness values found on the DOFs involved in the
//    MP constraint
//
// What: "@(#) AutoConstraintHandler.h, revA"

#ifndef AutoConstraintHandler_h
#define AutoConstraintHandler_h

#include "solution/analysis/handler/ConstraintHandler.h"
#include <vector>

namespace XC {
class FE_Element;
class TransformationDOF_Group;

class AutoConstraintHandler : public ConstraintHandler
  {
    friend class ModelWrapper;
    friend class FEM_ObjectBroker;
  private:
    bool verbose = false;
    bool auto_penalty = true;
    double auto_penalty_oom = 3.0;
    double user_penalty = 0.0;
    std::vector<TransformationDOF_Group*> theDOFs;
    
    AutoConstraintHandler(ModelWrapper *);
    AutoConstraintHandler(ModelWrapper *,
        bool _verbose,
        bool _auto_penalty,
        double _auto_penalty_oom,
        double _user_penalty);
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
    virtual ConstraintHandler *getCopy(void) const;
  public:
    ~AutoConstraintHandler();

    int handle(const ID* nodesNumberedLast = 0);
    int applyLoad();
    void clearAll(void);
    int doneNumberingDOF(void);

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  };
} // end of XC namespace

#endif




