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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:00:48 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/LoadControl.h,v $
                                                                        
                                                                        
#ifndef LoadControl_h
#define LoadControl_h

// File: ~/analysis/integrator/LoadControl.h
// 
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for LoadControl.
// LoadControl is an algorithmic class for performing a static analysis
// using a load control integration scheme.
//
// What: "@(#) LoadControl.h, revA"

#include "solution/analysis/integrator/static/BaseControl.h"

namespace XC {

//! @ingroup StaticIntegrator
//
//! @brief Integratior used to perform a static analysis using the load
//! control method. Sets the incremental factor to apply on the loads for
//! each analysis step.
//!
//! \indent LoadControl is a subclass of StaticIntegrator, it is
//! used to when performing a static analysis on the FE\_Model using the
//! load control method. In the load control method, the following
//! constraint equation is added to equation~\ref{staticFormTaylor} of the
//! StaticIntegrator class: 
//!
//! \[ 
//! \lambda_n^{(i)} - \lambda_{n-1} = \delta \lambda_n
//! \]
//!
//! \noindent where \f$\delta \lambda_n\f$ depends on
//! \f$\delta \lambda_{n-1}\f$ the load increment at the previous time step,
//! \f$J_{n-1}\f$, the number of iterations required to achieve convergence in
//! the previous load step, and \f$Jd\f$, the desired number of iteraions.
//! \f$\delta \lambda_n\f$ is bounded by \f$\delta \lambda_{min}\f$
//! and \f$\delta \lambda_{max}\f$.
//!
//! \[ 
//! \delta \lambda_n = max \left( \delta \lambda_{min}, min \left(
//! \frac{Jd}{J_{n-1}} \delta \lambda_{n-1}, \delta \lambda_{max} \right)
//! \right)
//! \]
//!
//! Knowing \f$\lambda_n^{(i)}\f$ prior to each iteration, the \f$N+1\f$
//! unknowns in equation~\ref{staticFormTaylor}, is reduced to \f$N\f$
//! unknowns and results in the following equation:
//!
//! \begin{equation} 
//! R(U_{n}) = \lambda_n^{(i)} P 
//! - \f_{R}\left(U_{n}^{(i)} \right) - 
//! K_n^{(i)} 
//! (U_{n} - U_{n}^{(i)})  
//! \label{staticFormLoadControl}
//! \end{equation} 
class LoadControl: public BaseControl
  {
  private:
    double deltaLambda;  //!< Valor de dLambda en el paso (i-1).
    double dLambdaMin, dLambdaMax; //!< Maximum and minimum values for dlambda at i-th step.
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);


    friend class AnalysisAggregation;
    friend class FEM_ObjectBroker;
    LoadControl(AnalysisAggregation *,double deltaLambda= 1.0, int numIncr= 1, double minLambda=1.0, double maxlambda=1.0);
    Integrator *getCopy(void) const;
  public:

    int newStep(void);    
    int update(const Vector &deltaU);
    void setDeltaLambda(const double &);
    inline double getDeltaLambda(void) const
      { return deltaLambda; }
    void setDeltaLambdaMin(const double &);
    inline double getDeltaLambdaMin(void) const
      { return dLambdaMin; }
    void setDeltaLambdaMax(const double &);
    inline double getDeltaLambdaMax(void) const
      { return dLambdaMax; }

    // Public methods for Output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0) const;    
  };
inline Integrator *LoadControl::getCopy(void) const
  { return new LoadControl(*this); }
} // end of XC namespace

#endif

