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

// $Revision: 1.9 $
// $Date: 2003/03/11 03:49:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/HardeningMaterial.h,v $

#ifndef HardeningMaterial_h
#define HardeningMaterial_h

// Written: MHS
// Created: May 2000
//
// Description: This file contains the class definition for 
// HardeningMaterial.  HardeningMaterial provides the abstraction
// for a one-dimensional rate-independent plasticity model
// with combined isotropic and kinematic hardening.

#include <material/uniaxial/UniaxialMaterial.h>

// XXX This material probably needs to be rebased
// from OpenSees. There are significant changes
// in the implementation.


namespace XC {
//! @ingroup MatUnx
//
//! @brief HardeningMaterial provides the abstraction
//! for a one-dimensional rate-independent plasticity model
//! with combined isotropic and kinematic hardening.
class HardeningMaterial: public UniaxialMaterial
  {
  private:
    // Material parameters
    double E;	// Elastic modulus
    double sigmaY;	// Yield stress
    double Hiso;	// Isotropic hardening parameter
    double Hkin;	// Kinematic hardening parameter
    double eta;
	
    // Committed history variables
    double CplasticStrain;	// Committed plastic strain
    double CbackStress;		// Committed back stress;
    double Chardening;		// Committed internal hardening variable

    // Trial history variables
    double TplasticStrain;	// Trial plastic strain
    double TbackStress;		// Trial back stress
    double Thardening;		// Trial internal hardening variable

    // Trial state variables
    double Tstrain;		// Trial strain
    double Tstress;		// Trial stress
    double Ttangent;	// Trial tangent

// AddingSensitivity:BEGIN //////////////////////////////////////////
    int parameterID;
    Matrix *SHVs;
// AddingSensitivity:END ///////////////////////////////////////////
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    HardeningMaterial(int tag, double E, double sigmaY,
		      double K, double H, double eta = 0.0);
    HardeningMaterial(int tag);
    HardeningMaterial(void);
    HardeningMaterial(const HardeningMaterial &);
    HardeningMaterial &operator=(const HardeningMaterial &);
    ~HardeningMaterial(void);

    int setTrialStrain(double strain, double strainRate = 0.0); 
    double getStrain(void) const;          
    double getStress(void) const;
    double getTangent(void) const;
    inline double getInitialTangent(void) const {return E;};

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    
    void Print(std::ostream &s, int flag =0) const;
    
// AddingSensitivity:BEGIN //////////////////////////////////////////
    int    setParameter(const std::vector<std::string> &argv, Parameter &param);
    int    updateParameter(int parameterID, Information &info);
    int    activateParameter        (int parameterID);
    double getStressSensitivity     (int gradNumber, bool conditional);
    double getInitialTangentSensitivity    (int gradNumber);
    int commitSensitivity        (double strainGradient, int gradNumber, int numGrads);
// AddingSensitivity:END ///////////////////////////////////////////
  };
} // end of XC namespace


#endif

