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
                                                                        
// $Revision: 1.3 $
// $Date: 2006/01/19 19:19:12 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/ReinforcingSteel.h,v $

/* ****************************************************************** **
** THIS FILE WAS DEVELOPED AT UC DAVIS                                **
**                                                                    **
** Programmed by: Jon Mohle (jfmohle@ucdavis.edu)                     **
** Supervisor: Sashi Kunnath (skkunnath@ucdavis.edu)                  **
**                                                                    **
********************************************************************* */
// Written: Jon Mohle
// Created: October 2003
// Updated: September 2005
// Description: This file contains the class definition for 
// ReinforcingSteel.

#ifndef ReinforcingSteel_h
#define ReinforcingSteel_h
#define HelpDebugMat        //Debugging info, JFM

#include "UniaxialMaterial.h"
#include "UniaxialStateVars.h"
#include <vector>

namespace XC {
//! @ingroup MatUnx
//
//! @brief ??.
class ReinforcingSteel : public UniaxialMaterial
  {
  private:
    static const int LastRule_RS=20;  // must be divisable by 4!!!!!!!!!!!
    static const int vSize= LastRule_RS/2+1;

  #ifdef HelpDebugMat
    static int classCount;
    int thisClassNumber;
    int thisClassCommit;
    int thisClassStep;
  #endif

    double ZeroTol;
    double reduction;
    double fsu_fraction;
    double beta;
    mutable int theBarFailed;

    // natural stress-strain variables
    double p;
    double Esp;   // natural Elastic Modulus
    double eshp;	// natural Hardening Strain
    double fshp;  // natural Hardening Stress
    double Eshp;  // natural Hardening Modulus
    double esup;  // natural Strain at Peak Stress
    double fsup;  // natural Peak Stress
    double Esup;  // natural peak stress Modulus
    double Eypp;  // natural Yield Plateu Modulus
    double fint;  // natural Stress yield plateu intersect
    double eyp;   // natural strain at yield
    double fyp;   // natural yield stress

    double esh;   // engineering hardening strain (user input)
    double Esh;   // engineering hardening slope (user input)

    double eshpa;	// Curve smoothing Parameters (at SH transition)
    double Eshpb;	// These are used to eliminate a sudden discontinuity in stiffness

    double a1;    // Linear Hardening Constant (with relation to accumulated plastic strain)
    double hardLim;

    double THardFact;
    double CHardFact;

    // Strength degradation parameters
    std::vector<double> T_ePlastic;
    std::vector<double> C_ePlastic;
    //double Nbf;               // Cyclic Backbone factor used correct backbone proporsional to return strain
    double TFatDamage;
    double CFatDamage;
    double LDratio;
    double Fat1;
    double Fat2;
    double Deg1;
    int    BuckleModel;
    mutable double BackStress;

    // Menegotto-Pinto Calibration Constants
    double RC1;
    double RC2;
    double RC3;

    // Menegotto-Pinto Equation paramenters
    double TR;
    double Tfch;
    double TQ;
    double TEsec;
    double Tea;
    double Tfa;
    double TEa;
    double Teb;
    double Tfb;
    double TEb;

    double re;
    double rE1;
    double rE2;

    // Converged Menegotto-Pinto Equation paramenters
    std::vector<double> CR;
    std::vector<double> Cfch;
    std::vector<double> CQ;
    std::vector<double> CEsec;
    std::vector<double> Cea;
    std::vector<double> Cfa;
    std::vector<double> CEa;
    std::vector<double> Ceb;
    std::vector<double> Cfb;
    std::vector<double> CEb;

    // Trial History Variables
    mutable int TBranchNum;
    int TBranchMem;
    double Teo_p;
    double Teo_n;
    double Temax;
    double Temin;
    double TeAbsMax;
    double TeAbsMin;
    double TeCumPlastic;

    // Converged History Variables
    int    CBranchNum;
    double Ceo_p;
    double Ceo_n;
    double Cemax;
    double Cemin;
    double CeAbsMax;
    double CeAbsMin;
    double CeCumPlastic;

    // Trial State Variables
    UniaxialStateVars trial;

    // Converged History Variables
    UniaxialStateVars converged;

    // Private Functions
    //int static inline Sign(double x);
    double Backbone_f(double ess) const;
    double Backbone_fNat(double essp) const;
    double Backbone_E(double ess) const;
    double Buckled_stress_Dhakal(double ess, double fss) const;
    double Buckled_stress_Gomes(double ess, double fss) const;
    double Buckled_mod_Gomes(double ess, double fss, double Ess) const;
    double Buckled_mod_Dhakal(double ess, double fss, double Ess) const;

    double inline MP_f(double e) const;
    double inline MP_E(double e) const;
    int SetMP(void);
    double MPfunc(double a);
    void inline SetTRp(void);
    void inline SetTRn(void);
    void inline SetTRp1(void);
    void inline SetTRn1(void);
    void SetPastCurve(int branch);

    int BranchDriver(int res);
    int Rule1(int res);
    int Rule2(int res);
    int Rule3(int res);
    int Rule4(int res);
    int Rule5(int res);
    int Rule6(int res);
    int Rule7(int res);
    int Rule8(int res);
    int Rule9(int res);
    int Rule10(int res);
    int Rule11(int res);
    int Rule12(int res);

    double inline damage(double ehalfPlastic);
    double inline getPlasticStrain(double ehalf, double stressAmp);
    double scalefactor(void) const;
    double inline ReturnSlope(double dea);
    void updateHardeningLoaction(double PlasticStrain);
    void updateHardeningLoactionParams(void);
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    ReinforcingSteel(int tag, double fyield, double fultimate, double youngs, double youngs_hard, 
		   double estrainhard, double eultimate, int buckModel, double slenderness, double alpha, double r, 
		   double gama, double Fatigue1, double Fatigue2, double Degrade1, double RC1, double RC2, double RC3, double A1, double HardLim);
    ReinforcingSteel(int tag);    
  
    int setTrialStrain(double strain, double strainRate = 0.0); 
    double getStrain(void) const;
    double getStress(void) const;
    double getTangent(void) const;
    double getInitialTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        
  
    UniaxialMaterial *getCopy(void) const;
  
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
  
    void Print(std::ostream &s, int flag =0);
  };
} // end of XC namespace

#endif

