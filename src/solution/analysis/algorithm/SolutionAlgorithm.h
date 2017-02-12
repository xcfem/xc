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
// $Date: 2004/11/13 08:08:14 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/SolutionAlgorithm.h,v $
                                                                        
                                                                        
#ifndef SolutionAlgorithm_h
#define SolutionAlgorithm_h

// File: ~/OOP/analysis/algorithm/SolutionAlgorithm.h
// 
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for SolutionAlgorithm.
// SolutionAlgorithm is an abstract base class, i.e. no objects of it's
// type can be created. 
//
// What: "@(#) SolutionAlgorithm.h, revA"

#include <utility/actor/actor/MovableObject.h>
#include "utility/recorder/ObjWithRecorders.h"
#include "utility/handler/DataOutputHandler.h"

namespace XC {
class FEM_ObjectBroker;
class AnalysisModel;
class Integrator;
class SystemOfEqn;
class Recorder;
class SoluMethod;

//! @ingroup Analysis
//
//! @defgroup AnalAlgo Analysis algorithms.
//
//! @ingroup AnalAlgo
//
//! @brief Base class for solution algorithms.
class SolutionAlgorithm: public MovableObject, public ObjWithRecorders
  {
  protected:
    SoluMethod *getSoluMethod(void);
    const SoluMethod *getSoluMethod(void) const;

    virtual Domain *get_domain_ptr(void);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);


    SolutionAlgorithm(SoluMethod *,int classTag);
    friend class SoluMethod;
    friend class FEM_ObjectBroker;
    virtual SolutionAlgorithm *getCopy(void) const= 0;
  public:
    virtual int domainChanged(void);

    virtual AnalysisModel *getAnalysisModelPtr(void);
    virtual const AnalysisModel *getAnalysisModelPtr(void) const;
    virtual Integrator *getIntegratorPtr(void);
    virtual const Integrator *getIntegratorPtr(void) const;
    virtual LinearSOE *getLinearSOEPtr(void);
    virtual const LinearSOE *getLinearSOEPtr(void) const;
    virtual EigenSOE *getEigenSOEPtr(void);
    virtual const EigenSOE *getEigenSOEPtr(void) const;
    virtual const DomainSolver *getDomainSolverPtr(void) const;
    virtual DomainSolver *getDomainSolverPtr(void);
    virtual const Subdomain *getSubdomainPtr(void) const;
    virtual Subdomain *getSubdomainPtr(void);
  };
} // end of XC namespace

#endif


