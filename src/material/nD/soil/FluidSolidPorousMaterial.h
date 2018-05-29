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
// $Revision: 1.11 $
// $Date: 2003/10/07 21:20:48 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/soil/FluidSolidPorousMaterial.h,v $
                                                                        
// Written: ZHY
// Created: August 2000
// Revision: A
//
// Description: This file contains the class prototype for FluidSolidPorousMaterial.
//
// What: "@(#) FluidSolidPorousMaterial.h, revA"

#ifndef FluidSolidPorousMaterial_h
#define FluidSolidPorousMaterial_h

#include <material/nD/NDMaterial.h>

namespace XC {
class Response;

//! @ingroup NDMat
//!
//! @defgroup SoilNDMat Constitutive models for soil materials.
//
//! @ingroup SoilNDMat
//
//! @brief ??.
class FluidSolidPorousMaterial: public NDMaterial
  {
  private:
    static int matCount;
    static int* ndmx;
    static int* loadStagex;
    static double *combinedBulkModulusx;
    static double pAtm;
    int matN;
    NDMaterial *theSoilMaterial;
    mutable double trialExcessPressure;
    double currentExcessPressure;
    double trialVolumeStrain;
    double currentVolumeStrain;
    mutable double initMaxPress;
    mutable int e2p;

    static Vector workV3;
    static Vector workV6;
    static Matrix workM3;
    static Matrix workM6;

    void free_mem(void);
    void alloc(const NDMaterial *);
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
     // Initialization constructor
     FluidSolidPorousMaterial(int tag= 0);
     FluidSolidPorousMaterial(int tag, int nd,const NDMaterial &soilMat, double combinedBulkModul, double atm=101.);
     FluidSolidPorousMaterial(const FluidSolidPorousMaterial &);
     FluidSolidPorousMaterial &operator=(const FluidSolidPorousMaterial &tro);

     // Destructor: clean up memory storage space.
     virtual ~FluidSolidPorousMaterial();

     // Sets the values of the trial strain tensor.
     int setTrialStrain(const Vector &strain);

     // Sets the values of the trial strain and strain rate tensors.
     int setTrialStrain(const Vector &v, const Vector &r);

     int setTrialStrainIncr(const Vector &v);
     int setTrialStrainIncr(const Vector &v, const Vector &r);

     // Calculates current tangent stiffness.
     const Matrix &getTangent(void) const;
     const Matrix &getInitialTangent(void) const;

     double getRho(void) const;

     // Calculates the corresponding stress increment(rate), for a given strain increment. 
     const Vector &getStress(void) const;
     const Vector &getStrain(void) const;
     const Vector &getCommittedStress(void);
     const Vector &getCommittedStrain(void);
     const Vector &getCommittedPressure(void);

     // Accepts the current trial strain values as being on the solution path, and updates 
     // all model parameters related to stress/strain states. Return 0 on success.
     int commitState(void);

     // Revert the stress/strain states to the last committed states. Return 0 on success.
     int revertToLastCommit(void);

     int revertToStart(void);

     // Return an exact copy of itself.
     NDMaterial *getCopy(void) const;

     // Return a copy of itself if "code"="PlainStrain" or "ThreeDimensional", otherwise return null.
     NDMaterial *getCopy(const std::string &) const;

     // Return the string "PlaneStrain" or "ThreeDimensional".
     const std::string &getType(void) const ;

     // Return 3 or 6.
     int getOrder(void) const ;

     int sendSelf(CommParameters &);  
     int recvSelf(const CommParameters &);     
     Response *setResponse(const std::vector<std::string> &argv, Information &matInfo);
     int getResponse(int responseID, Information &matInformation);
     void Print(std::ostream &s, int flag =0);

     int updateParameter(int responseID, Information &eleInformation);

     // RWB; PyLiq1 & TzLiq1 need to see the excess pore pressure and initial stresses.
     friend class PyLiq1;
     friend class TzLiq1;
  };
} // end of XC namespace

#endif
