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

// $Revision: 1.2 $
// $Date: 2006/08/03 23:42:19 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/Steel02.h,v $

// Written: fmk
// Created: 03/06
//
// Description: This file contains the class definition for
// Steel02. Steel02 is based on an f2c of the FEDEAS material
// Steel02.f which is:
//-----------------------------------------------------------------------
// MENEGOTTO-PINTO STEEL MODEL WITH FILIPPOU ISOTROPIC HARDENING
//            written by MOHD YASSIN (1993)
//          adapted to FEDEAS material library
//    by E. Spacone, G. Monti and F.C. Filippou (1994)
//-----------------------------------------------------------------------


#ifndef Steel02_h
#define Steel02_h

#include "material/uniaxial/steel/SteelBase.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief Uniaxial material for steel. Menegotto-Pinto steel
//! model with Filippou isotropic hardening.
class Steel02: public SteelBase
  {
  private:
    double sigini; //!< Initial strees.
    // matpar : STEEL FIXED PROPERTIES
    double R0;  //!<  = matpar(4)  : exp transition elastic-plastic
    double cR1; //!<  = matpar(5)  : coefficient for changing R0 to R
    double cR2; //!<  = matpar(6)  : coefficient for changing R0 to R
    // hstvP : STEEL HISTORY VARIABLES
    double epsminP; //!<  = hstvP(1) : max eps in compression
    double epsmaxP; //!<  = hstvP(2) : max eps in tension
    double epsplP;  //!<  = hstvP(3) : plastic excursion
    double epss0P;  //!<  = hstvP(4) : eps at asymptotes intersection
    double sigs0P;  //!<  = hstvP(5) : sig at asymptotes intersection
    double epssrP;  //!<  = hstvP(6) : eps at last inversion point
    double sigsrP;  //!<  = hstvP(7) : sig at last inversion point
    int    konP;    //!<  = hstvP(8) : index for loading/unloading
    // hstv : STEEL HISTORY VARIABLES
    double epsP;  //!<  = strain at previous converged step
    double sigP;  //!<  = stress at previous converged step
    double eP;    //!<   stiffness modulus at last converged step;

    double epsmin;
    double epsmax;
    double epspl;
    double epss0;
    double sigs0;
    double epsr;
    double sigr;
    int kon;
    double sig;
    double e;
    double eps;   //!< strain at current step
  protected:
    int setup_parameters(void);
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    Steel02(int tag, double fy, double E0, double b,
            double R0, double cR1, double cR2,
            double a1, double a2, double a3, double a4, double sigInit =0.0);
    Steel02(int tag, double fy, double E0, double b,double R0, double cR1, double cR2);
    // Constructor for no isotropic hardening
    // Also provides default values for R0, cR1, and cR2
    Steel02(int tag, double fy, double E0, double b);
    Steel02(int tag);
    Steel02(void);

    UniaxialMaterial *getCopy(void) const;

    int setTrialStrain(double strain, double strainRate = 0.0);
    double getStrain(void) const;
    double getStress(void) const;
    double getTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    void setInitialStress(const double &);
    inline double getInitialStress(void) const
      { return sigini; }


    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
// AddingSensitivity:BEGIN //////////////////////////////////////////
    int    setParameter(const std::vector<std::string> &argv, Parameter &param);
    int    updateParameter(int parameterID, Information &info);
// AddingSensitivity:END ///////////////////////////////////////////

    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif

