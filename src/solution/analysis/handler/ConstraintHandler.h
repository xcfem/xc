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
// $Date: 2005/08/31 17:28:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/handler/ConstraintHandler.h,v $
                                                                        
                                                                        
#ifndef ConstraintHandler_h
#define ConstraintHandler_h

// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for ConstraintHandler.
// ConstraintHandler is an abstract base class, i.e. no objects of it's
// type can be created. ConstraintHandlers enforce the single and multi point 
// constraints that exist in the domain by creating the appropriate FE_Element
// and DOF_Group objects.
//
// What: "@(#) ConstraintHandler.h, revA"

#include <utility/actor/actor/MovableObject.h>
#include "utility/kernel/CommandEntity.h"

namespace XC {
class AnalysisMethod;
class ID;
class Domain;
class AnalysisModel;
class Integrator;
class FEM_ObjectBroker;
class ModelWrapper;

//! @ingroup Analysis
//! 
//! @brief ConstraintHandlers enforce the single and multi freedom 
//! constraints that exist in the domain by creating the appropriate FE_Element
//! and DOF_Group objects.
//! 
//! A constraint handler is responsible for:
//! 
//!   - creating the FE_Element and DOF_Group objects and adding them to the AnalysisModel;
//! 
//!   - setting the initial DOF equation numbers to -1, -2 or -3. A -1 indicates to the DOF_Numberer object that no equation number is to be allocated for this DOF, a -3 that this DOF is to be among the last group of dof to be numbered.
//! 
//!   - deleting the DOF_Group and FE_Element objects that it created.
class ConstraintHandler: public MovableObject, public CommandEntity
  {
    ModelWrapper *getModelWrapper(void);
    const ModelWrapper *getModelWrapper(void) const;
  protected:
    const Domain *getDomainPtr(void) const;
    const AnalysisModel *getAnalysisModelPtr(void) const;
    const Integrator *getIntegratorPtr(void) const;
    Domain *getDomainPtr(void);
    AnalysisModel *getAnalysisModelPtr(void);
    Integrator *getIntegratorPtr(void);

    int sendData(Communicator &);
    int recvData(const Communicator &);

    friend class ModelWrapper;
    ConstraintHandler(ModelWrapper *,int classTag);
    virtual ConstraintHandler *getCopy(void) const= 0;
  public:
    inline virtual ~ConstraintHandler(void) {}
    // pure virtual functions
    //! @brief Invoked to handle the constraints imposed on the domain by the
    //! constraints.
    //!
    //! Invoked to handle the constraints imposed on the domain by the
    //! constraints. The ConstraintHandler object does
    //! this by instantiating the appropriate FE\_ELement and DOF\_Group objects 
    //! and adding them to the AnalysisModel. For all the dofs in each
    //! DOF\_Group the ConstraintHandler sets initial equation numbers as
    //! either \f$-1\f$, \f$-2\f$ or \f$-3\f$: A \f$-1\f$ indicates to the DOF\_Numberer object
    //! that no equation number is to be allocated for this dof, a \f$-2\f$ that
    //! an equation number is to be given for the dof, and a \f$-3\f$ that an
    //! equation number is to be allocated and that this dof is to
    //! be among the last group of dof to be numbered,i.e. all dof initially
    //! assigned a \f$-3\f$ are to be given a higher equation number than those
    //! given a \f$-2\f$. Those dof with a \f$-3\f$ should include all those dof
    //! associated with the nodes whose tags are in {\em
    //! nodesToBeNumberedLast}. Returns a positive number if successfully, a
    //! negative integer if not; the positive number is to be set at the
    //! number of dof assigned a value \f$-3\f$ (this will be the number of
    //! external dof for a subdomain), the negative value of which depends on
    //! the type of ConstraintHandler. For subdomains the constraint handler
    //! is responsible for setting the FE\_Element by calling {\em
    //! setFE\_elementPtr}.    
    virtual int handle(const ID *nodesNumberedLast =0) =0;
    virtual int update(void);
    virtual int applyLoad(void);
    virtual int doneNumberingDOF(void);
    virtual void clearAll(void);    
  };
} // end of XC namespace

#endif

