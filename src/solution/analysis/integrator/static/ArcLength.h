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
// $Date: 2003/02/14 23:00:46 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/ArcLength.h,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/ArcLength.h
// 
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for ArcLength.
// ArcLength is an algorithmic class for perfroming a static analysis
// using the arc length scheme, that is within a load step the follwing
// constraint is enforced: 
//  i=1        delta U^T delta U + alpha^2 delta lambda^2 = delta s^2
//  i>1        dU^T delta U + alpha^2 dLambda delta lambda = 0
// where dU is change in nodal displacements for step, dLambda is
// change in applied load and arcLength is a control parameter.
//
// What: "@(#) ArcLength.h, revA"

#ifndef ArcLength_h
#define ArcLength_h

#include <solution/analysis/integrator/static/ArcLengthBase.h>

namespace XC {

//! @ingroup StaticIntegrator
//!
//! @brief  ArcLength is an algorithmic class for perfroming a static analysis
//! using the arc length scheme.
//!
//!ArcLength is a subclass of StaticIntegrator, it is
//! used to when performing a static analysis on the FE\_Model using an
//! arc length method. In the arc length method implemented by this class,
//! the following constraint equation is added to
//! equation~\ref{staticFormTaylor} of the StaticIntegrator class: 
//! 
//! \begin{equation}
//! \Delta U_n^T \Delta U_n  + \alpha^2 \Delta \lambda_n^2  = \Delta s^2
//! \end{equation}
//! 
//! where 
//! 
//! \[
//! \Delta U_n = \sum_{j=1}^{i} \Delta U_n^{(j)} = \Delta U_n^{(i)} +
//! dU^{(i)} 
//! \]
//!
//! \[
//! \Delta \lambda_n = \sum_{j=1}^{i} \Delta \lambda_n^{(j)} = \Delta \lambda_n^{(i)} +
//! d\lambda^{(i)} 
//! \]
//!
//! \noindent this equation cannot be added directly into
//! equation~\ref{staticFormTaylor} to produce a linear system of \f$N+1\f$
//! unknowns. To add this equation we make some assumptions ala Yang
//! (REF), which in so doing allows us to solve a system of \f$N\f$
//! unknowns using the method of ??(REF).  Rewriting
//! equation~\ref{staticFormTaylor} as  
//!
//! \[
//! K_n^{(i)} \Delta U_n^{(i)} = \Delta \lambda_n^{(i)} P +
//! \lambda_n^{(i)} P - F_R(U_n^{(i)}) = \Delta \lambda_n^{(i)} P
//! + R_n^{(i)}
//! \]
//!
//! \noindent The idea of ?? is to separate this into two equations:
//!
//! \def Uh{\dot{\bf U}}
//! \def Ub{\overline{\bf U}}
//!
//! \[
//!  K_n^{(i)} \Delta  Uh_n^{(i)} =  P
//! \]
//!
//! \[
//!  K_n^{(i)} \Delta  Ub_n^{(i)} =  R_n^{(i)}
//! \]
//!
//! \noindent where now
//!
//! \begin{equation}
//! \Delta  U_n^{(i)} = \Delta \lambda_n^{(i)} \Delta  Uh_n^{(i)}
//! + \Delta  Ub_n^{(i)}  
//! \label{splitForm}
//! \end{equation}
//!
//! \noindent We now rewrite the constraint equation based on two conditions:
//!
//! \begin{enumerate}
//! \item {\bf \f$i = 1\f$}: assuming \f$ R_n^{(1)} = \zero\f$, i.e. the
//! system i in equilibrium at the start of the iteration, the following is
//! obtained
//!
//! \[ \Delta  U_n^{(1)} = \Delta \lambda_n^{(1)} \Delta  Uh_n^{(1)} + \zero \]
//!
//! \[ \Delta \lambda_n^{(1)} = \begin{array}{c} + - \end{array}
//! \sqrt{\frac{\Delta s^2}{ Uh^T  Uh + \alpha^2}} \]
//!
//! \noindent The question now is whether {\bf +} or {\bf -} should be
//! used. In this class, \f$d \lambda\f$ from the previous iteration \f$(n-1)\f$
//! is used, if it was positive {\bf +} is assumed, otherwise {\bf -}. This may
//! change. There are other ideas: ?(REF) number of negatives on diagonal
//! of decomposed matrix, ...
//!
//! \item {\bf \f$i > 1\f$}
//!
//! \[ \left( \Delta  U_n^{(i)} + dU^{(i)} \right)^T \left( \Delta  U_n^{(i)} +
//! dU^{(i)} \right) + \alpha^2 \left( \Delta \lambda_n^{(i)} + d\lambda^{(i)}
//! \right)^2 = \Delta s^2 \]
//!
//! \[
//! \Delta { U_n^{(i)}}^T\Delta  U_n^{(i)} + 2{dU^{(i)}}^T\Delta  U_n^{(i)} + {dU^{(i)}}^T dU^{(i)}
//! + \alpha^2 \Delta {\lambda_n^{(i)}}^2
//! + 2 \alpha^2 d\lambda^{(i)} \Delta \lambda_n^{(i)} + \alpha^2
//! {d\lambda^{(i)}}^2
//! = \Delta s^2
//! \]
//!
//! \noindent assuming the constraint equation was solved at \f$i-1\f$,
//! i.e. \f${dU^{(i)}}^T dU^{(i)} + \alpha^2 {d\lambda^{(i)}}^2 = \Delta
//! s^2\f$, this reduces to
//!
//! \[
//! \Delta { U_n^{(i)}}^T\Delta  U_n^{(i)} + 2{dU^{(i)}}^T\Delta  U_n^{(i)} + 
//! \alpha^2 \Delta {\lambda_n^{(i)}}^2
//! + 2 \alpha^2 d\lambda^{(i)} \Delta \lambda_n^{(i)} 
//! = 0
//! \]
//!
//! \noindent substituting for \f$\Delta { U_n^{(i)}} \f$ using
//! equation~\ref{splitForm} this can be expressed as:
//!
//! \[
//! \Delta \lambda_n^{(i)^2} \left( \Delta  Uh_n^{(i)} \Delta  Uh_n^{(i)} +
//! \alpha^2 \right) +
//! 2* \Delta \lambda_n^{(i)} \left( \Delta  Uh_n^{(i)} \Delta  Ub_n^{(i)}
//! + dU^{(i)} \Delta  Uh_n^{(i)} 
//! + \alpha^2d \lambda^{(i)} \right)
//! \]
//! \[
//! + \left (\Delta  Ub_n^{(i)} \Delta  Ub_n^{(i)} + dU^{(i)} \Delta
//!  Ub_n^{(i)}
//! \right) =0 
//! \]
//!
//! which is a quadratic in \f$\Delta \lambda_n^{(i)}\f$, which can be solved
//! for two roots. The root chosen is the one which will keep a positive angle
//! between the incremental displacement before and after this step.
class ArcLength : public ArcLengthBase
  {
  protected:
    virtual double getDLambdaUpdate(void) const;

    friend class AnalysisAggregation;
    friend class FEM_ObjectBroker;
    ArcLength(AnalysisAggregation *,double arcLength, double alpha = 1.0);
    Integrator *getCopy(void) const;
  public:
    void Print(std::ostream &s, int flag =0);    
  };
} // end of XC namespace

#endif

