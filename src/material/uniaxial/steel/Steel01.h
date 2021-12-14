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

// $Revision: 1.11 $
// $Date: 2003/03/05 01:02:51 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/Steel01.h,v $


#ifndef Steel01_h
#define Steel01_h

// File: Steel01.h
//
// Written: MHS
// Created: 06/99
// Revision: A
//
// Description: This file contains the class definition for
// Steel01.h
//
//
//
// What: "@(#) Steel01.h, revA"

#include "material/uniaxial/steel/SteelBase0103.h"
#include <utility/matrix/Matrix.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief Uniaxial material model for steel.
//!
//! provides the abstraction of a bilinear steel model
//! with isotropic hardening. The model contains a yield strength of fy,
//! an initial elastic tangent of E0, and a hardening ratio of b. The
//! optional parameters a1, a2, a3, and a4 control the amount of isotropic
//! hardening (default values are provided). Specification of minimum and
//! maximum failure strains through the -min and -max switches is optional
//! and must appear after the specification of the hardening parameters,
//! if present. The argument matTag is used to uniquely identify the
//! material object among material objects in the BasicBuilder object. 
class Steel01: public SteelBase0103
  {
  private:
    void determineTrialState(double dStrain);
    void detectLoadReversal(double dStrain);

// AddingSensitivity:BEGIN //////////////////////////////////////////
    int parameterID;
    Matrix SHVs;
// AddingSensitivity:END ///////////////////////////////////////////

  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);

    int setup_parameters(void);
  public:
    Steel01(int tag, double fy, double E0, double b,
       double a1 = STEEL_0103_DEFAULT_A1, double a2 = STEEL_0103_DEFAULT_A2,
       double a3 = STEEL_0103_DEFAULT_A3, double a4 = STEEL_0103_DEFAULT_A4);
    Steel01(int tag= 0);

    UniaxialMaterial *getCopy(void) const;

    int revertToStart(void);

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    void Print(std::ostream &s, int flag =0) const;

// AddingSensitivity:BEGIN //////////////////////////////////////////
    int    setParameter(const std::vector<std::string> &argv, Parameter &param);
    int    updateParameter          (int parameterID, Information &info);
    int    activateParameter        (int parameterID);
    double getStressSensitivity     (int gradNumber, bool conditional);
    double getInitialTangentSensitivity(int gradNumber);
    int    commitSensitivity        (double strainGradient, int gradNumber, int numGrads);
// AddingSensitivity:END ///////////////////////////////////////////

  };
} // end of XC namespace

#endif
