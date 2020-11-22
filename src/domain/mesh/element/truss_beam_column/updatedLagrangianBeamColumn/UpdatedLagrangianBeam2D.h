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
** See file 'COPYRIGHT'  in main directory for information on usage   **
** and redistribution of OpenSees, and for a DISCLAIMER OF ALL        **
** WARRANTIES.                                                        **
**                                                                    **
** UpdatedLagrangianBeam2D.h: interface for UpdatedLagrangianBeam2D   **
** Developed by:                                                      **
**    Rohit Kaul       (rkaul@stanford.edu)                           **
**    Greg Deierlein   (ggd@stanford.edu)                             **
**                                                                    **
**           John A. Blume Earthquake Engineering Center              **
**                    Stanford University                             **
** ****************************************************************** **/


// UpdatedLagrangianBeam2D.h: interface for the UpdatedLagrangianBeam2D class
// Written: rkaul
//
// Description: This file contains the class definition for UpdatedLagrangianBeam2D.

// UpdatedLagrangianBeam2D is an abstract class providing most methods required by
// the base class "Element", for 2D beam-column elements. Geometric
// nonlinearity is incorporated at this level using updated lagrangian
// formulation

#ifndef UpdatedLagrangianElement2D
#define UpdatedLagrangianElement2D



#include <domain/mesh/node/Node.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <domain/mesh/element/Element1D.h>

namespace XC {
class Response;

//! @ingroup BeamColumnElemGrp
//! @defgroup UpdatedLagrangianBeams Updated Lagrangian beam-column elements.
//! @ingroup UpdatedLagrangianBeams
//
//! @brief Updated Lagrangian 2D beam element.
//!
//! UpdatedLagrangianBeam2D is an abstract class providing most methods
//! required by the base class "Element", for 2D beam-column elements.
//! Geometric nonlinearity is incorporated at this level using updated
//! lagrangian formulation
class UpdatedLagrangianBeam2D: public Element1D
  {
  protected:
    bool    isLinear;
    int     numDof;
    mutable double  L; //!< Element length.
    mutable double sn, cs; //!< Element direction.
    double massDof;
  
    Node *end1Ptr(void);
    const Node *end1Ptr(void) const;
    Node *end2Ptr(void);
    const Node *end2Ptr(void) const;
    double  L_hist, cs_hist, sn_hist;
  
    mutable Vector eleForce;
    Vector eleForce_hist;

    int nodeRecord, dofRecord;
    mutable int m_Iter;

    mutable Matrix *Ki;

    static Matrix K, Kg, Kt; // stiffness matrices
    static Matrix M; // mass matrix
    static Matrix D; // damping matrix
    static Matrix T; // transformation matrix
    
    static Vector disp;
    static Vector force;
    
    // ZeroVector and ZeroMatrix should always have zero value,
    // used for quick return
    static Vector ZeroVector;
    static Matrix ZeroMatrix;
    
    // used for temporarily storing nodal displacements
    static Vector end1IncrDisp;
    static Vector end2IncrDisp;
    
    virtual void  getLocalStiff(Matrix &K) const=0;
    virtual void  getLocalMass(Matrix &M) const=0;
    
    
    void  getIncrLocalDisp(Vector &localDisp) const;
    void  getTrialNaturalDisp(Vector &localDisp);
    void  getIncrNaturalDisp(Vector &nDisp) const;
    void  getConvLocalDisp(Vector &lDisp);
    void  getTrialLocalDisp(Vector &lDisp) const;
    void  getTrialLocalForce(Vector &force) const;
    
    virtual void updateState(void) const;

    void  addInternalGeomStiff(Matrix &K) const;
    void  addExternalGeomStiff(Matrix &K) const;
    
    void  transformToGlobal(Matrix &K) const;
  public:
    UpdatedLagrangianBeam2D(int classTag);
    UpdatedLagrangianBeam2D(int tag, int classTag, int nd1, int nd2, bool islinear = false);
    virtual ~UpdatedLagrangianBeam2D(void);

    //////////////////////////////////////////////////////////////////////
    // Overridden public methods, defined in Element class
    // (see ~/OpenSees/SRC/element/Element.h)
    //////////////////////////////////////////////////////////////////////
 public:
    virtual int update(void);
    // void	setEndRelease(ID &g);
    
    int getNumDOF(void) const;
    virtual void setDomain(Domain *theDomain);
    
    virtual int commitState(void);
    virtual int revertToLastCommit(void);
    
    // methods to return the current linearized stiffness,
    // damping and mass matrices
    virtual const	Matrix &getTangentStiff(void) const;
    virtual const Matrix &getInitialStiff(void) const;
    virtual const Matrix &getMass(void) const;
    
    // methods for returning and applying loads
    virtual Vector &getUVLoadVector(double q1, double q2);
    int addLoad(const Vector &load);
    int addLoad(ElementalLoad *theLoad, double loadFactor)
      { return -1;}
    int     addInertiaLoadToUnbalance(const Vector &accel)
      { return -1;}
    
    virtual const Vector &getResistingForce(void) const;
    const	Vector &getResistingForceIncInertia(void) const;
    
    virtual Response *setResponse(const std::vector<std::string> &argv,
				Information &eleInformation);
    virtual int getResponse(int responseID, Information &eleInformation);
  };
} // end of XC namespace


#endif // !defined(UpdatedLagrangianElement2D)
