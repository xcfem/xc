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
/* ******************************************************************
***
**    OpenSees - Open System for Earthquake Engineering Simulation **
**          Pacific Earthquake Engineering Research Center **
** **
** **
** (C) Copyright 1999, The Regents of the University of California **
** All Rights Reserved. **
** **
** Commercial use of this program without express permission of the **
** University of California, Berkeley, is strictly prohibited.  See **
** file 'COPYRIGHT'  in main directory for information on usage and **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES. **
** **
** Developed by: **
**   Frank McKenna (fmckenna@ce.berkeley.edu) **
**   Gregory L. Fenves (fenves@ce.berkeley.edu) **
**   Filip C. Filippou (filippou@ce.berkeley.edu) **
** **
** ******************************************************************
*/

#ifndef CoulombDamperMaterial_h
#define CoulombDamperMaterial_h

#include "material/uniaxial/UniaxialMaterial.h"

namespace XC {

class CoulombDamperMaterial : public UniaxialMaterial
  {
  private:
    double trialStrain;
    double trialStrainRate;
    double tangent;
    double friction;
    double commitTrialStrainRate;
    int flipped;
    double tol;
    double dampOutTangent;
    int method;
    int numFlipped;

    // AddingSensitivity:BEGIN
    // //////////////////////////////////////////
    int parameterID;
    // AddingSensitivity:END
    // ///////////////////////////////////////////

    double sign() const;
    double dsign() const;
    double factor() const;
  protected:
    int sendData(Communicator &);  
    int recvData(const Communicator &);
  public:
    CoulombDamperMaterial(int tag= 0);
    CoulombDamperMaterial(int tag, double k, double fc, double t,
                          double damp, int m, int n);

    int setTrialStrain(double strain, double strainRate = 0.0);
    int setTrial(double strain, double &stress, double &tangent,
                 double strainRate = 0.0);
    double getStrain(void) const
      { return trialStrain; }
    double getStrainRate(void) const
      { return trialStrainRate; }
    double getStress(void) const;
    double getTangent(void) const;
    double getDampTangent(void) const;
    double getInitialTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    UniaxialMaterial *getCopy(void) const;

    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    
    void Print(std::ostream &s, int flag =0) const;


    // AddingSensitivity:BEGIN
    // //////////////////////////////////////////
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    int activateParameter(int parameterID);
    double getStressSensitivity(int gradIndex, bool conditional);
    double getTangentSensitivity(int gradIndex);
    double getDampTangentSensitivity(int gradIndex);
    double getInitialTangentSensitivity(int gradIndex);
    int commitSensitivity(double strainGradient, int gradIndex, int numGrads);
    // AddingSensitivity:END
    // ///////////////////////////////////////////
  };
} // end of XC namespace

#endif
