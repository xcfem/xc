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
                                                                        
// $Revision: 1.2 $
// $Date: 2005/11/28 21:37:12 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/handler/PenaltyConstraintHandler.h,v $
                                                                        
                                                                        
// Written: fmk 
// Created: May 1998.
// Revision: A
//
// Description: This file contains the class definition for 
// PenaltyConstraintHandler. PenaltyConstraintHandler is a 
// constraint handler for handling constraints using the penalty method.
// for each element and degree-of-freedom at a node it constructs regular
// FE_Element and DOF_Groups; for each SFreedom_Constraint and MFreedom_Constraint
// PenaltySFreedom_FE and PenaltyMFreedom_FE elements are created.
//
// What: "@(#) PenaltyConstraintHandler.h, revA"

#ifndef PenaltyConstraintHandler_h
#define PenaltyConstraintHandler_h

#include <solution/analysis/handler/FactorsConstraintHandler.h>

namespace XC {
class FE_Element;
class DOF_Group;

const double DefaultPenaltyFactor= 1e7; //10^(p/2) being p the number of decimal digits.

//! @ingroup AnalysisCH
//
//! @brief PenaltyConstraintHandler is a 
//! constraint handler for handling constraints using the penalty method.
//! for each element and degree-of-freedom at a node it constructs regular
//! FE_Element and DOF_Groups; for each SFreedom_Constraint and MFreedom_Constraint
//! PenaltySFreedom_FE and PenaltyMFreedom_FE elements are created.
class PenaltyConstraintHandler : public FactorsConstraintHandler
  {
    friend class ModelWrapper;
    friend class FEM_ObjectBroker;
    PenaltyConstraintHandler(ModelWrapper *,const double &alphaSP= DefaultPenaltyFactor, const double &alphaMP= DefaultPenaltyFactor);
    ConstraintHandler *getCopy(void) const;
  public:
    int handle(const ID *nodesNumberedLast =0);
  };
} // end of XC namespace

#endif




