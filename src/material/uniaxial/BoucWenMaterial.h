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
//! @brief Bouc-Wen smooth hysteretic material model.
//!
//! This material model is an extension of the original Bouc-Wen
//! model that includes stiffness and strength degradation
//! (Baber and Noori (1985)).
//! Parameter \f$\gamma\f$ is usually in the range from -1 to 1 and parameter
//! \f$\beta\f$ is usually in the range from 0 to 1. Depending on the values
//! of \f$\gamma\f$ and \f$\beta\f$ softening, hardening or quasi-linearity
//! can be simulated. The hysteresis loop will exhibit softening for the
//! following cases:
//!    (a) \f$\beta\f$ + \f$\gamma\f$ > 0 and \f$\beta\f$ - \f$\gamma\f$ > 0,
//!    (b) \f$\beta\f$+\f$\gamma\f$ >0 and \f$\beta\f$-\f$\gamma\f$ <0, and
//!    (c) \f$\beta\f$+\f$\gamma\f$ >0 and \f$\beta\f$-\f$\gamma\f$ = 0.
//! The hysteresis loop will exhibit hardening if \f$\beta\f$+\f$\gamma\f$ < 0
//! and \f$\beta\f$-\f$\gamma\f$ > 0, and quasi-linearity
//! if \f$\beta\f$+\f$\gamma\f$ = 0 and \f$\beta\f$-\f$\gamma\f$ > 0.
class BoucWenMaterial: public UniaxialMaterial
  {
  private:
    // Material parameters
    double alpha; //!< ratio of post-yield stiffness to the initial elastic stiffenss (0< \f$\alpha\f$ <1) 
    double ko; //!<initial elastic stiffness.
    double n; //!< parameter that controls transition from linear to nonlinear range (as n increases the transition becomes sharper; n is usually grater or equal to 1) 
    double gamma; //!< parameter that control shape of hysteresis loop; depending on the values of \f$\gamma\f$ and \f$\beta\f$ softening, hardening or quasi-linearity can be simulated. 
    double beta; //!< parameter that control shape of hysteresis loop; depending on the values of \f$\gamma\f$ and \f$\beta\f$ softening, hardening or quasi-li nearity can be simulated.
    double Ao; //!< parameter that control tangent stiffness.
    double deltaA; //!< parameter that control tangent stiffness.
    double deltaNu; //!< parameter that control material degradation.
    double deltaEta; //!< parameter that control material degradation.

    // History variables(trial and committed)
    double Tstrain, Cstrain;
    double Tz, Cz;
    double Te, Ce;

    // Other variables
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
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    void Print(std::ostream &s, int flag =0) const;

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

