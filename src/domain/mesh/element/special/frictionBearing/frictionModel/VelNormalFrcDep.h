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

// $Revision$
// $Date$
// $URL$

#ifndef VelNormalFrcDep_h
#define VelNormalFrcDep_h

// Developed: Nhan D. Dao (nhan.unr@gmail.com)
// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 08/14
// Revision: A
//
// Description: This file contains the class definition for the VelNormalFrcDep
// friction model after Dao et al. (2013). In the velocity and normal force
// dependent friction model the friction force is given in terms of the friction
// coefficients at low and high velocities with both of them being a function of
// the normal force. If the normal force N is negative the friction force is zero.

#include "Coulomb.h"

namespace XC {
//! @ingroup FrictionModelGrp
//
class VelNormalFrcDep : public Coulomb
  {
  private:
    double aSlow;      // constant for slow COF
    double nSlow;      // normal force exponent for slow COF (nSlow <= 0)
    double aFast;      // constant for fast COF
    double nFast;      // normal force exponent for fast COF (nFast <= 0)
    double alpha0;     // constant rate parameter
    double alpha1;     // linear rate parameter
    double alpha2;     // quadratic rate parameter
    double maxMuFact;  // factor for determining the maximum friction coefficients
    
    double DmuDn;      // derivative of COF wrt to normal force
    double DmuDvel;    // derivative of COF wrt to velocity
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    // constructor
    VelNormalFrcDep();
    VelNormalFrcDep(int tag,
        double aSlow, double nSlow, double aFast, double nFast,
        double alpha0, double alpha1, double alpha2, double maxMuFact);
    
    // public methods to set and obtain response
    int setTrial(double normalForce, double velocity = 0.0);
    double getDFFrcDNFrc() const;
    double getDFFrcDVel() const;
    
    int revertToStart();
    
    FrictionModel *getCopy();
    
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    
    void Print(std::ostream &, int flag = 0) const;
  };
} // end of XC namespace

#endif
