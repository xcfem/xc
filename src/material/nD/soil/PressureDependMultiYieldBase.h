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
//PressureDependMultiYieldBase.h

#ifndef PressureDependMultiYieldBase_h
#define PressureDependMultiYieldBase_h

#include "PressureMultiYieldBase.h"
#include "T2Vector.h"


namespace XC {

class MultiYieldSurface;

//! @ingroup NDMat
//
//! @brief ??.
class PressureDependMultiYieldBase: public PressureMultiYieldBase
  {
  protected:
  // user supplied 
     static double* refShearModulusx;
     static double* refBulkModulusx;
     static double* phaseTransfAnglex; 
     static double* contractParam1x;
     static double* dilateParam1x;
     static double* dilateParam2x;
     static double* einitx;    //initial void ratio
     static double* liquefyParam1x;
     static double* liquefyParam2x;
     static double* volLimit1x;
     static double* volLimit2x;
     static double* volLimit3x;
     static double pAtm;
     static double* Hvx;
     static double* Pvx;

     // internal

     mutable double modulusFactor;
     mutable double initPress;

     mutable double pressureD;
     double pressureDCommitted;
     mutable int onPPZ; //=-1 never reach PPZ before; =0 below PPZ; =1 on PPZ; =2 above PPZ
     int onPPZCommitted;
     double strainPTOcta;
     mutable double PPZSize;
     double PPZSizeCommitted;
     mutable double cumuDilateStrainOcta;
     mutable double maxCumuDilateStrainOcta;
     mutable double cumuTranslateStrainOcta;
     mutable double prePPZStrainOcta;
     mutable double oppoPrePPZStrainOcta;
     static T2Vector trialStrain;
     mutable T2Vector PPZPivot;
     mutable T2Vector PPZCenter;

     double cumuDilateStrainOctaCommitted;
     double maxCumuDilateStrainOctaCommitted;
     double cumuTranslateStrainOctaCommitted;
     double prePPZStrainOctaCommitted;
     double oppoPrePPZStrainOctaCommitted;
     T2Vector PPZPivotCommitted;
     T2Vector PPZCenterCommitted;

     static Vector workV6;
     static T2Vector workT2V;
     double maxPress;
     
     void elast2Plast(void) const;
     // Called by constructor
     double yieldFunc(const T2Vector & stress,const std::vector<MultiYieldSurface> &surfaces, int surface_num) const;
     void deviatorScaling(T2Vector & stress,const std::vector<MultiYieldSurface> &surfaces, int surfaceNum) const;
     void initSurfaceUpdate(void) const;

     // Return num_strain_subincre
     int setSubStrainRate(void) const;
     int isLoadReversal(const T2Vector &) const;
     void getContactStress(T2Vector &contactStress) const;
     void getSurfaceNormal(const T2Vector & stress, T2Vector &normal) const; 
     double getModulusFactor(const T2Vector &stress) const;
     void setTrialStress(const T2Vector &stress) const; 
     double getLoadingFunc(const T2Vector & contact, const T2Vector & surfaceNormal,
			   double plasticPotential,int crossedSurface) const;
     //return 1 if stress locked; o/w return 0.
     void updateActiveSurface(void) const;
     void updateInnerSurface(void) const;

     // Return 1 if crossing the active surface; return 0 o/w
     int  isCrossingNextSurface(void) const;  
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
     // Initialization constructor
     PressureDependMultiYieldBase (int tag, int classTag, 
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
			       double dilationParam1,
			       double dilationParam2,
			       double liquefactionParam1,
			       double liquefactionParam2,
			       double liquefactionParam4,
			       int   numberOfYieldSurf = 20,
				   double * gredu = 0,
		           double e = 0.6,
			       double volLimit1 = 0.9,
			       double volLimit2 = 0.02,
			       double volLimit3 = 0.7,
			       double atm = 101.,
				   double cohesi = 0.1,
				   double hv = 0.,
				   double pv = 1.);

     PressureDependMultiYieldBase(int tag, int classTag);

     // Copy constructor
     PressureDependMultiYieldBase(const PressureDependMultiYieldBase &);

     double getRho(void) const
       {return rhox[matN];} ;

     // Sets the values of the trial strain tensor.
     int setTrialStrain(const Vector &strain);

     // Sets the values of the trial strain and strain rate tensors.
     int setTrialStrain(const Vector &v, const Vector &r);

     int setTrialStrainIncr(const Vector &v);
     int setTrialStrainIncr(const Vector &v, const Vector &r);

     // Calculates current tangent stiffness.
     const Matrix &getInitialTangent(void);
        
     void getBackbone(Matrix &);

     // Calculates the corresponding stress increment (rate), for a given strain increment. 
     const Vector &getStrain (void);
     const Vector &getCommittedStress (void);
     const Vector &getCommittedStrain (void);

     int setTrialStrain(const Tensor &v) {return 0;}
     int setTrialStrain(const Tensor &v, const Tensor &r) {return 0;}
     int setTrialStrainIncr(const Tensor &v) {return 0;}
     int setTrialStrainIncr(const Tensor &v, const Tensor &r) {return 0;}

     // Accepts the current trial strain values as being on the solution path, and updates 
     // all model parameters related to stress/strain states. Return 0 on success.
     int commitState(void);

     // Revert the stress/strain states to the last committed states. Return 0 on success.
     int revertToLastCommit (void);

     int revertToStart(void) {return 0;}

     // Return ndm.
     int getOrder(void) const;

     int sendSelf(CommParameters &);  
     int recvSelf(const CommParameters &);
     Response *setResponse(const std::vector<std::string> &argv, Information &matInfo);
     int getResponse (int responseID, Information &matInformation);
     void Print(std::ostream &s, int flag =0);
     //void setCurrentStress(const Vector stress) { currentStress=T2Vector(stress); }
     int updateParameter(int responseID, Information &eleInformation);
  };
} // end of XC namespace

#endif
