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

// $Revision: 1.2 $
// $Date: 2004/10/06 19:21:12 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/Pinching4Material.h,v $


// Written: NM (nmitra@u.washington.edu)
// Created: December 2001
// Updated: September 2004
//
// Description: This file contains the class definition for


#ifndef Pinching4Material_h
#define Pinching4Material_h

#include <material/uniaxial/UniaxialMaterial.h>
#include <utility/handler/FileStream.h>
#include <utility/matrix/Vector.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief Pinching material which is defined by 4 points on the positive and
//! negative envelopes and a bunch of damage parameters.
//! 
//! Pinching material which is defined by 4 points on the positive and
//! negative envelopes and a bunch of damage parameters. The material accounts
//! for 3 types of damage rules : Strength degradation, Stiffness degradation,
//! unloading stiffness degradation.
class Pinching4Material: public UniaxialMaterial
  {
  private:
    // Backbone parameters
    double stress1p; double strain1p; double stress2p; double strain2p;
    double stress3p; double strain3p; double stress4p; double strain4p;
    double stress1n; double strain1n; double stress2n; double strain2n;
    double stress3n; double strain3n; double stress4n; double strain4n;
    Vector envlpPosStress; Vector envlpPosStrain;
    Vector envlpNegStress; Vector envlpNegStrain;

    int tagMat;  // material tag

    // Damage parameters

    double gammaK1; double gammaK2; double gammaK3; double gammaK4; double gammaKLimit;
    double gammaD1; double gammaD2; double gammaD3; double gammaD4; double gammaDLimit;
    double gammaF1; double gammaF2; double gammaF3; double gammaF4; double gammaFLimit;
    double gammaE;
    double TnCycle, CnCycle; // number of cycles contributing to damage calculation
    int DmgCyc; // flag for indicating whether no. of cycles are to be used for damage calculation

    // unloading-reloading parameters
    double rDispP; double rForceP; double uForceP;
    double rDispN; double rForceN; double uForceN;

    Vector state3Stress; Vector state3Strain; Vector state4Stress; Vector state4Strain;

    Vector envlpPosDamgdStress; Vector envlpNegDamgdStress;

    // Trial State Variables
    double Tstress;
    double Tstrain;
    double Ttangent;

    // Converged Material History parameters
    int Cstate;
    double Cstrain;
    double Cstress;
    double CstrainRate;
    double lowCstateStrain;
    double lowCstateStress;
    double hghCstateStrain;
    double hghCstateStress;
    double CminStrainDmnd;
    double CmaxStrainDmnd;
    double Cenergy;
    double CgammaK;
    double CgammaD;
    double CgammaF;
    double gammaKUsed;
    double gammaFUsed;

    // Trial Material History Parameters
    int Tstate;
    double dstrain;
    double TstrainRate;
    double lowTstateStrain;
    double lowTstateStress;
    double hghTstateStrain;
    double hghTstateStress;
    double TminStrainDmnd;
    double TmaxStrainDmnd;
    double Tenergy;
    double TgammaK;
    double TgammaD;
    double TgammaF;

    // strength and stiffness parameters;
    double kElasticPos;
    double kElasticNeg;
    double kElasticPosDamgd;
    double kElasticNegDamgd;
    double uMaxDamgd;
    double uMinDamgd;


    // energy parameters
    double energyCapacity;
    double kunload;
    double elasticStrainEnergy;

    void SetEnvelope(void);
    void getstate(double, double);
    double posEnvlpStress(double);
    double posEnvlpTangent(double);
    double negEnvlpStress(double);
    double negEnvlpTangent(double);
    void getState3(Vector& , Vector& , double);
    void getState4(Vector& , Vector& , double);
    double Envlp3Tangent(Vector , Vector , double);
    double Envlp3Stress(Vector , Vector , double);
    double Envlp4Tangent(Vector , Vector , double);
    double Envlp4Stress(Vector , Vector , double);
    void updateDmg(double, double);
  public :
    Pinching4Material(int tag,
  	double stress1p, double strain1p, double stress2p, double strain2p,
    	double stress3p, double strain3p, double stress4p, double strain4p,
    	double stress1n, double strain1n, double stress2n, double strain2n,
    	double stress3n, double strain3n, double stress4n, double strain4n,
    	double rDispP, double rForceP, double uForceP,
    	double rDispN, double rForceN, double uForceN,
    	double gammaK1, double gammaK2, double gammaK3,
    	double gammaK4, double gammaKLimit,
    	double gammaD1, double gammaD2, double gammaD3,
    	double gammaD4, double gammaDLimit,
    	double gammaF1, double gammaF2, double gammaF3,
    	double gammaF4, double gammaFLimit, double gammaE, int DmgCyc);

    Pinching4Material(int tag,
    	double stress1p, double strain1p, double stress2p, double strain2p,
    	double stress3p, double strain3p, double stress4p, double strain4p,
    	double rDispP, double rForceP, double uForceP,
    	double gammaK1, double gammaK2, double gammaK3,
    	double gammaK4, double gammaKLimit,
    	double gammaD1, double gammaD2, double gammaD3,
    	double gammaD4, double gammaDLimit,
    	double gammaF1, double gammaF2, double gammaF3,
    	double gammaF4, double gammaFLimit, double gammaE, int DmgCyc);

    Pinching4Material(int tag);
    Pinching4Material(void);

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
    void Print(std::ostream &s, int flag = 0) const;
  };
} // end of XC namespace
#endif
