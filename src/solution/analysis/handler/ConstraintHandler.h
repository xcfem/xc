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
#include "xc_utils/src/nucleo/EntCmd.h"

namespace XC {
class AnalysisMethod;
class ID;
class Domain;
class AnalysisModel;
class Integrator;
class FEM_ObjectBroker;
class ModelWrapper;

//! @ingroup Analisis
//
//
//! @defgroup AnalisisCH Analysis constraint handlers.
//
//! @ingroup AnalisisCH
//! @brief ConstraintHandlers enforce the single and multi point 
//! constraints that exist in the domain by creating the appropriate FE_Element
//! and DOF_Group objects.
class ConstraintHandler: public MovableObject, public EntCmd
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

    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    friend class ModelWrapper;
    ConstraintHandler(ModelWrapper *,int classTag);
    virtual ConstraintHandler *getCopy(void) const= 0;
  public:
    inline virtual ~ConstraintHandler(void) {}
    // pure virtual functions
    virtual int handle(const ID *nodesNumberedLast =0) =0;
    virtual int update(void);
    virtual int applyLoad(void);
    virtual int doneNumberingDOF(void);
    virtual void clearAll(void);    
  };
} // end of XC namespace

#endif

