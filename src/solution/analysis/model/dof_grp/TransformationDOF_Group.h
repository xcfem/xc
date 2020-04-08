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
                                                                        
// $Revision: 1.5 $
// $Date: 2005/08/03 19:12:26 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/dof_grp/TransformationDOF_Group.h,v $
                                                                        
                                                                        
#ifndef TransformationDOF_Group_h
#define TransformationDOF_Group_h

// Written: fmk 
// Created: 05/99
// Revision: A
//
// Description: This file contains the class definition for 
// TransformationDOF_Group. A TransformationDOF_Group object is 
// instantiated by the TransformationConstraintHandler for 
// every node in the domain which is constrained by an MFreedom_Constraint
// or an SFreedom_Constrant.
//
// What: "@(#) TransformationDOF_Group.h, revA"

#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include "utility/matrix/Matrix.h"
#include "solution/analysis/UnbalAndTangent.h"

namespace XC {
class MFreedom_ConstraintBase;
class MFreedom_Constraint;
class MRMFreedom_Constraint;
class SFreedom_Constraint;
class TransformationConstraintHandler;

//! @ingroup AnalysisDOF
//
//! @brief A TransformationDOF_Group object is 
//! instantiated by the TransformationConstraintHandler for 
//! every node in the domain which is constrained by an MFreedom_Constraint
//! or an SFreedom_Constrant.
//! This object stores the transformation matrix \f$T\f$ used by the
//! TransformationFE objects
class TransformationDOF_Group: public DOF_Group
  {
  private:
    MFreedom_ConstraintBase *mfc; //!< Pointer to multi-freedom constraint.
    
    Matrix Trans;
    ID modID;
    int modNumDOF;
    UnbalAndTangent unbalAndTangentMod;
    
    std::vector<SFreedom_Constraint *> theSPs; //!< Pointers to single-freedom constraints.
    
    // static variables - single copy for all objects of the class	    
    static UnbalAndTangentStorage unbalAndTangentArrayMod; //!< array of class wide vectors and matrices
    static int numTransDOFs; //!< number of objects        
    static TransformationConstraintHandler *theHandler; //!< Transformation constraint handler.

    void arrays_setup(int numNodalDOF, int numConstrainedNodeRetainedDOF, int numRetainedNodeDOF);
    void initialize(TransformationConstraintHandler *);
  protected:
    friend class AnalysisModel;
    TransformationDOF_Group(int tag, Node *myNode, MFreedom_ConstraintBase *, TransformationConstraintHandler*);
    TransformationDOF_Group(int tag, Node *myNode, TransformationConstraintHandler *);
    std::vector<SFreedom_Constraint *> getSFreedomConstraintArray(int ) const;
    MFreedom_ConstraintBase *getMFreedomConstraint(void);
    const MFreedom_ConstraintBase *getMFreedomConstraint(void) const;
    size_t getNumRetainedNodes(void) const;
    size_t getNumRetainedNodeDOFs(void) const;
    std::vector<Node *> getPointersToRetainedNodes(void);
    const Vector &setupResidual(int numCNodeDOF, const ID &,const ID &, const Vector &, const std::vector<Node *> &,const Vector &(Node::*response)(void) const);
    const Vector &getCommittedResponse(const Vector &(Node::*response)(void) const);
    void setupResidual(const Vector &,int (Node::*setTrial)(const Vector &));
  public:
    ~TransformationDOF_Group();
    
    // methods dealing with the ID and transformation matrix
    int doneID(void);    
    const ID &getID(void) const; 
    virtual void setID(int dof, int value);    
    Matrix *getT(void);
    virtual int getNumDOF(void) const;    
    virtual int getNumFreeDOF(void) const;
    virtual int getNumConstrainedDOF(void) const;
    
    // methods to form the tangent
    const Matrix &getTangent(Integrator *theIntegrator);

    // methods to form the unbalance
    const Vector &getUnbalance(Integrator *theIntegrator);
    void  addM_Force(const Vector &Udotdot, double fact = 1.0);        

    const Vector &getTangForce(const Vector &x, double fact = 1.0);
    const Vector &getC_Force(const Vector &x, double fact = 1.0);
    const Vector &getM_Force(const Vector &x, double fact = 1.0);
    
    // methods to obtain trial responses from the nodes
    const Vector &getTrialDisp(void);
    const Vector &getTrialVel(void);
    const Vector &getTrialAccel(void);

    // methods to obtain committed responses from the nodes
    const Vector &getCommittedDisp(void);
    const Vector &getCommittedVel(void);
    const Vector &getCommittedAccel(void);
    
    // methods to update the trial response at the nodes
    void setNodeDisp(const Vector &u);
    void setNodeVel(const Vector &udot);
    void setNodeAccel(const Vector &udotdot);

    void incrNodeDisp(const Vector &u);
    void incrNodeVel(const Vector &udot);
    void incrNodeAccel(const Vector &udotdot);

    virtual void setEigenvector(int mode, const Vector &eigenvalue);

    int addSFreedom_Constraint(SFreedom_Constraint &theSP);
    int enforceSPs(void);

// AddingSensitivity:BEGIN ////////////////////////////////////
    void addM_ForceSensitivity(const Vector &Udotdot, double fact = 1.0);        
    void addD_ForceSensitivity(const Vector &vel, double fact = 1.0);
    void addD_Force(const Vector &vel, double fact = 1.0);

    const Vector & getDispSensitivity(int gradNumber);
    const Vector & getVelSensitivity(int gradNumber);
    const Vector & getAccSensitivity(int gradNumber);
    int saveSensitivity(Vector *v,Vector *vdot,Vector *vdotdot,int gradNum,int numGrads);
// AddingSensitivity:END //////////////////////////////////////
  };
} // end of XC namespace

#endif

