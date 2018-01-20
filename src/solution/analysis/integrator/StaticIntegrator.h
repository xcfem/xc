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

//! @ingroup AnalysisIntegrator
//
//! @defgroup StaticIntegrator Integrators for static problems (Kx= F).
//
//! @ingroup StaticIntegrator
//
//! @brief Base class for static integrators.
//!
//! StaticIntegrator is an IncrementalIntegrator provided to implement the
//! common methods among integrator classes used in performing a static
//! analysis on the FE\_Model. The StaticIntegrator class provides an
//! implementation of the methods to form the FE\_Element and DOF\_Group
//! contributions to the tangent and residual. A pure virtual method newStep()
//! is also defined in the interface, this is the method first called at each
//! iteration in a static analysis, see the StaticAnalysis class.
//! 
//! In static nonlinear finite element problems we seek a solution
//! (\f$\mathbf{U}\f$, \f$\lambda\f$) to the nonlinear vector function
//! 
//! \begin{equation}
//! R(U, \lambda) = \lambda P - F_R(U) = \zero
//! \label{staticGenForm}
//! \end{equation}
//! 
//! The most widely used technique for solving the non-linear finite
//! element equation, equation~\ref{femGenForm}, is to use an incremental
//! scheme. In the incremental formulation, a solution to the equation is
//! sought at successive incremental steps.  
//! 
//! \begin{equation}
//! R(U_{n}, \lambda_n) = \lambda_n P - F_R(U_{n})
//! \label{staticIncForm}
//! \end{equation}
//! 
//! The solution of this equation is typically obtained using an iterative
//! procedure, in which a sequence of approximations
//! (\f$U_{n}^{(i)}\f$, \f$\lambda_n^{(i)}\f$), \f$i=1,2, ..\f$ is obtained
//! which converges to the solution (\f$U_n\f$, \f$\lambda_n)\f$. The most
//! frequently used iterative schemes, such as Newton-Raphson, modified
//! Newton, and quasi Newton schemes, are based on a Taylor expansion of
//! equation~\ref{staticIncForm} about (\f$U_{n}\f$, \f$\lambda_n\f$):     
//! 
//! \begin{equation} 
//! R(U_{n},\lambda_n) = \lambda_n^{(i)} P 
//!  - \f_{R}\left(U_{n}^{(i)} \right) - \left[
//! \begin{array}{cc}
//! K_n^{(i)} & -P
//! \end{array} \right] 
//! \left\{
//! \begin{array}{c}
//! U_{n} - U_{n}^{(i)} 
//! \lambda_n - \lambda_n^{(i)} 
//! \end{array} \right\}
//! \label{staticFormTaylor}
//! \end{equation} 
//! 
//! \noindent which  a system of of \f$N\f$ equations with (\f$N+1\f$)
//! unknowns. Two solve this, an additional equation is required, the
//! constraint equation. The constraint equation used depends on the
//! static integration scheme, of which there are a number, for example
//! load control, arc length, and displacement control.
class StaticIntegrator: public IncrementalIntegrator
  {
  protected:
    StaticIntegrator(AnalysisAggregation *,int classTag);
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

