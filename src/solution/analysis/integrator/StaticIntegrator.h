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
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/StaticIntegrator.h,v $
                                                                        
                                                                        
#ifndef StaticIntegrator_h
#define StaticIntegrator_h

// File: ~/analysis/integrator/StaticIntegrator.h
// 
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for StaticIntegrator.
// StaticIntegrator is an algorithmic class for setting up the finite element
// equations for a static analysis and for Incrementing the nodal displacements
// with the values in the soln vector to the LinearSOE object. 
//
// What: "@(#) StaticIntegrator.h, revA"

#include <solution/analysis/integrator/IncrementalIntegrator.h>

namespace XC {
class LinearSOE;
class AnalysisModel;
class FE_Element;
class Vector;

//! @ingroup AnalisisIntegrator
//
//! @defgroup StaticIntegrator Integración del system of equations estático.
//
//! @ingroup StaticIntegrator
//
//! @brief determine the next step for an analysis
class StaticIntegrator: public IncrementalIntegrator
  {
  protected:
    StaticIntegrator(SoluMethod *,int classTag);
  public:
    inline virtual ~StaticIntegrator(void) {}
    // methods which define what the FE_Element and DOF_Groups add
    // to the system of equation object.
    virtual int formEleTangent(FE_Element *theEle);
    virtual int formEleResidual(FE_Element *theEle);
    virtual int formNodTangent(DOF_Group *theDof);        
    virtual int formNodUnbalance(DOF_Group *theDof);    
    
    virtual int newStep(void) =0;
  };
} // end of XC namespace

#endif

