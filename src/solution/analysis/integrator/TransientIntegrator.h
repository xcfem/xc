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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/03/06 20:32:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/TransientIntegrator.h,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/TransientIntegrator.h
// 
// Written: fmk 
// Created: Tue Sept 17 15:54:47: 1996
// Revision: A
//
// Description: This file contains the class definition for TransientIntegrator.
// TransientIntegrator is an algorithmic class for setting up the finite element
// equations for a static analysis and for Incrementing the nodal displacements
// with the values in the soln vector to the LinearSOE object. 
//
// What: "@(#) TransientIntegrator.h, revA"

#ifndef TransientIntegrator_h
#define TransientIntegrator_h

#include <solution/analysis/integrator/IncrementalIntegrator.h>
namespace XC {
class Information;
class LinearSOE;
class AnalysisModel;
class FE_Element;
class DOF_Group;
class Vector;

//! @ingroup AnalysisIntegrator
//
//! @defgroup TransientIntegrator Integration of the dynamic equations of motion.
//
//! @ingroup TransientIntegrator
//
//! @brief Base class for dynamic equations of motion integrators.
//!
//!TransientIntegrator is an abstract subclass of IncrementalIntegrator.
//! A subclass of it is used when performing a nonlinear transient
//! analysis of the problem using a direct integration method. The
//! TransientIntegrator class redefines the formTangent() method of
//! the IncrementalIntegrator class and it defines a new method {\em
//! newStep()} which is invoked by the DirectIntegrationAnalysis class at
//! each new time step.
//! 
//! In nonlinear transient finite element problems we seek a solution
//! (\f$U\f$, \f$\dot U\f$, \f$\ddot U\f$) to the nonlinear vector function
//! 
//! \begin{equation}
//! R(U,Ud, Udd) = P(t) - F_I(Udd) - F_R(U, Ud) = \zero
//! \label{femGenForm}
//! \end{equation}
//!
//! 
//! The most widely used technique for solving the transient non-linear 
//! finite element equation, equation~\ref{femGenForm}, is to use an
//! incremental direct integration scheme. In the incremental formulation,
//! a solution to the equation is sought at successive time steps \f$\Delta
//! t\f$ apart.  
//! 
//! \begin{equation}
//! R(U_{n \Delta t},Ud_{n \Delta t}, Udd_{n \Delta t}) = P(n \Delta t) -
//! F_I(Udd_{n \Delta t}) - F_R(U_{n \Delta t}, Ud_{n \Delta t})
//! \label{fullTimeForm}
//! \end{equation}
//! 
//! For each time step, t, the integration schemes provide two operators,
//! \f$I_1\f$ and \f$I_2\f$, to relate the velocity and accelerations at the 
//! time step as a function of the displacement at the time step and the
//! response at previous time steps: 
//! 
//! \begin{equation} 
//! \dot U_{t} = {I}_1 (U_t, U_{t-\Delta t}, \dot U_{t-\Delta t},
//! \ddot U_{t - \Delta t}, U_{t - 2\Delta t}, \dot U_{t - 2 \Delta t}. ..., )
//! \label{I1}
//! \end{equation} 
//! 
//! \begin{equation} 
//! \ddot U_{t} = {I}_2 (U_t, U_{t-\Delta t}, \dot U_{t-\Delta t},
//! \ddot U_{t - \Delta t}, U_{t - 2\Delta t}, \dot U_{t - 2 \Delta t}. ..., )
//! \label{I2}
//! \end{equation} 
//! 
//! These allow us to rewrite equation~\ref{fullTimeForm}, in terms of a
//! single response quantity, typically the displacement:
//! 
//! \begin{equation}
//! R(U_t) = P(t) - F_I(Udd_t) - F_R(U_t, Ud_t)
//! \label{genForm}
//! \end{equation}
//! 
//! The solution of this equation is typically obtained using an iterative
//! procedure, i.e. making an initial prediction for \f$U_{t}\f$,
//! denoted \f$U_{t}^{(0)}\f$ a sequence of approximations \f$U_{t}^{(i)}\f$,
//! \f$i=1,2, ..\f$ is obtained which converges (we hope) to the solution \f$U_{t}\f$. The
//! most frequently used iterative schemes, such as Newton-Raphson,
//! modified Newton, and quasi Newton schemes, are based on a Taylor
//! expansion of equation~\ref{genForm} about \f$U_{t}\f$:    
//! 
//! \begin{equation} 
//! R(U_{t}) = 
//! R(U_{t}^{(i)}) +
//! \left[ {\frac{\partial R}{\partial U_t} \vert}_{U_{t}^{(i)}}\right]
//! \left( U_{t} - U_{t}^{(i)} \right) 
//! \end{equation}
//! 
//! \f$\f$
//! R(U_{t}) = 
//!  P (t) 
//!  - \f_{I} \left( \ddot U_{t}^{(i)} \right) -
//! \f_{R} \left( \dot U_{t}^{(i)}, U_{t}^{(i)} \right)
//! \f$\f$
//! \begin{equation} 
//! - \left[
//!    M^{(i)} {I}_2'
//! +  C^{(i)} {I}_1'
//! + K^{(i)}  \right]
//!  \left( U_{t} - U_{t}^{(i)} \right)
//! \label{femGenFormTaylor}
//! \end{equation} 
//! 
//! To start the iteration scheme, trial values for \f$U_{t}\f$, \f$\dot
//! U_{t} \f$ and \f$\ddot U_{t} \f$ are required. These are
//! obtained by assuming \f$U_{t}^{(0)} = U_{t-\Delta t}\f$. The
//! \f$\dot U_{t}^{(0)} \f$ and \f$\ddot U_{t}^{(0)}\f$
//! can then be obtained from the operators for the integration scheme.
//! 
//! Subclasses of TransientIntegrators provide
//! methods informing the FE\_Element and DOF\_Group objects how to build
//! the tangent and residual matrices and vectors. They also provide the
//! method for updating the response quantities at the DOFs with
//! appropriate values; these values being some function of the solution
//! to the linear system of equations. 
class TransientIntegrator : public IncrementalIntegrator
  {
  protected:
    TransientIntegrator(AnalysisAggregation *,int classTag);
  public:

    virtual int formTangent(int statFlag);
    virtual int formEleResidual(FE_Element *theEle);
    virtual int formNodUnbalance(DOF_Group *theDof);    
    virtual int initialize(void) {return 0;};    
  };
} // end of XC namespace

#endif

