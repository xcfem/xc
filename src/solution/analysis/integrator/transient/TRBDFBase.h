// -*-c++-*-
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

// $Revision: 1.1 $
// $Date: 2009-03-20 18:36:30 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/TRBDFBase.h,v $

#ifndef TRBDFBase_h
#define TRBDFBase_h

// Written : fmk 
// Created : 02/09
//
// Description: This file contains the class definition for TRBDFBase.
// TRBDFBase is the base class for TRBDF2 and TRBDF3 integration scheme.
// ref: K.J.Bathe, "Conserving Energy and Momentum in Nonlinear Dynamics: A Simple
//      Implicit Time Integration Scheme", Computers and Structures 85(2007),437-445
//
// note: the implementation does not do sub-step, it just alternates between trapezoidal
// and euler methods, if user specifies dt/2 step size result will be as per paper.
//
// What: "@(#) TRBDFBase.h, revA"

#include <solution/analysis/integrator/TransientIntegrator.h>
#include "ResponseQuantities.h"

namespace XC {

class TRBDFBase : public TransientIntegrator
  {
  protected:
    int step; //!< a flag indicating whether trap or euler step
    double dt; //!< last dt, if not same as previous we do trapezoidal step
    
    double c1, c2, c3; //!< some constants we need to kee
    ResponseQuantities Utm1; //!< response quantities at time t-1
    ResponseQuantities Ut; //!< response quantities at time t
    ResponseQuantities U; //!< response quantities at time t+deltaT

    virtual void resize(const size_t &);
    int sendData(Communicator &);
    int recvData(const Communicator &);
    
  public:
    TRBDFBase(SolutionStrategy *,int classTag);
    
    // methods which define what the FE_Element and DOF_Groups add
    // to the system of equation object.
    int formEleTangent(FE_Element *theEle);
    int formNodTangent(DOF_Group *theDof);        
    
    int domainChanged(void);    
    int revertToLastStep(void);        

    const Vector &getVel(void);
    
    void Print(std::ostream &, int flag = 0);
  };
} // end of XC namespace

#endif
