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
//<<<<<<< PressureDependMultiYield.h
// $Revision: 1.5 $
// $Date: 2005/04/01 20:21:29 $
//=======
// $Revision: 1.5 $
// $Date: 2005/04/01 20:21:29 $
//>>>>>>> 1.7
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/soil/PressureDependMultiYield02.h,v $

// Written: ZHY
// Created: May 2004


// Description: This file contains the class prototype for PressureDependMultiYield02.
//
// What: "@(#) PressureDependMultiYield02.h, revA"

#ifndef PressureDependMultiYield02_h
#define PressureDependMultiYield02_h

#include "material/nD/soil/PressureDependMultiYieldBase.h"


namespace XC {
  class MultiYieldSurface;

//! @ingroup NDMat
//
//! @brief ??
class PressureDependMultiYield02: public PressureDependMultiYieldBase
  {
  private:
    // user supplied
    static double* contractParam2x;
    static double* contractParam3x;
    static double* dilateParam3x;

    mutable double damage;
    mutable double check;
    mutable T2Vector updatedTrialStress;

    mutable Vector PivotStrainRate;
    Vector PivotStrainRateCommitted;

    // Called by constructor
    void setUpSurfaces(double *);
    void initStrainUpdate(void);

    // Return num_strain_subincre
    int isCriticalState(const T2Vector & stress) const;
    void updatePPZ(const T2Vector & stress) const;
    void PPZTranslation(const T2Vector & contactStress) const;
    double getPPZLimits(int which, const T2Vector &) const;
    double getPlasticPotential(const T2Vector &,const T2Vector &) const;
    //return 1 if stress locked; o/w return 0.
    int stressCorrection(int crossedSurface) const;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    // Initialization constructor
    PressureDependMultiYield02 (int tag,
			       int nd,
				   double rho,
			       double refShearModul,
			       double refBulkModul,
			       double frictionAng,
			       double peakShearStra,
			       double refPress,
			       double pressDependCoe,
			       double phaseTransformAngle,
			       double contractionParam1,
			       double contractionParam3,
			       double dilationParam1,
			       double dilationParam3,
			       int   numberOfYieldSurf = 20,
				   double * gredu = 0,
			       double contractionParam2 = 5.,
			       double dilationParam2 = 3.,
			       double liquefactionParam1 = 1. ,
			       double liquefactionParam2 = 0. ,
		           double e = 0.6,
			       double volLimit1 = 0.9,
			       double volLimit2 = 0.02,
			       double volLimit3 = 0.7,
			       double atm = 101.,
				   double cohesi = 0.1,
				   double hv = 0.,
				   double pv = 1.);

     PressureDependMultiYield02(int tag);
     // Default constructor
     PressureDependMultiYield02 ();

     // Copy constructor
     PressureDependMultiYield02 (const PressureDependMultiYield02 &);


     // Calculates current tangent stiffness.
     const Matrix &getTangent(void) const;

     // Calculates the corresponding stress increment (rate), for a given strain increment.
     const Vector &getStress(void) const;
     const Vector &getCommittedStress(void);

     int setTrialStrain(const Tensor &v) {return 0;}
     int setTrialStrain(const Tensor &v, const Tensor &r) {return 0;}
     int setTrialStrainIncr(const Tensor &v) {return 0;}
     int setTrialStrainIncr(const Tensor &v, const Tensor &r) {return 0;}

     // Return an exact copy of itself.
     NDMaterial *getCopy(void) const;

     // Return a copy of itself if "code"="PressureDependMultiYield02", otherwise return null.
     NDMaterial *getCopy(const std::string &) const;

     // Return the string "PressureDependMultiYield02".
     const std::string &getType(void) const ;

     int sendSelf(CommParameters &);
     int recvSelf(const CommParameters &);
     void Print(std::ostream &s, int flag =0) const;
     //void setCurrentStress(const Vector stress) { currentStress=T2Vector(stress); }

    // RWB; PyLiq1 & TzLiq1 need to see the excess pore pressure and initial stresses.
    friend class PyLiq1;
    friend class TzLiq1;
  };
} // end of XC namespace

#endif
