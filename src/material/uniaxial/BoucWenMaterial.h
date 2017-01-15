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
** (C) Copyright 2001, The Regents of the University of California    **
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
** Reliability module developed by:                                   **
**   Terje Haukaas (haukaas@ce.berkeley.edu)                          **
**   Armen Der Kiureghian (adk@ce.berkeley.edu)                       **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.1 $
// $Date: 2003/03/06 18:34:14 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/BoucWenMaterial.h,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#ifndef BoucWenMaterial_h
#define BoucWenMaterial_h

#include <material/uniaxial/UniaxialMaterial.h>

namespace XC {
  class Matrix;

//! @ingroup MatUnx
//
//! @brief ??.
class BoucWenMaterial : public UniaxialMaterial
  {
  private:
    // Material parameters
    double alpha;
    double ko;
    double n;
    double gamma;
    double beta;
    double Ao;
    double deltaA;
    double deltaNu;
    double deltaEta;

    // History variables(trial and commited)
    double Tstrain, Cstrain;
    double Tz, Cz;
    double Te, Ce;

    // Ohter variables
    double Tstress, Ttangent;

    double tolerance;
    int maxNumIter;

    // Sensitivit stuff
    int parameterID;
    Matrix *SHVs;
  public:
    BoucWenMaterial(int tag, double alpha, double ko, double n, double gamma, double beta, double Ao,
                             double deltaA, double deltaNu, double deltaEta, double tolerance, int maxNumIter);
    BoucWenMaterial(void);
    ~BoucWenMaterial(void);
    int setTrialStrain(double strain, double strainRate = 0.0);
    double getStrain(void) const;
    double getStress(void) const;
    double getTangent(void) const;
    static double signum(double);
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
    UniaxialMaterial *getCopy(void) const;
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);

    // Reliability and sensitivity stuff
    double getInitialTangent(void) const;
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    int activateParameter(int parameterID);
    double getStressSensitivity(int gradNumber, bool conditional);
    double getStrainSensitivity(int gradNumber);
    double getTangentSensitivity(int gradNumber);
    double getDampTangentSensitivity(int gradNumber);
    double getRhoSensitivity(int gradNumber);
    int    commitSensitivity(double strainGradient, int gradNumber, int numGrads);
  };
} // end of XC namespace


#endif

