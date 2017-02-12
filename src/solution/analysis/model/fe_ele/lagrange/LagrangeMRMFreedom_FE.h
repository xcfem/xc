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

// File: ~/analysis/model/fe_ele/lagrange/LagrangeMRMFreedom_FE.h
// 
// Written: lcpt 
// Created: 03/2015
// Revision: A
//
// Description: This file contains the class definition for LagrangeMRMFreedom_FE.
// LagrangeMRMFreedom_FE is a subclass of FE_Element which handles MRMFreedom_Constraints
// using the Lagrange method.
//
// What: "@(#) LagrangeMRMFreedom_FE.h, revA"


#ifndef LagrangeMRMFreedom_FE_h
#define LagrangeMRMFreedom_FE_h

#include "solution/analysis/model/fe_ele/MRMFreedom_FE.h"
#include "Lagrange_FE.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/Vector.h"

namespace XC {
class Element;
class Integrator;
class AnalysisModel;
class Domain;
class DOF_Group;

//! @ingroup AnalysisFE
//
//! @brief LagrangeMRMFreedom_FE is a subclass of FE_Element
//! which handles MRMFreedom_Constraints using the Lagrange method.
class LagrangeMRMFreedom_FE: public MRMFreedom_FE, public Lagrange_FE
  {
  private:
    void determineTangent(void);
    
    friend class AnalysisModel;
    LagrangeMRMFreedom_FE(int tag, Domain &theDomain, MRMFreedom_Constraint &,DOF_Group &, double alpha = 1.0);
  public:

    // public methods
    virtual int setID(void);
    virtual const Matrix &getTangent(Integrator *theIntegrator);    
    virtual const Vector &getResidual(Integrator *theIntegrator);    
    virtual const Vector &getTangForce(const Vector &x, double fact = 1.0);

    virtual const Vector &getK_Force(const Vector &x, double fact = 1.0);
    virtual const Vector &getC_Force(const Vector &x, double fact = 1.0);
    virtual const Vector &getM_Force(const Vector &x, double fact = 1.0);
  };
} // end of XC namespace

#endif


