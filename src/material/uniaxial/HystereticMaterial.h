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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/02/14 23:01:39 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/HystereticMaterial.h,v $

// Written: MHS
// Created: July 2000
//
// Description: This file contains the class definition for 
// HystereticMaterial.  HystereticMaterial provides the implementation
// of a one-dimensional hysteretic model with pinching of both
// force and deformation, damage due to deformation and energy, and
// degraded unloading stiffness based on maximum ductility.  This
// is a modified implementation of Hyster2.f90 by Filippou.

#ifndef HystereticMaterial_h
#define HystereticMaterial_h

#include "UniaxialMaterial.h"
#include "UniaxialStateVars.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief HystereticMaterial provides the implementation
//! of a one-dimensional hysteretic model with pinching of both
//! force and deformation, damage due to deformation and energy, and
//! degraded unloading stiffness based on maximum ductility.  This
//! is a modified implementation of Hyster2.f90 by Filippou.
class HystereticMaterial: public UniaxialMaterial
  {
  private:
    // Pinching parameters
    double pinchX; // Deformation pinching
    double pinchY; // Force pinching

    // Damage parameters
    double damfc1; // Deformation
    double damfc2; // Energy

    // Unloading parameter
    double beta;

    // Trial history variables
    double TrotMax;
    double TrotMin;
    double TrotPu;
    double TrotNu;
    double TenergyD;
    int TloadIndicator;

    // Trial state variables
    UniaxialStateVars trial;

    // Converged history variables
    double CrotMax;
    double CrotMin;
    double CrotPu;
    double CrotNu;
    double CenergyD;
    int CloadIndicator;

    // Converged state variables
    UniaxialStateVars converged;

    // Backbone parameters
    double mom1p, rot1p;
    double mom2p, rot2p;
    double mom3p, rot3p;
    double mom1n, rot1n;
    double mom2n, rot2n;
    double mom3n, rot3n;

    double E1p, E1n;
    double E2p, E2n;
    double E3p, E3n;

    double energyA;

    void setEnvelope(void);

    double posEnvlpStress(double strain);
    double negEnvlpStress(double strain);

    double posEnvlpTangent(double strain);
    double negEnvlpTangent(double strain);

    double posEnvlpRotlim(double strain);
    double negEnvlpRotlim(double strain);

    void positiveIncrement(double dStrain);
    void negativeIncrement(double dStrain);
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    HystereticMaterial(int tag,
    	double mom1p, double rot1p, double mom2p, double rot2p,
    	double mom3p, double rot3p,
    	double mom1n, double rot1n, double mom2n, double rot2n,
    	double mom3n, double rot3n,
    	double pinchX, double pinchY,
    	double damfc1 = 0.0, double damfc2 = 0.0,
    	double beta = 0.0);
    HystereticMaterial(int tag,
    	double mom1p, double rot1p, double mom2p, double rot2p,
    	double mom1n, double rot1n, double mom2n, double rot2n,
    	double pinchX, double pinchY,
    	double damfc1 = 0.0, double damfc2 = 0.0,
    	double beta = 0.0);
    HystereticMaterial(int tag);
    HystereticMaterial();

    int setTrialStrain(double strain, double strainRate = 0.0);
    double getStrain(void) const;
    double getStress(void) const;
    double getTangent(void) const;
    inline double getInitialTangent(void) const {return E1p;};

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif
