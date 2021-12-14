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
// $Revision: 1.15 $
// $Date: 2004/06/15 18:58:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/soil/PressureIndependMultiYield.h,v $
                                                                        
// Written: ZHY
// Created: August 2000

// Description: This file contains the class prototype for PressureIndependMultiYield.
//
// What: "@(#) PressureIndependMultiYield.h, revA"

#ifndef PressureIndependMultiYield_h
#define PressureIndependMultiYield_h

#include "PressureMultiYieldBase.h"
#include "T2Vector.h"

namespace XC {

//! @ingroup NDMat
//
//! @brief ??.
class PressureIndependMultiYield: public PressureMultiYieldBase
  {
  private:

    // user supplied

    // internal
    mutable double refShearModulus;
    mutable double refBulkModulus;

    void elast2Plast(void) const;
    // Called by constructor
    void setUpSurfaces(double *);  

    double yieldFunc(const T2Vector & stress, const std::vector<MultiYieldSurface> &,int surface_num) const;

    void deviatorScaling(T2Vector &,const std::vector<MultiYieldSurface> &,int, int count=0) const;

    void initSurfaceUpdate(void) const;

    void paramScaling(void) const;

    // Return num_strain_subincre
    int setSubStrainRate(void) const;

    int isLoadReversal(void) const;
    void getContactStress(T2Vector &) const;
    void getSurfaceNormal(const T2Vector &,Vector &) const; 
    void setTrialStress(const T2Vector &stress) const; 
    double getLoadingFunc(const T2Vector & contact, const Vector & surfaceNormal,int crossedSurface) const;

    void stressCorrection(int crossedSurface) const;
       
    void updateActiveSurface(void) const;
        
    void updateInnerSurface(void) const;

    // Return 1 if crossing the active surface; return 0 o/w
    int  isCrossingNextSurface(void) const;  
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    // Initialization constructor
    PressureIndependMultiYield(int tag, 
                                 int nd,
                                 double rho, 
                                 double refShearModul,
                                 double refBulkModul,
                                 double cohesi,
                                 double peakShearStra,
                                 double frictionAng = 0.,
                                 double refPress = 100, 
                                 double pressDependCoe = 0.0,
                                 int   numberOfYieldSurf = 20,
                                 double * gredu = 0);

     PressureIndependMultiYield(int tag= 0);

    void setup(int nd, double r, double refShearModul, double refBulkModul, double cohesi, double peakShearStra, double frictionAng, double refPress, double pressDependCoe, int numberOfYieldSurf, double * gredu);

     inline double getRho(void) const
       {return rhox[matN];}
     
     // Sets the values of the trial strain tensor.
     int setTrialStrain(const Vector &strain);

     // Sets the values of the trial strain and strain rate tensors.
     int setTrialStrain(const Vector &v, const Vector &r);

     int setTrialStrainIncr(const Vector &v);
     int setTrialStrainIncr(const Vector &v, const Vector &r);

     // Calculates current tangent stiffness.
     const Matrix &getTangent(void) const;
     const Matrix &getInitialTangent(void) const;
        
     void getBackbone(Matrix &);

     // Calculates the corresponding stress increment(rate), for a given strain increment. 
     const Vector &getStress(void) const;
     const Vector &getStrain(void) const;
     const Vector &getCommittedStress(void) const;
     const Vector &getCommittedStrain(void) const;

     // Accepts the current trial strain values as being on the solution path, and updates 
     // all model parameters related to stress/strain states. Return 0 on success.
     int commitState(void);

     // Revert the stress/strain states to the last committed states. Return 0 on success.
     int revertToLastCommit(void);

     int revertToStart(void) {return 0;}

     // Return an exact copy of itself.
     NDMaterial *getCopy(void) const;

     // Return a copy of itself if "code"="PressureIndependMultiYield", otherwise return null.
     NDMaterial *getCopy(const std::string &) const;

     // Return the string "PressureIndependMultiYield".
     const std::string &getType(void) const ;

     // Return ndm.
     int getOrder(void) const ;

     int sendSelf(Communicator &);  
     int recvSelf(const Communicator &);    

     Response *setResponse(const std::vector<std::string> &argv, Information &matInfo);
     int getResponse(int responseID, Information &matInformation);
     void Print(std::ostream &s, int flag =0) const;

     //void setCurrentStress(const Vector stress) { currentStress=T2Vector(stress); }
    int updateParameter(int responseID, Information &eleInformation);        

    // RWB; PyLiq1 & TzLiq1 need to see the excess pore pressure and initial stresses.
    friend class PyLiq1;
    friend class TzLiq1;
  };
} // end of XC namespace

#endif



