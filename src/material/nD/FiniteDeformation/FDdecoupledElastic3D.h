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
//===============================================================================
//# COPYRIGHT (C): Woody's license (by BJ):
//                 ``This    source  code is Copyrighted in
//                 U.S.,  for  an  indefinite  period,  and anybody
//                 caught  using it without our permission, will be
//                 mighty good friends of ourn, cause we don't give
//                 a  darn.  Hack it. Compile it. Debug it. Run it.
//                 Yodel  it.  Enjoy it. We wrote it, that's all we
//                 wanted to do.''
//
//# PROJECT:           Object Oriented Finite Element Program
//# PURPOSE:           Finite Deformation Hyper-Elastic classes
//# CLASS:
//#
//# VERSION:           0.6_(1803398874989) (golden section)
//# LANGUAGE:          C++
//# TARGET OS:         all...
//# DESIGN:            Zhao Cheng, Boris Jeremic (jeremic@ucdavis.edu)
//# PROGRAMMER(S):     Zhao Cheng, Boris Jeremic
//#
//#
//# DATE:              July 2004
//# UPDATE HISTORY:
//#
//===============================================================================

#ifndef FDdecoupledElastic3D_H
#define FDdecoupledElastic3D_H

#include "material/nD/FiniteDeformation/FiniteDeformationElastic3D.h"
#include "utility/matrix/nDarray/stresst.h"
#include "utility/matrix/nDarray/straint.h"



namespace XC {
  class WEnergy;

//! @ingroup FDNDMat
//
//! @brief ??.
class FDdecoupledElastic3D: public FiniteDeformationElastic3D
  {
  protected:
     WEnergy *W;

     straintensor F;
     straintensor C;
     double J;
     straintensor Cinv;
     double lambda1, lambda2, lambda3;
     double lambda_wave1, lambda_wave2, lambda_wave3;
     int caseIndex; 
     int FromForC;

     Tensor Stiffness;
     straintensor thisGreenStrain;
     stresstensor thisPK2Stress;

     void free_mem(void);
     void alloc(const WEnergy *);
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  private:
    const WEnergy *getWEnergy(void) const;

    const double getJ(void) const;
    const Vector getlambda(void) const;
    const Vector getlambda_wave(void) const;

    const Vector wa(void);
    const Tensor Yab(void);
    const Tensor FDisoStiffness(void);
    const Tensor FDvolStiffness(void);


//    int setInitialTangentTensor(void);
    int ComputeTrials(void);
    int getCaseIndex(void) const;
  public:
    FDdecoupledElastic3D(int tag, int classTag, WEnergy * , double );
    FDdecoupledElastic3D(int tag, WEnergy * , double );
    FDdecoupledElastic3D(int tag, WEnergy * );
    FDdecoupledElastic3D(int tag= 0);       
    FDdecoupledElastic3D(const FDdecoupledElastic3D &fde3d); 
    
    virtual ~FDdecoupledElastic3D();

    int setTrialF(const straintensor &f);
    int setTrialFIncr(const straintensor &df);
    int setTrialC(const straintensor &c);
    int setTrialCIncr(const straintensor &dc);

    const Tensor &getTangentTensor(void) const;	  // Default Lagrangian Tangent Tensor
    const Tensor &getInitialTangentTensor(void) const;

    const straintensor &getStrainTensor(void) const; // Default Green Lagrangian Strain
    const stresstensor &getStressTensor(void) const; // Default 2nd Piola Kirchhoff Stress
    const straintensor &getF(void) const;
    const straintensor &getC(void) const;

//    virtual const Vector &getStress(void) const;
//    virtual const Vector &getStrain(void) const;

//    virtual const stresstensor getCommittedStress(void);
//    virtual const straintensor getCommittedStrain(void);

//    virtual const straintensor getPlasticStrainTensor(void);

    int commitState(void) ;
    int revertToLastCommit(void) ;
    int revertToStart(void) ;

    NDMaterial *getCopy(void) const;
    NDMaterial *getCopy(const std::string &) const;

    const std::string &getType(void) const;
    int getOrder (void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag = 0);

//    int setParameter(const std::vector<std::string> &argv, Parameter &param);
//    int updateParameter(int parameterID, Information &info);

    const  stresstensor getPK1StressTensor(void) ;
    const  stresstensor getCauchyStressTensor(void) ;
  };
} // end of XC namespace

#endif

