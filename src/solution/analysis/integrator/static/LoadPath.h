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
// $Date: 2003/02/14 23:00:48 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/LoadPath.h,v $
                                                                        
                                                                        
#ifndef LoadPath_h
#define LoadPath_h

// File: ~/analysis/integrator/LoadPath.h
// 
// Written: fmk 
// Created: 05/99
// Revision: A
//
// Description: This file contains the class definition for LoadPath.
// LoadPath is an algorithmic class for perfroming a static analysis
// using a user defined load path (a user specified lambda path)
//
// What: "@(#) LoadPath.h, revA"

#include "solution/analysis/integrator/StaticIntegrator.h"
#include "utility/matrix/Vector.h"

namespace XC {
class LinearSOE;
class AnalysisModel;

//! @ingroup StaticIntegrator
//
//! @brief LoadPath is an algorithmic class for performing a static analysis
//! using a user defined load path (a user specified lambda path).
//!
//! LoadPath is a subclass of StaticIntegrator, it is
//! used to when performing a static analysis on the FE\_Model using a
//! user specified load path. The load path is specified in a Vector, {\em
//! path}, to the objects constructor and at each step in the analysis:
//! \[ 
//! \lambda_n^{(i)} - \lambda_{n-1} = path(n) - path(n-1)
//! \]
//! 
//! Knowing \f$\lambda_n^{(i)} = path(n)\f$ prior to each iteration, the \f$N+1\f$
//! unknowns in equation~\ref{staticFormTaylor}, is reduced to \f$N\f$ unknowns and
//! results in the following equation:
//! 
//! \begin{equation} 
//! \R(U_{n}) = \lambda_n^{(i)} P 
//!  - \f_{R}\left(U_{n}^{(i)} \right) - 
//! \K_n^{(i)} 
//! (U_{n} - U_{n}^{(i)})  
//! \label{staticFormLoadPath}
//! \end{equation}
class LoadPath: public StaticIntegrator
  {
  private:
    Vector loadPath; //!< vector that defines the load path.
    int currentStep; //!< current step number.
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    friend class AnalysisAggregation;
    LoadPath(AnalysisAggregation *);
    LoadPath(AnalysisAggregation *,const Vector &theLoadPath);
    Integrator *getCopy(void) const;
  public:

    int newStep(void);    
    int update(const Vector &deltaU);

    // Public methods for Output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace

#endif

