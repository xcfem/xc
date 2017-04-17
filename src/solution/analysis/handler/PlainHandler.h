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
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/handler/PlainHandler.h,v $
                                                                        
                                                                        
#ifndef PlainHandler_h
#define PlainHandler_h

// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for PlainHandler.
// PlainHandler is a simple ConstraintHandler; simple in that it can only
// handle 0 boundary constraints, i.e. no non-zero SFreedom_Constraints and no 
// MFreedom_Constraints can be handled by this type of ConstraintHandler.
//
// What: "@(#) PlainHandler.h, revA"

#include <solution/analysis/handler/ConstraintHandler.h>

namespace XC {
class FE_Element;
class DOF_Group;

//! @ingroup AnalysisCH
//
//! @brief Trivial constraint handler.
class PlainHandler : public ConstraintHandler
  {
    friend class ModelWrapper;
    friend class FEM_ObjectBroker;
    PlainHandler(ModelWrapper *);
    ConstraintHandler *getCopy(void) const;
  public:
    int handle(const ID *nodesNumberedLast =0);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace

#endif




