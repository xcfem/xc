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

// $Revision: 1.1 $
// $Date: 2009/11/03 23:13:08 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/frictionBearing/SingleFPSimple2d.h,v $

#ifndef SingleFPSimple2d_h
#define SingleFPSimple2d_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the class definition for SingleFPSimple2d.
// SingleFPSimple2d is a single-concave friction pendulum element defined by
// two nodes. This simplified version uses small angle approximations and
// accounts for the rotation of the sliding surface by shifting the shear force.

#include "SimpleBearingBase.h"

namespace XC {
class FrictionModel;
class UniaxialMaterial;
class Response;

//! @brief Single-concave friction pendulum element for two dimensional problems.
//!
//! SingleFPSimple2d is a single-concave friction pendulum element defined by
//! two nodes. This simplified version uses small angle approximations and
//! accounts for the rotation of the sliding surface by shifting the shear
//! force.
class SingleFPSimple2d: public SimpleBearingBase
  {
  private:
    // private methods
    void setUp();
    
    // parameters
    double Reff; //!< effective radius of concave sliding dish
    int inclVertDisp; //!< flag to include vertical displacements
    
    // state variables
    double ubPlastic; //!< plastic displacement in basic system
    static Matrix theMatrix;
    static Vector theVector;
   
    // committed history variables
    double ubPlasticC; //!< plastic displacement in basic system

  protected:  
    //! @brief initial stiffness in local shear direction.
    inline double kInit() const
      { return k0; }
    int sendData(Communicator &);
    int recvData(const Communicator &);
    void initializeStiffnessMatrix(void);
    void initialize(void);
  public:
    // constructors
    SingleFPSimple2d(int tag, int Nd1, int Nd2,
		     const FrictionModel &theFrnMdl,
		     const double &Reff, const double &kInit,
		     const std::vector<UniaxialMaterial *> &theMaterials,
		     const Vector &y= Vector(), const Vector &x= Vector(),
		     const double &shearDistI= 0.0, const int &addRayleigh= 0,
		     int inclVertDisp= 0, const double &mass= 0.0,
		     const int &maxIter= 25,const double &tol= 1E-12,
		     const double &kFactUplift= 1E-6);
    SingleFPSimple2d(void);
	
    void setDomain(Domain *theDomain);
	
    // public methods to set the state of the element    
    int commitState();
    int revertToLastCommit();        
    int revertToStart();        
    int update();
	
    // public methods to obtain stiffness, mass, damping and residual information    
    const Matrix &getTangentStiff() const;
    const Matrix &getInitialStiff() const;
    const Matrix &getDamp() const;
    const Matrix &getMass() const;

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);
    
    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;
    
    // public methods for element output
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    void Print(std::ostream &, int flag = 0) const;    
	
    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);
    // AddingSensitivity:BEGIN //////////////////////////////////////////
    int   setParameter(const std::vector<std::string> &argv, Parameter &param);
    int   updateParameter(int parameterID, Information &info);
    // AddingSensitivity:END ///////////////////////////////////////////
  };
} // end of XC namespace

#endif
