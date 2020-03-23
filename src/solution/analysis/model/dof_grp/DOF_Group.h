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
                                                                        
// $Revision: 1.11 $
// $Date: 2005/11/28 21:40:46 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/dof_grp/DOF_Group.h,v $
                                                                        
                                                                        
#ifndef DOF_Group_h
#define DOF_Group_h

// File: ~/analysis/model/dof_grp/DOF_Group.h
// 
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for DOF_Group.
// A DOF_Group object is instantiated by the ConstraintHandler for 
// every unconstrained node in the domain. The constrained nodes require 
// specialised types of DOF_Group; which deal with the constraints. DOF_Group
// objects can handle 0 boundary constraints; if the eqn number of a DOF is 
// less than START_EQN_NUM a value of 0.0 is set for disp, vel and accel when
// a setNode*(Vector &) is invoked.
//
// What: "@(#) DOF_Group.h, revA"

#include <utility/matrix/ID.h>
#include <utility/tagged/TaggedObject.h>
#include <vector>
#include "solution/analysis/UnbalAndTangent.h"

namespace XC {
class Node;
class Vector;
class Matrix;
class TransientIntegrator;
class Integrator;

//! @ingroup Analysis
//!
//! @defgroup AnalysisDOF DOF groups as seen by the analysis.
//
//! @ingroup AnalysisDOF
//
//! @brief A DOF_Group object is instantiated by the ConstraintHandler for 
//! every unconstrained node in the domain. The constrained nodes require 
//! specialised types of DOF_Group; which deal with the constraints. DOF_Group
//! objects can handle 0 boundary constraints; if the eqn number of a DOF is 
//! less than START_EQN_NUM a value of 0.0 is set for disp, vel and accel when
//! a setNode*(Vector &) is invoked.
//!
//! Each node in the domain is associated with one DOF\_Group. DOF\_Groups are
//! called upon in the analysis to provide their contributions of
//! unbalanced load to the system of equations. Subclasses are used by the
//! constraint handler to to introduce new dofs into the analysis. 
//!
//! The DOF\_Group is responsible for providing operations to set and
//! access the mapping betwwen equation numbers and DOFs, allowing the
//! Integrator to from the tangent (if nodal masses in transient problem)
//! and unbalanced load information, and for setting and obtaining the
//! nodal trial response quantities.
class DOF_Group: public TaggedObject
  {
  private:
    // private variables - a copy for each object of the class        
    ID 	myID;

    // static variables - single copy for all objects of the class	    
    static Matrix errMatrix;
    static Vector errVect;
    static UnbalAndTangentStorage unbalAndTangentArray; //!< array of class wide vectors and matrices
    static int numDOF_Groups; //!< number of objects of this class

    void inicID(void);
  protected:
    void  addLocalM_Force(const Vector &Udotdot, double fact = 1.0);     

    // protected variables - a copy for each object of the class            
    UnbalAndTangent unbalAndTangent;
    Node *myNode;

    friend class AnalysisModel;
    DOF_Group(int tag, Node *myNode);
    DOF_Group(int tag, int ndof);
  public:
    virtual ~DOF_Group();    

    virtual void setID(int dof, int value);
    virtual void setID(const ID &values);
    virtual const ID &getID(void) const;
    int inicID(const int &value);

    virtual int getNodeTag(void) const;
    //! @brief Returns the total number of DOFs in the DOF\_Group. 
    inline virtual int getNumDOF(void) const
      { return myID.Size(); }
    virtual int getNumFreeDOF(void) const;
    virtual int getNumConstrainedDOF(void) const;

    // methods to form the tangent
    virtual const Matrix &getTangent(Integrator *theIntegrator);
    virtual void  zeroTangent(void);
    virtual void  addMtoTang(double fact = 1.0);    
    virtual void  addCtoTang(double fact = 1.0);    

    // methods to form the unbalance
    virtual const Vector &getUnbalance(Integrator *theIntegrator);
    virtual void  zeroUnbalance(void);
    virtual void  addPtoUnbalance(double fact = 1.0);
    virtual void  addPIncInertiaToUnbalance(double fact = 1.0);    
    virtual void  addM_Force(const Vector &Udotdot, double fact = 1.0);        

    virtual const Vector &getTangForce(const Vector &x, double fact = 1.0);
    virtual const Vector &getC_Force(const Vector &x, double fact = 1.0);
    virtual const Vector &getM_Force(const Vector &x, double fact = 1.0);

    // methods to obtain committed responses from the nodes
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

    virtual const Vector &getTrialDisp(void) const;
    virtual const Vector &getTrialVel(void) const;
    virtual const Vector &getTrialAccel(void) const;

    // methods to set the eigen vectors
    virtual void setEigenvector(int mode, const Vector &eigenvalue);
	
    // method added for TransformationDOF_Groups
    virtual Matrix *getT(void);

// AddingSensitivity:BEGIN ////////////////////////////////////
    virtual void addM_ForceSensitivity(const Vector &Udotdot, double fact = 1.0);        
    virtual void addD_ForceSensitivity(const Vector &vel, double fact = 1.0);
    virtual void addD_Force(const Vector &vel, double fact = 1.0);

    virtual const Vector & getDispSensitivity(int gradNumber);
    virtual const Vector & getVelSensitivity(int gradNumber);
    virtual const Vector & getAccSensitivity(int gradNumber);
    virtual int saveSensitivity(Vector *v,Vector *vdot,Vector *vdotdot,int gradNum,int numGrads);
// AddingSensitivity:END //////////////////////////////////////
    virtual void  Print(std::ostream &, int = 0) {return;};
    virtual void resetNodePtr(void);
  };
} // end of XC namespace

#endif

