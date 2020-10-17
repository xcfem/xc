// -*-c++-*-
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
// $Date: 2003/02/14 23:00:49 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/MinUnbalDispNorm.h,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/MinUnbalDispNorm.h
// 
// Written: fmk 
// Created: 07/99
// Revision: A
//
// Description: This file contains the class definition for MinUnbalDispNorm.
// MinUnbalDispNorm is an algorithmic class for performing a static analysis
// using the minimum unbalanced displacement norm (Chan IJNME 26(2657:2669)1988
//
// What: "@(#) MinUnbalDispNorm.h, revA"

#ifndef MinUnbalDispNorm_h
#define MinUnbalDispNorm_h

#include "DispBase.h"

namespace XC {

#define SIGN_LAST_STEP      1
#define CHANGE_DETERMINANT  2

//! @ingroup StaticIntegrator
//
//! @brief Specifies the incremental load factor such that the residual
//! displacement norm in minimized.
//!
//! MinUnbalDispNorm is a subclass of StaticIntegrator, it is
//! used to when performing a static analysis on the FE\_Model using the
//! minimum unbalanced displacement norm method. In this method WHAT
class MinUnbalDispNorm: public DispBase
  {
  private:
    mutable double dLambda1LastStep; //!< dLambda1 at step (i-1)

    mutable int signLastDeltaLambdaStep; //!< sign of dLambda(i-1)
    double dLambda1min, dLambda1max; //!< min & max values for dlambda1 at step (i) 
    mutable double signLastDeterminant;
    int signFirstStepMethod;
  protected:
    double getDLambdaNewStep(void) const;
    double getDLambdaUpdate(void) const;
    int sendData(Communicator &);
    int recvData(const Communicator &);


    friend class SolutionStrategy;
    MinUnbalDispNorm(SolutionStrategy *,double lambda1, int specNumIterStep, 
		     double dlambda1min, double dlambda1max,
		     int signFirstStepMethod = SIGN_LAST_STEP);
    Integrator *getCopy(void) const;
  public:

    int newStep(void);    
    int update(const Vector &deltaU);
    int domainChanged(void);
    
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);

  };
} // end of XC namespace

#endif

