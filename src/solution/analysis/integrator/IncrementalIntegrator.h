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
                                                                        
// $Revision: 1.7 $
// $Date: 2003/03/06 20:32:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/IncrementalIntegrator.h,v $
                                                                        
                                                                        
#ifndef IncrementalIntegrator_h
#define IncrementalIntegrator_h

// File: ~/analysis/integrator/IncrementalIntegrator.h
// 
// Written: fmk 
// Created: Tue Sept 17 15:54:47: 1996
// Revision: A
//
// Description: This file contains the interface for IncrementalIntegrator. 
// IncrementalIntegrator is an algorithmic class for setting up the finite 
// element equations in an incremental analysis and for updating the nodal
// response quantities based on the values in the soln vector.
//
// What: "@(#) IncrementalIntegrator.h, revA"

#include <solution/analysis/integrator/Integrator.h>

namespace XC {
class LinearSOE;
class AnalysisModel;
class FE_Element;
class DOF_Group;
class Vector;

#define CURRENT_TANGENT 0
#define INITIAL_TANGENT 1
#define CURRENT_SECANT  2
#define INITIAL_THEN_CURRENT_TANGENT  3

//! @ingroup AnalysisIntegrator
//
//! @brief IncrementalIntegrator is an algorithmic class for setting up the finite 
//! element equations in an incremental analysis and for updating the nodal
//! response quantities based on the values in the soln vector.
class IncrementalIntegrator : public Integrator
  {
  protected:
    LinearSOE *getLinearSOEPtr(void);
    const LinearSOE *getLinearSOEPtr(void) const;

    friend class IntegratorVectors;
    virtual int formNodalUnbalance(void);        
    virtual int formElementResidual(void);
    int statusFlag;

    IncrementalIntegrator(SoluMethod *,int classTag);
  public:
    // methods to set up the system of equations
    virtual int formTangent(int statusFlag = CURRENT_TANGENT);    
    virtual int formUnbalance(void);

    // pure virtual methods to define the FE_ELe and DOF_Group contributions
    virtual int formEleTangent(FE_Element *theEle) =0;
    virtual int formNodTangent(DOF_Group *theDof) =0;    
    virtual int formEleResidual(FE_Element *theEle) =0;
    virtual int formNodUnbalance(DOF_Group *theDof) =0;    

    // methods to update the domain
    virtual int newStep(double deltaT);
    virtual int update(const Vector &deltaU) =0;
    virtual int commit(void);
    virtual int revertToLastStep(void);
    virtual int initialize(void);

// AddingSensitivity:BEGIN //////////////////////////////////
    virtual int revertToStart();
// AddingSensitivity:END ////////////////////////////////////
    
    // method introduced for domain decomposition
    virtual int getLastResponse(Vector &result, const ID &id);
    
  };
} // end of XC namespace

#endif

