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

#ifndef RJWatsonEQS2d_h
#define RJWatsonEQS2d_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 12/13
// Revision: A
//
// Description: This file contains the class definition for RJWatsonEQS2d.
// RJWatsonEQS2d (R.J. Watson EradiQuake System) is a two-node friction
// slider element with mass energy regulator (MER) polyurethane springs.
// This simplified version uses a unidirectional plasticity model to
// simulate the sliding shear behavior and three uniaxial material models
// to simulate the axial, MER spring shear resistance, and moment behaviors.
// The total P-Delta moment is fully distributed to the sliding surface end
// node of the element.

#include "SimpleBearingBase.h"

namespace XC {
class FrictionModel;
class UniaxialMaterial;
class Response;

//
//! @brief R.J. Watson EradiQuake System two-node friction slider.
//!
//! RJWatsonEQS2d (R.J. Watson EradiQuake System) is a two-node friction
//! slider element with mass energy regulator (MER) polyurethane springs.
//! This simplified version uses a unidirectional plasticity model to
//! simulate the sliding shear behavior and three uniaxial material models
//! to simulate the axial, MER spring shear resistance, and moment behaviors.
//! The total P-Delta moment is fully distributed to the sliding surface end
//! node of the element.
//! @ingroup FrictionElementGrp
class RJWatsonEQS2d : public SimpleBearingBase
  {
  private:
    // private methods
    void setUp();
    
    
    // state variables
    double ubPlastic;   // plastic displacement in basic system
    
    // committed history variables
    double ubPlasticC;  // plastic displacement in basic system
    
    static Matrix theMatrix;  // a class wide Matrix
    static Vector theVector;  // a class wide Vector
  protected:  
    int sendData(Communicator &);
    int recvData(const Communicator &);
    void initializeStiffnessMatrix(void);
    void initialize(void);
  public:
    // constructors
    RJWatsonEQS2d(int tag, int Nd1, int Nd2,
		  const FrictionModel &theFrnMdl, double kInit,
		  const std::vector<UniaxialMaterial *> &,
		  const Vector &y, const Vector &x,
		  const double &shearDistI = 1.0, const int &addRayleigh = 0,
		  const double &mass= 0.0,
		  const int &maxIter = 25,
		  const double &tol = 1E-12,
		  const double &kFactUplift = 1E-12);
    RJWatsonEQS2d();
    Element *getCopy() const;
    
    // public methods to obtain information about dof & connectivity
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
    
    const Vector &getResistingForce() const;
    const Vector &getResistingForceIncInertia() const;
    
    // public methods for element output
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    void Print(std::ostream &s, int flag = 0) const;    
    
    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);
  };
} // end of XC namespace

#endif
