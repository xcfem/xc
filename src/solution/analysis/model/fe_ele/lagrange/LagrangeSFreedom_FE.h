// -*-c++-*-
//----------------------------------------------------------------------------
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
                                                                        
// $Revision: 1.3 $
// $Date: 2005/11/28 21:38:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/fe_ele/lagrange/LagrangeSFreedom_FE.h,v $
                                                                        
                                                                        
#ifndef LagrangeSFreedom_FE_h
#define LagrangeSFreedom_FE_h

// Written: fmk 
// Created: 02/99
// Revision: A
//
// Description: This file contains the class definition for LagrangeSFreedom_FE.
// LagrangeSFreedom_FE is a subclass of SFreedom_FE which handles
// SFreedom_Constraints using the Lagrange method.
//
// What: "@(#) LagrangeSFreedom_FE.h, revA"

#include <solution/analysis/model/fe_ele/SFreedom_FE.h>
#include "Lagrange_FE.h"

namespace XC {
class Element;
class Integrator;
class AnalysisModel;
class Domain;
class DOF_Group;

//! @ingroup AnalysisFE
//
//! @brief LagrangeSFreedom_FE is a subclass of SFreedom_FE
//! which handles SFreedom_Constraints using the Lagrange method.
//!
//! LagrangeSFreedom\_FE is a subclass of FE\_Element used to enforce a
//! single point constraint. It does this by adding to the tangent and the
//! residual:
//! \[ \left[ \begin{array}{cc} 0 & \alpha \alpha & 0 \end{array}
//! \right] ,
//! \left\{ \begin{array}{c} 0 \alpha(u_s - u_t) \end{array} \right\} \]
//! at the locations
//! corresponding to the constrained degree-of-freedom specified by the
//! SFreedom\_Constraint and the lagrange multiplier degree-of-freedom
//! introduced by the LagrangeConstraintHandler for this constraint, where
//! \f$U_s\f$ is the specified value of the constraint 
//! and \f$U_t\f$ the current trial displacement at the node corresponding to
//! the constraint.
class LagrangeSFreedom_FE: public SFreedom_FE, public Lagrange_FE
  {
  private:

    friend class AnalysisModel;
    LagrangeSFreedom_FE(int tag, Domain &theDomain, SFreedom_Constraint &theSP, DOF_Group &theDofGrp, double alpha = 1.0);
  public:

    // public methods
    virtual int setID(void);
    virtual const Matrix &getTangent(Integrator *theIntegrator);    
    virtual const Vector &getResidual(Integrator *theIntegrator);    
    virtual const Vector &getTangForce(const Vector &x, double fact = 1.0);

    virtual const Vector &getK_Force(const Vector &x, double fact = 1.0);
    virtual const Vector &getKi_Force(const Vector &x, double fact = 1.0);
    virtual const Vector &getC_Force(const Vector &x, double fact = 1.0);
    virtual const Vector &getM_Force(const Vector &x, double fact = 1.0);        
  };
} // end of XC namespace

#endif


