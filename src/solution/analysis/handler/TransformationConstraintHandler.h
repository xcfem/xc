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
                                                                        
// $Revision: 1.4 $
// $Date: 2005/11/28 21:35:54 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/handler/TransformationConstraintHandler.h,v $
                                                                        
                                                                        
// Written: fmk 
// Created: May 1998.
// Revision: A
//
// Description: This file contains the class definition for 
// TransformationConstraintHandler. TransformationConstraintHandler is a 
// constraint handler for handling constraints using the Transformation method.
// for each element and degree-of-freedom at a node it constructs regular
// FE_Element and DOF_Groups if there is no SFreedom_Constraint or MFreedom_Constraint
// constraining an elements node or the node; otherwise a TransformationFE
// element and a TransformationDOF_Group are created. 
//
// What: "@(#) TransformationConstraintHandler.h, revA"

#ifndef TransformationConstraintHandler_h
#define TransformationConstraintHandler_h

#include <solution/analysis/handler/ConstraintHandler.h>

namespace XC {
class FE_Element;
class DOF_Group;

//! @ingroup AnalysisCH
//
//! @brief TransformationConstraintHandler is a constraint handler
//! for handling constraints using the Transformation method.
//!
//! TransformationConstraintHandler objects deal with
//! both single and multi freedom constraints using the Transformation method.
//! For each element and degree-of-freedom at a node it constructs regular
//! FE_Element and DOF_Groups if there is no SFreedom_Constraint or MFreedom_Constraint
//! constraining an elements node or the node; otherwise a TransformationFE
//! element and a TransformationDOF_Group are created.It is these objects that enforce the
//! constraints by modifying the tangent matrix and residual vector.
class TransformationConstraintHandler : public ConstraintHandler
  {
  private:
    std::set<FE_Element *> theFEs;
    std::vector<DOF_Group *> theDOFs;

    int numDOF; //!< Number or degrees of freedom.
    int numConstrainedNodes; //!< number of constrained nodes.

    int numTransformationFEs; //!< number of TransformationFE elements.

    friend class ModelWrapper;
    friend class FEM_ObjectBroker;
    TransformationConstraintHandler(ModelWrapper *);
    ConstraintHandler *getCopy(void) const;
  public:

    int handle(const ID *nodesNumberedLast =0);
    int applyLoad();
    void clearAll(void);    
    int enforceSPs(void);    
    int doneNumberingDOF(void);        

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace

#endif




