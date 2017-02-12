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
// $Date: 2004/10/12 21:52:25 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/dof_grp/LagrangeDOF_Group.h,v $
                                                                        
                                                                        
#ifndef LagrangeDOF_Group_h
#define LagrangeDOF_Group_h

// File: ~/analysis/model/dof_grp/LagrangeDOF_Group.h
// 
// Written: fmk 
// Created: 02/99
// Revision: A
//
// Description: This file contains the class definition for LagrangeDOF_Group.
// A LagrangeDOF_Group object is instantiated by a LagrangeConstraintHandler for 
// every constrained node in the domain. 
//
// What: "@(#) LagrangeDOF_Group.h, revA"

#include <solution/analysis/model/dof_grp/DOF_Group.h>
namespace XC {
class SFreedom_Constraint;
class MFreedom_Constraint;
class MRMFreedom_Constraint;

//! @ingroup AnalysisDOF
//
//! @brief A LagrangeDOF_Group object is instantiated 
//! by a LagrangeConstraintHandler for 
//! every constrained node in the domain. 
class LagrangeDOF_Group: public DOF_Group
  {
  protected:
    friend class AnalysisModel;
    LagrangeDOF_Group(int tag, SFreedom_Constraint &);    
    LagrangeDOF_Group(int tag, MFreedom_Constraint &);        
    LagrangeDOF_Group(int tag, MRMFreedom_Constraint &);        
  public:
    // methods to form the tangent
    virtual const Matrix &getTangent(Integrator *theIntegrator);

    // methods to form the unbalance
    virtual const Vector &getUnbalance(Integrator *theIntegrator);

    // methods to obtain committed responses .. always 0
    virtual const Vector &getCommittedDisp(void);
    virtual const Vector &getCommittedVel(void);
    virtual const Vector &getCommittedAccel(void);
    
    // methods to update the trial response at the nodes
    virtual void setNodeDisp(const Vector &u);
    virtual void setNodeVel(const Vector &udot);
    virtual void setNodeAccel(const Vector &udotdot);

    virtual void incrNodeDisp(const Vector &u);
    virtual void incrNodeVel(const Vector &udot);
    virtual void incrNodeAccel(const Vector &udotdot);

    virtual void  zeroTangent(void);
    virtual void  addMtoTang(double fact = 1.0);    
    virtual void  zeroUnbalance(void);
    virtual void  addPtoUnbalance(double fact = 1.0);
    virtual void  addPIncInertiaToUnbalance(double fact = 1.0);    
    virtual void  addM_Force(const Vector &Udotdot, double fact = 1.0);        

    virtual const Vector &getTangForce(const Vector &x, double fact = 1.0);
    virtual const Vector &getC_Force(const Vector &x, double fact = 1.0);
    virtual const Vector &getM_Force(const Vector &x, double fact = 1.0);
  };
} // end of XC namespace

#endif

